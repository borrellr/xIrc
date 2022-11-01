/***************************************************************************
**    xIrcServerList.cpp  $Revision: 1.1.1.1 $ - $Name:  $ 
**    Maintains a List of Servers
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
#include "xIrcServerList.h"

static bool dbg = false;

xIrcServerList::xIrcServerList()
{
   setAutoDelete(TRUE);
}

xIrcServerList::xIrcServerList(xIrcServerList &list, xIrcServerEntry *entry)
{
   const QString wc = "*";
   QRegExp groupRE;
   QRegExp countryRE;
   QRegExp stateRE;
   QRegExp cityRE;
   QRegExp serverRE;
   QRegExp portsRE;
   xIrcServerListIterator si(list);

   portsRE.setWildcard(TRUE);
   serverRE.setWildcard(TRUE);
   stateRE.setWildcard(TRUE);
   cityRE.setWildcard(TRUE);
   countryRE.setWildcard(TRUE);
   groupRE.setWildcard(TRUE);

   if (entry != NULL && !entry->group().isEmpty())
      groupRE.setPattern(entry->group());
   else
      groupRE.setPattern(wc);

   if (entry != NULL && !entry->country().isEmpty())
      countryRE.setPattern(entry->country());
   else
      countryRE.setPattern(wc);

   if (entry != NULL && !entry->state().isEmpty())
      stateRE.setPattern(entry->state());
   else
      stateRE.setPattern(wc);

   if (entry != NULL && !entry->city().isEmpty())
      cityRE.setPattern(entry->city());
   else
      cityRE.setPattern(wc);

   if (entry != NULL && !entry->city().isEmpty())
      serverRE.setPattern(entry->server());
   else
      serverRE.setPattern(wc);

   // Display patterns
   printf ("Group pattern is [%s]\n", groupRE.pattern().latin1());
   printf ("Country pattern is [%s]\n", countryRE.pattern().latin1());
   printf ("State pattern is [%s]\n", stateRE.pattern().latin1());
   printf ("City pattern is [%s]\n", cityRE.pattern().latin1());
   printf ("Server pattern is [%s]\n", serverRE.pattern().latin1());

   for (; si.current() != NULL; ++si)
   {
      if (groupRE.exactMatch(si.current()->group()) &&
          countryRE.exactMatch(si.current()->country()) && 
          stateRE.exactMatch(si.current()->state()) && 
          cityRE.exactMatch(si.current()->city()) && 
          serverRE.exactMatch(si.current()->server()))
      {
         add(*si.current());
      }
   }
}

xIrcServerList::~xIrcServerList()
{
}

int xIrcServerList::readFile(const QString &fn)
{
   char buf[512], *cp;
   int parm;
   FILE *fp;

   if ((fp = fopen(fn.latin1(), "r")) == NULL)
      return(-1);

   while (fgets(buf, sizeof(buf), fp) != NULL)
   {
      QString groupStr, countryStr, stateStr, cityStr, serverStr, portsStr;
      for (cp = buf, parm = 0; *cp; cp++)
      {
         if (*cp == '\n')
            continue;
         else if (*cp == ':')
         {
            parm++;
            continue;
         }

         switch (parm)
         {
            
            case 0:
               groupStr += *cp;
               break;

            case 1:
               countryStr += *cp;
               break;

            case 2:
               stateStr += *cp;
               break;

            case 3:
               cityStr += *cp;
               break;

            case 4:
               serverStr += *cp;
               break;

            case 5:
               portsStr += *cp;
               break;
         }
      }
      xIrcServerEntry e(groupStr, countryStr, stateStr, cityStr, serverStr, portsStr);
      add(e);
   }
   return(0);
}

int xIrcServerList::writeFile(const QString &fn)
{
   xIrcServerListIterator si(*this);
   char buf[512];
   FILE *fp;

   if (dbg) fprintf(stdout, "xIrcServerList::writeFile(%s):Enter\n", fn.latin1());
   if (dbg) fflush(stdout);
   if ((fp = fopen(fn.latin1(), "w")) == NULL)
      return(-1);

   for (; si.current() != NULL; ++si)
   {
      sprintf(buf, "%s:%s:%s:%s:%s:%s\n", 
                  (const char *)si.current()->group().latin1(),    
                  (const char *)si.current()->country().latin1(),    
                  (const char *)si.current()->state().latin1(),    
                  (const char *)si.current()->city().latin1(),    
                  (const char *)si.current()->server().latin1(),    
                  (const char *)si.current()->ports().latin1());
      fputs(buf,fp);
   }
   fclose(fp);
   if (dbg) fprintf(stdout, "xIrcServerList::writeFile(%s):Exit\n", fn.latin1());
   if (dbg) fflush(stdout);
   return(0);
}

void xIrcServerList::add(xIrcServerList &list)
{
   xIrcServerListIterator si(list);

   for (; si.current() != NULL; ++si)
      add(*si.current());
}
   
void xIrcServerList::add(xIrcServerEntry &entry)
{
   xIrcServerListIterator si(*this);
   
   for (; si.current() != NULL; ++si)
   {
      if (si.current()->server() == entry.server())
      {
         remove(si.current());
         break;
      }
   }
   append(new xIrcServerEntry(entry));
   sort();
}

void xIrcServerList::showEntries()
{
   xIrcServerListIterator si(*this);
   QString tmpStr;

   while (si.current() != NULL) {
      tmpStr = si.current()->group();
      printf("The group is: |%s|\n",  tmpStr.latin1());
      tmpStr = si.current()->country();
      printf("The country is: |%s|\n",  tmpStr.latin1());
      tmpStr = si.current()->state();
      printf("The state is: |%s|\n",  tmpStr.latin1());
      tmpStr = si.current()->city();
      printf("The city is: |%s|\n",  tmpStr.latin1());
      tmpStr = si.current()->server();
      printf("The server is: |%s|\n",  tmpStr.latin1());
      tmpStr = si.current()->ports();
      printf("The ports are: |%s|\n", tmpStr.latin1());
      ++si;
   }
}
