/***************************************************************************
**    xIrcDccChatFrame.h  $Revision: 1.5 $ - $Name:  $ 
**    Frame for handling Dcc Chat sessions
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
#ifndef _XIRCDCCCHATFRAME_H
#define _XIRCDCCCHATFRAME_H

#include <qcheckbox.h>
#include <xResources.h>
#include <xPshBtnFrame.h>
#include <xEdit.h>
#include "xMultiLineFrame.h"
#include "xIrcDccSocket.h"
#include "xDialog.h"

class xIrcDccChatFrame : public xDialog
{
   Q_OBJECT

public:
   xIrcDccChatFrame(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
                    const char *pName = NULL);
   ~xIrcDccChatFrame();
   struct sockaddr_in socketName() { return(pSocket->socketName()); };
   int connectTo(const char *pNetAddr = NULL);
   xSocketTCP *socket();
   int acceptdcc();

signals:
   void connStatus(int err);
   void connProgress(const char *);
   void connFailed(int err);
   void connGood(int sock);
   void textOut(const char *);
   void textSelected(xMultiLineTextSelection txt);
   void initiateDCCChat(xIrcDccChatFrame*);

public slots:
   void changeColors(bool f);

protected slots:
   virtual void gotKeyboardInput();
   virtual void buttonPressed(int btn);
   virtual void socketIn(char *pData);
   void haveTextSelection(xMultiLineTextSelection);

private slots:
   void sockStatus(int err);
   void failedConnection(int err);
   void goodConnection(int sock);
   void socketClosed();

protected:
   enum btnCodes { btnClose, btnClear, btnColors };
   virtual void resizeEvent(QResizeEvent *pEvt);

private:
   void closeFrame();
   void clearLine();
   void procCommand(const char *pStr);

   QString           socketData;
   QString           strSocket;
   xIrcDccSocket     *pSocket;
   xPshBtnFrame      *pButtons;
   xMultiLineFrame   *pMsgFrame;
   xEdit             *pEdit;
   QCheckBox         *pMircColors;
   QCheckBox         *pCtcp2;
   QCheckBox         *pBeepMsg;
   xFrame            *pFrame1;
   xFrame            *pFrame11;
   int               resizeCnt;
   xWidgetResInfo    *wdtRes;
};

#endif
