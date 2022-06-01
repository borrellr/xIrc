/***************************************************************************
**    xMultiLineEdit.h  $Revision: 1.2 $ - $Name:  $ 
**    QMultiLineEdit Replacement
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
#ifndef _XMULTILINEEDIT_H
#define _XMULTILINEEDIT_H

#include <qaccel.h>
#include <qmultilinedit.h>
#include "xMouseEvent.h"
#include "xResources.h"

class xMultiLineEdit : public QMultiLineEdit
{
   Q_OBJECT

public:
   xMultiLineEdit(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
          const char *pName = NULL);
   void setWidth(int width);
   void setHeight(int h);
   void setEnabled(bool f);
   void setFrameStyles(int enabled, int disabled);
   
private:
   QPalette       paletteEnabled;
   QPalette       paletteDisabled;
   QPalette       paletteHighlight;
   bool           enabled;
   int            frameStyleEnabled;
   int            frameStyleDisabled;
   xWidgetResInfo *wdtRes;
};

#endif

