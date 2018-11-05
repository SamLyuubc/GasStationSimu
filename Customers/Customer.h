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

class Customer : public ActiveClass 
{
	static const int GAS_GRADE_NUM = 4;

private:
	struct CustomerInfo customer_info_;
	string pipeline_name_;
	static string createRandomName();
	static long long createRandomCard();
	static double createRandomVolume();
	static int createRandomGasGrade();

public:
	Customer();
	Customer(string pipename);
	~Customer();
	int	main();
};