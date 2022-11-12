/***************************************************************************
**    xIrcChannelQuery.h  $Revision: 1.1.1.1 $ - $Name:  $ 
**    Dialog Box to get channel name to join
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
#ifndef _xIRCCHANNELQUERY_H
#define _xIRCCHANNELQUERY_H

#include <xPshBtnFrame.h>
#include <xEditList.h>
#include <xDialog.h>
#include "xIrcSocket.h"

#define xCQ_Clear    2
#define xCQ_Names    3
#define xCQ_Join     4

class xChannelQuery : public xDialog
{
   Q_OBJECT

public:   
   xChannelQuery(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
                 const char *pName = NULL);
   ~xChannelQuery();
   void initClass(const QString &Name);
   
   const char *editText() { return(pChannel->text()); };
   void setText(const char *pText) { pChannel->setText(pText); };
   
   enum  chanResults { Accepted, Rejected, Clear, Names, Join, DccChat };   

signals:
   void ircMessageOut(xIrcMessage *pMsg);
   void hasResult(int results);

protected slots:
   virtual void buttonPressed(int results);
   void reject();
   void accept();
   
private slots:
   void gotReturn();
   void gotReturnFromChannel(const char *pStr);

private:   
   xEditList         *pChannel;
   xPshBtnFrame      *pButtons;
   xWidgetResInfo *wdtRes;
};

#endif
