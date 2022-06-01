/***************************************************************************
**    xInputFrame.h  $Revision: 1.2 $ - $Name:  $ 
**    QFrame w/ xMultiLineFrame & xLEdit widgets
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
#ifndef _xINPUTFRAME_H
#define _xINPUTFRAME_H

#include <qlineedit.h>
#include <qlabel.h>
#include <qmenubar.h>
#include "xMultiLineFrame.h"
#include "xResources.h"
#include "xDefaults.h"

class xLEdit : public QLineEdit
{
   Q_OBJECT

public:
   xLEdit(xWidgetResInfo *pPRes, QWidget *parent=0, const char *name=0) :
      QLineEdit(parent, name)
   {
      wdtRes = new xWidgetResInfo(pPRes, QString("input"), QString("Input"));
   };

signals:
   void gotInput(char *pText);

protected:
   virtual void keyPressEvent(QKeyEvent *pEvt);

private:
   xWidgetResInfo *wdtRes;
};

class xInputFrame : public QFrame
{
   Q_OBJECT

public:
   xInputFrame(xWidgetResInfo *pPRes, QWidget *parent=0, const char *name=0,
               WFlags iFlags = 0,
               int width = 80, int height = 25,
               int maxLines = 50);
   ~xInputFrame();
   void attachMenu(QMenuBar *pMenu);
   void setTitle(char *pText);
   
   xLEdit          *pEdit;
   QLabel           *pTitle;
   QMenuBar         *pMenuBar;
   xMultiLineFrame *pTerm;

protected:
   virtual void resizeEvent(QResizeEvent *pEvt);

private:
   xWidgetResInfo *wdtRes;
};

#endif
