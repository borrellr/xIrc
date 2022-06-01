/***************************************************************************
**    xSQLTable.cpp  $Revision: 1.2 $ - $Name:  $ 
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
#ifndef _XSQLTABLE_H
#define _XSQLTABLE_H

#include <qaccel.h>
#include <qdict.h>
#include <qlabel.h>
#include <qscrollbar.h>
#include <qstring.h>
#include "xFrame.h"
#include "xResources.h"
#include "xSQLField.h"
#include "xTable.h"

class xSQLTable : public xFrame, public xSQLBase
{
   Q_OBJECT

public:
   xSQLTable(xWidgetResInfo *pPRes, QWidget *pParent, const char *pName,
             int db, xSQLFieldDef *pFields, const char *pTitleStr = NULL,
             int rows = 10);
   ~xSQLTable();

   virtual bool query(int db, QList<QString> sort);
   virtual bool query(int db, QString index, QList<QString> sort);
   virtual bool query(int db, long index, QList<QString> sort);
   virtual bool query(int db, QDict<QString> &match, QDict<QString> &links,
                      QList<QString> sort);
   virtual bool query(int db, QDict<QString> &match, QDict<QString> &links,
                      QDict<QString> &skip, QList<QString> sort);
   virtual bool validate(QString &err);
   virtual bool validate(const char *pFieldName);
   virtual bool validate(const char *pFieldName, const char *pData);
   virtual xSQLField *getWidget(const char *pFieldName);
   virtual QString getField(const char *pFieldName, bool editFirst = TRUE);
   virtual QDict<QString> getEditFields(bool blankFields = FALSE);
   virtual void clearEditFields();
   virtual void clearEditFields(int row);
   virtual void clearEntryFields();

   QDict<QString> getRow();
   long insertBlank(int db, int row);
   void setFrameStyles(int enabled, int disabled);
   void showRows();
   void showRows(int row);
   bool enableRow(int row, bool f);
   int selectedRow() { return(selRow); };
   void setSelectedRow(int row) { selRow = row; showRows(); };
   int topRowOfTable() { return(curRow); };
   void setTopRowOfTable(int row) { curRow = row; };
   int tableHeight() { return(pFieldTable->rows()); };
   void setEnableMargin(int n) { enableMargin = n; };

signals:
   void rowClicked(int row);
   void rowDoubleClicked(int row);
   void entryComplete(int row);
   void invalidEntry(xSQLField*);
   void abortEntry(int row);

public slots:
   virtual void abandonEntry(xSQLField*);
   virtual void haveNewData(xSQLField*);
   virtual void haveInvalidEntry(xSQLField*);

   virtual void haveMousePressed(xMouseEvent*);
   virtual void haveMouseDoubleClicked(xMouseEvent*);

   virtual void highlightTopRow();
   virtual void highlightRow(int idx);
   virtual void highlightNext();
   virtual void highlightPrev();

   virtual void scroll(int idx);
   virtual void scrollBack();
   virtual void scrollFwd();
   virtual void scrollPageBack();
   virtual void scrollPageFwd();

private:
   void setSB();

   xTable                  *pLabelTable;
   xTable                  *pFieldTable;
   xFrame                  *pFrame;
   xFrame                  *pFieldFrame;
   QScrollBar              *pSB;
   QLabel                  *pTitle;
   QAccel                  *pAccel;
   int                     curRow;
   int                     selRow;
   int                     enbRow;
   int                     insRow;
   bool                    insMode;
   bool                    connected;
   int                     enableMargin;
   xWidgetResInfo          *wdtRes;
};

#endif
