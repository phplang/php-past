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
/* $Id: mod_php.h,v 1.11 1997/06/16 14:13:43 rasmus Exp $ */

typedef struct {
	int ShowInfo;
	int Logging;
	char *UploadTmpDir;
	char *dbmLogDir;
	char *SQLLogDB;
	char *SQLLogHost;
	char *AccessDir;
	char *IncludePath;
	int XBitHack;        
	int MaxDataSpace;
	int Debug;
	int engine;
} php_module_conf;
