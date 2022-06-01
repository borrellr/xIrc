/***************************************************************************
**    xSQLBase.cpp  $Revision: 1.1.1.1 $ - $Name:  $ 
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
#ifndef _XSQLBASE_H
#define _XSQLBASE_H

#include <qobject.h>
#include <qstring.h>
#include <qlist.h>
#include <qdict.h>
#ifndef POSTGRES
#include "xMsql.h"
#else
#include <libpq-fe.h>
#include <libpq/libpq-fs.h>
#endif

class xSQLField;

typedef enum 
{
   sqlPlain = 0, sqlPhone, sqlName, sqlCurrency, sqlInteger,
   sqlFloat, sqlZip, sqlDate, sqlComputation, sqlYesNo,
   sqlHidden = 128
} xSQLFieldType;

typedef struct 
{
   xSQLFieldType  type;
   const char     *name;
   const char     *label;
   const char     *link;
   int            len;
   bool           enabled;
} xSQLFieldDef;

class xSQLBase
{
public:
   xSQLBase(xSQLFieldDef *fields);
   virtual ~xSQLBase();

   virtual bool query(int db, QList<QString> sort);
   virtual bool query(int db, QString index, QList<QString> sort);
   virtual bool query(int db, long index, QList<QString> sort);
   virtual bool query(int db, QDict<QString> &match, QDict<QString> &links,
                      QList<QString> sort);
   virtual bool query(int db, QDict<QString> &match, QDict<QString> &links,
                      QDict<QString> &skip, QList<QString> sort);
   virtual bool del(int db, QString index);
   virtual bool del(int db, long index);
   virtual bool del(int db, QDict<QString> &match, QDict<QString> &links);
   virtual long insert(int db, QDict<QString> &fields);
   virtual bool update(int db, QDict<QString> &fields, QDict<QString> &links);
   virtual bool update(int db, QDict<QString> &fields, QString index);
   virtual bool update(int db, QDict<QString> &fields, long index);
   virtual QDict<QString> getRow(int row);
   virtual QDict<QString> getRow(long index);
   virtual bool validate(QString) { return(TRUE); };
   virtual bool validate(const char *) { return(TRUE); };
   virtual bool validate(const char *, const char *) { return(TRUE); };
   virtual xSQLField *getWidget(const char *) { return((xSQLField*)NULL); };
   virtual QString getField(const char *, bool) { return(QString("")); };
   virtual QDict<QString> getEditFields() { return(QDict<QString>(71)); };
   virtual void clearEditFields() {};
   virtual void clearEditFields(int) {};
   virtual void clearEntryFields() {};

   QString getIndex(int row = 1);
   int findRow(long index);
   int rows();
   QString getPriTable();
   virtual double sum(const char *pFieldName);
   QString getTableName(const char *pFieldName);
   QString getFieldName(const char *pFieldName);
   QString escString(const char *pStr);
   int fieldLen(int db, const char *pFName, const char *pTName);

   static int openDB(const char *pHost, const char *pPort, const char *pName);
   static void closeDB(int db);
   static const char *dbError(int db);

protected:
   QString calcField(QDict<QString> &fields, const char *pName,
                     QString *pCalcStr);

private:
   QString                 prevQuery;
   QList<QString>          tableNames;
   QList<QString>          fieldNames;
   QDict<QString>          fieldLinks;
   QDict<QString>          calcFields;
   QList<QString>          calcNames;
#ifndef POSTGRES
   m_result                *pViewData;
#else
   PGresult                *pViewData;
#endif
};

#endif
