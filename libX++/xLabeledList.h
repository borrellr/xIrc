/***************************************************************************
**    xLabeledList.h  $Revision: 1.1 $ - $Name:  $
**    QListBox replacement, just because
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
#ifndef _xLABELEDLIST_H
#define _xLABELEDLIST_H

#include <qglobal.h>
#include <qkeycode.h>
#include <qlistbox.h>
#include <stdio.h>
#include "xFrame.h"
#include "xLabel.h"
#include "xListBox.h"
#include "xResources.h"

class xLabeledList : public xFrame
{
   Q_OBJECT

public:
   xLabeledList(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
            const char *pName = NULL, int alignLabel = xALIGN_Vert);
   ~xLabeledList();
   xListBox *listBox() { return(pList); };
   void setLabel(const char *pText);
   void fitFrame(QSize *frameSize);
   void setSize(int width, int height);
   int width();

private:
   xListBox       *pList;
   xLabel         *pLabel;
   xWidgetResInfo *wdtRes;
   int            eHeight;
   int            eWidth;
   xWidgetResInfo *wdtResPrv;

};

#endif
