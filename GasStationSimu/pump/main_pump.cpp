//Name:Yujia Lyu Suheng He	
//Student Number:55054134 51513133
#include "Pump.h"
#include "../rt.h"

int main() 
{
	Pump pump_1("Pump1");
	Pump pump_2("Pump2");
	Pump pump_3("Pump3");
	Pump pump_4("Pump4");

	pump_1.Resume();
	pump_2.Resume();
	pump_3.Resume();
	pump_4.Resume();

	pump_1.WaitForThread();
	pump_2.WaitForThread();
	pump_3.WaitForThread();
	pump_4.WaitForThread();
}