#pragma once
#include "../GasStationSimu/rt.h"
#include <ctime>

//CRendezvous r1("StationRendezvousStart", 12);
//Name:Yujia Lyu Suheng He	
//Student Number:55054134 51513133
struct CustomerInfo
{
	int gas_grade;
	double volume;
	long long card_number;
	string name;
};

static const int GAS_GRADE_NUM = 4;
static const int GAS_PUMP_NUM = 8;

class Customer : public ActiveClass 
{
private:
	struct CustomerInfo customer_info_;
	string pipeline_name_;
	int pump_num_;
	bool customer_is_done_;
	static int createRandomPumpNum();
	static string createRandomName();
	static long long createRandomCard();
	static double createRandomVolume();
	static int createRandomGasGrade();

public:
	Customer();
	Customer(string pipename);
	bool isCustomerDone();
	~Customer();
	int	main();
};