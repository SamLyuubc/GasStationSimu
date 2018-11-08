//Name:Yujia Lyu Suheng He	 
//Student Number:55054134 51513133
#include "Customer.h"
#include <queue>
#include "../GasStationSimu/rt.h"

queue<Customer*> CustomerQueue;
Customer *activeCustomers[GAS_PUMP_NUM];


void CreateNewCustomer()
{
	Customer *customer = new Customer();
	CustomerQueue.push(customer);

	string message = "Queue Size: " + to_string(CustomerQueue.size());

	//LogMessage(message.c_str());
}

void RemoveFinishedCustomers()
{
	for (int i = 0; i < GAS_PUMP_NUM; i++)
	{
		Customer *customer = activeCustomers[i];
		if (customer == nullptr) continue;

		if (customer->isCustomerDone())
		{
			delete customer;
			printf("delete cumstomer....");
			activeCustomers[i] = nullptr;
		}
	}
}

void AssignNewCustomers()
{
	if (CustomerQueue.size() == 0) return;

	for (int i = 0; i < GAS_PUMP_NUM; i++)
	{
		if (activeCustomers[i] == nullptr)
		{
			Customer *customer = CustomerQueue.front();

			CustomerQueue.pop();
			activeCustomers[i] = customer;
			customer->Resume();

			string message = "Queue Size: " + to_string(CustomerQueue.size()) + " ";

			if (CustomerQueue.size() == 0) return;
		}
	}
}

int main()
{
	srand(time(NULL));
	//string pipename_1 = "MypipPump0";
	//string pipename_2 = "MypipPump1";
	//string pipename_3 = "MypipPump2";
	//string pipename_4 = "MypipPump3";

	//Customer *customer_1= new Customer();
	//Customer *customer_2 = new Customer();
	//Customer *customer_3 = new Customer();
	//Customer *customer_4 = new Customer();

	//customer_1->Resume();
	//customer_2->Resume();
	//customer_3->Resume();
	//customer_4->Resume();

	//customer_1->WaitForThread();
	//customer_2->WaitForThread();
	//customer_3->WaitForThread();
	//customer_4->WaitForThread();

	int customerWaitTime, count;

	while (1)
	{
		CreateNewCustomer();

		// Calculate random time until next customer
		customerWaitTime = rand() % 6 + 5;
		count = 0;

		while (count < customerWaitTime)
		{
			RemoveFinishedCustomers();
			AssignNewCustomers();

			SLEEP(1000);
			count++;
		}
	}
}

