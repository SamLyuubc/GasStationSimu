//Name:Yujia Lyu Suheng He	 
//Student Number:55054134 51513133
#include "Customer.h"
#include "../GasStationSimu/rt.h"

int main()
{
	srand(time(NULL));
	string pipename_1 = "MypipPump0";
	string pipename_2 = "MypipPump1";
	string pipename_3 = "MypipPump2";
	string pipename_4 = "MypipPump3";

	Customer customer_1(pipename_1);
	Customer customer_2(pipename_2);
	Customer customer_3(pipename_3);
	Customer customer_4(pipename_4);

	customer_1.Resume();
	customer_2.Resume();
	customer_3.Resume();
	customer_4.Resume();

	customer_1.WaitForThread();
	customer_2.WaitForThread();
	customer_3.WaitForThread();
	customer_4.WaitForThread();
}

