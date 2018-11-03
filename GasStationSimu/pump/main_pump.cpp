//Name:Yujia Lyu Suheng He	
//Student Number:55054134 51513133
#include "Pump.h"
#include "../rt.h"

int main() 
{
	Pump pump_1(1);
	Pump pump_2(2);
	Pump pump_3(3);
	Pump pump_4(4);

	pump_1.Resume();
	pump_2.Resume();
	pump_3.Resume();
	pump_4.Resume();

	pump_1.WaitForThread();
	pump_2.WaitForThread();
	pump_3.WaitForThread();
	pump_4.WaitForThread();
}