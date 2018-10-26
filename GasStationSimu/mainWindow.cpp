//Name:Yujia Lyu Suheng He	 
//Student Number:55054134 51513133
#include "rt.h"
#include "PumpDataCollection.h"

int main()
{
	PumpDataCollection collection_1("Pump1", 0, 10);	// create 5 threads and pass them a thread number
	PumpDataCollection collection_2("Pump2", 40, 10);
	PumpDataCollection collection_3("Pump3", 0, 15);
	PumpDataCollection collection_4("Pump4", 40, 15);

	collection_1.Resume();
	collection_2.Resume();
	collection_3.Resume();
	collection_4.Resume();


	collection_1.WaitForThread();
	collection_2.WaitForThread();
	collection_3.WaitForThread();
	collection_4.WaitForThread();


	return 0;
}
