/***************************************************************************
**    xDialog.h  $Revision: 1.2 $ - $Name:  $ 
**    Self fitting Frame class
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
#ifndef _XFRAME_H
#define _XFRAME_H

#include <qdialog.h>
#include <qdict.h>
#include <qframe.h>
#include <qlist.h>
#include <qwidget.h>
#include "xResources.h"

#define  xALIGN_Vert         0
#define  xALIGN_Horz         1

#define  xSPACE_ToFit        0
#define  xSPACE_Resize       1

#define  xPOS_Right          0
#define  xPOS_Left           1
#define  xPOS_Below          0
#define  xPOS_Above          1

#define  xCOUPLE_Left        0
#define  xCOUPLE_Right       1
#define  xCOUPLE_Top         0
#define  xCOUPLE_Bottom      1

class xDialog;

class xWdtSpec
{
public:
   xWdtSpec(QWidget *pWdt1, int _side)
   {
      pWdt = pWdt1;
      side = _side;
   }     
   QWidget  *pWdt;
   int      side;
};

class xFrame : public QFrame
{
   Q_OBJECT

   friend class xDialog;
      
public:
   xFrame(xWidgetResInfo *pPRes, QWidget *pParent = NULL, 
          const char *pName = NULL);
   xFrame(xWidgetResInfo *pPRes, int align = xALIGN_Vert,
          int resz = xSPACE_ToFit, int space = 10, QWidget *pParent = NULL,
          const char *pName = NULL);
   ~xFrame();
   
   void setAutoDelete(bool f);
   int addWidget(QWidget *pWidget, QWidget *pNeighbor = NULL,
             int relativePos = xPOS_Right);
   void removeWidget(QWidget *pWidget);
   void setAlignment(int alignment = xALIGN_Vert);
   void setWidgetSpacing(int space = 0);
   void setWidgetEqualize(bool eq) { widgetEqualize = eq; };
   void setMargins(int vertMargin = 10, int horzMargin = 10);
   void setResizeMode(int reszMode) { widgetResz = reszMode; };
   void margins(int& vertMargin, int& horzMargin);
   int widgetSpacing() { return(widgetSpac); };
   virtual void fitFrame(const QSize *frameSize = NULL, bool force = FALSE);
   virtual void resize(QSize *frameSize);
   int resizeMode() { return(widgetResz); };
   void coupleWidgets(QWidget *pWdt1, QWidget *pWdt2, int side);
   
private:
   QList<QWidget> widgetList;
   QDict<xWdtSpec> couplings;
   bool     fitted;
   int      widgetAlgn;
   int      widgetSpac;
   int      widgetListLength;
   int      widgetCount;
   int      widgetVMarg;
   int      widgetHMarg;
   int      widgetResz;
   bool     widgetEqualize;
   int      fitFrameRecurse;
   int      fc;
   xWidgetResInfo *wdtRes;

protected:
   void resizeEvent(QResizeEvent *pEvt);
   void doubleClickEvent(QMouseEvent *pEvt);
/*
   virtual void mousePressEvent(QMouseEvent *);
   virtual void mouseReleaseEvent(QMouseEvent *);
   virtual void mouseDoubleClickEvent(QMouseEvent *);
   virtual void mouseMoveEvent(QMouseEvent *);
   virtual void keyPressEvent(QKeyEvent *);
   virtual void keyReleaseEvent(QKeyEvent *);
*/
};

#endif


