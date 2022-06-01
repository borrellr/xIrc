/***************************************************************************
**    xLineEdit.h  $Revision: 1.1.1.1 $ - $Name:  $ 
**    xFrame w/ xEdit and QLabel widgets
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
#ifndef _x_EDITFRAME_H
#define _x_EDITFRAME_H

// #include <qlined.h>
#include <qlabel.h>
#include "xEdit.h"
#include "xResources.h"
#include "xDefaults.h"
#include "xDialog.h"

#define xLBL_Left   1
#define xLBL_Right  2

class xLineEdit : public xFrame
{
   Q_OBJECT
   
public:
   xLineEdit(xWidgetResInfo *pPRes, QWidget *pParent = NULL, 
             const char *pName = NULL, int alignLabel = xALIGN_Vert);
          
   xLineEdit(xWidgetResInfo *pPRes, xDefaults *pDef, QWidget *pParent = NULL,
              const char *pName = NULL, int alignLabel = xALIGN_Vert);
/*
   xLineEdit(const char *pLabel = NULL, const char *pEditText, 
          QWidget *pParent = NULL, const char *pName = NULL,
          int align = xLBL_Left);
*/
   ~xLineEdit();
   
   void  setText(const char *pStr) { pEdit->setText(pStr); };
   void  setLabel(const char *pStr);

   const char *text()   { return(pEdit->text()); };
   const char *label()  { return(pLabel->text()); };
   void  setEnabled(bool f);   
   void setEditWidth(int width);
   int editWidth();
   virtual void fitFrame(QSize *frameSize = NULL);

signals:
   void returnPressed();
   void escapePressed();
   void tabPressed();
   
protected slots:
   virtual void gotReturn() { emit returnPressed(); };
   virtual void gotEscape() { emit escapePressed(); };

protected:
   xEdit      *pEdit;
   QLabel      *pLabel;

private:
   int            eWidth;
   xDefaults      *pDefaults;
   xWidgetResInfo *wdtRes;
   xWidgetResInfo *wdtResPrv;
};

#endif
