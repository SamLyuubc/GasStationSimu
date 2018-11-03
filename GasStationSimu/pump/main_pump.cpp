//Name:Yujia Lyu Suheng He	
//Student Number:55054134 51513133
#include "Pump.h"
#include "../rt.h"

const int NUM_PUMPS = 4;
Pump *pumps[NUM_PUMPS];

int main() 
{
	for (int i = 0; i < NUM_PUMPS; i++)
	{
		pumps[i] = new Pump(i);
		pumps[i]->Resume();
	

	}

	for (int i = 0; i < NUM_PUMPS; i++)
	{
		pumps[i]->WaitForThread();

	}
}