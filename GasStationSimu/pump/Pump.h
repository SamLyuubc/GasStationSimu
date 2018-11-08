#pragma once
#include "../rt.h"
#include "GasTank.h"
//Name:Yujia Lyu Suheng He	
//Student Number:55054134 51513133
CRendezvous r1("StationRendezvousStart", 20);

enum
{
	STATUS_IDLE = 0,
	STATUS_IN_USE = 1
};

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


struct CustomerInfo
{
	int gas_grade;
	double volume;
	long long card_number;
	string name;
};


class Pump : public ActiveClass 
{
private:
	struct StationInfo*	datapool_ptr;
	struct CustomerInfo customer_info_;

	GasTank *gastank_;

	int pump_num_;
	double transaction_cost_;
	
	const int pos_x_ = 35;
	const int pos_y_ = 9;
	const int pos_offset_ = 4;

	CMutex *pumpIO_mutex_;
	CMutex *pump_mutex_;

	// Pump status constant
	static const int STATUS_PUMP_DISABLED = 7;							// Gray
	static const int STATUS_PUMP_WAITING_CUSTOMER = 6;					// Dark Yellow
	static const int STATUS_PUMP_WAITING_AUTHORIZATION = 12;			// Red
	static const int STATUS_PUMP_WAITING_TANK = 14;						// Yellow
	static const int STATUS_PUMP_DISPENSING_GAS = 10;					// Green

public:
	Pump(int num, GasTank *gas_tank) 
	{
		this->gastank_ = gas_tank;
		pump_num_ = num;
		pumpIO_mutex_ = new CMutex("PumpIo");
		pump_mutex_ = new CMutex(string("Mutex") + string("Pump") + to_string(pump_num_));
	};

	~Pump() 
	{
		delete pumpIO_mutex_;
	};

	void PrintCustomeInfo() const
	{
		int x = (pump_num_ % 2) * pos_x_;
		int y = (pump_num_ / 2) * pos_y_ + pos_offset_;

		int grade = gastank_->GetOctaneGrade(customer_info_.gas_grade);

		pumpIO_mutex_->Wait();
		MOVE_CURSOR(x, y);		printf("                                 ");
		MOVE_CURSOR(x, y + 1);	printf("                                 ");
		MOVE_CURSOR(x, y + 2);	printf("                                 ");
		MOVE_CURSOR(x, y + 3);	printf("                                 ");
		MOVE_CURSOR(x, y + 4);	printf("                                 ");
		MOVE_CURSOR(x, y + 5);	printf("                                 ");
		MOVE_CURSOR(x, y + 6);	printf("                                 ");
		MOVE_CURSOR(x, y + 7);	printf("                                 ");

		MOVE_CURSOR(x, y);		printf("PUMP %d:\n", pump_num_);
		MOVE_CURSOR(x, y + 1);	printf("Customer Name: %s\n", customer_info_.name.c_str());
		MOVE_CURSOR(x, y + 2);	printf("Credit Card: %lld\n", customer_info_.card_number);
		MOVE_CURSOR(x, y + 3);	printf("Requested Vol.: %3.1f\n", customer_info_.volume);
		MOVE_CURSOR(x, y + 4);	printf("Dispensed Vol.: %3.1f\n", datapool_ptr->dispensedvolume);
		MOVE_CURSOR(x, y + 5);	printf("Fuel Grade: Octane %d\n", grade);
		MOVE_CURSOR(x, y + 6);	printf("Cost: $%.2f\n", transaction_cost_ * datapool_ptr->dispensedvolume);

		TEXT_COLOUR(datapool_ptr->pump_status, 0);
		MOVE_CURSOR(x, y + 7);	printf("Status: %s\n", ReadStatus(datapool_ptr->pump_status).c_str());
		TEXT_COLOUR(15, 0);

		fflush(stdout);
		pumpIO_mutex_->Signal();
	}

	string ReadStatus(int status) const
	{
		switch (status)
		{
		case STATUS_PUMP_WAITING_CUSTOMER: return "Waiting for Customer";
		case STATUS_PUMP_WAITING_AUTHORIZATION: return "Waiting for Authorization";
		case STATUS_PUMP_WAITING_TANK: return "Waiting for Fuel";
		case STATUS_PUMP_DISABLED: return "Pump Disabled";
		case STATUS_PUMP_DISPENSING_GAS: return "Dispensing Gas";
		default: return "Error";
		}
	}


