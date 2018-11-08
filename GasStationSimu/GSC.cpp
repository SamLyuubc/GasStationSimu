#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include "..\..\rt.h"
#include "pump/GasTank.h"
#include "Transaction.h"
#include <vector>

CRendezvous r1("StationRendezvousStart", 16);
struct StationInfo
{
	int gas_grade;
	double dispensedvolume;
	int pump_status;
	double cost;
	bool isAuthorized;
	bool isDone;
	bool isEnabled;
	long long card_number;
	char customer_name[];
};

// Position for printing pump information
const int pos_x_ = 35;
const int pos_y_ = 9;
const int pos_offset_ = 4;

// Position for printing tank information
const int pos_tank_info_x_ = 70;
const int pos_tank_info_y_ = 6;

// Position for printing transaction information
const int pos_trans_info_x_ = 70;
const int pos_trans_info_y_ = 8;

// Positon for printing command
const int pos_console_output_y_ = 2;
const int pos_console_input_y_ = 4;

// Initialize constants for system setup
const int NUM_PUMPS = 8;
const int NUM_TANKS = 4;

GasTank gas_tank;
CMutex gsc_mutex("Mutex_GSC");
CMutex test_mutex("Mutex_GSC_TEST");

// Initialize producer and consumer for data
CSemaphore *CSs[NUM_PUMPS];
CSemaphore *PSs[NUM_PUMPS];
CDataPool *pump_dps[NUM_PUMPS];
struct StationInfo *pumpInfoData[NUM_PUMPS];

PerThreadStorage int thread_pumpnum;

bool isAuthorized[NUM_PUMPS];
bool isCustomerDone[NUM_PUMPS];
//bool isPumpEnabled[NUM_PUMPS];

// Command line and buffer constants

//Pump status constants
static const int STATUS_PUMP_DISABLED = 7;							// Gray
static const int STATUS_PUMP_WAITING_CUSTOMER = 6;					// Dark Yellow
static const int STATUS_PUMP_WAITING_AUTHORIZATION = 12;			// Red
static const int STATUS_PUMP_WAITING_TANK = 14;						// Yellow
static const int STATUS_PUMP_DISPENSING_GAS = 10;					// Green

// Command line buffers and constants
const int SIZE_LINE_MAX = 20;
const int SIZE_PARAM_MAX = 3;
const int LENGTH_PARAM_MAX = 10;
char parameter_array[SIZE_PARAM_MAX][LENGTH_PARAM_MAX + 1];
char lineBuffer[SIZE_LINE_MAX + 1];

void LogMessage(char const *message)
{
	gsc_mutex.Wait();
	MOVE_CURSOR(0, pos_console_output_y_ + 1); printf("                                   \n");
	fflush(stdout);
	MOVE_CURSOR(0, pos_console_output_y_ + 1); printf("Input: %s\n", message);
	fflush(stdout);
	gsc_mutex.Signal();

	MOVE_CURSOR(0, pos_console_input_y_);
}

void ErrorMessage(char const *message)
{
	Beep(523, 500);
	gsc_mutex.Wait();
	TEXT_COLOUR(12, 0);
	MOVE_CURSOR(0, pos_console_output_y_); printf("                                   \n");
	fflush(stdout);
	MOVE_CURSOR(0, pos_console_output_y_); printf("ERROR INPUT: %s\n", message);
	fflush(stdout);
	TEXT_COLOUR(15, 0);
	gsc_mutex.Signal();

	MOVE_CURSOR(0, pos_console_input_y_);
}

void SuccessMessage(char const *message)
{
	Beep(659, 300);

	gsc_mutex.Wait();
	TEXT_COLOUR(11, 0);
	MOVE_CURSOR(0, pos_console_output_y_);
	printf("                                   \n");
	fflush(stdout);
	MOVE_CURSOR(0, pos_console_output_y_);
	printf("SUCCESS INPUT: %s\n", message);
	fflush(stdout);
	TEXT_COLOUR(15, 0);
	gsc_mutex.Signal();

	MOVE_CURSOR(0, pos_console_input_y_);
}

string ReadPumpStatus(int status)
{
	switch (status)
	{
	case STATUS_PUMP_WAITING_CUSTOMER: return "Waiting for Customer";
	case STATUS_PUMP_WAITING_AUTHORIZATION: return "Waiting for Authorization";
	case STATUS_PUMP_WAITING_TANK: return "Waiting for Fuel";
	case STATUS_PUMP_DISPENSING_GAS: return "Dispensing Gas";
	default: return "Error";
	}
}

