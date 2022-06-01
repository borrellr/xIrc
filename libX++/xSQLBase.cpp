/***************************************************************************
**    xSQLBase.cpp  $Revision: 1.3 $ - $Name:  $ 
**    SQL View frame
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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <qlist.h>
#include <qobject.h>
#include <qlineedit.h>
#include <qlabel.h>
//#include <qobjcoll.h>
#include <qregexp.h> 
#include "xEdit.h"
#include "xMisc.h"
#include "xMessageBox.h"
#include "xSQLField.h"
#include "xSQLIndex.h"
#include "xSQLBase.h"

static int dbg = 0;

xSQLIndex Index;

xSQLBase::xSQLBase(xSQLFieldDef *pFields)
{
   xSQLFieldDef *fp;
   QString strTmp;

   if (dbg) fprintf(stdout, "xSQLBase::xSQLBase:Enter\n");
   if (dbg) fflush(stdout);
   pViewData = NULL;
   fieldNames.setAutoDelete(TRUE);
   calcNames.setAutoDelete(TRUE);
   tableNames.setAutoDelete(TRUE);
   fieldLinks.setAutoDelete(TRUE);
   for (fp = pFields; fp != NULL && fp->name != NULL; fp++)
   {
      if (dbg) fprintf(stdout, "xSQLBase::xSQLBase:Have field: Name: |%s|, Label: |%s|, Link: |%s|, Type: %d\n",
                               fp->name, fp->label, fp->link, fp->type);
      if (dbg) fflush(stdout);
      if ((fp->type & ~sqlHidden) != sqlComputation)
      {
         if (dbg) fprintf(stdout, "xSQLBase::xSQLBase:Adding field to fieldNames\n");
         if (dbg) fflush(stdout);
         fieldNames.append(new QString(fp->name));
         if (dbg) fprintf(stdout, "xSQLBase::xSQLBase:getting tablename\n");
         if (dbg) fflush(stdout);
         strTmp = getTableName(fp->name);
         if (dbg) fprintf(stdout, "xSQLBase::xSQLBase:Testing if table is already in the list\n");
         if (dbg) fflush(stdout);
         if (inList(tableNames, strTmp) == FALSE)
         {
            if (dbg) fprintf(stdout, "xSQLBase::xSQLBase:Adding table to the list\n");
            if (dbg) fflush(stdout);
            tableNames.append(new QString(strTmp));
         }
         if (fp->link != NULL)
         {
            if (dbg) fprintf(stdout, "xSQLBase::xSQLBase:Adding link!\n");
            if (dbg) fflush(stdout);
            fieldLinks.insert(fp->name, new QString(fp->link));
         }
      }
      else
      {
         if (dbg) fprintf(stdout, "xSQLBase::xSQLBase:Inserting Computation field to list\n");
         if (dbg) fflush(stdout);
         calcNames.append(new QString(fp->name));
         calcFields.insert(fp->name, new QString(fp->link));
      }
   }
   if (dbg) fprintf(stdout, "xSQLBase::xSQLBase:Exit\n");
   if (dbg) fflush(stdout);
}

xSQLBase::~xSQLBase()
{
#ifndef POSTGRES
   if (pViewData)
      msqlFreeResult(pViewData);
#else
   PQclear((PGresult*)pViewData);
#endif POSTGRES
}

bool xSQLBase::query(int db, QList<QString> sort)
{
   QDict<QString> links, match, skip;
   return(query(db, match, links, skip, sort));
}

bool xSQLBase::query(int db, QString index, QList<QString> sort)
{
   QDict<QString> links, match, skip;
   QString indexName;

#ifndef POSTGRES
   indexName = getPriTable() + ".index";
#else
   indexName = getPriTable() + ".ident";
#endif POSTGRES
   links.setAutoDelete(TRUE);
   links.insert((const char *)indexName, new QString(index));
   return(query(db, match, links, skip, sort));
}

bool xSQLBase::query(int db, long index, QList<QString> sort)
{
   QDict<QString> links, match, skip;
   QString indexName, strIndex;

#ifndef POSTGRES
   indexName = getPriTable() + ".index";
#else
   indexName = getPriTable() + ".ident";
#endif POSTGRES
   strIndex.setNum(index);
   links.setAutoDelete(TRUE);
   links.insert((const char *)indexName, new QString(strIndex));
   return(query(db, match, links, skip, sort));
}

bool xSQLBase::query(int db, QDict<QString> &match, QDict<QString> &links, 
                     QList<QString> sort)
{
   QDict<QString> skip;
   return(query(db, match, links, skip, sort));
}

bool xSQLBase::query(int db, QDict<QString> &match, QDict<QString> &links, 
                     QDict<QString> &skip, QList<QString> sort)
{
   QString strQuery, strIndexName;
   const char *ccp;
   int x;
   bool rv;

   if (dbg) fprintf(stdout, "xSQLBase::query(QDict):Enter\n");
   if (dbg) fflush(stdout);
#ifndef POSTGRES
   if (pViewData)
      msqlFreeResult(pViewData);
#else
   PQclear((PGresult*)pViewData);
#endif POSTGRES

   strQuery = "SELECT ";

   //
   // Add in all of the fields associated with the view
   // 
   QListIterator<QString> fieldIt(fieldNames);
   for (x = 0; fieldIt.current(); ++fieldIt, x++)
   {
      if (x > 0)
         strQuery += ", ";
      strQuery += *fieldIt.current();
      if (dbg) fprintf(stdout, "xSQLBase::query():1.strQuery = |%s|\n",
                                (const char *)strQuery);
      if (dbg) fflush(stdout);
   }

   //
   // Force getting the index field if it was not already specified
   // in the fields.
   //
#ifndef POSTGRES
   strIndexName = getPriTable() + ".index";
#else
   strIndexName = getPriTable() + ".ident";
#endif POSTGRES

   if (inList(fieldNames, strIndexName) == FALSE)
      strQuery += ", " + strIndexName;

   //
   // Now Add in the tablenames
   //
   strQuery += " FROM ";
   QListIterator<QString> tableIt(tableNames);
   for (x = 0; tableIt.current(); ++tableIt, x++)
   {
      if (x > 0)
         strQuery += ", ";
      strQuery += *tableIt.current();
      if (dbg) fprintf(stdout, "xSQLBase::query():2.strQuery = |%s|\n",
                                (const char *)strQuery);
      if (dbg) fflush(stdout);
   }

   x = 0;
   if (dbg) fprintf(stdout, "xSQLBase::query():Adding in %d Links\n",
                             links.count());
   if (match.count() > 0)
   {
      QDictIterator<QString> dataIt(match);
      for (; dataIt.current(); ++dataIt)
      {
         if (dbg) fprintf(stdout, "xSQLBase::query():Field |%s| has |%s|\n",
                                  dataIt.currentKey().latin1(),
                                  dataIt.current());
         if (strlen((const char *)*dataIt.current()) > 0)
         {
            if (x > 0)
               strQuery += " AND ";
            else
               strQuery += " WHERE ";
            x++;
            strQuery += dataIt.currentKey();
            strQuery += " like '%";
            strQuery += escString(*dataIt.current());
            strQuery += "%'";
            if (dbg) fprintf(stdout, "xSQLBase::query():3.strQuery = |%s|\n",
                                      (const char *)strQuery);
            if (dbg) fflush(stdout);
         }
      }
   }
      
   if (dbg) fprintf(stdout, "xSQLBase::query():Adding in %d Skips\n",
                             skip.count());
   if (dbg)
   {
      QDictIterator<QString> it(skip);
      fprintf(stdout, "xSQLBase::query():Skip Count = %d\n",
                      skip.count());
      for (int x1 = 0; it.current(); ++it, x1++)
      {
         fprintf(stdout, "xSQLBase::query():link %d: |%s:%s|\n", x1,
                         it.currentKey().latin1(),
                         (const char *)escString(*it.current()));
      }
      if (dbg) fflush(stdout);
   }

   if (skip.count() > 0)
   {
      if (dbg) fflush(stdout);
      QDictIterator<QString> skipIt(skip);
      for (; skipIt.current(); ++skipIt)
      {
         if (dbg) fprintf(stdout, "xSQLBase::query():Found |%s|:|%s|\n",
                                   skipIt.currentKey().latin1(),
                                   (const char *)escString(*skipIt.current()));
         if (dbg) fflush(stdout);
         if (x > 0)
            strQuery += " AND ";
         else
            strQuery += " WHERE ";
         x++;
         strQuery += skipIt.currentKey();
         strQuery += " <> '";
         strQuery += escString(*skipIt.current());
         strQuery += "'";
         if (dbg) fprintf(stdout, "xSQLBase::query():4.strQuery = |%s|\n",
                                   (const char *)strQuery);
         if (dbg) fflush(stdout);
      }
   }

   if (dbg)
   {
      QDictIterator<QString> it(links);
      fprintf(stdout, "xSQLBase::query():Link Count = %d\n",
                      links.count());
      for (int x1 = 0; it.current(); ++it, x1++)
      {
         fprintf(stdout, "xSQLBase::query():link %d: |%s:%s|\n", x1,
                         it.currentKey().latin1(),
                         (const char *)escString(*it.current()));
      }
      if (dbg) fflush(stdout);
   }

   if (links.count() > 0)
   {
      if (dbg) fflush(stdout);
      QDictIterator<QString> linksIt(links);
      for (; linksIt.current(); ++linksIt)
      {
         if (dbg) fprintf(stdout, "xSQLBase::query():Found |%s|:|%s|\n",
                                   linksIt.currentKey().latin1(),
                                   (const char *)escString(*linksIt.current()));
         if (dbg) fflush(stdout);
         if (x > 0)
            strQuery += " AND ";
         else
            strQuery += " WHERE ";
         x++;
         strQuery += linksIt.currentKey();
         strQuery += " = '";
         strQuery += escString(*linksIt.current());
         strQuery += "'";
         if (dbg) fprintf(stdout, "xSQLBase::query():4.strQuery = |%s|\n",
                                   (const char *)strQuery);
         if (dbg) fflush(stdout);
      }
   }

   if (dbg)
   {
      QDictIterator<QString> it(fieldLinks);
      fprintf(stdout, "xSQLBase::query():Field Link Count = %d\n",
                      fieldLinks.count());
      for (int x1 = 0; it.current(); ++it, x1++)
      {
         fprintf(stdout, "xSQLBase::query():link %d: |%s:%s|\n", x1,
                         it.currentKey().latin1(),
                         (const char *)escString(*it.current()));
      }
      if (dbg) fflush(stdout);
   }

   if (fieldLinks.count() > 0)
   {
      if (dbg) fflush(stdout);
      QDictIterator<QString> linksIt(fieldLinks);
      for (; linksIt.current(); ++linksIt)
      {
         if (dbg) fprintf(stdout, "xSQLBase::query():Adding Field link |%s|:|%s|\n",
                                   linksIt.currentKey().latin1(),
                                   (const char *)escString(*linksIt.current()));
         if (dbg) fflush(stdout);
         if (x > 0)
            strQuery += " AND ";
         else
            strQuery += " WHERE ";
         x++;
         strQuery += getTableName(linksIt.currentKey());
#ifndef POSTGRES
         strQuery += ".index";
#else
         strQuery += ".ident";
#endif POSTGRES
         strQuery += " = ";
         strQuery += escString(*linksIt.current());
         if (dbg) fprintf(stdout, "xSQLBase::query():5.strQuery = |%s|\n",
                                   (const char *)strQuery);
         if (dbg) fflush(stdout);
      }
   }

   if (sort.count() > 0)
   {
      strQuery += " ORDER BY ";
      QListIterator<QString> sortIt(sort);
      for (x = 0; sortIt.current(); ++sortIt)
      {
         if (dbg) fprintf(stdout, "xSQLBase::query():Adding Field Ordering |%s|\n",
                                   (const char *)*sortIt.current());
         if (dbg) fflush(stdout);
         if (x > 0)
            strQuery += ", ";
         x++;
         strQuery += *sortIt.current();
         if (dbg) fprintf(stdout, "xSQLBase::query():6.strQuery = |%s|\n",
                                   (const char *)strQuery);
         if (dbg) fflush(stdout);
      }
   }

   ccp = strQuery;
   if (dbg) fprintf(stdout, "xSQLBase::query():Final strQuery = |%s|\n", ccp);
   if (dbg) fflush(stdout);

#ifndef POSTGRES
   if (msqlQuery(db, (char *)ccp) < 0)
   {
      if (dbg) fprintf(stdout, "xSQLBase::query():Query Failed!!!\n");
      if (dbg) fflush(stdout);
      xMessageBox::message("Query Error", dbError(db), "Ok");
      prevQuery = "";
      pViewData = NULL;
      rv = FALSE;
   }
   else
   {
      if (dbg) fprintf(stdout, "xSQLBase::query():Query Success!!!\n");
      if (dbg) fflush(stdout);
      prevQuery = strQuery;
      pViewData = msqlStoreResult();
      rv = TRUE;
   }
#else
   pViewData = PQexec((PGconn*)db, (char*)ccp);
   if (PQresultStatus(pViewData) != PGRES_TUPLES_OK)
   {
      if (dbg) fprintf(stdout, "xSQLBase::query():Query Failed!!!\n");
      if (dbg) fflush(stdout);
      xMessageBox::message("Query Error", PQerrorMessage((PGconn*)db), "Ok");
      prevQuery = "";
      pViewData = NULL;
      rv = FALSE;
   }
   else
   {
      if (dbg) fprintf(stdout, "xSQLBase::query():Query Success!!!\n");
      if (dbg) fflush(stdout);
      prevQuery = strQuery;
      rv = TRUE;
   }
#endif POSTGRES

   if (dbg) fprintf(stdout, "xSQLBase::query():Exit(%d)\n", rv);
   if (dbg) fflush(stdout);
   return(rv);
}

bool xSQLBase::del(int db, QString index)
{
   QDict<QString> links, match;
   QString indexName;

#ifndef POSTGRES
   indexName = getPriTable() + ".index";
#else
   indexName = getPriTable() + ".ident";
#endif POSTGRES
   links.setAutoDelete(TRUE);
   links.insert((const char *)indexName, new QString(index));
   return(del(db, match, links));
}

bool xSQLBase::del(int db, long index)
{
   QDict<QString> links, match;
   QString indexName, strIndex;

#ifndef POSTGRES
   indexName = getPriTable() + ".index";
#else
   indexName = getPriTable() + ".ident";
#endif POSTGRES
   strIndex.setNum(index);
   links.setAutoDelete(TRUE);
   links.insert((const char *)indexName, new QString(strIndex));
   return(del(db, match, links));
}

bool xSQLBase::del(int db, QDict<QString> &match, QDict<QString> &links) 
{
   QString strQuery, strIndexName;
   const char *ccp;
   int x;
   bool rv;

   if (dbg) fprintf(stdout, "xSQLBase::delete(QDict):Enter\n");
   if (dbg) fflush(stdout);

#ifndef POSTGRES
   if (pViewData)
      msqlFreeResult(pViewData);
#else
   PQclear(pViewData);
#endif POSTGRES

   strQuery = "DELETE FROM ";

   //
   // Add in the tablename
   //
   strQuery += getPriTable();

   x = 0;
   if (dbg) fprintf(stdout, "xSQLBase::delete():Adding in %d Matches\n",
                             links.count());
   if (match.count() > 0)
   {
      QDictIterator<QString> dataIt(match);
      for (; dataIt.current(); ++dataIt)
      {
         if (strcmp(getTableName(dataIt.currentKey()), getPriTable()) != 0)
         {
            if (dbg) fprintf(stdout, "xSQLBase::delete():Skipping Field |%s|\n",
                                     dataIt.currentKey().latin1());
            continue;
         }

         if (dbg) fprintf(stdout, "xSQLBase::delete():Field |%s| has |%s|\n",
                                  dataIt.currentKey().latin1(),
                                  (const char *)escString(*dataIt.current()));
         if (strlen((const char *)*dataIt.current()) > 0)
         {
            if (x > 0)
               strQuery += " AND ";
            else
               strQuery += " WHERE ";
            x++;
            strQuery += dataIt.currentKey();
            strQuery += " like '%";
            strQuery += escString(*dataIt.current());
            strQuery += "%'";
            if (dbg) fprintf(stdout, "xSQLBase::delete():3.strQuery = |%s|\n",
                                      (const char *)strQuery);
            if (dbg) fflush(stdout);
         }
      }
   }
      
   if (dbg) fprintf(stdout, "xSQLBase::delete():Adding in %d Links\n",
                             links.count());
   if (dbg)
   {
      QDictIterator<QString> it(links);
      fprintf(stdout, "xSQLBase::delete():Link Count = %d\n",
                      links.count());
      for (int x1 = 0; it.current(); ++it, x1++)
      {
         fprintf(stdout, "xSQLBase::delete():link %d: |%s:%s|\n", x1,
                         it.currentKey().latin1(),
                         (const char *)escString(*it.current()));
      }
      if (dbg) fflush(stdout);
   }

   if (links.count() > 0)
   {
      if (dbg) fflush(stdout);
      QDictIterator<QString> linksIt(links);
      for (; linksIt.current(); ++linksIt)
      {
         if (strcmp(getTableName(linksIt.currentKey()), getPriTable()) != 0)
         {
            if (dbg) fprintf(stdout, "xSQLBase::delete():Skipping Field |%s|\n",
                                     linksIt.currentKey().latin1());
            continue;
         }

         if (dbg) fprintf(stdout, "xSQLBase::delete():Found |%s|:|%s|\n",
                                   linksIt.currentKey().latin1(),
                                   (const char *)escString(*linksIt.current()));
         if (dbg) fflush(stdout);
         if (x > 0)
            strQuery += " AND ";
         else
            strQuery += " WHERE ";
         x++;
         strQuery += linksIt.currentKey();
         strQuery += " = '";
         strQuery += escString(*linksIt.current());
         strQuery += "'";
         if (dbg) fprintf(stdout, "xSQLBase::delete():4.strQuery = |%s|\n",
                                   (const char *)strQuery);
         if (dbg) fflush(stdout);
      }
   }

   ccp = strQuery;
   if (dbg) fprintf(stdout, "xSQLBase::delete():Final strQuery = |%s|\n", ccp);
   if (dbg) fflush(stdout);

#ifndef POSTGRES
   if (msqlQuery(db, (char *)ccp) < 0)
   {
      if (dbg) fprintf(stdout, "xSQLBase::delete():Query Failed!!!\n");
      if (dbg) fflush(stdout);
      xMessageBox::message("Query Error", dbError(db), "Ok");
      prevQuery = "";
      pViewData = NULL;
      rv = FALSE;
   }
   else
   {
      if (dbg) fprintf(stdout, "xSQLBase::delete():Query Success!!!\n");
      if (dbg) fflush(stdout);
      prevQuery = strQuery;
      pViewData = msqlStoreResult();
      rv = TRUE;
   }
#else
   pViewData = PQexec((PGconn*)db, (char*)ccp);
   if (PQresultStatus(pViewData) != PGRES_COMMAND_OK)
   {
      if (dbg) fprintf(stdout, "xSQLBase::query():Query Failed!!!\n");
      if (dbg) fflush(stdout);
      xMessageBox::message("Query Error", PQerrorMessage((PGconn*)db), "Ok");
      prevQuery = "";
      pViewData = NULL;
      rv = FALSE;
   }
   else
   {
      if (dbg) fprintf(stdout, "xSQLBase::query():Query Success!!!\n");
      if (dbg) fflush(stdout);
      prevQuery = strQuery;
      rv = TRUE;
   }
#endif POSTGRES

   if (dbg) fprintf(stdout, "xSQLBase::delete():Exit(%d)\n", rv);
   if (dbg) fflush(stdout);
   return(rv);
}

long xSQLBase::insert(int db, QDict<QString> &fields)
{
   QList<QString> tables;
   const char *ccp;
   QString query, ident;
   int x;
   long rv;

   if (dbg) fprintf(stdout, "xSQLBase::insert():Enter\n");
   if (dbg) fflush(stdout);

#ifndef POSTGRES
   if (pViewData)
      msqlFreeResult(pViewData);
#else
   PQclear(pViewData);
#endif POSTGRES

   query = "INSERT INTO ";

   QListIterator<QString> fieldIt(fieldNames);
   fieldIt.toFirst();
   QListIterator<QString> tableIt(tableNames);
   query += *tableIt.current();
   if (dbg) fprintf(stdout, "xSQLBase::insert():2.query = |%s|\n",
                             (const char *)query);
   if (dbg) fflush(stdout);

   ident = Index.nextIndex(db, *tableIt.current());

   QDictIterator<QString> dataIt(fields);
   query += " ( ";
   for (x = 0, dataIt.toFirst(); dataIt.current(); ++dataIt, x++)
   {
      if (dbg) fprintf(stdout, "xSQLBase::insert():3.Next Field: Field:|%s|, Data:|%s|\n",
                                dataIt.currentKey().latin1(),
                                (const char *)escString(*dataIt.current()));

      if (calcFields[dataIt.currentKey()] != NULL)
      {
         if (dbg) fprintf(stdout, "xSQLBase::insert():Field:|%s| is Computaion type\n",
                                dataIt.currentKey().latin1());
         x--;
         continue;
      }

      if (strcmp(getTableName(dataIt.currentKey()),
                 getPriTable()) != 0)
      {
         if (dbg) fprintf(stdout, "xSQLBase::insert():Field:|%s| is not of primary table!\n",
                                dataIt.currentKey().latin1());
         x--;
         continue;
      }

      if (strlen((const char *)*dataIt.current()) > 0)
      {
         if (x > 0)
            query += ", ";
         query += getFieldName(dataIt.currentKey());
         if (dbg) fprintf(stdout, "xSQLBase::insert():3.query = |%s|\n",
                                   (const char *)query);
         if (dbg) fflush(stdout);
      }
   }

#ifndef POSTGRES
   query += ", index) VALUES ( ";
#else
   query += ", ident) VALUES ( ";
#endif POSTGRES
   for (x = 0, dataIt.toFirst(); dataIt.current(); ++dataIt, x++)
   {
      if (dbg) fprintf(stdout, "xSQLBase::insert():3.Next Field: Field:|%s|, Data:|%s|\n",
                                dataIt.currentKey().latin1(),
                                (const char *)escString(*dataIt.current()));
      if (strlen((const char *)*dataIt.current()) > 0)
      {
         if (x > 0)
            query += ", ";
         query += " '";
         query += escString(dataIt.current()->stripWhiteSpace());
         query += "'";
         if (dbg) fprintf(stdout, "xSQLBase::insert():3.query = |%s|\n",
                                   (const char *)query);
         if (dbg) fflush(stdout);
      }
   }
   query += ", '";
   query += ident;
   query += "')";
   ccp = query;
   if (dbg) fprintf(stdout, "xSQLBase::insert():Final query = |%s|\n", ccp);
   if (dbg) fflush(stdout);

#ifndef POSTGRES
   if (msqlQuery(db, (char *)ccp) < 0)
   {
      
      if (dbg) fprintf(stdout, "xSQLBase::insert():Query Failed!!!\n");
      if (dbg) fflush(stdout);
      xMessageBox::message("Insert Error", dbError(db), "Ok");
      rv = -1;
   }
   else
   {
      if (dbg) fprintf(stdout, "xSQLBase::insert():Query Success!!!\n");
      if (dbg) fflush(stdout);
      rv = ident.toLong(NULL);
   }
#else
   pViewData = PQexec((PGconn*)db, (char*)ccp);
   if (PQresultStatus(pViewData) != PGRES_COMMAND_OK)
   {
      if (dbg) fprintf(stdout, "xSQLBase::query():Query Failed!!!\n");
      if (dbg) fflush(stdout);
      xMessageBox::message("Query Error", PQerrorMessage((PGconn*)db), "Ok");
      rv = -1;
   }
   else
   {
      if (dbg) fprintf(stdout, "xSQLBase::query():Query Success!!!\n");
      if (dbg) fflush(stdout);
      rv = ident.toLong(NULL);
   }
#endif POSTGRES

   if (dbg) fprintf(stdout, "xSQLBase::insert():Exit(%ld)\n", rv);
   if (dbg) fflush(stdout);
   return(rv);
}

bool xSQLBase::update(int db, QDict<QString> &fields, QString index)
{
   QDict<QString> links, match;
   QString indexName;

#ifndef POSTGRES
   indexName = getPriTable() + ".index";
#else
   indexName = getPriTable() + ".ident";
#endif POSTGRES
   links.setAutoDelete(TRUE);
   links.insert((const char *)indexName, new QString(index));
   return(update(db, fields, links));
}

bool xSQLBase::update(int db, QDict<QString> &fields, long index)
{
   QDict<QString> links, match;
   QString indexName, strIndex;

#ifndef POSTGRES
   indexName = getPriTable() + ".index";
#else
   indexName = getPriTable() + ".ident";
#endif POSTGRES
   strIndex.setNum(index);
   links.insert((const char *)indexName, &strIndex);
   return(update(db, fields, links));
}

bool xSQLBase::update(int db, QDict<QString> &fields, QDict<QString> &links)
{
   QList<QString> tables;
   const char *ccp;
   QString query;
   bool rv;
   int x;

   if (dbg) fprintf(stdout, "xSQLBase::update():Enter\n");
   if (dbg) fflush(stdout);

#ifndef POSTGRES
   if (pViewData)
      msqlFreeResult(pViewData);
#else
   PQclear(pViewData);
#endif POSTGRES

   query = "UPDATE ";

   QListIterator<QString> fieldIt(fieldNames);
   fieldIt.toFirst();
   QListIterator<QString> tableIt(tableNames);
   query += *tableIt.current();
   if (dbg) fprintf(stdout, "xSQLBase::update():2.query = |%s|\n",
                             (const char *)query);
   if (dbg) fflush(stdout);

   QDictIterator<QString> dataIt(fields);

   query += " SET ";
   for (x = 0, dataIt.toFirst(); dataIt.current(); ++dataIt, x++)
   {
      if (dbg) fprintf(stdout, "xSQLBase::update():3.Next Field: Field:|%s|, Data:|%s|\n",
                                dataIt.currentKey().latin1(),
                                (const char *)escString(*dataIt.current()));

      if (calcFields[dataIt.currentKey()] != NULL)
      {
         if (dbg) fprintf(stdout, "xSQLBase::update():Field:|%s| is Computaion type\n",
                                dataIt.currentKey().latin1());
         x--;
         continue;
      }

      if (strcmp(getTableName(dataIt.currentKey()),
                 getPriTable()) != 0)
      {
         if (dbg) fprintf(stdout, "xSQLBase::update():Field:|%s| is not of primary table!\n",
                                dataIt.currentKey().latin1());
         x--;
         continue;
      }

//      if (strlen((const char *)*dataIt.current()) > 0)
//      {
         if (x > 0)
            query += ", ";
         query += getFieldName(dataIt.currentKey());
         query += " = '";
         query += escString(dataIt.current()->stripWhiteSpace());
         query += "'";
         if (dbg) fprintf(stdout, "xSQLBase::update():3.query = |%s|\n",
                                   (const char *)query);
         if (dbg) fflush(stdout);
//      }
//      else
//         x--;
   }
      
   if (dbg)
   {
      QDictIterator<QString> it(links);
      fprintf(stdout, "xSQLBase::doQuery():Link Count = %d\n",
                      links.count());
      if (dbg) fflush(stdout);
      for (int x1 = 0; it.current(); ++it, x1++)
      {
         fprintf(stdout, "xSQLBase::doQuery():link %d: |%s:%s|\n", x1,
                         it.currentKey().latin1(),
                         (const char *)escString(*it.current()));
         if (dbg) fflush(stdout);
      }
      if (dbg) fflush(stdout);
   }

   if (links.count() > 0)
   {
      QDictIterator<QString> linksIt(links);
      for (x = 0; linksIt.current(); ++linksIt, x++)
      {
         if (x > 0)
            query += " AND ";
         else
            query += " WHERE ";
         query += linksIt.currentKey();
         query += " = '";
         query += escString(*linksIt.current());
         query += "'";
         if (dbg) fprintf(stdout, "xSQLBase::update():4.query = |%s|\n",
                                   (const char *)query);
         if (dbg) fflush(stdout);
      }
   }

   if (dbg)
   {
      QDictIterator<QString> it(fieldLinks);
      fprintf(stdout, "xSQLBase::update():Field Link Count = %d\n",
                      fieldLinks.count());
      if (dbg) fflush(stdout);
      for (int x1 = 0; it.current(); ++it, x1++)
      {
         fprintf(stdout, "xSQLBase::update():link %d: |%s:%s|\n", x1,
                         it.currentKey().latin1(),
                         (const char *)*it.current());
         if (dbg) fflush(stdout);
      }
      if (dbg) fflush(stdout);
   }

   if (fieldLinks.count() > 0)
   {
      QDictIterator<QString> linksIt(fieldLinks);
      for (; linksIt.current(); ++linksIt, x++)
      {
         if (strcmp(getTableName(linksIt.currentKey()),
                    getPriTable()) != 0)
         {
            if (dbg) fprintf(stdout, "xSQLBase::update():Field:|%s| is not of primary table!\n",
                                     linksIt.currentKey().latin1());
            if (dbg) fflush(stdout);
            x--;
            continue;
         }

         if (x > 0)
            query += " AND ";
         else
            query += " WHERE ";
         query += linksIt.currentKey();
         query += " = ";
         query += escString(*linksIt.current());
         if (dbg) fprintf(stdout, "xSQLBase::update():4.query = |%s|\n",
                                   (const char *)query);
         if (dbg) fflush(stdout);
      }
   }

   ccp = query;
   if (dbg) fprintf(stdout, "xSQLBase::update():Final query = |%s|\n", ccp);
   if (dbg) fflush(stdout);

#ifndef POSTGRES
   if (msqlQuery(db, (char *)ccp) < 0)
   {
      
      if (dbg) fprintf(stdout, "xSQLBase::update():Query Failed!!!\n");
      if (dbg) fflush(stdout);
      xMessageBox::message("Update Error", dbError(db), "Ok");
      rv = FALSE;
   }
   else
   {
      if (dbg) fprintf(stdout, "xSQLBase::update():Query Success!!!\n");
      if (dbg) fflush(stdout);
      rv = TRUE;
   }
#else
   pViewData = PQexec((PGconn*)db, (char*)ccp);
   if (PQresultStatus(pViewData) != PGRES_COMMAND_OK)
   {
      if (dbg) fprintf(stdout, "xSQLBase::query():Query Failed!!!\n");
      if (dbg) fflush(stdout);
      xMessageBox::message("Query Error", PQerrorMessage((PGconn*)db), "Ok");
      rv = FALSE;
   }
   else
   {
      if (dbg) fprintf(stdout, "xSQLBase::query():Query Success!!!\n");
      if (dbg) fflush(stdout);
      rv = TRUE;
   }
#endif POSTGRES

   if (dbg) fprintf(stdout, "xSQLBase::update():Exit(%d)\n", rv);
   if (dbg) fflush(stdout);
   return(rv);
}

QDict<QString> xSQLBase::getRow(int row)
{
   int x;
   QDict<QString> rv(71);
#ifndef POSTGRES
   m_row pRow;
#endif
   QString *pField, *pCalc;
   QListIterator<QString> fieldIt(fieldNames);
   QListIterator<QString> calcIt(calcNames);

   if (dbg) fprintf(stdout, "xSQLBase::getRow():Enter\n");
   if (dbg) fflush(stdout);
#ifndef POSTGRES
   if (pViewData == NULL)
#else
   if (pViewData == NULL || PQresultStatus(pViewData) != PGRES_TUPLES_OK)
#endif
   {
      if (dbg > 2) fprintf(stdout, "xSQLBase::getRow():Abort!\n");
      if (dbg > 2) fflush(stdout);
      return(rv);
   }
#ifndef POSTGRES
   msqlDataSeek(pViewData, row - 1);
   if ((pRow = msqlFetchRow(pViewData)) != NULL)
#else
   if (row <= PQntuples(pViewData))
#endif
   {
      for (x = 0; (pField = fieldIt.current()) != NULL; ++fieldIt, x++)
      {
         if (dbg > 2) fprintf(stdout, "xSQLBase::getRow():Getting Field |%s| for row %d\n",
                                  (const char *)*pField, row);
         if (dbg > 2) fflush(stdout);
#ifndef POSTGRES
         rv.insert(*pField, new QString(pRow[x]));
#else
         QString tmp(PQgetvalue(pViewData, row-1, x));
         rv.insert(*pField, 
               new QString(tmp.stripWhiteSpace()));
/*         
         for (int y = 0; y < PQnfields(pViewData); y++)
         {
            if (dbg > 2) fprintf(stdout, "xSQLBase::getRow():Testing against |%s| col %d\n",
                                     (const char *)PQfname(pViewData, y), y);
            if (strcmp(getFieldName(*pField), PQfname(pViewData, y)) == 0)
            {
               break;
            }
         }
*/
#endif POSTGRES
      }
      if (calcIt.count() > 0)
      {
         for (x = 0; calcIt.current() != NULL; ++calcIt)
         {
            pCalc = calcFields[*calcIt.current()];
            if (dbg > 2) fprintf(stdout, "xSQLBase::getRow():Getting Calc Field |%s| for row %d\n",
                                         (const char *)*calcIt.current(), row);
            if (dbg > 2) fflush(stdout);
            rv.insert(*calcIt.current(), 
                      new QString(calcField(rv, *calcIt.current(), pCalc)));
         }
      }
   }
   else
   {
      if (dbg > 2) fprintf(stdout, "xSQLBase::getRow():No Data for row %d\n",
                               row);
      if (dbg > 2) fflush(stdout);
#ifndef POSTGRES
      msqlDataSeek(pViewData, 0);
#endif
   }
   if (dbg) fprintf(stdout, "xSQLBase::getRow():Exit\n");
   if (dbg) fflush(stdout);
   return(rv);
}

