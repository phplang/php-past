/***[date.c]******************************************************[TAB=4]****\
*                                                                            *
* PHP/FI                                                                     *
*                                                                            *
* Copyright 1995,1996,1997 Rasmus Lerdorf                                    *
*                                                                            *
*  This program is free software; you can redistribute it and/or modify      *
*  it under the terms of the GNU General Public License as published by      *
*  the Free Software Foundation; either version 2 of the License, or         *
*  (at your option) any later version.                                       *
*                                                                            *
*  This program is distributed in the hope that it will be useful,           *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU General Public License for more details.                              *
*                                                                            *
*  You should have received a copy of the GNU General Public License         *
*  along with this program; if not, write to the Free Software               *
*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
*                                                                            *
\****************************************************************************/
/* $Id: date.c,v 1.25 1998/01/09 18:55:40 rasmus Exp $ */
#include <stdlib.h>
#ifdef TM_IN_SYS_TIME
#include <sys/time.h>
#else
#include <time.h>
#endif
#include <string.h>
#include "php.h"
#include "parse.h"

static char *Months[] = {
	"Jan","Feb","Mar","Apr","May","Jun","Jul",
	"Aug","Sep","Oct","Nov","Dec"
};

static char *FullMonths[] = {
	"January", "February", "March", "April", "May", "June", "July",
	"August", "September", "October", "November", "December"
};

static char *Days[] = {
	"Sun","Mon","Tue","Wed","Thu","Fri","Sat"
};

static char *FullDays[] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

static int  phpday_tab[2][12] = {
	{31,28,31,30,31,30,31,31,30,31,30,31},
	{31,29,31,30,31,30,31,31,30,31,30,31}  };

/*
 * Date
 * 
 * Function prints out the date and/or time based on the passed
 * format argument.  If type is 0, the local time is used, and if
 * type is 1, Greenwich Mean time is used.
 *
 * Any character in the format string not part of the following
 * will be output without modification:
 *
 *   year     month      day      hour   minute  second
 * Y - 1995  M - Sept  D - Sun   H - 22  i - 05  s - 08
 * y - 95    m - 09    d - 10    h - 10  
 *                     z - (day of year 0-365) 
 *                     U - seconds since Unix Epoch (unix time format)
 */
void Date(int arg, int type) {
	Stack *st;
	struct tm *tm1;
	char *s;
	char *out;
	char temp[32];
	time_t t;
	char *format;
	int y,h;

	t = time(NULL);
	if(arg) {
		st = Pop();
		if(!st) {
			Error("Stack error in %s expression",type?"gmdate":"date");
			return;
		}
		t = st->intval;
	}			
	st = Pop();
	if(!st) {
		Error("Stack error in %s expression",type?"gmdate":"date");
		return;
	}
	if(st->strval) format = (char *)st->strval;
	else {
		Error("No format string specified");
		return;
	}
	out = emalloc(1,sizeof(char) * 10 * (strlen(format)+1));
	*out = '\0';

	if(type==0) tm1 = localtime(&t);
	else tm1 = gmtime(&t);

	s = format;
	while(*s) {
		switch(*s) {
			case 'y':
				y = (tm1->tm_year)%100;
				sprintf(temp,"%02d",y);
				strcat(out,temp);
				break;
			case 'z':
				sprintf(temp,"%03d",tm1->tm_yday);
				strcat(out,temp);
				break;
			case 'U':
				sprintf(temp,"%ld",(long)t);
				strcat(out,temp);
				break;
			case 'Y':
				sprintf(temp,"%d",1900+tm1->tm_year);
				strcat(out,temp);
				break;
			case 'M':
				strcat(out,Months[tm1->tm_mon]);
				break;	
			case 'F':
				strcat(out,FullMonths[tm1->tm_mon]);
				break;	
			case 'm':
				sprintf(temp,"%02d",tm1->tm_mon+1);
				strcat(out,temp);
				break;
			case 'D':
				strcat(out,Days[tm1->tm_wday]);
				break;
			case 'l':
				strcat(out,FullDays[tm1->tm_wday]);
				break;
			case 'd':
				sprintf(temp,"%02d",tm1->tm_mday);
				strcat(out,temp);
				break;
			case 'H':
				sprintf(temp,"%02d",tm1->tm_hour);
				strcat(out,temp);
				break;
			case 'h':
				h = tm1->tm_hour%12;
				if(h==0) h=12;
				sprintf(temp,"%02d",h);
				if(temp[0]=='0') temp[0]=' ';
				strcat(out,temp);
				break;
			case 'i':
				sprintf(temp,"%02d",tm1->tm_min);
				strcat(out,temp);
				break;
			case 's':
				sprintf(temp,"%02d",tm1->tm_sec);
				strcat(out,temp);
				break;
			case 'a':
				if(tm1->tm_hour > 11) strcat(out, "pm");
				else strcat(out,"am");
				break;
			case 'A':
				if(tm1->tm_hour > 11) strcat(out, "PM");
				else strcat(out,"AM");
				break;
			case '\\':
				s++;
				if (*s == '\\') strcat(out,"\\");
				else {
					sprintf(temp,"%c",*s);
					strcat(out,temp);
				}
				break;
			default:
				sprintf(temp,"%c",*s);
				strcat(out,temp);
				break;
		}
		s++;
	}
	y = CheckType(out);
	Push((char *)out,(y==LNUMBER)?LNUMBER:STRING); /* no floating point here */
}

