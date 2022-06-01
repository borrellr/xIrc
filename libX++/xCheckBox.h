/***************************************************************************
**    xCheckBox.h  $Revision: 1.2 $ - $Name:  $ 
**    CheckBox Button
**
**    Copyright (C) 1997 1998 Joseph Croft <joe@croftj.net>
**    Copyright (C) 1997 Joseph Croft <jcroft@unicomp.net>
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
#ifndef _XCHECKBOX_H
#define _XCHECKBOX_H

#include <qcheckbox.h>
#include "xResources.h"

class xCheckBox : public QCheckBox
{
   Q_OBJECT
   
public:
   xCheckBox(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
           const char *pName = NULL,
           const char *pText = NULL, int id = 0);
   
   ~xCheckBox()
   {
      disconnect(this);
   };
   
   int      id()           { return(ident); };
   void     setId(int id)  { ident = id; };

signals:
   void clicked(int id);
   
protected slots:
   void buttonClicked();

private:
   int      ident;
   xWidgetResInfo *wdtRes;
};

#endif
