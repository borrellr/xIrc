/***************************************************************************
**    xDialog.h  $Revision: 1.3 $ - $Name:  $ 
**    Self fitting Dialog and Frame classes
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
#ifndef _xDIALOG_H
#define _xDIALOG_H

#include <qdialog.h>
#include <qframe.h>
#include <qwidget.h>
#include "xFrame.h"
#include "xResources.h"

class xDialog : public QDialog
{
   Q_OBJECT

public:
#ifdef QT2
   xDialog(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
           const char *pName = NULL, bool modal = FALSE, WFlags f = 0);
#else
   xDialog(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
           const char *pName = NULL, bool modal = FALSE);
#endif
   xDialog(xWidgetResInfo *pPRes, xFrame *pFrame = NULL,
           QWidget *pParent = NULL, const char *pName = NULL);
   ~xDialog();
   
   xFrame  *mainFrame;

protected slots:
   void grabFocus();

private:
   bool     needsDeleting;
   bool     af;
   int      resizeRecurse;
   xWidgetResInfo *wdtRes;

public:   
   int addWidget(QWidget *pWidget, QWidget *pNeighbor = NULL, int relativePos = xPOS_Right);
   void removeWidget(QWidget *pWidget);
   void setAlignment(int alignment = xALIGN_Vert);
   void setWidgetSpacing(int space = 0);
   void setMargins(int vertMargin = 10, int horzMargin = 10);
   void margins(int& vertMargin, int& horzMargin);
   void setResizeMode(int reszMode);
   int resizeMode() { return(mainFrame->resizeMode()); };
   int widgetSpacing() { return(mainFrame->widgetSpacing()); };
   void fitFrame(const QSize *frameSize = NULL, bool force = FALSE);
   void initFrame(const QSize *frameSize = NULL);
   virtual void setPalette(const QPalette &p);
   virtual void show();
   void hide();
   
protected:
   virtual void resize(QSize *frameSize);
   virtual void resizeEvent(QResizeEvent *pEvt);
   virtual void doubleClickEvent(QMouseEvent *pEvt);
};

#endif

