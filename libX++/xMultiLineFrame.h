/***************************************************************************
**    xMultiLineFrame.h  $Revision: 1.2 $ - $Name:  $ 
**    QFrame for manipulating Multiple lines of text in a window
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
#ifndef _xMultiLineFrame_H
#define _xMultiLineFrame_H

#include <qscrollbar.h>
#include <qframe.h>
#include "xResources.h"
#include "xMultiLineWin.h"

typedef struct
{
   QString  winName;
   QString  text;
   int      iData;
   void     *pData;
} xMultiLineTextSelection;

class xVertScrollBar : public QScrollBar
{
   Q_OBJECT
   
public:
   xVertScrollBar(xWidgetResInfo *pPRes, QWidget *pParent = NULL, 
                  const char *pName = NULL);

private:
   int      actualRange;
   xWidgetResInfo *wdtRes;
};

class xMultiLineFrame : public QFrame
{
   Q_OBJECT

public:
   xMultiLineFrame(xWidgetResInfo *pPRes, QWidget *pParent = NULL, 
                   const char *pName = NULL,
                   int width = 80, int height = 25,
                   int maxLines = 300);
   ~xMultiLineFrame();
   
   xMultiLineWin    *pWin;
   xVertScrollBar   *pScrollBar;
   virtual void     mouseDoubleClickEvent(QMouseEvent *pEvt);

signals:   
   void textSelected(xMultiLineTextSelection txt);

public slots:
   void changePageStep(int pageValue);
   void changeSBRange(int lines);
   void gotoOffset(int offset);
   void gotoPrevPage();
   void gotoNextPage();
   void gotoPrevLine();
   void gotoNextLine();

protected slots:
   void haveTextSelection(QString);

private:
   int               widthSB;          // Width of Scroll Bar (set in constructor)
   virtual void resizeEvent(QResizeEvent *pEvt);
   xWidgetResInfo    *wdtRes;
   xWidgetResInfo    *wdtPrv;
};

#endif