	void PrintEmptyInfo() const
	{
		int x = (pump_num_ % 2) * pos_x_;
		int y = (pump_num_ / 2) * pos_y_ + pos_offset_;

		pumpIO_mutex_->Wait();
		MOVE_CURSOR(x, y);		printf("                                 ");
		fflush(stdout);
		MOVE_CURSOR(x, y + 1);	printf("                                 ");
		fflush(stdout);
		MOVE_CURSOR(x, y + 2);	printf("                                 ");
		fflush(stdout);
		MOVE_CURSOR(x, y + 3);	printf("                                 ");
		fflush(stdout);
		MOVE_CURSOR(x, y + 4);	printf("                                 ");
		fflush(stdout);
		MOVE_CURSOR(x, y + 5);	printf("                                 ");
		fflush(stdout);
		MOVE_CURSOR(x, y + 6);	printf("                                 ");
		fflush(stdout);
		MOVE_CURSOR(x, y + 7);	printf("                                 ");
		fflush(stdout);

		MOVE_CURSOR(x, y);		printf("PUMP %d:                         \n", pump_num_);
		fflush(stdout);
		MOVE_CURSOR(x, y + 1);	printf("Customer Name:                   \n");
		fflush(stdout);
		MOVE_CURSOR(x, y + 2);	printf("Credit Card:                     \n");
		fflush(stdout);
		MOVE_CURSOR(x, y + 3);	printf("Requested Vol.:                  \n");
		fflush(stdout);
		MOVE_CURSOR(x, y + 4);	printf("Dispensed Vol.:                  \n");
		fflush(stdout);
		MOVE_CURSOR(x, y + 5);	printf("Fuel Grade:                      \n");
		fflush(stdout);
		MOVE_CURSOR(x, y + 6);	printf("Cost:                            \n");
		fflush(stdout);

		TEXT_COLOUR(datapool_ptr->pump_status, 0);
		MOVE_CURSOR(x, y + 7);	printf("Status: %s\n", ReadStatus( datapool_ptr->pump_status).c_str());
		TEXT_COLOUR(15, 0);

		fflush(stdout);
		pumpIO_mutex_->Signal();
	}

	int main()
	{
		//initialize rendezvous synchronization
		r1.Wait();
		
		// construct name for mutex structure
		std::string pipe_name, cs_name, ps_name, datapool_name;
		// ex. MypipPump1
		pipe_name = "MypipPump" + to_string(pump_num_);
		// ex. CsPump1
		cs_name = "CsPump" + to_string(pump_num_);
		// ex. PsPump1
		ps_name = "PsPump" + to_string(pump_num_);
		// ex. DataPoolPump1
		datapool_name = "DataPoolPump" + to_string(pump_num_);

		// Initialize mutex structure
		CPipe data_pipeline(pipe_name, 1024);
		CDataPool data_pool(datapool_name, sizeof(struct StationInfo));
		datapool_ptr = (struct StationInfo*)data_pool.LinkDataPool();

		CSemaphore cs(cs_name, 1, 1);
		CSemaphore ps(ps_name, 0, 1);

		while (true)
		{
			datapool_ptr->pump_status = STATUS_PUMP_WAITING_CUSTOMER;
			while (data_pipeline.TestForData() == 0)
			{
				//datapool_ptr->pump_status = !datapool_ptr->isEnabled ? STATUS_PUMP_DISABLED : STATUS_PUMP_WAITING_CUSTOMER;
				PrintEmptyInfo();
				SLEEP(100);
			}

			data_pipeline.Read(&customer_info_, sizeof(struct CustomerInfo));
			pump_mutex_->Wait();

			datapool_ptr->pump_status = STATUS_PUMP_WAITING_AUTHORIZATION;
			PrintCustomeInfo();

			cs.Wait();
			datapool_ptr->card_number = customer_info_.card_number;
			strcpy_s(datapool_ptr->customer_name, 30, customer_info_.name.c_str());
			datapool_ptr->gas_grade = customer_info_.gas_grade;
			datapool_ptr->dispensedvolume = 0;
			datapool_ptr->cost = 0;
			datapool_ptr->isAuthorized = false;
			datapool_ptr->isDone = false;
			transaction_cost_ = gastank_->GetGasCost(customer_info_.gas_grade);
			ps.Signal();

			// waiting for GSC authorization
			PrintCustomeInfo();
			cs.Wait();
			// Reset this for the next customer
			if (datapool_ptr->isAuthorized)
				datapool_ptr->isAuthorized = false;

			datapool_ptr->pump_status = STATUS_PUMP_WAITING_TANK;
			PrintCustomeInfo();

			// wait gasstation ready
			while (gastank_->GetGas(datapool_ptr->gas_grade) < 200)
			{
				SLEEP(200);
			}

			datapool_ptr->pump_status = STATUS_PUMP_DISPENSING_GAS;

			while (gastank_->GetGas(datapool_ptr->gas_grade) > 0 && datapool_ptr->dispensedvolume < customer_info_.volume)
			{
				datapool_ptr->dispensedvolume += gastank_->WithdrawGas(0.5, datapool_ptr->gas_grade);
				PrintCustomeInfo();

				if (datapool_ptr->dispensedvolume >= customer_info_.volume || (int)gastank_->GetGas(datapool_ptr->gas_grade) <= 0)
				{
					datapool_ptr->isDone = true;
					datapool_ptr->cost = datapool_ptr->dispensedvolume * transaction_cost_;
				}

				//ps.Signal();
				//cs.Wait();

				SLEEP(1000);
			}

			// Signal GSC to say we're done transaction
			ps.Signal();

			pump_mutex_->Signal();
		}
	};
};