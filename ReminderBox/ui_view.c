#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

#include "menu.h"
#include "screen.h"
#include "led.h"
#include "clock.h"
#include "real_time_clock.h"
#include "strings.h"
#include "bill.h"

#include "ui_view.h"


volatile menu_system_t *current_menu;

#define BUF_LEN 25
static char last_displayed_str[BUF_LEN];
static uint16_t last_displayed_num = 65535;
volatile static bool *sleep;



/*
 * Static functions
 */

static void update_last_string(const char *str);
static void display_menu_item(const char *str);
static void display_time(void);
static void display_enter_bill_sub_menu(void);
static void display_number(const char *str, uint16_t i);
static void display_coming_due_bills(void);
static bool concatenate_month(month_e month, char *buf);
static bool concatenate_day(uint8_t day, char *buf);
static bool concatenate_person(person_e person, char *buf);
static bool concatenate_company(company_e company, char *buf);

void ui_view_init(volatile bool *sleep_ptr, volatile menu_system_t *menu_ptr)
{
	/*
	 * Initialize LCD module
	 */
    lcd_pin_init();
    lcd_start();
    lcd_light_on();
    lcd_clear();

    /*
     * Initialize LEDs
     */
    led_init();
    led_off();

    current_menu = menu_ptr;
    sleep = sleep_ptr;
}

void ui_view_display(void)
{
    while (!(*sleep))
    {
    	switch (current_menu->current_choice)
    	{
    	case DISPLAY_TIME:
    		display_menu_item("CURRENT TIME");
    		break;
    	case CHECK_DUE_DATES:
    		display_menu_item("CHECK COMING DUE");
    		break;
    	case PAY_BILL:
    		display_menu_item("PAY BILL");
    		break;
    	case ENTER_NEW_DATE:
    		display_menu_item("ENTER NEW BILL");
    		break;
    	case SET_TIME:
    		display_menu_item("SET CURRENT TIME");
    		break;
    	case CHOICE_DISPLAY_TIME:
    		display_time();
    		break;
    	case CHOICE_DUE_MENU:
    		display_coming_due_bills();
    		break;
    	case CHOICE_PAY_MENU:
    		display_menu_item("due items will show up here");
    		break;
    	case CHOICE_ENTER_BILL:
    		display_enter_bill_sub_menu();
    		break;
    	case CHOICE_SET_TIME:
    		display_menu_item("will ask for time info here");
    		break;
    	default:
    		display_menu_item("Update view switch");
    		break;
    	}
    }
}
static void display_menu_item(const char *str)
{
	last_displayed_num = 65535;

	if (!strings_compare(str, last_displayed_str))
	{
		lcd_clear();
		lcd_write_str(str);
		update_last_string(str);
	}
}

static void display_number(const char *str, uint16_t i)
{
	if (!strings_compare(str, last_displayed_str))
	{
		lcd_clear();
		lcd_write_str(str);
		update_last_string(str);
		last_displayed_num = 65535;
	}

	if (i != last_displayed_num)
	{
		lcd_goto(0, 1);
		lcd_write_int(i);
		last_displayed_num = i;
	}
}

static void display_time(void)
{
	//We are displaying time now, so the last string should be reset
	update_last_string("");
	last_displayed_num = 65535;

	static uint32_t seconds_last = 0;
	uint32_t seconds_since_turn_on = clock_get_seconds();

	uint8_t time_array[7];
	if (seconds_last != seconds_since_turn_on)
	{
		rtc_get_time(time_array);
		lcd_write_time(time_array[0], time_array[1], time_array[2], time_array[3], time_array[4], time_array[5], time_array[6]);
	}

	seconds_last = seconds_since_turn_on;
}

static void update_last_string(const char *str)
{
	unsigned int i = 0;
	for (i = 0; i < BUF_LEN; i++)
		last_displayed_str[i] = '\0';

	unsigned int j = 0;
	while (*str != '\0')
		last_displayed_str[j++] = *str++;
}

static void display_coming_due_bills(void)
{
	bill_t to_display = current_menu->selected_bill;

	person_e person = to_display.person;
	company_e company = to_display.company;
	month_e month = to_display.date.month;
	uint8_t day = to_display.date.day;

	if (day == 0)
	{
		display_menu_item("Scroll to view upcoming.");
	}
	else if (person == 0xFFFF)
	{
		display_menu_item(last_displayed_str);
	}
	else
	{
		char str[32];

		strings_concatenate("", "", str);
		concatenate_month(month, str);
		concatenate_day(day, str);
		concatenate_person(person, str);
		concatenate_company(company, str);

		display_menu_item(str);
	}
}

