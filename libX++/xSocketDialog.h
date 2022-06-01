/***************************************************************************
**    xSocketDialog.h  $Revision: 1.1.1.1 $ - $Name:  $ 
**    Dialog Box to reflect a sockets status of connection
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
#ifndef _XSOCKETDIALOG_H
#define _XSOCKETDIALOG_H

// #include <qlabel.h>
#include "xPshBtnFrame.h"
#include "xResources.h"
#include "xLabel.h"
#include "xDialog.h"

class xSocketDialog : public xDialog
{
   Q_OBJECT

public:
   xSocketDialog(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
                 const char *pName = NULL);
   ~xSocketDialog();

signals:
   void stopConnection();
   void nextConnection();

public slots:
   void connectMessage(const char *);
   void connected(const char *);
   void buttonPressed(int btn);

private:
   QLabel         *pText;
   xPshBtnFrame   *pButtons;
   xWidgetResInfo *wdtRes;
};

#endif
