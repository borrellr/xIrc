/***************************************************************************
**    xSQLView.cpp  $Revision: 1.1.1.1 $ - $Name:  $ 
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
#ifndef _XSQLVIEW_H
#define _XSQLVIEW_H

#include <qaccel.h>
#include <qdict.h>
#include <qlabel.h>
#include <qstring.h>
#include "xFrame.h"
#include "xResources.h"
#include "xSQLField.h"

class xSQLView : public xFrame, public xSQLBase
{
   Q_OBJECT

public:
   xSQLView::xSQLView(xWidgetResInfo *pRes, QWidget *pParent,
                      const char *pName, int db, xSQLFieldDef *pFields,
                      const char *pTitleStr = NULL);
   virtual xSQLView::~xSQLView();

   virtual void setEnabled(bool f = TRUE);
   virtual void setEnabled(const char *pFieldName, bool f = TRUE);
   virtual bool validate(QString &err);
   virtual bool validate(const char *pFieldName);
   virtual bool validate(const char *pFieldName, const char *pData);
   virtual xSQLField *getWidget(const char *pFieldName);
   virtual QString getField(const char *pFieldName, bool editFirst = TRUE);
   virtual QDict<QString> getEditFields(bool blankFields = FALSE);
   virtual void clearEditFields();

   void showRow(int row = 1);

signals:
   void entryComplete();
   void invalidEntry(xSQLField*);
   void abortEntry();

protected slots:
   virtual void haveMouseDoubleClicked(xMouseEvent*);
   virtual void haveMousePressed(xMouseEvent*);
   virtual void haveNewData(xSQLField*);
   virtual void haveInvalidEntry(xSQLField*);
   virtual void abandonEntry(xSQLField*);

private:
   int                     curRow;
   xFrame                  *pLabelFrame;
   xFrame                  *pFieldFrame;
   xFrame                  *pFrame;
   QLabel                  *pTitle;
   xWidgetResInfo          *wdtRes;
};

#endif
