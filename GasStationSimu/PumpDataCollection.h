//Name:Yujia Lyu Suheng He	 
//Student Number:55054134 51513133
#pragma once
#include "rt.h"

CRendezvous r1("StationRendezvousStart", 12);

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

class PumpDataCollection : public ActiveClass		// A thread to use a rendezvous, args points to an int to identify the thread number
{
	string pump_name_;
	int pos_x_;
	int pos_y_;

public:
	// constructor to initialise my thread number which affects how long I take to arrive at the rendezvous

	PumpDataCollection(string pump, int x, int y)
	{
		pump_name_ = pump;
		pos_x_ = x;
		pos_y_ = y;
	}

	int main()
	{
		printf("Thread Arriving at Rendezvous.....\n");
		r1.Wait();

		std::string cs_name, ps_name, datapool_name, screen_mutex_name;
		cs_name = "Cs" + pump_name_;
		ps_name = "Ps" + pump_name_;
		datapool_name = "DataPool" + pump_name_;
		screen_mutex_name = "ScreenCenter";

		CMutex screen_mutex(screen_mutex_name);

		CDataPool data_pool(datapool_name, sizeof(struct StationInfo));
		struct StationInfo*	datapool_ptr;
		datapool_ptr = (struct StationInfo*)data_pool.LinkDataPool();

		CSemaphore cs(cs_name, 1, 1);
		CSemaphore ps(ps_name, 0, 1);

		while (true)
		{
			//SLEEP(1000);								// sleep for an amount of time based on my thread number
			ps.Wait();
			screen_mutex.Wait();
			MOVE_CURSOR(pos_x_, pos_y_);
			printf("Pump selcting Grade: %d\n", datapool_ptr->gas_grade);
			fflush(stdout);
			printf("Customer with card: %lld\n", datapool_ptr->card_number);
			//std::cout << "Customer with card: " << datapool_ptr->card_number;
			fflush(stdout);
			/*printf("Pump Status is: %d\n", datapool_ptr->status);
			fflush(stdout);*/
			SLEEP(200);
			screen_mutex.Signal();
			cs.Signal();
		}																// go back at wait again (10 times)
		return 0;														// terminate thread
	}
};