static bool concatenate_month(month_e month, char *buf)
{
	switch (month)
	{
	case JANUARY_M:
		strings_concatenate(buf, "JAN", buf);
		break;
	case FEBRUARY_M:
		strings_concatenate(buf, "FEB", buf);
		break;
	case MARCH_M:
		strings_concatenate(buf, "MAR", buf);
		break;
	case APRIL_M:
		strings_concatenate(buf, "APR", buf);
		break;
	case MAY_M:
		strings_concatenate(buf, "MAY", buf);
		break;
	case JUNE_M:
		strings_concatenate(buf, "JUN", buf);
		break;
	case JULY_M:
		strings_concatenate(buf, "JUL", buf);
		break;
	case AUGUST_M:
		strings_concatenate(buf, "AUG", buf);
		break;
	case SEPTEMBER_M:
		strings_concatenate(buf, "SEP", buf);
		break;
	case OCTOBER_M:
		strings_concatenate(buf, "OCT", buf);
		break;
	case NOVEMBER_M:
		strings_concatenate(buf, "NOV", buf);
		break;
	case DECEMBER_M:
		strings_concatenate(buf, "DEC", buf);
		break;
	}

	strings_concatenate(buf, " ", buf);

	return true;
}

static bool concatenate_day(uint8_t day, char *buf)
{
	char day_as_str[3];
	strings_itoa(day, day_as_str);
	strings_concatenate(buf, day_as_str, buf);
	strings_concatenate(buf, " ", buf);

	return true;
}

static bool concatenate_person(person_e person, char *buf)
{
	switch (person)
	{
	case MAX_P:
		strings_concatenate(buf, "MAX", buf);
		break;
	case MISH_P:
		strings_concatenate(buf, "MISH", buf);
		break;
	}

	strings_concatenate(buf, " ", buf);

	return true;
}

static bool concatenate_company(company_e company, char *buf)
{
	switch (company)
	{
	case CHASE_FREEDOM_C:
		strings_concatenate(buf, "CHSE_FRE", buf);
		break;
	case CHASE_SAPPHIRE_C:
		strings_concatenate(buf, "CHSE_SAP", buf);
		break;
	case CAPITAL_ONE_C:
		strings_concatenate(buf, "CAP_ONE", buf);
		break;
	case DISCOVER_C:
		strings_concatenate(buf, "DISC", buf);
		break;
	case AMEX_C:
		strings_concatenate(buf, "AMEX", buf);
		break;
	case CITI_C:
		strings_concatenate(buf, "CITI", buf);
		break;
	case ELECTRICITY_C:
		strings_concatenate(buf, "ELEC", buf);
		break;
	case WATER_C:
		strings_concatenate(buf, "WATER", buf);
		break;
	case COMCAST_C:
		strings_concatenate(buf, "COMCAST", buf);
		break;
	case MEDICAL_C:
		strings_concatenate(buf, "MEDICAL", buf);
		break;
	case MISC_C:
		strings_concatenate(buf, "OTHER", buf);
		break;
	}

	return true;
}

static void display_enter_bill_sub_menu(void)
{
	switch (current_menu->current_sub_menu_choice)
	{
	case SUB_CHOICE_MAX:
		display_menu_item("Bill is for:    MAX");
		break;
	case SUB_CHOICE_MISH:
		display_menu_item("Bill is for:    MISH");
		break;
	case CHASE_FREEDOM:
		display_menu_item("To pay: CHASE FREEDOM");
		break;
	case CHASE_SAPPHIRE:
		display_menu_item("To pay: CHASE SAPPHIRE");
		break;
	case CAPITAL_ONE:
		display_menu_item("To pay: CAPITAL ONE");
		break;
	case DISCOVER:
		display_menu_item("To pay: DISCOVER");
		break;
	case AMEX:
		display_menu_item("To pay: AMEX");
		break;
	case CITI:
		display_menu_item("To pay: CITI");
		break;
	case ELECTRICITY:
		display_menu_item("To pay: ELECTRICITY");
		break;
	case WATER:
		display_menu_item("To pay: WATER/SEWAGE/GARBAGE");
		break;
	case COMCAST:
		display_menu_item("To pay: COMCAST");
		break;
	case MEDICAL:
		display_menu_item("To pay: MEDICAL");
		break;
	case MISC:
		display_menu_item("To pay: OTHER");
		break;
	case JANUARY:
		display_menu_item("Due: JANUARY");
		break;
	case FEBRUARY:
		display_menu_item("Due: FEBRUARY");
		break;
	case MARCH:
		display_menu_item("Due: MARCH");
		break;
	case APRIL:
		display_menu_item("Due: APRIL");
		break;
	case MAY:
		display_menu_item("Due: MAY");
		break;
	case JUNE:
		display_menu_item("Due: JUNE");
		break;
	case JULY:
		display_menu_item("Due: JULY");
		break;
	case AUGUST:
		display_menu_item("Due: AUGUST");
		break;
	case SEPTEMBER:
		display_menu_item("Due: SEPTEMBER");
		break;
	case OCTOBER:
		display_menu_item("Due: OCTOBER");
		break;
	case NOVEMBER:
		display_menu_item("Due: NOVEMBER");
		break;
	case DECEMBER:
		display_menu_item("Due: DECEMBER");
		break;
	case CONFIRM_ENTER_BILL:
		display_menu_item("Confirm?");
		break;
	case WAIT_E:
		display_menu_item("Saving to memory...");
		break;
	default:
		//The default case is for when the sub choice is 1-31, in which case, it means that is the date
		display_number("Date: ", current_menu->current_sub_menu_choice);
		break;
	}
}



