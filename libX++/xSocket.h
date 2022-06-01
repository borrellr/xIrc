/***************************************************************************
**    xSocket.h  $Revision: 1.3 $ - $Name:  $ 
**    Class to manipulate sockets
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
#ifndef _xSOCKET_H
#define _xSOCKET_H

#include "xResources.h"
#include <qobject.h>
#include <qstring.h>
#include <qsocketnotifier.h>
#include <qtimer.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

typedef struct
{
   const unsigned char  *buf;
   unsigned int  n;
} xSocketBuffer;

class xSocketTCP : public QObject
{
   Q_OBJECT
   
public:
   enum Errors
   {
      ERROR_INVALIDSERVICE = 1,
      ERROR_INVALIDHOST,
      ERROR_BINDINGSOCKET,
      ERROR_OPENINGSOCKET
   };

   xSocketTCP(xWidgetResInfo *pPRes, QObject *pParent, const char *pHost,
              const char *pService, QObject *pSocketDialog, 
              const char *pSlotStatus, const char *pSlotProgress, 
              int &err);
   xSocketTCP(xWidgetResInfo *pPRes, QObject *pParent, const char *pService,
              const char *pSlotStatus, int &err);
   virtual ~xSocketTCP();
  
   void           off();  
   virtual int    disconnectTCP();
   bool           isConnected() { return(connectFlag); };
   struct sockaddr_in socketName(struct sockaddr_in *p = NULL);
   bool isBinary() { return(bMode); };
   void connectToHost();
   
signals:
   void socketError(int err);
   void readFromSocket(char *buf);
   void readFromSocketBin(xSocketBuffer);
   void connecting(const char *inet_addr);
   void socketClosed();
   void status(int err);
   void binaryWriteReady();

public slots:
   void sendToSocket(xSocketBuffer sb);
   void sendToSocket(const char *pText);
   void stopConnect();
   
private slots:
   void readyRead(int sfd);
   void readyWrite(int sfd);
   void readyAccept(int sock);
   void retryConnect(int sock);
   void timeout();

protected:
   FILE     *dbf;
   
private:
   bool              deadSocket;
   bool              bMode;
   int               bCnt;
   char              *bPtr;
   char              *bBuf;
   QTimer            *pTimer;
   QString           writeBuffer;
   int               counter;
   int               fd;
   int               connIndex;
   FILE              *pFdIn;
   char              *pHost;
   char              *pService;
   bool              connectFlag;
   bool              socketOff;
   QSocketNotifier   *snr;
   QSocketNotifier   *snw;
   QSocketNotifier   *sna;
   QSocketNotifier   *snc;
   struct hostent    *phe;
   struct sockaddr_in acceptAddr;
   struct sockaddr_in connAddr;
   xWidgetResInfo    *wdtRes;
};

#define ESOCK_BadWrite        1
#define ESOCK_BadRead         2
#endif
