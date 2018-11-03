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

	// Same name to the datapipeline
	int pump_num_;
	int storage_;

public:
	Pump(int num) 
	{
		pump_num_ = num;
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
			data_pipeline.Read(&customer_info_, sizeof(struct CustomerInfo));
			cs.Wait();
			printf("Passing gas grade: %d from Pump ..\n",customer_info_.gas_grade);
			datapool_ptr->card_number = customer_info_.card_number;
			datapool_ptr->volume = customer_info_.volume;
			datapool_ptr->gas_grade = customer_info_.gas_grade;
			datapool_ptr->status = STATUS_IN_USE;
			ps.Signal();
		}
	};
};