QDict<QString> xSQLBase::getRow(long index)
{
   QDict<QString> rv(71);
   QString idx;

   rv.setAutoDelete(TRUE);
   rv.clear();
   for (int row = 1; row <= rows(); row++)
   {
      idx = getIndex(row);
      if (idx.toLong() == index)
      {
         rv = getRow(row);
         break;
      }
   }
   return(rv);
}

QString xSQLBase::getIndex(int row)
{
#ifndef POSTGRES
   int x;
   m_row pRow;
   m_field *pField;
#endif POSTGRES
   QString rv;
   QListIterator<QString> fieldIt(fieldNames);

   if (dbg) fprintf(stdout, "xSQLBase::getIndex(%d):Enter\n", row);
   if (dbg) fflush(stdout);
   if (pViewData == NULL)
   {
      if (dbg) fprintf(stdout, "xSQLBase::getIndex():Abort!\n");
      if (dbg) fflush(stdout);
      return(rv);
   }
#ifndef POSTGRES
   msqlDataSeek(pViewData, row - 1);
   if ((pRow = msqlFetchRow(pViewData)) != NULL)
   {
      msqlFieldSeek(pViewData, 0);
      for (x = 0;; x++)
      {
         if ((pField = msqlFetchField(pViewData)) != NULL)
         {
            if (strcmp(pField->table, getPriTable()) == 0 &&
                strcmp(pField->name, "index") == 0)
            {
               rv = pRow[x];
               break;
            }
	      }
         else
            break;
      }
   }
   else
   {
      msqlDataSeek(pViewData, 0);
   }
#else
   if (PQresultStatus(pViewData) == PGRES_TUPLES_OK &&
       row <= PQntuples(pViewData))
   {
      for (int y = 0; y < PQnfields(pViewData); y++)
      {
         if (strcmp("ident", PQfname(pViewData, y)) == 0)
         {
            if (dbg) fprintf(stdout, "xSQLBase::getIndex():Got it!\n");
            if (dbg) fflush(stdout);
            rv = PQgetvalue(pViewData, row-1, y);
         }
      }
   }
   else
   {
      if (dbg) fprintf(stdout, "xSQLBase::getIndex():Row not found\n");
      if (dbg) fflush(stdout);
   }
#endif POSTGRES
   if (dbg) fprintf(stdout, "xSQLBase::getIndex(%s):Exit\n", (const char*)rv);
   if (dbg) fflush(stdout);
   return(rv);
}

