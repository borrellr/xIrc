/***************************************************************************
**    xLineEditQuery.h  $Revision: 1.1.1.1 $ - $Name:  $ 
**    Dialog box w/ xLineEdit and xPshBtnFrame widgets
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
#ifndef _xLINEEDITQUERY_H
#define _xLINEEDITQUERY_H

#include <qstring.h>
#include "xLineEdit.h"
#include "xPshBtnFrame.h"
#include "xResources.h"
#include "xDefaults.h"
#include "xDialog.h"

class xLineEditQuery : public xDialog
{
   Q_OBJECT
   
public:   
   xLineEditQuery(xWidgetResInfo *pPRes, QWidget *pParent = NULL, 
                  const char *pName = NULL,
                  const char *pTag = NULL, xDefaults *pDef = NULL);
   xLineEditQuery(xWidgetResInfo *pPRes, const char *pText = NULL,
                  const char *pEdtText = NULL, 
                  QWidget *pParent = NULL, const char *pName = NULL,
                  const char *pTag = NULL, xDefaults *pDef = NULL);
   ~xLineEditQuery();
   void initClass(const char *pText, const char *pEdtText, 
                  QWidget *pParent, const char *pName,
                  const char *pTag = NULL, xDefaults *pDef = NULL);
                           
   const char *editText() { return(pEditField->text()); };
   void setText(const char *pTxt) { pEditField->setText(pTxt); };
   const char *text() { return(pEditField->text()); };
   void setDefault();
   
protected slots:
   virtual void done(int results) { QDialog::done(results); };
   void reject();
   void accept();
   virtual void gotReturn();
   virtual void buttonClicked(int iBtn);

private:   
   xLineEdit         *pEditField;
   xPshBtnFrame      *pButtons;
   QString           tag;
   xDefaults         *pDefaults;
   xWidgetResInfo    *wdtRes;
};

#endif
