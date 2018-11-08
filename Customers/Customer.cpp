#include "Customer.h"
#include "../GasStationSimu/rt.h"

CRendezvous r1("StationRendezvousStart", 16);

Customer::Customer()
{
	customer_info_.card_number = createRandomCard();
	customer_info_.gas_grade = createRandomGasGrade();
	customer_info_.name = createRandomName();
	customer_info_.volume = createRandomVolume();
	pump_num_ = createRandomPumpNum();
	customer_is_done_ = false;
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
	//r1.Wait();
	//CPipe	MyPipe(pipeline_name_);							// Create a pipe with the name "Pipe1"
	CPipe	MyPipe("MypipPump" + to_string(pump_num_));
	CMutex	mutex(string("Mutex") + string("Pump") + to_string(pump_num_));
	CMutex  mutex_p("ScreenCustomer");


	mutex_p.Wait();
	std::cout << "Customer Card is: " << customer_info_.card_number << "\n";
	std::cout << "Customer Name is: " << customer_info_.name << "\n";
	std::cout << "Customer volume is: " << customer_info_.volume << std::endl;
	mutex_p.Signal();
	mutex.Wait();
	MyPipe.Write(&customer_info_, sizeof(struct CustomerInfo));
	mutex.Signal();

	mutex.Wait();
	customer_is_done_ = true;
	mutex.Signal();
	SLEEP(2000); // Give the GasStation simulation time to delete customer object and assign new one

	return 0;
}

bool Customer::isCustomerDone() 
{
	return customer_is_done_;
}

string Customer::createRandomName() 
{
	//Array of First Names
	string Names[] = { "Sam", "Tony", "Armin", "Masiwei", "Dingzheng",  "Melo", "Drake", "Quavo", "Hardwell", "Susan", "Monika", "Sophia", "Harish", "Zenia", "Mela", "Cameron", "Mira", "Anita" };
	string lastNames[] = { "Shepard", "Bob", "Bobson", "Petersan", "Chang", "Chun", "Chin", "Zhang", "Lee", "Lu", "Liu", "Davan", "Bob", "Peter", "TheGreat", "Ho" };


	int lengthFirstName = sizeof(Names) / sizeof(Names[0]);
	int lengthLastNmae = sizeof(lastNames) / sizeof(lastNames[0]);

	string Name = Names[rand() % lengthFirstName];
	string lastName = lastNames[rand() % lengthLastNmae];

	string name = Name + " " + lastName;

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
	return (rand() %  (GAS_GRADE_NUM));
}

int Customer::createRandomPumpNum() 
{
	return (rand() % (GAS_PUMP_NUM));
}

double Customer::createRandomVolume() 
{
	return rand() % 61 + 10;
}