QString xSQLBase::getFieldName(const char *pFieldName)
{
   QString rv;

   for (; *pFieldName && *pFieldName != '.'; pFieldName++);
   if (*pFieldName == '.')
      pFieldName++;
   rv = pFieldName;
   return(rv);
}

QString xSQLBase::getTableName(const char *pFieldName)
{
   const char *cp;
   QString rv("");

   for (cp = pFieldName; *cp && *cp != '.'; cp++)
      rv += *cp;
   return(rv);
}

QString xSQLBase::getPriTable()
{
   QString rv(*tableNames.getFirst());
   return(rv);
}

int xSQLBase::findRow(long index)
{
   int row, rv = 0;
   QString indexName;
   QDict<QString> fields(71);
   
   if (dbg) fprintf(stdout, "xSQLBase::findRow(%ld):Enter\n", index);
   if (dbg) fflush(stdout);
   for (row = 1; rv == 0 && row <= rows(); row++)
   {
      if (dbg) fprintf(stdout, "xSQLBase::findRow():Getting Row %d\n", row);
      if (dbg) fflush(stdout);
      if (getIndex(row).toLong() == index)
         rv = row;
   }
   if (dbg) fprintf(stdout, "xSQLBase::findRow():Exit(%d)\n", rv);
   if (dbg) fflush(stdout);
   return(rv);
}

