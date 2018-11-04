#include "..\rt.h"
#include "GasTank.h"

GasTank::GasTank()
{
	for (int i = 0; i < GAS_NUM; i++)
	{
		tank_mutex[i] = new CMutex("Mutex_FuelTank" + to_string(i));
		tank_dps[i] = new CDataPool("Datapool_GasTank" + to_string(i), sizeof(struct GasTankInfo));
		tank_data[i] = (struct GasTankInfo *)(tank_dps[i]->LinkDataPool());
	}
}

void GasTank::Initialize()
{
	const double gas_price[] = { 0.98, 1.02, 1.10, 1.25 };

	for (int i = 0; i < GAS_NUM; i++)
	{
		tank_mutex[i]->Wait();
		tank_data[i]->tank_level = MAX_LEVEL;
		tank_data[i]->tank_status = OK_STATUS_COLOR;
		tank_data[i]->gas_cost = gas_price[i];
		tank_mutex[i]->Signal();
	}
}

double GasTank::WithdrawGas(double amount, int gasType)
{
	double gas = 0;

	tank_mutex[gasType]->Wait();

	// If there's enough gas, withdraw
	if (tank_data[gasType]->tank_level >= amount)
	{
		tank_data[gasType]->tank_level -= amount;
		gas = amount;
	}

	// If gas drops below level, update status
	if (tank_data[gasType]->tank_level <= LOW_LEVEL)
	{
		tank_data[gasType]->tank_status = LOW_STATUS_COLOR;
	}

	if (tank_data[gasType]->tank_level == 0)
	{
		tank_data[gasType]->tank_status = EMPTY_STATUS_COLOR;
	}

	tank_mutex[gasType]->Signal();

	return gas;
}

void GasTank::RefillTanks()
{
	for (int i = 0; i < GAS_NUM; i++)
	{
		tank_mutex[i]->Wait();
		tank_data[i]->tank_status = REFILLING_STATUS_COLOR;
		tank_mutex[i]->Signal();
	}

	SLEEP(1000);

	for (int i = 0; i < GAS_NUM; i++)
	{
		tank_mutex[i]->Wait();
		tank_data[i]->tank_level = MAX_LEVEL;
		tank_data[i]->tank_status = OK_STATUS_COLOR;
		tank_mutex[i]->Signal();
	}
}

double GasTank::GetGas(int gasType) const
{
	return tank_data[gasType]->tank_level;
}

string GasTank::GetStatus(int gasType) const
{
	switch (GetStatusNumber(gasType))
	{
	case OK_STATUS_COLOR: return "OK";
	case REFILLING_STATUS_COLOR: return "Refilling";
	case LOW_STATUS_COLOR: return "Low";
	case EMPTY_STATUS_COLOR: return "Empty";
	default: return "Error";
	}
}

int GasTank::GetStatusNumber(int gasType) const
{
	return tank_data[gasType]->tank_status;
}

int GasTank::GetOctaneGrade(int gasType) const
{
	return INT_OctaneGrade[gasType];
}

void GasTank::setGasCost(int gasType, double gasCost)
{
	tank_mutex[gasType]->Wait();
	tank_data[gasType]->gas_cost = gasCost;
	tank_mutex[gasType]->Signal();
}

double GasTank::GetGasCost(int gasType) const
{
	return tank_data[gasType]->gas_cost;
}