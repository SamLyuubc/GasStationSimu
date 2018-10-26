#pragma once
#include "../rt.h"
//Name:Yujia Lyu Suheng He	
//Student Number:55054134 51513133
CRendezvous r1("StationRendezvousStart", 12);

enum
{
	STATUS_IDLE = 0,
	STATUS_IN_USE = 1
};

struct StationInfo
{
	int gas_grade;
	int status;
	string card_number;
};

struct CustomerInfo 
{
	int gas_grade;
	int volume;
	string card_number;
};


class Pump : public ActiveClass 
{
private:
	struct StationInfo station_info_;
	struct CustomerInfo customer_info_;

	// Same name to the datapipeline
	string name_;
	int storage_;

public:
	Pump(string name) 
	{
		name_ = name;
		storage_ = 100;
	};

	int main()
	{
		//initialize rendezvous synchronization
		printf("ready to start...\n");
		r1.Wait();
		
		// construct name for mutex structure
		std::string pipe_name, cs_name, ps_name, datapool_name;
		// ex. MypipPump1
		pipe_name = "Mypip" + name_;
		// ex. CsPump1
		cs_name = "Cs" + name_;
		// ex. PsPump1
		ps_name = "Ps" + name_;
		// ex. DataPoolPump1
		datapool_name = "DataPool" + name_;

		// Initialize mutex structure
		CPipe data_pipeline(pipe_name, 1024);
		CDataPool data_pool(datapool_name, sizeof(struct StationInfo));
		struct StationInfo*	datapool_ptr;
		datapool_ptr = (struct StationInfo*)data_pool.LinkDataPool();

		CSemaphore cs(cs_name, 1, 1);
		CSemaphore ps(ps_name, 0, 1);

		while (true)
		{
			data_pipeline.Read(&customer_info_, sizeof(struct CustomerInfo));
			cs.Wait();
			printf("Passing gas grade: %d from Pump ..\n",customer_info_.gas_grade);
			datapool_ptr->card_number = customer_info_.card_number;
			datapool_ptr->gas_grade = customer_info_.gas_grade;
			datapool_ptr->status = STATUS_IN_USE;
			ps.Signal();
		}
	};
};