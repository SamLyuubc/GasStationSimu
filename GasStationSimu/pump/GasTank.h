#pragma once
#include "../rt.h"

class GasTank {
	struct GasTankInfo
	{
		double tank_level;
		int tank_status;
		double gas_cost;

	};

private:
	static const int GAS_NUM = 4;
	static const int MAX_LEVEL = 500;
	static const int LOW_LEVEL = 200;

	// Status constants
	static const int OK_STATUS_COLOR = 10;			// Green
	static const int REFILLING_STATUS_COLOR = 2;	// Dark Green
	static const int LOW_STATUS_COLOR = 6;			// Yellow
	static const int EMPTY_STATUS_COLOR = 12;		// Red


	const int INT_OctaneGrade[GAS_NUM] = { 87, 89, 92, 93 };

	CMutex *tank_mutex[GAS_NUM];
	CDataPool *tank_dps[GAS_NUM];
	struct GasTankInfo *tank_data[GAS_NUM];

public:
	GasTank();
	~GasTank() {}

	void Initialize();
	double WithdrawGas(double amount, int gasType);
	void RefillTanks();
	double GetGas(int gasType) const;
	int GetStatusNumber(int gasType) const;
	string GetStatus(int gasType) const;
	int GetOctaneGrade(int gasType) const;
	void setGasCost(int gasType, double gasCost);
	double GetGasCost(int gasType) const;
};