int xSQLBase::rows()
{
   int rv;

#ifndef POSTGRES
   if (pViewData != NULL)
      rv = msqlNumRows(pViewData);
#else
   if (dbg) fprintf(stdout, "xSQLBase::rows(): pViewData = %u\n", (unsigned int)pViewData);
   if (dbg) fprintf(stdout, "xSQLBase::rows(): pViewData status: %d\n", 
                           PQresultStatus(pViewData));
   if (PQresultStatus(pViewData) == PGRES_TUPLES_OK)
   {
      rv = PQntuples(pViewData);
   }
#endif POSTGRES
   else
      rv = 0;
   return(rv);
}

double xSQLBase::sum(const char *pFieldName)
{
   int row;
   double f, rv = 0.0;
   QDict<QString> fields(71);
   QString *pField;

   if (dbg) fprintf(stdout, "xSQLBase::sum():Enter\n");
   if (dbg) fflush(stdout);
   for (row = 1; row <= rows(); row++)
   {
      fields = getRow(row);
      if ((pField = fields[pFieldName]) != NULL)
      {
         f = pField->toDouble();
         if (dbg) fprintf(stdout, "xSQLBase::sum():Adding %f to %f\n", f, rv);
         if (dbg) fflush(stdout);
         rv += f;
      }
   }
   if (dbg) fprintf(stdout, "xSQLBase::sum():Exit(%f)\n", rv);
   if (dbg) fflush(stdout);
   return(rv);
}