void PrintEmptyPumpDetails(int pumpNumber)
{
	int x = (pumpNumber % 2) * pos_x_;
	int y = (pumpNumber / 2) * pos_y_ + pos_offset_;

	int pumpStatus = pumpInfoData[pumpNumber]->pump_status;

	gsc_mutex.Wait();
	CURSOR_OFF();
	MOVE_CURSOR(x, y);
	printf("                                   ");
	fflush(stdout);
	MOVE_CURSOR(x, y + 1);
	printf("                                   ");
	fflush(stdout);
	MOVE_CURSOR(x, y + 2);
	printf("                                   ");
	fflush(stdout);
	MOVE_CURSOR(x, y + 3);
	printf("                                   ");
	fflush(stdout);
	MOVE_CURSOR(x, y + 4);
	printf("                                   ");
	fflush(stdout);
	MOVE_CURSOR(x, y + 5);
	printf("                                   ");
	fflush(stdout);

	MOVE_CURSOR(x, y);
	printf("PUMP %d:                           \n", pumpNumber);
	fflush(stdout);
	MOVE_CURSOR(x, y + 1);
	printf("Customer Name:                     \n");
	fflush(stdout);
	MOVE_CURSOR(x, y + 2);
	printf("Credit Card:                       \n");
	fflush(stdout);
	MOVE_CURSOR(x, y + 3);
	printf("Dispensed Vol.:                    \n");
	fflush(stdout);
	MOVE_CURSOR(x, y + 4);
	printf("Fuel Grade:                        \n");
	fflush(stdout);

	TEXT_COLOUR(pumpInfoData[pumpNumber]->pump_status, 0);
	MOVE_CURSOR(x, y + 5);
	printf("Status: %s\n", ReadPumpStatus(pumpInfoData[pumpNumber]->pump_status).c_str());
	fflush(stdout);
	TEXT_COLOUR(15, 0);

	MOVE_CURSOR(0, pos_console_input_y_);
	CURSOR_ON();
	gsc_mutex.Signal();
}

void PrintPumpDetails(int pumpNumber)
{
	int x = (pumpNumber % 2) * pos_x_;
	int y = (pumpNumber / 2) * pos_y_ + pos_offset_;

	string name = pumpInfoData[pumpNumber]->customer_name;
	int pumpStatus = pumpInfoData[pumpNumber]->pump_status;

	int grade = gas_tank.GetOctaneGrade(pumpInfoData[pumpNumber]->gas_grade);

	gsc_mutex.Wait();
	CURSOR_OFF();
	MOVE_CURSOR(x, y);
	printf("                                   ");
	fflush(stdout);
	MOVE_CURSOR(x, y + 1);
	printf("                                   ");
	fflush(stdout);
	MOVE_CURSOR(x, y + 2);
	printf("                                   ");
	fflush(stdout);
	MOVE_CURSOR(x, y + 3);
	printf("                                   ");
	fflush(stdout);
	MOVE_CURSOR(x, y + 4);
	printf("                                   ");
	fflush(stdout);
	MOVE_CURSOR(x, y + 5);
	printf("                                   ");
	fflush(stdout);

	MOVE_CURSOR(x, y);
	printf("PUMP %d:\n", pumpNumber);
	fflush(stdout);
	MOVE_CURSOR(x, y + 1);
	std::cout << "Customer Name: " << name << std::endl;
	fflush(stdout);
	MOVE_CURSOR(x, y + 2);
	printf("Credit Card: %lld\n", pumpInfoData[pumpNumber]->card_number);
	fflush(stdout);
	MOVE_CURSOR(x, y + 3);
	printf("Dispensed Vol.: %3.1f\n",
	pumpInfoData[thread_pumpnum]->dispensedvolume);
	fflush(stdout);
	MOVE_CURSOR(x, y + 4);
	printf("Fuel Grade: Octane %d\n", grade);
	fflush(stdout);

	TEXT_COLOUR(pumpInfoData[pumpNumber]->pump_status, 0);
	MOVE_CURSOR(x, y + 5);
	printf("Status: %s\n", ReadPumpStatus(pumpInfoData[pumpNumber]->pump_status).c_str());
	fflush(stdout);
	TEXT_COLOUR(15, 0);

	MOVE_CURSOR(0, pos_console_input_y_);
	CURSOR_ON();
	gsc_mutex.Signal();
}

