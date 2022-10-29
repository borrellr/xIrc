/***************************************************************************
**    xIrcMircServerParse.cpp  $Revision: 1.1.1.1 $ - $Name:  $ 
**    Parses and Build a list of Servers derived from a mIrc
**    servers.ini file
**
**    Copyright (C) 1997 1998 Joseph Croft <joe@croftj.net>
** Copyright (C) 1995, 1996  Joseph Croft <jcroft@unicomp.net>  
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 1, or (at your option)
** any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
 ***************************************************************************/
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <qt.h>
#include <xMessageBox.h>
#include "xIrcServerList.h"

static bool dbg = false;

bool xIrcMircServerParse(const char *fn, xIrcServerList &ServerList)
{
   char buf[1024];
   FILE *fp;

   if (dbg) fprintf(stdout, "xIrcMircServerParse():Enter\n");
   if (dbg) fflush(stdout);

   if ((fp = fopen(fn, "r")) == NULL)
   {
      sprintf(buf, "Error: Could not open Server File \"%s\":%s", 
                   fn, strerror(errno));
      xMessageBox("Error", buf);
      return(FALSE);
   }

   if (dbg) fprintf(stdout, "xIrcMircServerParse():Reading file:\n");
   if (dbg) fflush(stdout);
   while (fgets(buf, sizeof(buf), fp) != NULL)
   {
      if (dbg) fprintf(stdout, "xIrcMircServerParse():Ignored |%s|\n", buf);
      if (dbg) fflush(stdout);
      if (strstr(buf, "[Servers]") != NULL)
         break;
   }

   while (fgets(buf, sizeof(buf), fp) != NULL)
   {
      if (dbg) fprintf(stdout, "xIrcMircServerParse():Read |%s|\n", buf);
      if (dbg) fflush(stdout);
      if (strlen(buf) < 3)
         continue;
      if (*buf != ';')
      {
         xIrcServerEntry entry(buf);
         ServerList.add(entry);
      }
   }
   return(TRUE);
}
