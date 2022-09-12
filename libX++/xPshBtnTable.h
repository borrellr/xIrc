/***************************************************************************
**    xPshBtnFrame.h  $Revision: 1.1.1.1 $ - $Name:  $ 
**    xFrame forhandling multiple buttons
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
#ifndef _xPSHBUTTONTABLE_H
#define _xPSHBUTTONTABLE_H

#include <qptrlist.h>
#include "xDialog.h"
#include "xResources.h"
#include "xPshBtnFrame.h"

class xPshBtnTable : public xFrame
{
   Q_OBJECT
   
public:
   xPshBtnTable(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
                const char *pName = NULL);
   ~xPshBtnTable();

   void     addButton(xPshBtn *button, int id);
   void     addButton(xPshBtn *button);
   xPshBtn  *addButton(const char *pBtnName, int id);
   void     rmvButton(int value);
   void     arrangeButtons(int cols = 0);
   
signals:
   void clicked(int id);

protected slots:
   void buttonClicked(int id);
   
private:
   QPtrList<xPshBtn>       btnList;
   QPtrList<xPshBtnFrame>  btnFrameList;
   xWidgetResInfo *wdtRes;
   xWidgetResInfo *wdtResPrv;
};

#endif