void PrintFuelTankDetails()
{
	int x = pos_tank_info_x_;
	int y = pos_tank_info_y_ + pos_offset_;

	gsc_mutex.Wait();
	CURSOR_OFF();
	MOVE_CURSOR(x, y);		printf("                                      ");
	MOVE_CURSOR(x, y + 1);	printf("                                      ");
	MOVE_CURSOR(x, y + 2);	printf("                                      ");
	MOVE_CURSOR(x, y + 3);	printf("                                      ");
	MOVE_CURSOR(x, y + 4);	printf("                                      ");

	MOVE_CURSOR(x, y);		printf("Fuel Tank Station:\n");

	//TODO: Refactor this to INT_NumFuelGrades
	for (int i = 0; i < NUM_TANKS; i++)
	{
		MOVE_CURSOR(x, y + i + 1);
		printf("Fuel Grade: %d Price: $%.2f: %.1fL ", gas_tank.GetOctaneGrade(i),
			gas_tank.GetGasCost(i), gas_tank.GetGas(i));

		int status = gas_tank.GetStatusNumber(i);

		printf("-> %s\n", gas_tank.GetStatus(i).c_str());
		TEXT_COLOUR(15, 0);
	}

	fflush(stdout);

	MOVE_CURSOR(0, pos_console_input_y_);
	CURSOR_ON();
	gsc_mutex.Signal();

}

void WaitForGSCAuthorization(int num)
{
	while (!isAuthorized[num])
	{
		SLEEP(100);
	}

	isAuthorized[num] = false; // Reset it
	pumpInfoData[num]->isAuthorized = true;
}

UINT __stdcall PumpThread(void *args)	// thread function
{
	r1.Wait();
	thread_pumpnum = *(int *)(args);
	int x = (thread_pumpnum % 2) * pos_x_;
	int y = (thread_pumpnum / 2) * pos_y_ + pos_offset_;

	while (1)
	{
		PrintEmptyPumpDetails(thread_pumpnum);

		PSs[thread_pumpnum]->Wait();
		PrintPumpDetails(thread_pumpnum);
		WaitForGSCAuthorization(thread_pumpnum);
		CSs[thread_pumpnum]->Signal();


		isCustomerDone[thread_pumpnum] = false;

		while (PSs[thread_pumpnum]->Read() == 0)
		{
			PrintPumpDetails(thread_pumpnum);
			SLEEP(100);
		}

		while (!isCustomerDone[thread_pumpnum])
		{
			PSs[thread_pumpnum]->Wait();
			PrintPumpDetails(thread_pumpnum);
			if (pumpInfoData[thread_pumpnum]->isDone) isCustomerDone[thread_pumpnum] = true;
			CSs[thread_pumpnum]->Signal();
		}

		PSs[thread_pumpnum]->Wait();
		pumpInfoData[thread_pumpnum]->isDone = false;
		CSs[thread_pumpnum]->Signal();
		PrintPumpDetails(thread_pumpnum);
	}

	return 0;
}

bool isPumpNumberCorrect(char *pumpArg)
{
	char str[10];

	for (int pump = 0; pump < NUM_PUMPS; pump++)
	{
		sprintf(str, "%d", pump);
		if (0 == _stricmp(pumpArg, str))
		{
			return true;
		}
	}

	return false;
}

void processCommand(char *command)
{
	// Commands:
	// RF - Refill
	// A # - Authorize
	// C # ##.## - Change cost of gas

	if (0 == _stricmp(command, "RF"))
	{
		SuccessMessage("Refilling Fuel Tanks");
		gas_tank.RefillTanks();
	}
	else if (0 == _stricmp(command, "C"))
	{
		if (isPumpNumberCorrect(parameter_array[0]))
		{
			int pump = atoi(parameter_array[0]);
			double cost = atof(parameter_array[1]);

			if (cost > 0 && cost < 20)
			{
				gas_tank.setGasCost(pump, cost);
				SuccessMessage(string("Set gas cost to $" + to_string(cost)).c_str());
			}
			else
			{
				ErrorMessage("Gas cost needs to be between $0-20 per liter");
			}
		}
		else
		{
			ErrorMessage("Invalid pump number");
		}
	}
	else if (0 == _stricmp(command, "A"))
	{
		if (0 == _stricmp(parameter_array[0], "ALL"))
		{
			for (int i = 0; i < NUM_PUMPS; i++)
			{
				isAuthorized[i] = true;
			}

			SuccessMessage("Authorized all pumps");
		}
		else if (isPumpNumberCorrect(parameter_array[0]))
		{
			int pump = atoi(parameter_array[0]);
			isAuthorized[pump] = true;
			SuccessMessage(string("Authorized pump " + to_string(pump)).c_str());
		}
		else
		{
			ErrorMessage("Invalid pump number");
		}
	}
	else if (0 == _stricmp(command, "C"))
	{
		if (isPumpNumberCorrect(parameter_array[0]))
		{
			int pump = atoi(parameter_array[0]);
			double cost = atof(parameter_array[1]);

			if (cost > 0 && cost < 20)
			{
				gas_tank.setGasCost(pump, cost);
				SuccessMessage(string("Set gas cost to $" + to_string(cost)).c_str());
			}
			else
			{
				ErrorMessage("Gas cost needs to be between $0-20 per liter");
			}
		}
		else
		{
			ErrorMessage("Invalid pump number");
		}
	}
	else
	{
		ErrorMessage("Invalid command");
	}
}

