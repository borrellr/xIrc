/***************************************************************************
**    xEdit.h  $Revision: 1.2 $ - $Name:  $ 
**    QEdit w/ Macro Replacement
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
#ifndef _x_EDIT_H
#define _x_EDIT_H

#include <qaccel.h>
#include <qlineedit.h>
#include "xMouseEvent.h"
#include "xResources.h"

class xEdit : public QLineEdit
{
   Q_OBJECT

public:
   xEdit(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
          const char *pName = NULL);
   virtual const char *text();
   void setWidth(int width);
   void setEnabled(bool f);
   void setHighlight(bool f);
   void setFrameStyles(int enabled, int disabled);
   
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
   void escapePressed();

private:
   QString     macroReplace(const char *cp);
   QString     txtStr;
   QPalette    paletteEnabled;
   QPalette    paletteDisabled;
   QPalette    paletteHighlight;
   bool        enabled;
   bool        highlight;
   int         frameStyleEnabled;
   int         frameStyleDisabled;
   xWidgetResInfo *wdtRes;
};

#endif

