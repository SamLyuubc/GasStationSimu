//Name:Yujia Lyu Suheng He	 
//Student Number:55054134 51513133
#include "Customer.h"
#include "../GasStationSimu/rt.h"

int main()
{
	struct CustomerInfo test1 {95, 10, "123456789" };
	struct CustomerInfo test2 { 93, 10, "7788613563" };
	struct CustomerInfo test3 { 91, 10, "2323232323" };
	struct CustomerInfo test4 { 89, 10, "23245566666" };

	string pipename_1 = "MypipPump1";
	string pipename_2 = "MypipPump2";
	string pipename_3 = "MypipPump3";
	string pipename_4 = "MypipPump4";

	Customer customer_1(test1,pipename_1);
	Customer customer_2(test2, pipename_2);
	Customer customer_3(test3, pipename_3);
	Customer customer_4(test4, pipename_4);

	customer_1.Resume();
	customer_2.Resume();
	customer_3.Resume();
	customer_4.Resume();

	customer_1.WaitForThread();
	customer_2.WaitForThread();
	customer_3.WaitForThread();
	customer_4.WaitForThread();
}

