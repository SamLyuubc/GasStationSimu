#pragma once
#include "../GasStationSimu/rt.h"
CRendezvous r1("StationRendezvousStart", 12);
//Name:Yujia Lyu Suheng He	
//Student Number:55054134 51513133
struct CustomerInfo
{
	int gas_grade;
	int volume;
	string card_number;
};

class Customer : public ActiveClass 
{
private:
	struct CustomerInfo customer_info_;
	string pipeline_name_;

public:
	Customer(struct CustomerInfo info, string pipeline) 
	{
		customer_info_ = info;
		pipeline_name_ = pipeline;
	};

	int	main()
	{
		printf("Creating the Customer. Ready to go.....\n");
		r1.Wait();
		CPipe	MyPipe(pipeline_name_);							// Create a pipe with the name "Pipe1"
		CMutex  mutex(pipeline_name_);

		for (int x = 500; x < 10000; x++) {
			printf("customer is sending gas grade: %d\n",customer_info_.gas_grade);
			mutex.Wait();
			MyPipe.Write(&customer_info_, sizeof(struct CustomerInfo));
			mutex.Signal();
			Sleep(2000);
		}

		return 0;
	};
};