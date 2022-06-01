/***************************************************************************
**    xIrcDccFile.h  $Revision: 1.5 $ - $Name:  $ 
**    Frame for handling Dcc File Transfers
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
#ifndef _XIRCDCCFILE_H
#define _XIRCDCCFILE_H

#include <qtimer.h>
#include <xEdit.h>
#include <qlabel.h>
#include <xResources.h>
#include <xProgressBar.h>
#include <xPshBtnFrame.h>
#include <qlineedit.h>
#include "xIrcDccSocket.h"
#include "xMultiLineFrame.h"
#include "xDialog.h"

class xIrcDccFile : public xDialog
{
   Q_OBJECT

public:
   xIrcDccFile(xWidgetResInfo *pPRes, const char *fn,
                    QWidget *pParent = NULL,
                    const char *pName = NULL);
   ~xIrcDccFile();
   struct sockaddr_in socketName() { return(pSocket->socketName()); };
   int connectTo(const char *pNetAddr = NULL, int fd = -1);
   xSocketTCP *socket();
   int accept(int fd);
   const char *filename() { return((const char*)fileName); };
   int file_d() { return(fd); };


signals:
   void connStatus(int err);
   void connProgress(const char *);
   void connFailed(int err);
   void connGood(int sock);
   void initiateDCCFile(xIrcDccFile*);

protected slots:
   virtual void buttonPressed(int btn);
   virtual void socketIn(xSocketBuffer sb);
   virtual void readyWrite();

private slots:
   void sockStatus(int err);
   void failedConnection(int err);
   void goodConnection(int sock);
   void socketClosed();
   void socketWait();

protected:
   enum btnCodes { btnClose };

private:
   void closeFrame();

   bool              doSend;
   bool              waitingTransmit;
   bool              waited;
   int               fd;
   unsigned int      dataAckCnt;
   unsigned int      dataAckBCnt;
   unsigned int      dataAckTmp;
   unsigned int      dataAcked;
   unsigned int      bCnt;
   int               fSize;
   QTimer            *pTimer;
   QString           fileName;
   QString           socketData;
   QString           strSocket;
   QLabel            *pFileName;
   xProgressBar      *pGauge;
   QLineEdit         *pBps;
   xFrame            *pEditFrame;
   xFrame            *pFrame;
   xFrame            *pFrame1;
   xMultiLineFrame   *pMsgFrame;
   xIrcDccSocket     *pSocket;
   xPshBtnFrame      *pButtons;
   int               resizeCnt;
   xWidgetResInfo    *wdtRes;
};

#endif
