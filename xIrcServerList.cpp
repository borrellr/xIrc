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
#include "xIrcServerList.h"

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

   portsRE.setWildcard(TRUE);
   serverRE.setWildcard(TRUE);
   stateRE.setWildcard(TRUE);
   cityRE.setWildcard(TRUE);
   countryRE.setWildcard(TRUE);
   groupRE.setWildcard(TRUE);

   if (entry != NULL && !entry->group().isEmpty())
      groupRE = entry->group();
   else
      groupRE = wc;

   if (entry != NULL && !entry->country().isEmpty())
      countryRE = entry->country();
   else
      countryRE = wc;

   if (entry != NULL && !entry->state().isEmpty())
      stateRE = entry->state();
   else
      stateRE = wc;

   if (entry != NULL && !entry->city().isEmpty())
      cityRE = entry->city();
   else
      cityRE = wc;

   if (entry != NULL && !entry->server().isEmpty())
      serverRE = entry->server();
   else
      serverRE = wc;

   xIrcServerListIterator si(list);
   while (si.current() != NULL)
   {
      if ((groupRE.match(si.current()->group(), 0, &x) >= 0) &&
          (countryRE.match(si.current()->country(), 0, &x) >= 0) && 
          (stateRE.match(si.current()->state(), 0, &x) >= 0) && 
          (cityRE.match(si.current()->city(), 0, &x) >= 0) && 
          (serverRE.match(si.current()->server(), 0, &x) >= 0))
      {
         add(*si.current());
      }
      ++si;
   }
}

xIrcServerList::~xIrcServerList()
{
}

bool xIrcServerList::readFile(const QString &fn)
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

bool xIrcServerList::writeFile(const QString &fn)
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
   for (; si.current() != NULL; ++si)
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
   }
   f.close();

   return true;
}

void xIrcServerList::add(xIrcServerList &list)
{
   xIrcServerListIterator si(list);

   while (si.current() != NULL)
   {
      add(*si.current());
      ++si;
   }
}
   
void xIrcServerList::add(xIrcServerEntry &entry)
{
   xIrcServerListIterator si(*this);
   
   while (si.current() != NULL)
   {
      if (si.current()->server() == entry.server())
      {
         remove(si.current());
         break;
      }
      ++si;
   }
   append(new xIrcServerEntry(entry));
   sort();
}

void xIrcServerList::removeEntry(xIrcServerEntry *e)
{
   xIrcServerListIterator si(*this);
   
   while (si.current() != NULL)
   {
      if (compareItems(si.current(), e))
      {
         remove(si.current());
         break;
      }
      ++si;
   }
}

void xIrcServerList::showEntries()
{
   xIrcServerListIterator si(*this);
   QStringList dataLines;

   while(si.current() != NULL) {
        dataLines.append(si.current()->group());
        dataLines.append(si.current()->country());
        dataLines.append(si.current()->state());
        dataLines.append(si.current()->city());
        dataLines.append(si.current()->server());
        dataLines.append(si.current()->ports());
        QString dataStr = dataLines.join(":");
        printf("%s\n", dataStr.latin1());
        dataLines.clear();
        ++si;
   }
}

bool xIrcServerList::compareItems(xIrcServerEntry *e1, xIrcServerEntry *e2)
{
     return (e1->server() == e2->server());
}

void xIrcServerList::replaceEntry(xIrcServerEntry *e1, xIrcServerEntry *e2)
{
   xIrcServerListIterator si(*this);
   
//   e1->showEntries();
//   e2->showEntries();

   while (si.current() != NULL)
   {
      if (compareItems(si.current(), e1))
      {
         remove(si.current());
         append(e2);
         return;
      }
      ++si;
   }
}

xIrcServerEntry *xIrcServerList::findEntry(xIrcServerEntry *e)
{
   xIrcServerListIterator si(*this);

   while (si.current() != NULL)
   {
      if (compareItems(si.current(), e))
         return si.current();
      ++si;
   }
   return NULL;
}
