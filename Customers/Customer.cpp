#include "Customer.h"
#include "../GasStationSimu/rt.h"

CRendezvous r1("StationRendezvousStart", 12);

Customer::Customer()
{
	customer_info_.card_number = createRandomCard();
	customer_info_.gas_grade = createRandomGasGrade();
	customer_info_.name = createRandomName();
	customer_info_.volume = createRandomVolume();
}

Customer::Customer(string pipename)
{
	pipeline_name_ = pipename;
	customer_info_.card_number = createRandomCard();
	customer_info_.gas_grade = createRandomGasGrade();
	customer_info_.name = createRandomName();
	customer_info_.volume = createRandomVolume();
}

Customer::~Customer() 
{
}

int Customer::main() 
{
	printf("Creating the Customer. Ready to go.....\n");
	r1.Wait();
	CPipe	MyPipe(pipeline_name_);							// Create a pipe with the name "Pipe1"
	CMutex  mutex(pipeline_name_);
	CMutex  mutex_p("ScreenCustomer");

	for (int x = 500; x < 10000; x++) {
		mutex_p.Wait();
		std::cout << "Customer Card is: " << customer_info_.card_number << "\n";
		std::cout << "Customer Name is: " << customer_info_.name << "\n";
		mutex_p.Signal();
		mutex.Wait();
		MyPipe.Write(&customer_info_, sizeof(struct CustomerInfo));
		mutex.Signal();
		Sleep(2000);
	}

	return 0;
}

string Customer::createRandomName() 
{
	//Array of First Names
	string Names[] = { "Harley", "Rahul", "Armin", "Alexander", "Renee",  "Brandon", "Herman", "Evelyn", "Sasha", "Susan", "Monika", "Sophia", "Harish", "Zenia", "Mela", "Cameron", "Mira", "Anita" };


	int lengthFirstName = sizeof(Names) / sizeof(Names[0]);

	string Name = Names[rand() % lengthFirstName];

	string name = Name;

	return name;
}

long long Customer::createRandomCard() 
{
	long long CardNumber = 4LL;

	for (int i = 0; i < 15; i++)
	{
		CardNumber *= 10;
		CardNumber += rand() % 10;
	}

	return CardNumber;
}

int Customer::createRandomGasGrade() 
{
	return (rand() %  (GAS_GRADE_NUM-1) + 1);
}

double Customer::createRandomVolume() 
{
	return rand() % 61 + 10;
}