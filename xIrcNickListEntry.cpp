/***************************************************************************
**    xIrcNickListEntry.cpp  $Revision: 1.1.1.1 $ - $Name:  $ 
**    Class for handling Nick names
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
#include <stdio.h>
#include <ctype.h>
#include "xIrcNickListEntry.h"

static int dbg = 0;

xIrcNickListEntry::xIrcNickListEntry(const char *pName, xIrcNickListEntry *entry)
{
   nick = "";
   addr = "";
   if (pName)
      setNick(pName);
   pNext = pPrev = NULL;
   if (entry)
      link(entry);
}

xIrcNickListEntry::xIrcNickListEntry(xIrcNickListEntry *pNick, xIrcNickListEntry *entry = NULL)
{
   nick = "";
   addr = "";
   if (pNick)
   {
      setNick(pNick->getNick());
      setAddr(pNick->getAddr());
   }
   pNext = pPrev = NULL;
   if (entry)
      link(entry);
}

xIrcNickListEntry::xIrcNickListEntry()
{
   nick = "";
   addr = "";
   pNext = pPrev = NULL;
}

xIrcNickListEntry::~xIrcNickListEntry()
{
   if (dbg > 2) fprintf(stdout, "xIrcNickListEntry::xIrcNickListEntry():Enter\n");
   if (dbg > 2) fflush(stdout);
   unlink();
   if (dbg > 2) fprintf(stdout, "xIrcNickListEntry::xIrcNickListEntry():Exit\n");
   if (dbg > 2) fflush(stdout);
}

bool xIrcNickListEntry::is(const char *pName, bool byNick)
{
   QString tmpName, tmpNick;
   const char *cp, *cp1, *cp2;

   if (dbg > 2) fprintf(stdout, "xIrcNickListEntry::is():Enter\n");
   if (dbg > 2) fflush(stdout);
   for (tmpName = "", cp = pName; *cp && !isspace(*cp); cp++)
      tmpName += toupper(*cp);

   if (dbg > 2) fprintf(stdout, "xIrcNickListEntry::is():Checking if test name or address\n");
   if (dbg > 2) fflush(stdout);
   if (byNick)
   {
      if (dbg > 2) fprintf(stdout, "xIrcNickListEntry::is():Test by name\n");
      if (dbg > 2) fflush(stdout);
      for (tmpNick = "", cp = nick; *cp; cp++)
         tmpNick += toupper(*cp);
   }
   else
   {
      if (dbg > 2) fprintf(stdout, "xIrcNickListEntry::is():Test by addr\n");
      if (dbg > 2) fflush(stdout);
      for (tmpNick = "", cp = addr; *cp; cp++)
         tmpNick += toupper(*cp);
   }

   if (dbg > 2) fprintf(stdout, "xIrcNickListEntry::is():Testing |%s| against |%s|\n",
                              (const char *)tmpName, (const char *)tmpNick);
   if (dbg > 2) fflush(stdout);
   cp1 = (const char *)tmpNick;
   cp2 = (const char *)tmpName;
   if (dbg > 2) fprintf(stdout, "xIrcNickListEntry::is():cp1 = |%s|\n", cp1);
   if (dbg > 2) fflush(stdout);
   if (dbg > 2) fprintf(stdout, "xIrcNickListEntry::is():cp2 = |%s|\n", cp2);
   if (dbg > 2) fflush(stdout);
   if (dbg > 2) fprintf(stdout, "xIrcNickListEntry::is():Testing op name??\n");
   if (dbg > 2) fflush(stdout);
   if (byNick && *cp1 == '@' && *cp2 != '@')
   {
      if (dbg > 2) fprintf(stdout, "xIrcNickListEntry::is():Testing for old op name\n");
      if (dbg > 2) fflush(stdout);
      if (strcmp(cp2, ++cp1) == 0)
         return(TRUE);
      else
         return(FALSE);
   }
   else if (byNick && *cp2 == '@' && *cp1 != '@')
   {
      if (dbg > 2) fprintf(stdout, "xIrcNickListEntry::is():Testing for new op name\n");
      if (dbg > 2) fflush(stdout);
      if (strcmp(++cp2, cp1) == 0)
         return(TRUE);
      else
         return(FALSE);
   }
   else
   {
      if (dbg > 2) fprintf(stdout, "xIrcNickListEntry::is():Testing for reg name\n");
      if (dbg > 2) fflush(stdout);
      if (strcmp(cp2, cp1) == 0)
         return(TRUE);
      else
         return(FALSE);
   }
}

const char *xIrcNickListEntry::getNick(bool fullName)
{
   const char *cp;

   if (dbg) fprintf(stdout, "xIrcNickListEntry::getNick():Enter\n");
   if (dbg) fflush(stdout);
   cp = nick;
   if (dbg) fprintf(stdout, "xIrcNickListEntry::getNick():Raw nick = |%s|, fullName = %d\n", cp, fullName);
   if (dbg) fflush(stdout);
   if (fullName || *cp != '@')
   {
      if (dbg) fprintf(stdout, "xIrcNickListEntry::getNick(%s):Exit\n", cp);
      if (dbg) fflush(stdout);
      return(cp);
   }
   else
   {
      cp++;
      if (dbg) fprintf(stdout, "xIrcNickListEntry::getNick(%s):Exit\n", cp);
      if (dbg) fflush(stdout);
      return(cp);
   }
}
  
void xIrcNickListEntry::setNick(const char *pNick)
{
   const char *cp;

   nick = "";
   for (cp = pNick; *cp && !isspace(*cp); cp++)
      nick += *cp;
}

void xIrcNickListEntry::setAddr(const char *pAddr)
{
   const char *cp;

   addr = "";
   for (cp = pAddr; *cp && !isspace(*cp); cp++)
      nick += *cp;
}

xIrcNickListEntry *xIrcNickListEntry::next(xIrcNickListEntry  *pEntry)
{
   xIrcNickListEntry *rv;

   rv = pNext;
   pNext = pEntry;
   return(rv);
}

xIrcNickListEntry *xIrcNickListEntry::prev(xIrcNickListEntry  *pEntry)
{
   xIrcNickListEntry *rv;

   rv = pPrev;
   pPrev = pEntry;
   return(rv);
}

void xIrcNickListEntry::link(xIrcNickListEntry *pEntry)
{
   pNext = pEntry->next(this);
   if (pNext)
      pNext->prev(this);
   pPrev = pEntry;
}

void xIrcNickListEntry::unlink()
{
   if (pNext)
      pNext->prev(pPrev);
   if (pPrev)
      pPrev->next(pNext);
   pPrev = pNext = NULL;
}

