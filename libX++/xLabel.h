/***************************************************************************
**    xLabel.h  $Revision: 1.1.1.1 $ - $Name:  $ 
**    QLabel
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
#ifndef _XLABEL_H
#define _XLABEL_H

#include <qlabel.h>
#include "xMouseEvent.h"
#include "xResources.h"

class xLabel : public QLabel
{
   Q_OBJECT

public:
   xLabel(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
          const char *pName = NULL);
   xLabel(xWidgetResInfo *pPRes, const char *pText, QWidget *pParent = NULL,
          const char *pName = NULL);
   void setWidth(int width);

protected:
   virtual void mousePressEvent(QMouseEvent *pEvt);
   virtual void mouseReleaseEvent(QMouseEvent *pEvt);
   virtual void mouseDoubleClickEvent(QMouseEvent *pEvt);
   virtual void mouseMoveEvent(QMouseEvent *pEvt);

signals:
   void mousePressed(xMouseEvent *pEvt);
   void mouseReleased(xMouseEvent *pEvt);
   void mouseDoubleClicked(xMouseEvent *pEvt);
   void mouseMoved(xMouseEvent *pEvt);

private:
   xWidgetResInfo *wdtRes;
};

#endif

