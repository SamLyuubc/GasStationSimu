//Name:Yujia Lyu Suheng He	
//Student Number:55054134 51513133
#include "../GasStationSimu/rt.h"

int main() 
{
	CProcess p_costumers("C:\\Users\\hesuh\\source\\repos\\GasStationSimu\\Debug\\Customers.exe",  // pathlist to child program executable				
		NORMAL_PRIORITY_CLASS,			// priority
		OWN_WINDOW,						// process has its own window					
		ACTIVE							// process is active immediately
	);

	CProcess p_pumps("C:\\Users\\hesuh\\source\\repos\\GasStationSimu\\Debug\\Pump.exe",	// pathlist to child program executable				
		NORMAL_PRIORITY_CLASS,			// priority
		OWN_WINDOW,						// process has its own window					
		ACTIVE							// process is active immediately
	);

	CProcess p_collects("C:\\Users\\hesuh\\source\\repos\\GasStationSimu\\Debug\\CenterControl.exe",	// pathlist to child program executable				
		NORMAL_PRIORITY_CLASS,			// priority
		OWN_WINDOW,						// process has its own window					
		ACTIVE							// process is active immediately
	);

	getchar();

	p_pumps.WaitForProcess();
	p_costumers.WaitForProcess();
	p_collects.WaitForProcess();

}