void clearParameters()
{
	for (int i = 0; i < SIZE_PARAM_MAX; i++)
	{
		strcpy(parameter_array[i], "");
	}
}

void processParameters(char *parameterString)
{
	clearParameters();

	if (NULL != parameterString && strlen(parameterString))
	{
		uint8_t i = 0;
		const char delimiter[] = " ";
		char *token;

		// break up remaining strings into parameters
		token = strtok(parameterString, delimiter);

		while (NULL != token)
		{
			// copy parameters into the global parameter variables
			strcpy(parameter_array[i++], token);
			token = strtok(NULL, delimiter);
		}
	}
}

void processLine(char *line)
{
	char buffer[SIZE_LINE_MAX + 1];
	const char delimiter[] = " ";
	char *commandString = NULL;
	char *remainingString = NULL;

	strcpy(buffer, line);

	// get first token
	commandString = strtok(buffer, delimiter);

	if (NULL != commandString)
	{
		if (strlen(line) - strlen(commandString) > 0)
		{
			remainingString = line + strlen(commandString) + 1;
		}

		processParameters(remainingString);
		processCommand(commandString);
	}
}

void ScanKeyboard()
{
	uint16_t print_gas_info = 0;
	uint8_t linePointer = 0;
	char incomingChar;
	char tmpstr[SIZE_LINE_MAX];

	while (true)
	{
		while (TEST_FOR_KEYBOARD() == 0)
		{
			if (++print_gas_info >= 4000)
			{
				print_gas_info = 0;
				PrintFuelTankDetails();
			}
		}

		if (TEST_FOR_KEYBOARD() != 0)
		{
			incomingChar = toupper(_getch());

			gsc_mutex.Wait();
			MOVE_CURSOR(0, pos_console_input_y_);
			printf("                               ");
			MOVE_CURSOR(0, pos_console_input_y_);
			gsc_mutex.Signal();

			if (incomingChar)
			{
				if (incomingChar == '\n') // End of input
				{
				}
				else if (incomingChar == '\r') // Discard the linefeed
				{
					lineBuffer[linePointer] = 0;
					linePointer = 0;
					sprintf_s(tmpstr, SIZE_LINE_MAX, "%s", lineBuffer);
					LogMessage(tmpstr);
					processLine(lineBuffer);
				}
				else // Store any other characters in the buffer
				{
					lineBuffer[linePointer++] = incomingChar;
					lineBuffer[linePointer] = 0;
					if (linePointer >= SIZE_LINE_MAX - 1)
					{
						linePointer = SIZE_LINE_MAX - 1;
						lineBuffer[linePointer] = 0;
					}
				}
			}
		}
	}
}

void Initialize(int threadNums[], CThread *threads[])
{
	for (int i = 0; i < NUM_PUMPS; i++)
	{
		isAuthorized[i] = false;
		isCustomerDone[i] = false;
		//isPumpEnabled[i] = true;

		std::string pipe_name, cs_name, ps_name, datapool_name;
		// ex. MypipPump1
		pipe_name = "MypipPump" + to_string(i);
		// ex. CsPump1
		cs_name = "CsPump" + to_string(i);
		// ex. PsPump1
		ps_name = "PsPump" + to_string(i);
		// ex. DataPoolPump1
		datapool_name = "DataPoolPump" + to_string(i);
		PSs[i] = new CSemaphore(ps_name, 0, 1);
		CSs[i] = new CSemaphore(cs_name, 1, 1);
		pump_dps[i] = new CDataPool(datapool_name, sizeof(struct StationInfo));
		pumpInfoData[i] = (struct StationInfo *)(pump_dps[i]->LinkDataPool());

		threadNums[i] = i;
		threads[i] = new  CThread(PumpThread, ACTIVE, &threadNums[i]);
	}

	gas_tank.Initialize();
}

int main()
{
	int threadNums[NUM_PUMPS];
	CThread *threads[NUM_PUMPS];

	Initialize(threadNums, threads);

	ScanKeyboard();

	return 0;
}
