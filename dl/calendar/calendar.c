/* interface functions.

 * code by Shane Caraveo shane@caraveo.com
 * copy freely!
 *
 */

#include "sdncal.h"

/* 
 * this array should be set up as:
 * {"PHPScriptFunctionName",dllFunctionName,1} 
 */

function_entry calendar_functions[] =
{
	{"jdtogregorian", cal_jdtogreg},
	{"gregoriantojd", cal_gregtojd},
	{"jdtojulian", cal_jdtojul},
	{"juliantojd", cal_jultojd},
	{"jdtojewish", cal_jdtojewish},
	{"jewishtojd", cal_jewishtojd},
	{"jdtofrench", cal_jdtofrench},
	{"frenchtojd", cal_frenchtojd},
	{"jddayofweek", cal_jddayofweek},
	{"jdmonthname", cal_monthname},
	{NULL, NULL}
};


php3_module_entry calendar_module_entry = {
	"Calendar", calendar_functions, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, NULL
};


#if COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &calendar_module_entry; }
#endif

DLEXPORT void cal_jdtogreg(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *julday;
	int year, month, day;
	char date[10];

	if (getParameters(ht, 1, &julday) == SUCCESS) {
		convert_to_long(julday);
		SdnToGregorian(julday->value.lval, &year, &month, &day);
		sprintf(date, "%i/%i/%i", month, day, year);

		return_value->value.str.val = estrdup(date);
		return_value->value.str.len = strlen(date);
		return_value->type = IS_STRING;
	} else {
		WRONG_PARAM_COUNT;
	}
	return;
}


DLEXPORT void cal_gregtojd(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *year, *month, *day;
	int jdate;

	if (getParameters(ht, 3, &month, &day, &year) == SUCCESS) {
		convert_to_long(month);
		convert_to_long(day);
		convert_to_long(year);

		jdate = GregorianToSdn(year->value.lval, month->value.lval, day->value.lval);

		return_value->value.lval = jdate;
		return_value->type = IS_LONG;
	} else {
		WRONG_PARAM_COUNT;
	}
}


DLEXPORT void cal_jdtojul(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *julday;
	int year, month, day;
	char date[10];

	if (getParameters(ht, 1, &julday) == SUCCESS) {
		convert_to_long(julday);
		SdnToJulian(julday->value.lval, &year, &month, &day);
		sprintf(date, "%i/%i/%i", month, day, year);

		return_value->value.str.val = estrdup(date);
		return_value->value.str.len = strlen(date);
		return_value->type = IS_STRING;
	} else {
		WRONG_PARAM_COUNT;
	}
	return;
}


DLEXPORT void cal_jultojd(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *year, *month, *day;
	int jdate;

	if (getParameters(ht, 3, &month, &day, &year) == SUCCESS) {
		convert_to_long(month);
		convert_to_long(day);
		convert_to_long(year);

		jdate = JulianToSdn(year->value.lval, month->value.lval, day->value.lval);

		return_value->value.lval = jdate;
		return_value->type = IS_LONG;
	} else {
		WRONG_PARAM_COUNT;
	}
}


DLEXPORT void cal_jdtojewish(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *julday;
	int year, month, day;
	char date[10];

	if (getParameters(ht, 1, &julday) == SUCCESS) {
		convert_to_long(julday);
		SdnToJewish(julday->value.lval, &year, &month, &day);
		sprintf(date, "%i/%i/%i", month, day, year);

		return_value->value.str.val = estrdup(date);
		return_value->value.str.len = strlen(date);
		return_value->type = IS_STRING;
	} else {
		WRONG_PARAM_COUNT;
	}
	return;
}


DLEXPORT void cal_jewishtojd(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *year, *month, *day;
	int jdate;

	if (getParameters(ht, 3, &month, &day, &year) == SUCCESS) {
		convert_to_long(month);
		convert_to_long(day);
		convert_to_long(year);

		jdate = JewishToSdn(year->value.lval, month->value.lval, day->value.lval);

		return_value->value.lval = jdate;
		return_value->type = IS_LONG;
	} else {
		WRONG_PARAM_COUNT;
	}
}


