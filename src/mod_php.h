/***[mod_php.h]***************************************************[TAB=4]****\
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
/* $Id: mod_php.h,v 1.15 1997/09/13 16:14:29 shane Exp $ */

typedef struct {
	int ShowInfo;
	int Logging;
	char *UploadTmpDir;
	char *dbmLogDir;
	char *SQLLogDB;
	char *SQLLogHost;
	char *AccessDir;
	char *IncludePath;
	char *AutoPrependFile;
	char *AutoAppendFile;
	int XBitHack;        
	int MaxDataSpace;
	int Debug;
	int engine;
	int LastModified;
	char *AdaUser;
	char *AdaPW;
	char *AdaDB;
} php_module_conf;

#if WINNT|WIN32
#define S_IXUSR _S_IEXEC
#endif
