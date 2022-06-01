/***************************************************************************
**    xConfirmation.h  $Revision: 1.1.1.1 $ - $Name:  $ 
**    Dialog box to display error messages
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
#ifndef _XCONFIRMATION_H
#define _XCONFIRMATION_H

#include <qaccel.h>
#include "xLabel.h"
#include "xPshBtnFrame.h"
#include "xDialog.h"
#include "xResources.h"

class xConfirmation : public xDialog
{
   Q_OBJECT
   
public:
   xConfirmation(xWidgetResInfo *pPRes, QWidget *pParent, 
                  const char *pName, const char *pCaption,
                  const char *pMessage);
   ~xConfirmation()
   {
      disconnect(pButton, SIGNAL(clicked(int)), this, SLOT(gotButton(int)));
      disconnect(this);
      if (pLabel)
         delete pLabel;
      if (pButton)
         delete pButton;
   };
   static int confirm(xWidgetResInfo *pPRes, const char *pCaption,
                      const char *pMsg, QWidget *pParent = NULL, 
                      const char * pName = NULL);

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
