/*
   This file is part of the KDE libraries
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// helper functions

#include "win32_utils.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

//---------------------------------------------
#define _fcopy_BUFLEN 1024*32
int fcopy(const char *src, const char *dest)
{
	static char _fcopy_buf[_fcopy_BUFLEN]; //not reentrant!
	FILE *in, *out;
	int c_in=0, c_out=0;
	int res=0;
	
	in=fopen(src, "rb");
	if (!in)
		return fcopy_src_err;
	out=fopen(dest, "wb");
	if (!out)
		return fcopy_dest_err;
	while (!feof(in) && !ferror(in) && !ferror(out)) {
		c_in=fread(_fcopy_buf, 1, _fcopy_BUFLEN, in);
		if (ferror(in) || c_in==0) {
			break;
		}
		c_out=fwrite(_fcopy_buf, 1, c_in, out);
		if (ferror(out) || c_in!=c_out) {
			break;
		}
	}
	
	if (ferror(in)) {
		res=fcopy_src_err;
	}
	else if (ferror(out)) {
		res=fcopy_dest_err;
	}
	else if (c_in!=c_out) {
		res=fcopy_dest_err;
	}
	fclose(in);
	fclose(out);
	return res;
}

KDEWIN32_EXPORT 
void win32_slashify(char *path, int maxlen)
{
	int len = 0;
	if (!path)
		return;
	for (; *path && len < maxlen ; path++)
		if ( *path == '\\' )
			*path = '/';
}