QString xSQLBase::calcField(QDict<QString> &fields, const char *pName,
                            QString *pCalcStr)
{
   static int recurse = 0;
   int iVal1, iVal2;
   double dVal1, dVal2;
   double d = 0.0;
   int i = 0;
   char buf1[256], buf2[256], op[2], *cp;
   char *compField1, *compField2;
   char compOp, type;
   QString rv("Error"), sVal1, sVal2;

   if (dbg) fprintf(stdout, "xSQLBase::calcField:Enter- name = |%s|\n",
                            pName);
   if (dbg) fflush(stdout);
   recurse++;      
   if (sscanf((const char *)*pCalcStr, "%c %s %1[*/+-] %s",
              &type, buf1, op, buf2) == 4)
   {
      compField1 = buf1;
      compField2 = buf2;
      compOp = op[0];
      type = toupper(type);
      
      if (dbg) fprintf(stdout, "xSQLBase::calcField:Field1 = |%s|, field2 = |%s|, Op = %c, type = %c\n",
                               buf1, buf2, compOp, type);
      if (dbg) fflush(stdout);
      if (type == 'I')
      {
         if ((const char *)*fields[compField1] == NULL)
            fprintf(stderr, "xSQLBase::calcField: For field %s: Field %s not Found\n",
                            pName, (const char *)compField1);
         else if ((const char *)*fields[compField2] == NULL) 
            fprintf(stderr, "xSQLBase::calcField: For field %s: Field %s not Found\n",
                            pName, (const char *)compField2);
         else
         {
            iVal1 = fields[compField1]->toInt();
            iVal2 = fields[compField2]->toInt();
      
            if (dbg) fprintf(stdout, "xSQLBase::calcField:Have integer- \"%s %c %s\n",
                                      (const char *)compField1,
                                      compOp,
                                      (const char *)compField2);
            if (dbg) fprintf(stdout, "xSQLBase::calcField:Val1 = %d, val2 = %d\n",
                                    iVal1, iVal2);
            if (dbg) fflush(stdout);
            if (compOp == '+')
               i = iVal1 + iVal2;
            else if (compOp == '-')
               i = iVal1 - iVal2;
            else if (compOp == '*')
               i = iVal1 * iVal2;
            else if (compOp == '/')
               i = iVal1 / iVal2;
            else if (compOp == '^')
               i = iVal1 ^ iVal2;
            if (dbg) fprintf(stdout, "xSQLBase::calcField:i = %d\n", i);
            if (dbg) fflush(stdout);
            rv.sprintf("%d", i);
         }
      }
      else if (type == 'F' || type == 'C')
      {
         if ((const char *)*fields[compField1] == NULL)
            fprintf(stderr, "xSQLBase::calcField: For field %s: Field %s not Found\n",
                            pName, (const char *)compField1);
         else if ((const char *)*fields[compField2] == NULL) 
            fprintf(stderr, "xSQLBase::calcField: For field %s: Field %s not Found\n",
                            pName, (const char *)compField2);
         else
         {
            if (dbg) fprintf(stdout, "xSQLBase::calcField:Have Float- \"%s = |%s| %c %s = |%s|\"\n",
                                      (const char *)compField1, (const char *)*fields[compField1],
                                      compOp,
                                      (const char *)compField2, (const char *)*fields[compField2]);
            if (dbg) fflush(stdout);
            sVal1 = *fields[compField1];
            sVal2 = *fields[compField2];
            sVal1 = sVal1.replace(QRegExp("[\\$,]"),"");
            sVal2 = sVal2.replace(QRegExp("[\\$,]"),"");
            dVal1 = sVal1.toDouble();
            dVal2 = sVal2.toDouble();
            if (dbg) fprintf(stdout, "xSQLBase::calcField:Val1 = %f, val2 = %f\n",
                                     dVal1, dVal2);
            if (dbg) fflush(stdout);
            if (compOp == '+')
               d = dVal1 + dVal2;
            else if (compOp == '-')
               d = dVal1 - dVal2;
            else if (compOp == '*')
               d = dVal1 * dVal2;
            else if (compOp == '/')
               d = dVal1 / dVal2;
            if (dbg) fprintf(stdout, "xSQLBase::calcField:d = %f\n", d);
            if (dbg) fflush(stdout);
            if (type == 'F')
               rv.sprintf("%f", d);
            else
               rv.sprintf("%.2f", d);
         }
      }
   }
   else if (sscanf((const char *)*pCalcStr, "%c %s %s",
                   &type, buf1, buf2) == 3)
   {
      if (recurse <= 1)
      {
         for (cp = buf1; *cp; cp++)
            *cp = toupper(*cp);
         if (strcmp(buf1, "SUM") == 0)
         {
            d = sum(buf2);
            if (type == 'I')
               rv.sprintf("%d", (int)d);
            else if (type == 'F')
               rv.sprintf("%f", d);
            else if (type == 'C')
               rv.sprintf("%.2f", d);
            else
               fprintf(stderr, "xSQLBase::calcField:Invalid Computation Field Type %s:\"%c\"\n",
                                pName, type);
         }
         else
            fprintf(stderr, "xSQLBase::calcField:Invalid Computation Field Function %s:\"%s\"\n",
                             pName, buf1);
      }
      else
         rv = "0.0";
   }   
   else
      fprintf(stderr, "xSQLBase::calcField:Computation Field Syntax %s:\"%s\"\n",
                       pName, (const char *)pCalcStr);
   if (dbg) fprintf(stdout, "xSQLBase::calcField:Exit(%s = \"%s\")\n",
                            pName, (const char *)rv);
   if (dbg) fflush(stdout);
   recurse--;
   return(rv);
}

