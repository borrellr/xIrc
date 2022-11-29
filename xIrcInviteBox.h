/***************************************************************************
**    xIrcInvite.h  $Revision: 1.1.1.1 $ - $Name:  $ 
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
#ifndef _xIRCINVITEBOX_H
#define _xIRCINVITEBOX_H

#include <qstring.h>
#include <qlabel.h>
#include <xPshBtnFrame.h>
#include <xDialog.h>
#include <xResources.h>
#include "xIrcSocket.h"

class xIrcInviteBox : public xDialog
{
   Q_OBJECT
   
public:
   xIrcInviteBox(xWidgetResInfo *pPRes, QWidget *pParent, 
                 const char *pName, QObject *pReceiver, 
                 char *pSlot, xIrcMessage *pMsg);
   ~xIrcInviteBox()
   {
      disconnect(pButton, SIGNAL(clicked(int)), this, SLOT(buttonClicked(int)));
//      disconnect(this);
      if (pLabel)
         delete pLabel;
      if (pButton)
         delete pButton;
   };
   
signals:
   void acknowledged(QString channel);
   
protected slots:
   void buttonClicked(int resp);

private:
   QString        strChannel;
   QString        strNick;
   QLabel         *pLabel;
   xPshBtnFrame  *pButton;
   xWidgetResInfo *wdtRes;
};

#endif

