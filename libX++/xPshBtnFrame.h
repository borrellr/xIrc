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
#ifndef _xPSHBUTTONFRAME_H
#define _xPSHBUTTONFRAME_H

#include <qptrlist.h>
#include "xResources.h"
#include "xDialog.h"
#include "xPshBtn.h"

class xPshBtnFrame : public xFrame
{
   Q_OBJECT
   
public:
   xPshBtnFrame(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
                const char *pName = NULL,
                bool autoDelete = TRUE);
   ~xPshBtnFrame();

   void     addButton(xPshBtn *button, int id);
   void     addButton(xPshBtn *button);
   xPshBtn  *addButton(const char *pBtnName, int id);
   void     rmvButton(int value);

signals:
   void clicked(int id);

protected slots:
   void buttonClicked(int id);
   
private:
   QList<xPshBtn> btnList;
   xWidgetResInfo *wdtRes;
};

#endif

