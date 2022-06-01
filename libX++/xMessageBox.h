/***************************************************************************
**    xMessageBox.h  $Revision: 1.1.1.1 $ - $Name:  $ 
**    Dialog box to display single button messages
**
**    Copyright (C) 1997 1998 Joseph Croft <joe@croftj.net>
** Copyright (C) 1995, 1996  Joseph Croft <jcroft@unicomp.net>  
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 1, or (at your option)
** any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
 ***************************************************************************/
#ifndef _XMESSAGEBOX_H
#define _XMESSAGEBOX_H

#include <qaccel.h>
#include "xLabel.h"
#include "xPshBtnFrame.h"
#include "xDialog.h"
#include "xResources.h"

class xMessageBox : public xDialog
{
   Q_OBJECT
   
public:
   xMessageBox(const char *pCaption, const char *pMessage,
               const char *pBtnText = NULL,
               QWidget *pParent = NULL, const char *pName = NULL);
   ~xMessageBox()
   {
      disconnect(pButton, SIGNAL(clicked(int)), this, SLOT(gotButton(int)));
      disconnect(this);
      if (pLabel)
         delete pLabel;
      if (pButton)
         delete pButton;
   };
   static int message(const char *pCaption, const char *pMsg,
                      const char *pBtnText = NULL,
                      QWidget *pParent = NULL, const char *pName = NULL);

protected slots:
   void gotButton(int btn);
   void gotEscape();
   void gotEnter();

private:
   xLabel         *pLabel;
   xPshBtnFrame   *pButton;
   QAccel         *pAccel;
   int            rspCode;
   xWidgetResInfo *wdtRes;
};

#endif
