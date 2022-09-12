/***************************************************************************
**    xSQLField.h $Revision: 1.1.1.1 $ - $Name:  $ 
**    SQL Edit Field
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
#ifndef _XSQLFIELD_H
#define _XSQLFIELD_H

#include <qevent.h>
#ifndef POSTGRES
#include "xMsql.h"
#else
#include <libpq-fe.h>
#include <libpq/libpq-fs.h>
#endif POSTGRES
#include "xResources.h"
#include "xSQLBase.h"
#include "xEdit.h"

class xSQLField : public xEdit
{
   Q_OBJECT

public:
   xSQLField(xWidgetResInfo *pPRes, QWidget *pParent, xSQLBase *pSqlBase,
            int db, xSQLFieldDef *field);
   virtual ~xSQLField();

   QString tableName();
   QString fieldName();
#ifndef POSTGRES
   bool isKey();
   bool canBeNull();
#endif POSTGRES
   void setValidations(QList<QString> &list);
   int editWidth() { return(edtWidth); };
   void setEditWidth(int eWdth);
   void setName(const char *pName);
   virtual bool valid();  
   virtual bool validate(const char *pStr);  
   virtual bool compare(const char *pStr);  
   virtual bool compare(QString &str);
   QString label() { return(labelStr); };
   const char *text();
   void setText(const char *pText);
   const char *defaultText();
   xSQLFieldType fieldType()
   { 
      return( (xSQLFieldType)((int)type & ~((int)sqlHidden)) );
   };

   void setAutoSelect(bool f);
   bool isComputation();
   void setEnabled(bool f);
   bool isHidden() { return((type & sqlHidden) ? TRUE : FALSE); };

signals:
   void entryAbandoned(xSQLField*);
   void newData(xSQLField*);
   void invalidEntry(xSQLField*);

protected slots:
   virtual void haveReturn();

protected:
   virtual void keyPressEvent(QKeyEvent *pEvt);
   virtual void focusInEvent(QFocusEvent *pEvt);
   virtual void focusOutEvent(QFocusEvent *pEvt);

private:
   void setDefValidation(int t);
   
   QList<QString>    validateList;
   QString           labelStr;
   bool              autoSelect;
   int               flags;
   int               edtWidth;
   int               db;
   int               len;
   xSQLFieldType     type;
   xSQLBase          *pBase;
   QString           compField1;
   QString           compField2;
   int               compOp;
   xWidgetResInfo    *wdtRes;
   xWidgetResInfo    *wdtValidate;
};

#endif
