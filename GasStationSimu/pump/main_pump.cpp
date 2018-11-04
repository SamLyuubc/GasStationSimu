//Name:Yujia Lyu Suheng He	
//Student Number:55054134 51513133
#include "Pump.h"
#include "../rt.h"

const int NUM_PUMPS = 8;
Pump *pumps[NUM_PUMPS];

int main() 
{
	GasTank *gas_tank = new GasTank();

	for (int i = 0; i < NUM_PUMPS; i++)
	{
		pumps[i] = new Pump(i, gas_tank);
		pumps[i]->Resume();
	

	}

	for (int i = 0; i < NUM_PUMPS; i++)
	{
		pumps[i]->WaitForThread();

	}
}