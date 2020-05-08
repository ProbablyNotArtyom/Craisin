
	#include <stddef.h>
	#include <stdio.h>
	#include <string>
	#include <../include/cpu.hpp>
	#include <cpu.hpp>

	using namespace std;

//-----------------------------------------------------------------------------

const string cpuModels_tbl[NUM_CPU_MODELS] {
	"INVALID",
	"6502",
	"6502_ILLEGAL",
	"65C02",
	"65C02_WDC",
	"65816"
};

//-----------------------------------------------------------------------------

cpu_model_t cpu_get_model(string model_name) {
	for (int i = 0; i < NUM_CPU_MODELS; i++)
		if (cpuModels_tbl[i] == model_name) return cpu_model_t(i);
	return CPU_INVALID;
}

string cpu_get_model_string(cpu_model_t model) {
	if (model > NUM_CPU_MODELS) return cpuModels_tbl[CPU_INVALID];
	return cpuModels_tbl[model];
}
