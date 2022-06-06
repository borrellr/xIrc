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
#include <qptrlist.h>
#include <qregexp.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <stdio.h>
#include "xIrcServerList.h"

static int dbg = 1;

xIrcServerList::xIrcServerList()
{
   setAutoDelete(TRUE);
}

xIrcServerList::xIrcServerList(xIrcServerList &list, xIrcServerEntry *entry)
{
   int x;
   const QString wc = "*";
   QRegExp groupRE;
   QRegExp countryRE;
   QRegExp stateRE;
   QRegExp cityRE;
   QRegExp serverRE;
   QRegExp portsRE;
   xIrcServerListIterator si(list);

   portsRE.setWildcard(TRUE);

   if (entry != NULL && strlen(entry->group()) > 0)
      groupRE = entry->group();
   else {
      groupRE = wc;
      groupRE.setWildcard(TRUE);
   }
   if (entry != NULL && strlen(entry->country()) > 0)
      countryRE = entry->country();
   else {
      countryRE = wc;
      countryRE.setWildcard(TRUE);
   }
   if (entry != NULL && strlen(entry->state()) > 0)
      stateRE = entry->state();
   else {
      stateRE = wc;
      stateRE.setWildcard(TRUE);
   }
   if (entry != NULL && strlen(entry->city()) > 0)
      cityRE = entry->city();
   else {
      cityRE = wc;
      cityRE.setWildcard(TRUE);
   }
   if (entry != NULL && strlen(entry->city()) > 0)
      serverRE = entry->server();
   else {
      serverRE = wc;
      serverRE.setWildcard(TRUE);
   }
      
   for (; si.current() != NULL; ++si)
   {
      if ((groupRE.match(si.current()->group(), 0, &x) >= 0) &&
          (countryRE.match(si.current()->country(), 0, &x) >= 0) && 
          (stateRE.match(si.current()->state(), 0, &x) >= 0) && 
          (cityRE.match(si.current()->city(), 0, &x) >= 0) && 
          (serverRE.match(si.current()->server(), 0, &x) >= 0))
      {
         add(*si.current());
      }
   }
}

xIrcServerList::~xIrcServerList()
{
}

int xIrcServerList::readFile(const char *fn)
{
#ifdef QT2
   char buf[512], *cp;
   int parm;
   FILE *fp;

   if ((fp = fopen(fn, "r")) == NULL)
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
      if(dbg) {
          printf("Entries:\nGroup: %s\nCountry: %s\nState: %s\nCity: %s\nServer: %s\nPorts: %s\n", groupStr.ascii(), countryStr.ascii(), stateStr.ascii(), cityStr.ascii(), serverStr.ascii(), portsStr.ascii());
      }
      add(e);
   }
#else
   QFile file(fn);

   if ( file.open( IO_ReadOnly ) ) {
      QTextStream stream( &file );
      QString line;
      QString groupStr, countryStr, stateStr, cityStr, serverStr, portsStr;
      while ( !stream.atEnd() ) {
         line = stream.readLine(); 
         if (!line.isEmpty()) {
            xIrcServerEntry e(line);
            add(e);
         }
      }
      file.close();
   }
#endif
   return(0);
}

int xIrcServerList::writeFile(const char *fn)
{
   xIrcServerListIterator si(*this);
   char buf[512];
   FILE *fp;

   if (dbg) fprintf(stdout, "xIrcServerList::writeFile(%s):Enter\n", fn);
   if (dbg) fflush(stdout);
   if ((fp = fopen(fn, "w")) == NULL)
      return(-1);

   for (; si.current() != NULL; ++si)
   {
      sprintf(buf, "%s:%s:%s:%s:%s:%s\n", 
                  (const char *)si.current()->group(),    
                  (const char *)si.current()->country(),    
                  (const char *)si.current()->state(),    
                  (const char *)si.current()->city(),    
                  (const char *)si.current()->server(),    
                  (const char *)si.current()->ports());
      fputs(buf,fp);
   }
   fclose(fp);
   if (dbg) fprintf(stdout, "xIrcServerList::writeFile(%s):Exit\n", fn);
   if (dbg) fflush(stdout);
   return(0);
}

void xIrcServerList::add(xIrcServerList &list)
{
   if(dbg) { printf("Entering xIrcServerList::add list function\n");}
   xIrcServerListIterator si(list);

   for (; si.current() != NULL; ++si)
      add(*si.current());
}
   
void xIrcServerList::add(xIrcServerEntry &entry)
{
   if(dbg) { printf("Entering xIrcServerList::add entry function\n");}
   xIrcServerListIterator si(*this);

   if(dbg) { printf("Entering xIrcServerList::add entry function for loop\n");}
   
   for (; si.current() != NULL; ++si)
   {
      if (si.current()->server() == entry.server())
      {
         remove(si.current());
         break;
      }
   }

   if(dbg) { printf("Entering xIrcServerList::add entry function inSort\n");}

//   inSort(new xIrcServerEntry(entry));

   if(dbg) { printf("Entering xIrcServerList::add entry function Exit\n");}
}
