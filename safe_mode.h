/* $Id: safe_mode.h,v 1.7 1998/12/28 09:43:53 sas Exp $ */

#ifndef _SAFE_MODE_H_
#define _SAFE_MODE_H_

extern PHPAPI int _php3_checkuid(const char *filename, int mode);
extern PHPAPI char *_php3_get_current_user(void);

#endif
