/***************************************************************************
**    xGauge.h  $Revision: 1.2 $ - $Name:  $ 
**    Progress Indicator, looks sort of like a thermometer
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
#ifndef _XGAUGE_H
#define _XGAUGE_H

#include <qaccel.h>
#include <qrangecontrol.h>
#include "xMouseEvent.h"
#include "xResources.h"

class xGauge : public QWidget, public QRangeControl
{
   Q_OBJECT

public:
   enum Orientation { Vertical, Horizontal };

   xGauge(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
          const char *pName = NULL);
   xGauge(xWidgetResInfo *pPRes, Orientation orientation, 
          QWidget *pParent = NULL, const char *pName = NULL);
   void setLineThickness(int t);

protected:
//   void resizeEvent(QResizeEvent *pEvt);
   void paintEvent(QPaintEvent *pEvt);
   void paintGaugeVert(QPainter *p, QRect r);

   void valueChanged();
   void rangeChanged();
   void stepChanged();

private:
   void xGauge::initGauge(xWidgetResInfo *pPRes);

   int         thick;
   Orientation orient;
   QColor      baseHighlight;
   xWidgetResInfo *wdtRes;
};

#endif
