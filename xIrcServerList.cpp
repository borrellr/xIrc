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

#ifndef QT3
static bool dbg = false;
#endif

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
   QFile f(fn);

   if (!f.exists()) {
       qWarning("File %s does not exists\n", fn.latin1());
       return false;
   }

   if (!f.open(IO_ReadOnly)) {
      qWarning("File %s is not readonly\n", fn.latin1());
      return false;
   }

   QTextStream stream(&f);
   QString line;
   QString groupStr, countryStr, stateStr, cityStr, serverStr, portsStr;
   QStringList dataLines;

   while ( !stream.atEnd()) {
      line = stream.readLine();
      if (!line.isEmpty()) {
         dataLines = QStringList::split(":", line);
         if (dataLines.count() == 6) {
            groupStr = dataLines[0];
            countryStr = dataLines[1];
            stateStr = dataLines[2];
            cityStr = dataLines[3];
            serverStr = dataLines[4];
            portsStr = dataLines[5];
            xIrcServerEntry e(groupStr, countryStr, stateStr, cityStr, serverStr, portsStr);
            add(e);
         }
         dataLines.clear();
      }
   }
   f.close();
   return true;
}

int xIrcServerList::writeFile(const QString &fn)
{
   QFile f(fn);

   if (!f.open(IO_WriteOnly)) {
      qWarning("File %s is not writable\n", fn.latin1());
      return false;
   }

   QTextStream stream(&f);
   QStringList writeData;
   QString writeBuf;

   xIrcServerListIterator si(*this);
   while ( si.current() != NULL)
   {
       writeData.append(si.current()->group());
       writeData.append(si.current()->country());
       writeData.append(si.current()->state());
       writeData.append(si.current()->city());
       writeData.append(si.current()->server());
       writeData.append(si.current()->ports());
       writeBuf = writeData.join(":");
       stream << writeBuf.latin1() << endl;
       writeData.clear();
       ++si;
   }
   f.close();

   return true;
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
