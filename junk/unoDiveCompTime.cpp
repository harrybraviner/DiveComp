#include "unoDiveCompTime.h"

uint32_t sysTime = 0;	// Time elapsed in seconds since 1970, updated by now() calls
uint32_t prevMillis = 0;
int isTimeSet = 0;	// 1 if times has been set, zero otherwise

static const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0

void setTime(uint8_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second){
	int i;
	uint32_t seconds;

	// seconds from 1970 till 1 jan 00:00:00 of the given year
	seconds= year*(SECS_PER_DAY * 365);
	for (i = 0; i < year; i++) {
		if (LEAP_YEAR(i)) {
			seconds +=  SECS_PER_DAY;   // add extra days for leap years
		}
	}
  
	// add days for this year, months start from 1
	for (i = 1; i < month; i++) {
		if ( (i == 2) && LEAP_YEAR(year)) { 
			seconds += SECS_PER_DAY * 29;
		} else {
			seconds += SECS_PER_DAY * monthDays[i-1];  //monthDay array starts from 0
		}
	}
	seconds+= (date-1) * SECS_PER_DAY;
	seconds+= hour * SECS_PER_HOUR;
	seconds+= minute * SECS_PER_MIN;
	seconds+= second;
	
	sysTime = seconds;
	isTimeSet = 1;
}

uint32_t now_s(){
	// Return the current time in seconds since 1970
	while(millis() - prevMillis >= 1000){
		sysTime++;
		prevMillis += 1000;
	}

	// Restart counting from now
	prevMillis = millis();

	return sysTime;
}
