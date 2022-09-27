/***************************************************************************
**    xIrcPeopleList.cpp  $Revision: 1.1.1.1 $ - $Name:  $ 
**    Maintains a List of People (Friends and Enimies)
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
#include <qt.h>
#include <qptrlist.h>
#include <qregexp.h>
#include <stdio.h>
#include "xIrcPeopleList.h"

static bool dbg = false;

xIrcPeopleList::xIrcPeopleList()
{
   setAutoDelete(TRUE);
}

xIrcPeopleList::xIrcPeopleList(xIrcPeopleList &list)
{
   xIrcPeopleListIterator si(list);

   for (; si.current() != NULL; ++si)
      add(*si.current());
}

xIrcPeopleList::~xIrcPeopleList()
{
}

int xIrcPeopleList::readFile(const char *fn)
{
   char buf[512], *cp;
   int parm;
   FILE *fp;

   if (dbg) fprintf(stdout, "xIrcPeopleList::readFile(%s):Enter\n", fn);
   if (dbg) fflush(stdout);
   if ((fp = fopen(fn, "r")) == NULL)
      return(-1);

   while (fgets(buf, sizeof(buf), fp) != NULL)
   {
      QString nickStr, maskStr, msgStr, flagStr;
      int flag;

      if (dbg) fprintf(stdout, "xIrcPeopleList::readFile():Line: |%s|\n", buf);
      if (dbg) fflush(stdout);
      for (cp = buf, parm = 0; *cp; cp++)
      {
         if (*cp == '\n')
            continue;
         else if (*cp == ':')
         {
            parm++;
            continue;
         }

         if (dbg) fprintf(stdout, "xIrcPeopleList::readFile():parm: %d, ch = %c\n",
                                    parm, *cp);
         if (dbg) fflush(stdout);
         switch (parm)
         {
            
            case 0:
               nickStr += *cp;
               break;

            case 1:
               maskStr += *cp;
               break;

            case 2:
               flagStr += *cp;
               break;

            case 3:
               msgStr += *cp;
               break;
         }
      }
      if (dbg) fprintf(stdout, "xIrcPeopleList::readFile():flagStr: |%s|\n", 
                        (const char *)flagStr);
      if (dbg) fflush(stdout);
      if (strcmp(flagStr, "+") == 0)
         flag = 1;
      else if (strcmp(flagStr, "-") == 0)
         flag = -1;
      else
         flag = 0;
      if (dbg) fprintf(stdout, "xIrcPeopleList::readFile():flag = %d\n", flag);
      if (dbg) fflush(stdout);

      xIrcPeopleEntry e(nickStr, maskStr, msgStr, flag);
      add(e);
   }
   return(0);
}

int xIrcPeopleList::writeFile(const char *fn)
{
   xIrcPeopleListIterator si(*this);
   char buf[512];
   FILE *fp;

   if (dbg) fprintf(stdout, "xIrcPeopleList::writeFile(%s):Enter\n", fn);
   if (dbg) fflush(stdout);
   if ((fp = fopen(fn, "w")) == NULL)
      return(-1);

   for (; si.current() != NULL; ++si)
   {
      
      sprintf(buf, "%s:%s:%s:%s\n", 
                  (const char *)si.current()->nick(),    
                  (const char *)si.current()->mask(),

                  (si.current()->flag() > 0) ? "+" :
                    (si.current()->flag() < 0) ? "-" : ".",
                  ((const char *)si.current()->message() != NULL) ?
                     (const char *)si.current()->message() : "");

      fputs(buf,fp);
   }
   fclose(fp);
   if (dbg) fprintf(stdout, "xIrcPeopleList::writeFile(%s):Exit\n", fn);
   if (dbg) fflush(stdout);
   return(0);
}

void xIrcPeopleList::add(xIrcPeopleList &list)
{
   xIrcPeopleListIterator si(list);

   for (; si.current() != NULL; ++si)
      add(*si.current());
}
   
void xIrcPeopleList::add(xIrcPeopleEntry &entry)
{
   xIrcPeopleListIterator si(*this);
   bool f = TRUE;

   for (; si.current() != NULL; ++si)
   {
      if (si.current()->mask() == entry.mask())
         f = FALSE;
   }
   if (f) {
      append(new xIrcPeopleEntry(entry));
      sort();
   }
}

bool xIrcPeopleList::matches(xIrcMessage *pMsg)
{
   xIrcPeopleListIterator si(*this);

   for (; si.current() != NULL; ++si)
   {
      if (si.current()->is(pMsg, si.current()->flag()))
         return(TRUE);
   }
   return(FALSE);
}

xIrcPeopleEntry *xIrcPeopleList::entry(xIrcMessage *pMsg)
{
   xIrcPeopleListIterator si(*this);

   for (; si.current() != NULL; ++si)
   {
      if (si.current()->is(pMsg, si.current()->flag()))
         return(si.current());
   }
   return(NULL);
}