QString xSQLBase::escString(const char *pStr)
{
   QString rv;

   for (; pStr != NULL && *pStr; pStr++)
   {
      switch(*pStr)
      {
         case '\?':
         case '\%':
         case '\\':
         case '\'':
            rv += '\\';
            break;
      }
      rv += *pStr;
   }
   return(rv);
}

int xSQLBase::fieldLen(int db, const char *pFName, const char *pTName)
{
   int rv;

   if (dbg) fprintf(stdout, "xSQLBase::fieldLen():Enter\n");
   if (dbg) fflush(stdout);
#ifndef POSTGRES
   m_result *fields;
   m_field *field;

   fields = msqlListFields(db, pTName);
   if (dbg) fprintf(stdout, "xSQLBase::fieldLen():Got Fields\n");
   if (dbg) fflush(stdout);
   for (msqlFieldSeek(fields, 0); 
        (field = msqlFetchField(fields)) != NULL;)
   {
      if (dbg) fprintf(stdout, "xSQLBase::fieldLen():Comparing |%s| to |%s|\n",
                               field->name, pFName);
      if (dbg) fflush(stdout);
      if (dbg) fflush(stdout);
      if (strcmp(field->name, pFName) == 0)
         break;
   }
   if (field != NULL)
      rv = field->length;
   else
      rv = -1;
   msqlFreeResult(fields);
#else
   QString q(2500);
   QString attrelid;
   q.sprintf("select attlen,atttypid from pg_attribute,pg_class where pg_class.relname='%s' "
             "and attname='%s' and attrelid=pg_class.oid", 
              pTName, pFName);
   if (dbg) fprintf(stdout, "xSQLBase::fieldLen():DB query: |%s|\n", (const char *)q);
   if (dbg) fflush(stdout);
   PGresult *r = PQexec((PGconn*)db, q);
   if (PQresultStatus(r) == PGRES_TUPLES_OK && PQntuples(r) > 0)
   {
      const char *cp = PQgetvalue(r, 0, 0);
      if (dbg) fprintf(stdout, "xSQLBase::fieldLen():Field len = %s\n", cp);
      if (dbg) fflush(stdout);
      rv = atoi(cp) - 4;
   }
   else
      rv = -1;
   PQclear(r);
#endif POSTGRES
   if (dbg) fprintf(stdout, "xSQLBase::fieldLen():Exit, len = %d\n", rv);
   if (dbg) fflush(stdout);
   return(rv);
}

int xSQLBase::openDB(const char *pHost, const char *pPort, const char *pName)
{
   int rv;

#ifndef POSTGRES
   pPort++;
   rv = msqlConnect(pHost);
   if (msqlSelectDB(rv, pName) < 0)
      rv = -1;
#else
   rv = (int)PQsetdb(pHost, pPort, NULL, NULL, pName);
   if (PQstatus((PGconn*)rv) == CONNECTION_BAD)
   {
      PQfinish((PGconn*)rv);
      rv = -1;
   }
#endif POSTGRES
   return(rv);
}

void xSQLBase::closeDB(int db)
{
#ifndef POSTGRES
   db++;
   return;
#else
   PQfinish((PGconn*)db);
   return;
#endif POSTGRES
}

const char *xSQLBase::dbError(int db)
{
#ifndef POSTGRES
   db++;
   return(msqlErrMsg);
#else
   return(PQerrorMessage((PGconn*)db));
#endif POSTGRES
}