void UnixTime(void) {
	char temp[32];

	sprintf(temp,"%ld",(long)time(NULL));
	Push((char *)temp,LNUMBER);
}

/* arguments: hour minute second month day year */
void MkTime(int args) {
	struct tm tm1;
	struct tm *tm2;
	time_t t;
	int j;
	Stack *s;
	char temp[32];
	int vals[10];

	s = Pop();
	j=0;
	while(args) {
		vals[j++] = s->intval;
		s=Pop();
		args--;
	}	
	t = time(NULL);
	tm2 = localtime(&t);	
	memcpy(&tm1, tm2, sizeof(struct tm));
	tm1.tm_isdst = -1;
	switch(j) {
	case 6:	
		tm1.tm_hour = vals[5];
		tm1.tm_min = vals[4];
		tm1.tm_sec = vals[3];
		tm1.tm_mon = vals[2]-1;
		tm1.tm_mday = vals[1];
		if(vals[0] > 1000) vals[0]-=1900;
		tm1.tm_year = vals[0];
		break;
	case 5:
		tm1.tm_hour = vals[4];
		tm1.tm_min = vals[3];
		tm1.tm_sec = vals[2];
		tm1.tm_mon = vals[1]-1;
		tm1.tm_mday = vals[0];
		break;
	case 4:
		tm1.tm_hour = vals[3];
		tm1.tm_min = vals[2];
		tm1.tm_sec = vals[1];
		tm1.tm_mon = vals[0]-1;
		break;
	case 3:
		tm1.tm_hour = vals[2];
		tm1.tm_min = vals[1];
		tm1.tm_sec = vals[0];
		break;
	case 2:
		tm1.tm_hour = vals[1];
		tm1.tm_min = vals[0];
		break;
	case 1:
		tm1.tm_hour = vals[0];
		break;
	case 0:
		break;
	}
	if(tm1.tm_hour > 24 || tm1.tm_hour < 0) {
		Error("Hour argument to mktime is invalid");
	}
	if(tm1.tm_min > 60 || tm1.tm_min < 0) {
		Error("Minute argument to mktime is invalid");
	}
	if(tm1.tm_sec > 61 || tm1.tm_sec < 0) {
		Error("Second argument to mktime is invalid");
	}
	if(tm1.tm_mon > 11 || tm1.tm_mon < 0) {
		Error("Month argument to mktime is invalid");
	}
	if(tm1.tm_mday > 31 || tm1.tm_mday < 0) {
		Error("Day of month argument to mktime is invalid");
	}
	if(tm1.tm_year && (tm1.tm_year > 138 || tm1.tm_year < 70)) {
		Error("Calendar times before 00:00:00 UTC, January 1, 1970 or after 03:14:07 UTS, January 19, 2038 cannot be represented by mktime");
	}
	t = mktime(&tm1);
	sprintf(temp,"%ld\n",t);
	Push((char *)temp,LNUMBER);
}

char *std_date(time_t t) {
	struct tm *tm1;
	static char str[80];
	
	tm1 = gmtime(&t);
	sprintf(str,"%s, %02d-%s-%02d %02d:%02d:%02d GMT",
		FullDays[tm1->tm_wday],
		tm1->tm_mday,
		Months[tm1->tm_mon],
		((tm1->tm_year)%100),
		tm1->tm_hour, tm1->tm_min, tm1->tm_sec);
	return(str);
}

/* 
 * CheckDate(month, day, year);
 *  returns True(1) if it is valid date style
 *
 */
#define isleap(year) (((year%4) == 0 && (year%100)!=0) || (year%400)==0)
void CheckDate(void) {
 	Stack *s;
 	int m, d, y;
 	s = Pop();
 	if(!s) {
 		Error("Stack error in CheckDate");
 		return;
 	}
 	y = s->intval;
 	if(y<100) y += 1900;
 
 	s = Pop();
 	if(!s) {
 		Error("Stack error in CheckDate");
 	return;
 	}
 	d = s->intval;
 
 	s = Pop();
 	if(!s) {
 		Error("Stack error in CheckDate");
 		return;
 	}
	m = s->intval;
 
 	if(y<0 || y>32767) {
 		Push("0", LNUMBER); /* False */
 		return;
 	}
 	if(m<1 || m>12) {
 		Push("0", LNUMBER); /* False */
 		return;
 	}
 	if(d<1 || d>phpday_tab[isleap(y)][m-1]) {
 		Push("0", LNUMBER); /* False */
 		return;
 	}
 	Push("1", LNUMBER); /* True : This month,day,year arguments are valid */
	return;
}
