/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <soc/mt6359p.h>
#include <soc/mt6360.h>
#include <soc/regulator.h>

static int get_mt6360_regulator_id(enum mtk_regulator regulator)
{
	switch (regulator) {
	case MTK_REGULATOR_VDD2:
		return MT6360_BUCK1;
	case MTK_REGULATOR_VDDQ:
		return MT6360_LDO7;
	case MTK_REGULATOR_VMDDR:
		return MT6360_LDO6;
	default:
		break;
	}

	return -1;
}

static int get_mt6359p_regulator_id(enum mtk_regulator regulator)
{
	switch (regulator) {
	case MTK_REGULATOR_VCORE:
		return MT6359P_GPU11;
	default:
		break;
	}

	return -1;
}

void mainboard_set_regulator_vol(enum mtk_regulator regulator,
				 uint32_t voltage_uv)
{
	/*
	 * Handle the regulator that does not have a regulator ID
	 * in its underlying implementation.
	 */
	if (regulator == MTK_REGULATOR_VDD1) {
		mt6359p_set_vm18_voltage(voltage_uv);
		return;
	}

	int id;

	id = get_mt6360_regulator_id(regulator);
	if (id >= 0) {
		uint32_t voltage_mv = voltage_uv / 1000;
		google_chromeec_regulator_set_voltage(id, voltage_mv, voltage_mv);
		return;
	}

	id = get_mt6359p_regulator_id(regulator);
	if (id >= 0) {
		mt6359p_buck_set_voltage(id, voltage_uv);
		return;
	}

	printk(BIOS_WARNING, "Invalid regulator ID: %d\n", regulator);
}

uint32_t mainboard_get_regulator_vol(enum mtk_regulator regulator)
{
	/*
	 * Handle the regulator that does not have a regulator ID
	 * in its underlying implementation.
	 */
	if (regulator == MTK_REGULATOR_VDD1)
		return mt6359p_get_vm18_voltage();

	int id;

	id = get_mt6360_regulator_id(regulator);
	if (id >= 0) {
		uint32_t voltage_mv = 0;
		google_chromeec_regulator_get_voltage(id, &voltage_mv);
		return voltage_mv * 1000;
	}

	id = get_mt6359p_regulator_id(regulator);
	if (id >= 0)
		return mt6359p_buck_get_voltage(id);

	printk(BIOS_WARNING, "Invalid regulator ID: %d\n", regulator);

	return 0;
}
