/***************************************************************************
**    xSQLIndex.cpp  $Revision: 1.1.1.1 $ - $Name:  $ 
**    SQL Table Index Keeper
**
**    Copyright (C) 1997 1998 Joseph Croft <joe@croftj.net>
**    Copyright (C) 1996 Joseph Croft <jcroft@unicomp.net>
**
**    This library is free software; you can redistribute it and/or
**    modify it under the terms of the GNU Library General Public
**    License as published by the Free Software Foundation; either
**    version 2 of the License, or (at your option) any later version.
**
**    This library is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**    Library General Public License for more details.
**
**    You should have received a copy of the GNU Library General Public
**    License along with this library; if not, write to the Free
**    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <qstring.h>
#include <qdict.h>
#include "xSQLIndex.h"

xSQLFieldDef fields[] =
{
   {sqlPlain, "indexes.tablename", "", NULL, 0},
#ifndef POSTGRES
   {sqlPlain, "indexes.index", "", NULL, 0}
#else
   {sqlPlain, "indexes.ident", "", NULL, 0}
#endif
};

xSQLIndex::xSQLIndex() : xSQLBase(fields)
{
}

xSQLIndex::~xSQLIndex()
{
}

QString xSQLIndex::nextIndex(int database, QString &table)
{
   QDict<QString> fields, data, links;
   QList<QString> sort;
   QString rv, tmp;
   char buf[20];
   int x;

#ifndef POSTGRES
   tmp = table;
#else
   tmp = table.lower();
#endif
   links.insert("indexes.tablename", &tmp);
   if (query(database, fields, links, sort) == TRUE)
   {
      data = getRow(1);
//      data.toFirst();
#ifndef POSTGRES
      x = strtol((const char *)*data.find("indexes.index"), NULL, 10);
#else
      x = strtol((const char *)*data.find("indexes.ident"), NULL, 10);
#endif
      if (x > 0)
      {
         x++;
         sprintf(buf, "%d", x);
         rv = buf;
#ifndef POSTGRES
         data.replace("indexes.index", &rv);
#else
         data.replace("indexes.ident", &rv);
#endif
         update(database, data, links);
      }
   }
   return(rv);
}
