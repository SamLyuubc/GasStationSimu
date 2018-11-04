#pragma once
#include "../rt.h"
#include "GasTank.h"
//Name:Yujia Lyu Suheng He	
//Student Number:55054134 51513133
CRendezvous r1("StationRendezvousStart", 16);

enum
{
	STATUS_IDLE = 0,
	STATUS_IN_USE = 1
};

struct StationInfo
{
	int gas_grade;
	int volume;
	int status;
	long long card_number;
};

struct CustomerInfo
{
	int gas_grade;
	int volume;
	long long card_number;
	string name;
};


class Pump : public ActiveClass 
{
private:
	struct StationInfo station_info_;
	struct CustomerInfo customer_info_;

	GasTank *gastank_;

	int pump_num_;
	int storage_;
	int pos_x_ = 35;
	int pos_y_ = 9;
	int pos_offset_ = 4;

	CMutex *pumpIO_mutex_;

public:
	Pump(int num, GasTank *gas_tank) 
	{
		this->gastank_ = gas_tank;
		pump_num_ = num;
		pumpIO_mutex_ = new CMutex("PumpIo");
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
		//MOVE_CURSOR(x, y + 4);	printf("Dispensed Vol.: %3.1f\n", data->dispensedVolume);
		MOVE_CURSOR(x, y + 5);	printf("Fuel Grade: Octane %d\n", grade);
		//MOVE_CURSOR(x, y + 6);	printf("Cost: $%.2f\n", transactionCost * data->dispensedVolume);

		//TEXT_COLOUR(data->pumpStatus, 0);
		//MOVE_CURSOR(x, y + 7);	printf("Status: %s\n", ReadStatus(data->pumpStatus).c_str());
		//TEXT_COLOUR(15, 0);

		fflush(stdout);
		pumpIO_mutex_->Signal();
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

		/*TEXT_COLOUR(data->pumpStatus, 0);
		MOVE_CURSOR(x, y + 7);	printf("Status: %s\n", ReadStatus(data->pumpStatus).c_str());
		TEXT_COLOUR(15, 0);*/

		/*fflush(stdout);*/
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
		struct StationInfo*	datapool_ptr;
		datapool_ptr = (struct StationInfo*)data_pool.LinkDataPool();

		CSemaphore cs(cs_name, 1, 1);
		CSemaphore ps(ps_name, 0, 1);

		while (true)
		{
			while (data_pipeline.TestForData() == 0)
			{
				PrintEmptyInfo();
				SLEEP(100);
			}

			data_pipeline.Read(&customer_info_, sizeof(struct CustomerInfo));

			PrintCustomeInfo();
			cs.Wait();
			//printf("Passing gas grade: %d from Pump ..\n",customer_info_.gas_grade);
			datapool_ptr->card_number = customer_info_.card_number;
			datapool_ptr->volume = customer_info_.volume;
			datapool_ptr->gas_grade = customer_info_.gas_grade;
			datapool_ptr->status = STATUS_IN_USE;
			ps.Signal();
		}
	};
};