DLEXPORT void cal_jdtofrench(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *julday;
	int year, month, day;
	char date[10];

	if (getParameters(ht, 1, &julday) == SUCCESS) {
		convert_to_long(julday);
		SdnToFrench(julday->value.lval, &year, &month, &day);
		sprintf(date, "%i/%i/%i", month, day, year);

		return_value->value.str.val = estrdup(date);
		return_value->value.str.len = strlen(date);
		return_value->type = IS_STRING;
	} else {
		WRONG_PARAM_COUNT;
	}
	return;
}


DLEXPORT void cal_frenchtojd(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *year, *month, *day;
	int jdate;

	if (getParameters(ht, 3, &month, &day, &year) == SUCCESS) {
		convert_to_long(month);
		convert_to_long(day);
		convert_to_long(year);

		jdate = FrenchToSdn(year->value.lval, month->value.lval, day->value.lval);

		return_value->value.lval = jdate;
		return_value->type = IS_LONG;
	} else {
		WRONG_PARAM_COUNT;
	}
}


DLEXPORT void cal_jddayofweek(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *julday, *mode;
	int day;
	char *daynamel, *daynames;

	if (getParameters(ht, 2, &julday, &mode) == SUCCESS) {
		convert_to_long(julday);
		convert_to_long(mode);

		day = DayOfWeek(julday->value.lval);
		daynamel = DayNameLong[day];
		daynames = DayNameShort[day];

		switch (mode->value.lval) {
			case 0L:
				return_value->value.lval = day;
				return_value->type = IS_LONG;
				break;
			case 1L:
				return_value->value.str.val = estrdup(daynamel);
				return_value->value.str.len = strlen(daynamel);
				return_value->type = IS_STRING;
				break;
			case 2L:
				return_value->value.str.val = estrdup(daynames);
				return_value->value.str.len = strlen(daynames);
				return_value->type = IS_STRING;
				break;
			default:
				return_value->value.lval = day;
				return_value->type = IS_LONG;
				break;
		}
	} else {
		WRONG_PARAM_COUNT;
	}
	return;
}


DLEXPORT void cal_monthname(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *julday, *mode;
	char *monthname = NULL;
	int month, day, year;

	if (getParameters(ht, 2, &julday, &mode) == SUCCESS) {
		convert_to_long(julday);
		convert_to_long(mode);

		switch (mode->value.lval) {
			case 0L:			/* gregorian or julian month */
				SdnToGregorian(julday->value.lval, &year, &month, &day);
				monthname = MonthNameShort[month];
				break;
			case 1L:			/* gregorian or julian month */
				SdnToGregorian(julday->value.lval, &year, &month, &day);
				monthname = MonthNameLong[month];
				break;
			case 2L:			/* gregorian or julian month */
				SdnToJulian(julday->value.lval, &year, &month, &day);
				monthname = MonthNameShort[month];
				break;
			case 3L:			/* gregorian or julian month */
				SdnToJulian(julday->value.lval, &year, &month, &day);
				monthname = MonthNameLong[month];
				break;
			case 4L:			/* jewish month */
				SdnToJewish(julday->value.lval, &year, &month, &day);
				monthname = JewishMonthName[month];
				break;
			case 5L:			/* french month */
				SdnToFrench(julday->value.lval, &year, &month, &day);
				monthname = FrenchMonthName[month];
				break;
			default:			/* default gregorian */
				/* FIXME - need to set monthname to something here ?? */
				break;
		}
		return_value->value.str.val = estrdup(monthname);
		return_value->value.str.len = strlen(monthname);
		return_value->type = IS_STRING;
		return;
	} else {
		WRONG_PARAM_COUNT;
	}
	return;
}
