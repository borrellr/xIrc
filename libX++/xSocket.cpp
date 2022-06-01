/***************************************************************************
**    xSocket.cpp  $Revision: 1.6 $ - $Name:  $ 
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
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <qmessagebox.h>
#include <qglobal.h>    // Added by borrell for checking pointers
#include "xSocket.h"

#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif

extern int errno;
//extern char *sys_errlist[];

//static int dbg = 1;
#define dbg 0

static const char *pInitialResources[] =
{
   "*ConnectTime: 60000",
   "*AcceptTime: 300000",
   NULL
};

xSocketTCP::xSocketTCP(xWidgetResInfo *pPRes, QObject *pParent, const char *pHost, 
                       const char *pService, QObject *pSocketDialog, 
                       const char *pSlotStatus, const char *pSlotProgress, 
                       int &err) :
            QObject(NULL, NULL)
{
   struct servent *se = NULL;
   int port;
   int timeOut;
   const char *pTimeVal;

   if (dbg) {
       fprintf(stderr, "xSocketTCP--xSocketTCP.connect()-Enter\n");
       fprintf(stderr, "The hostname is [%s]\n", pHost);
       fprintf(stderr, "The service  is [%s]\n", pService);
       fflush(stderr);
   }
   wdtRes = new xWidgetResInfo(pPRes, QString("tcpsocket"), 
                               QString("TCPSocket"));
   Resources->setWidgetInit(pInitialResources);
   err = 0;
   fd = -1;
//   dbf = fopen("qtTextOut.txt", "w");
   dbf = NULL;
   snw = NULL;
   snr = NULL;
   sna = NULL;
   snc = NULL;
   bBuf = NULL;
   bPtr = NULL;
   bCnt = 0;
   bMode = FALSE;
   socketOff = FALSE;
   deadSocket = FALSE;
   pTimer = new QTimer(this);

   if (dbg) {
       fprintf(stderr, "xSocketTCP::xSocketTCP.connect():Getting service name: |%s|, %d\n",
                             pService, atoi(pService));
       fflush(stderr);
   }

   memset((void *)&connAddr, '\0', sizeof(connAddr));

   if ((se = getservbyname(pService, "tcp")) != NULL)
      port = se->s_port;
   else if ((port = htons((u_short)atoi(pService))) == 0)
   {
      if (dbg) fprintf(stderr, "xSocketTCP::xSocketTCP.connect():Invalid Service!\n");
      if (dbg) fflush(stderr);
      err = xSocketTCP::ERROR_INVALIDSERVICE;
      return;
   }

   phe = NULL;
   if (dbg) fprintf(stderr, "xSocketTCP::connectTCP():getting Host Entry\n");
   if (dbg) fflush(stderr);
   if ((phe = gethostbyname(pHost)) == NULL)
   {
      if (dbg) fprintf(stderr, "xSocketTCP::connectTCP():Maybe by Dotted Quad\n");
      if (dbg) fflush(stderr);
      if ((connAddr.sin_addr.s_addr = inet_addr(pHost)) == INADDR_NONE)
      {
         if (dbg) fprintf(stderr, "xSocketTCP::connectTCP():Must be INET address number\n");
         if (dbg) fflush(stderr);
         if ((connAddr.sin_addr.s_addr = atoi(pHost)) == 0)
         {
            err = xSocketTCP::ERROR_INVALIDHOST;
            return;
         }
      }
      if (dbg) fprintf(stderr, "xSocketTCP::connectTCP():Found Lone address: %d\n",
                               connAddr.sin_addr.s_addr);
      if (dbg) fflush(stderr);
   }
   else
   {
      if (dbg) fprintf(stderr, "xSocketTCP::connectTCP():Was a name after all!!!\n");
      if (dbg) fflush(stderr);
      bcopy(phe->h_addr, (char *)&connAddr.sin_addr, phe->h_length);
   }

   connAddr.sin_family = AF_INET;
   connAddr.sin_port = port;
   fd = socket(AF_INET, SOCK_STREAM, 0);
   if (fd < 0)
   {
      err = xSocketTCP::ERROR_OPENINGSOCKET;
      return;
   }
   if (dbg) fprintf(stderr, "xSocketTCP::connectTCP():Socket Opened: %d\n", fd);
   if (dbg) fflush(stderr);

#if 0
   snc = new QSocketNotifier(fd, QSocketNotifier::Write, this);
   QObject::connect(snc, SIGNAL(activated(int)), this, SLOT(retryConnect(int)));
#endif

   QObject::connect(this, SIGNAL(connecting(const char*)),
                    pSocketDialog, pSlotProgress);

   QObject::connect(this, SIGNAL(status(int)),
                    pParent, pSlotStatus);
   
   emit connecting(inet_ntoa(connAddr.sin_addr));

   if ((pTimeVal = Resources->get(wdtRes, "connecttime", "ConnectTime"))
       == NULL)
      timeOut = 60000;
   else
   {
      if ((timeOut = atol(pTimeVal)) == 0)
         timeOut = 60000;
   }
   QObject::connect(pTimer, SIGNAL(timeout()), this, SLOT(timeout()));
   pTimer->start(timeOut);

   if (dbg) fprintf(stderr, "xSocketTCP::connectTCP():Exit\n");
   if (dbg) fflush(stderr);

   //connectToHost();
}   

xSocketTCP::xSocketTCP(xWidgetResInfo *pPRes, QObject *pParent,
                       const char *pService, 
                       const char *pSlotStatus,
                       int &err) :
            QObject(NULL, NULL)
{
   struct servent *se;
   int port, sock, rv = 0;
   char *cp;
   const char *pTimeVal;
   long timeOut;

   if (dbg) fprintf(stderr, "xSocketTCP::xSocketTCP.Accept():Enter\n");
   if (dbg) fflush(stderr);
   err = 0;
   fd = -1;
//   dbf = fopen("qtTextOut.txt", "w");
   dbf = NULL;
   snw = NULL;
   snr = NULL;
   sna = NULL;
   snc = NULL;
   bBuf = NULL;
   bPtr = NULL;
   bCnt = 0;
   bMode = FALSE;
   socketOff = FALSE;
   deadSocket = FALSE;
   wdtRes = new xWidgetResInfo(pPRes, QString("tcpsocket"), 
                               QString("TCPSocket"));
   Resources->setWidgetInit(pInitialResources);
   err = 0;

   pTimer = new QTimer(this);
   if (dbg) fprintf(stderr, "xSocketTCP::accept():getting Service\n");
   if (dbg) fflush(stderr);
   if (pService == NULL)
      pService = "0";
   if ((se = getservbyname(pService, "tcp")) != NULL)
      port = se->s_port;
   else
      port = htons((u_short)atoi(pService));
   if (dbg) fprintf(stderr, "xSocketTCP::accept():got port %d\n", port);
   if (dbg) fflush(stderr);

   acceptAddr.sin_family = AF_INET;
   acceptAddr.sin_port = port;
   acceptAddr.sin_addr.s_addr = htonl(INADDR_ANY);
   
   cp = NULL;
   if (dbg) fprintf(stderr, "xSocketTCP::accept():Calling socket()\n");
   if (dbg) fflush(stderr);
   if ((sock = socket(AF_INET, SOCK_STREAM, 0)) >= 0)
   {
      if (dbg) fprintf(stderr, "xSocketTCP::accept():Socket Opened: %d\n", sock);
      if (dbg) fprintf(stderr, "xSocketTCP::accept():Calling bind()\n");
      if (dbg) fflush(stderr);
      if (::bind(sock, (struct sockaddr *)&acceptAddr, 
                 sizeof(acceptAddr)) >= 0)
      {
         sna = new QSocketNotifier(sock, QSocketNotifier::Read, this);
         QObject::connect(sna, SIGNAL(activated(int)), 
                          this, SLOT(readyAccept(int)));

         QObject::connect(this, SIGNAL(status(int)),
                          pParent, pSlotStatus);
   
         if (dbg) fprintf(stderr, "xSocketTCP::accept():Calling listen()\n");
         if (dbg) fflush(stderr);
         ::listen(sock, 5);
         fd = sock;
         if ((pTimeVal = Resources->get(wdtRes, "accepttime", "AcceptTime"))
             == NULL)
            timeOut = 300000;
         else
         {
            if ((timeOut = atol(pTimeVal)) == 0)
               timeOut = 300000;
         }
         QObject::connect(pTimer, SIGNAL(timeout()), this, SLOT(timeout()));
         pTimer->start(timeOut);

         if (dbg) fprintf(stderr, "xSocketTCP::accept():fd = %d\n", fd);
         if (dbg) fflush(stderr);
      }
      else 
      {
         if (dbg) fprintf(stderr, "xSocketTCP::accept():Error binding name\n");
         if (dbg) fflush(stderr);
         err = xSocketTCP::ERROR_BINDINGSOCKET;
      }
   }
   else
   {
      if (dbg) fprintf(stderr, "xSocketTCP::accept():Error opening socket\n");
      if (dbg) fflush(stderr);
         err = xSocketTCP::ERROR_OPENINGSOCKET;
   }
   if (dbg) fprintf(stderr, "xSocketTCP::accept():Exit(%d)\n", fd);
   if (dbg) fflush(stderr);
   err = rv;
}

xSocketTCP::~xSocketTCP()
{
   if (snw)
   {
      QObject::disconnect(snw, SIGNAL(activated(int)), this, SLOT(readyWrite(int)));
      delete snw;
   }
   snw = NULL;
   if (snr)
   {
      QObject::disconnect(snr, SIGNAL(activated(int)), this, SLOT(readyRead(int)));
      delete snr;
   }
   snr = NULL;
//   if (dbf)
//      fclose(dbf);
   if (pTimer)
      delete pTimer;
}

void xSocketTCP::connectToHost()
{
   if (dbg) {
      fprintf(stderr, "xSocketTCP establishing connection\n");
      fflush(stderr);
   }

   // Connect call missing here ???
   if (::connect(fd, (struct sockaddr *)&connAddr, sizeof(connAddr)) < 0) {
      fprintf(stderr, "xSocketTCP connection error errno = [%d]\n", errno);
      fflush(stderr);
      // Need some error processing here -borrell
      emit status(1);
      return;
   }

   if (dbg) {
      fprintf(stderr, "xSocketTCP established connection\n");
      fflush(stderr);
   }

   pTimer->stop();

   snr = new QSocketNotifier(fd, QSocketNotifier::Read, this);
   CHECK_PTR(snr);
   QObject::connect(snr, SIGNAL(activated(int)), this, SLOT(readyRead(int)));

   snw = new QSocketNotifier(fd, QSocketNotifier::Write, this);
   CHECK_PTR(snw);
   QObject::connect(snw, SIGNAL(activated(int)), this, SLOT(readyWrite(int)));
   snw->setEnabled(FALSE);

   emit status(0);
}

void xSocketTCP::retryConnect(int sock)
{
   int n;

   if (dbg) fprintf(stderr, "xSocketTCP::retryConnect(%d:%d):Enter\n", sock, fd);
   if (dbg) fflush(stderr);

   if (deadSocket) {
      if (dbg) {
      fprintf(stderr, "xSocketTCP::retryConnect: dead socket\n");
      fflush(stderr);
      }
      return;
   }
   if ((n = fcntl(sock, F_GETFL, 0)) < 0) {
      if (dbg) {
      fprintf(stderr, "xSocketTCP::retryConnect: Error fcntl errno = [%d]\n", errno);
      fflush(stderr);
      }
      return;
   }
   //n |= FNDELAY;
   fcntl(sock, F_SETFL, n); // SVR4 non-blocking incantation

   if (dbg) fprintf(stderr, "xSocketTCP::retryConnect():Calling ::connect!!!\n");
   if (dbg) fflush(stderr);
   if (::connect(sock, (struct sockaddr *)&connAddr, sizeof(connAddr)) >= 0)
   {
      if (dbg) fprintf(stderr, "xSocketTCP::retryConnect():Success!!!\n");
      if (dbg) fflush(stderr);
      pTimer->stop();
      if (snc)
      {
         QObject::disconnect(snc, SIGNAL(activated(int)), this, SLOT(retryConnect(int)));
         delete snc;
      }
      snc = NULL;

      snr = new QSocketNotifier(sock, QSocketNotifier::Read, this);
      connect(snr, SIGNAL(activated(int)), this, SLOT(readyRead(int)));

      snw = new QSocketNotifier(sock, QSocketNotifier::Write, this);
      connect(snw, SIGNAL(activated(int)), this, SLOT(readyWrite(int)));
      if (dbg) fprintf(stderr, "xSocketTCP::retryConnect():setting snw->Enabled to %d\n",
                               FALSE);
      if (dbg) fflush(stderr);
      snw->setEnabled( FALSE );
      emit status(0);
   }
   else
   {
      if (dbg) fprintf(stderr, "xSocketTCP::retryConnect():Connection?? Uh??\n");
      if (dbg) fflush(stderr);
      if (errno != EWOULDBLOCK &&
          errno != EINPROGRESS &&
          errno != EALREADY &&
          errno != EAGAIN)
      {
         if (dbg) fprintf(stderr, "xSocketTCP::retryConnect():Failed!!!\n");
         if (dbg) fflush(stderr);
         pTimer->stop();
         close(sock);
         fd = -1;
         if (dbg) fprintf(stderr, "xSocketTCP::retryConnect():Connection failed:%s\n", strerror(errno));
         if (dbg) fflush(stderr);
         perror("xSocketTCP::retryConnect():Connection Failed!!!\n");
         if (snc)
         {
            QObject::disconnect(snc, SIGNAL(activated(int)), this, SLOT(retryConnect(int)));
            delete snc;
         }
         snc = NULL;
         pTimer->stop();
         QObject::disconnect(pTimer, SIGNAL(timeout()), this, SLOT(timeout()));
         emit status(errno);
      }
      else
      {
         if (dbg) fprintf(stderr, "xSocketTCP::retryConnect():Expected error!\n");
         if (dbg) fflush(stderr);
      }
   }
   if (dbg) fprintf(stderr, "xSocketTCP::retryConnect(%d:%d):Exit\n", sock, fd);
   if (dbg) fflush(stderr);
}

void xSocketTCP::readyAccept(int sock)
{
   int n;

   if (dbg) fprintf(stderr, "xSocketTCP::readyAccept(%d:%d):Enter\n", sock, fd);
   if (dbg) fflush(stderr);
   n = sizeof(acceptAddr);
   if ((fd = ::accept(sock, (struct sockaddr *)&acceptAddr, (socklen_t *)&n)) < 0)
   {
      if (errno != EWOULDBLOCK &&
          errno != EINPROGRESS &&
          errno != EALREADY &&
          errno != EAGAIN)
      {
         if (dbg) fprintf(stderr, "xSocketTCP::readyAccept():Error Found!\n");
         if (dbg) fflush(stderr);
         close(sock);
         perror("xSocketTCP::readyAccept():Connection Failed!!!\n");
         if (sna)
         {
            if (dbg) fprintf(stderr, "xSocketTCP::readyAccept():Disconnecting socket!\n");
            if (dbg) fflush(stderr);
            QObject::disconnect(sna, SIGNAL(activated(int)), this, SLOT(readyAccept(int)));
            delete sna;
         }
         sna = NULL;
         if (dbg) fprintf(stderr, "xSocketTCP::readyAccept():Emiting failed signal!\n");
         if (dbg) fflush(stderr);
         emit status(errno);
         if (dbg) fprintf(stderr, "xSocketTCP::readyAccept():Back from signal!\n");
         if (dbg) fflush(stderr);
      }
   }
   else
   {
      if (dbg) fprintf(stderr, "xSocketTCP::readyAccept():Accept succeded!\n");
      if (dbg) fflush(stderr);
      pTimer->stop();

      if (sna)
      {
         if (dbg) fprintf(stderr, "xSocketTCP::readyAccept():Disconnecting/deleting Accept Notifier!\n");
         if (dbg) fflush(stderr);
         QObject::disconnect(sna, SIGNAL(activated(int)), this, SLOT(readyAccept(int)));
         delete sna;
      }
      sna = NULL;
      close(sock);

      n = fcntl(fd, F_GETFL, 0);
      //n |= FNDELAY;
      fcntl(fd, F_SETFL, n); // SVR4 non-blocking incantation

      snr = new QSocketNotifier(fd, QSocketNotifier::Read, this);
      connect(snr, SIGNAL(activated(int)), this, SLOT(readyRead(int)));

      snw = new QSocketNotifier(fd, QSocketNotifier::Write, this);
      connect(snw, SIGNAL(activated(int)), this, SLOT(readyWrite(int)));
      if (dbg) fprintf(stderr, "xSocketTCP::readyAccept():setting snw->Enabled to %d\n",
                               FALSE);
      snw->setEnabled( FALSE );
      socketOff = FALSE;
      connectFlag = TRUE;
      if (dbg) fprintf(stderr, "xSocketTCP::readyAccept():Emitting signal!\n");
      emit status(0);
   }
   if (dbg) fprintf(stderr, "xSocketTCP::readyAccept(%d:%d):Exit\n", sock, fd);
   if (dbg) fflush(stderr);
}

void xSocketTCP::timeout()
{
   close(fd);
   connectFlag = FALSE;
   if (dbg) fprintf(stderr, "xSocketTCP::timeout():Emitting signal!\n");
   emit status(ETIMEDOUT);
}

void xSocketTCP::readyRead(int sfd)
{
   int n;
   char buf[16 * 1024];

   if (dbg) fprintf(stderr, "xSocketTCP::readyRead(%d:%d):Enter\n", sfd, fd);
   if (dbg) fflush(stderr);

   if (deadSocket)
      return;

   if ((n = ::read(sfd, buf, sizeof(buf - 1))) < 0)
   {
      if (dbg) fprintf(stderr, "xSocketTCP::readyRead():Error Reading from Socket %d\n", sfd);
      if (dbg) fflush(stderr);
      close(sfd);
      if (sfd == fd)
      {
         if (snw)
         {
            QObject::disconnect(snw, SIGNAL(activated(int)), this, SLOT(readyWrite(int)));
            delete snw;
         }
         snw = NULL;
         if (snr)
         {
            QObject::disconnect(snr, SIGNAL(activated(int)), this, SLOT(readyRead(int)));
            delete snr;
         }
         snr = NULL;
         connectFlag = FALSE;
         if (dbg) fprintf(stderr, "xSocketTCP::readyRead():SOCKET %d:%d CLOSED!\n", sfd, fd);
         if (dbg) fflush(stderr);
         if (dbg) fprintf(stderr, "xSocketTCP::readyRead():Emitting Socket Closed Signal\n");
         if (dbg) fflush(stderr);
         close(fd);
         emit socketClosed();
         fd = -1;
         deadSocket = TRUE;
//         delete this;
      }
   }
   else if (n > 0)
   {
      if (dbg > 3) fprintf(stderr, "xSocketTCP::readyRead(%d):Read %d chars from socket\n", sfd, n);
      if (dbg) fflush(stderr);
      buf[n] = '\0';
//      if (dbf) fprintf(dbf, buf);
//      if (dbf) fflush(dbf);
      if (dbg > 3) fprintf(stderr, "xSocketTCP::readyRead():Emitin readFromSocket signal\n");
      if (dbg > 3) fflush(stderr);
      emit readFromSocket(buf);
      xSocketBuffer sb = { (unsigned char*)buf, n};
      emit readFromSocketBin(sb);
      if (dbg > 3) fprintf(stderr, "xSocketTCP::readyRead():Back from sending signal\n");
      if (dbg > 3) fflush(stderr);
   }
   else
   {
      connectFlag = FALSE;
      if (dbg) fprintf(stderr, "xSocketTCP::readyRead():SOCKET %d CLOSED!\n", sfd);
      if (dbg) fflush(stderr);
      close(sfd);
      if (sfd == fd)
      {
         if (snw)
         {
            QObject::disconnect(snw, SIGNAL(activated(int)), this, SLOT(readyWrite(int)));
            delete snw;
         }
         snw = NULL;
         if (snr)
         {
            QObject::disconnect(snr, SIGNAL(activated(int)), this, SLOT(readyRead(int)));
            delete snr;
         }
         snr = NULL;
         fd = -1;
         if (dbg > 3) fprintf(stderr, "xSocketTCP::readyRead():Emitting Socket Closed Signal\n");
         if (dbg) fflush(stderr);
         emit socketClosed();
      }
      deadSocket = TRUE;
//      delete this;
   }
   if (dbg > 3) fprintf(stderr, "xSocketTCP::readyRead(%d:%d):Exit\n", sfd, fd);
}

void xSocketTCP::readyWrite(int sfd)
{
   int n = 1;
   bool tf;

   if (dbg > 3) fprintf(stderr, "xSocketTCP::readyWrite(%d:%d):Enter\n", sfd, fd);
   if (dbg > 3) fflush(stderr);
   if (deadSocket)
   {
      if (dbg > 3) fprintf(stderr, "xSocketTCP::readyWrite(%d:%d):Dead Socket- Exiting...\n", sfd, fd);
      if (dbg > 3) fflush(stderr);
      return;
   }

   if (!isBinary() && writeBuffer.length() > 0)
   {
      if (dbg) fprintf(stderr, "xSocketTCP::readyWrite(%d:%d):writing %d chars, |%s|, to socket\n", sfd, fd, n, (const char *)writeBuffer);
      if (dbg) fflush(stderr);
//      sprintf(buf, "Client>>|%s|\n", (const char *)writeBuffer);
//      if (dbf) fprintf(dbf, buf);
//      if (dbf) fflush(dbf);
      if ((n = ::write(fd, (const char *)writeBuffer,
                       writeBuffer.length())) > 0)
      {
         if (dbg) fprintf(stderr, "xSocketTCP::readyWrite():Wrote %d chars to socket\n", n);
         if (dbg) fprintf(stderr, "xSocketTCP::readyWrite():Removing string from buffer\n");
         if (dbg) fflush(stderr);
         writeBuffer.remove(0, n);

         if (dbg) fprintf(stderr, "xSocketTCP::readyWrite():Chars removed\n");
         if (dbg) fflush(stderr);
      }
   }
   else if (isBinary() == TRUE)
   {
      if (dbg > 3) fprintf(stderr, "xSocketTCP::readyWrite(%d:%d):Binary file, have %d bytes to send\n",
                                    sfd, fd, bCnt);
      if (dbg > 3) fflush(stderr);
      if (dbg > 1) fprintf(stderr, "xSocketTCP::readyWrite(%d:%d):bBuf = %lx, bPtr = %lx, bCnt = %d\n",
                           sfd, fd, (unsigned long)bBuf, (unsigned long)bPtr, bCnt);
      if (dbg > 1) fflush(stderr);
      if (bCnt > 0)
      {
         if (dbg > 3) fprintf(stderr, "xSocketTCP::readyWrite(%d:%d):Writing data\n",
                                       sfd, fd);
         if (dbg > 3) fflush(stderr);
         if ((n = ::write(fd, bPtr, bCnt)) > 0)
         {
            if (dbg) fprintf(stderr, "xSocketTCP::readyWrite(%d:%d):Wrote %d byte binary to socket\n",
                            sfd, fd, n);
            if (dbg) fflush(stderr);
            bCnt -= n;
            bPtr += n;
         }
         else 
         {
            if (dbg) fprintf(stderr, "xSocketTCP::readyWrite(%d:%d):Error writing data!, n = %d\n",
                             sfd, fd, n);
            if (dbg) fflush(stderr);
            perror("Error writing to socket");
         }

      }
   }

   /*
   ** Handle Closed Sockets and Socket Errors Accordingly
   */
   if (n < 0)
   {
      if (dbg) fprintf(stderr, "xSocketTCP::readyWrite(%d:%d):Error writing!\n",
                               sfd, fd);
      if (dbg) fflush(stderr);
      snw->setEnabled(FALSE);
      close(sfd);
      if (bBuf != NULL)
         delete bBuf;
      bBuf = bPtr = NULL;
      bCnt = 0;
      if (sfd == fd)
      {
         fd = -1;
         writeBuffer = "";
         connectFlag = FALSE;
         emit socketClosed();
         if (snw)
         {
            QObject::disconnect(snw, SIGNAL(activated(int)), this, SLOT(readyWrite(int)));
            delete snw;
         }
         snw = NULL;
         if (snr)
         {
            QObject::disconnect(snr, SIGNAL(activated(int)), this, SLOT(readyRead(int)));
            delete snr;
         }
         snr = NULL;
         deadSocket = TRUE;
//         delete this;
      }
   }
   else if (n == 0)
   {
      if (dbg) fprintf(stderr, "xSocketTCP::readyWrite(%d:%d):Write SOCKET CLOSED!\n", sfd, fd);
      if (dbg) fflush(stderr);
      snw->setEnabled(FALSE);
      close(sfd);
      if (bBuf != NULL)
         delete bBuf;
      bBuf = bPtr = NULL;
      bCnt = 0;
      if (sfd == fd)
      {
         fd = -1;
         writeBuffer = "";
         connectFlag = FALSE;
         emit socketClosed();
         if (snw)
         {
            QObject::disconnect(snw, SIGNAL(activated(int)), this, SLOT(readyWrite(int)));
            delete snw;
         }
         snw = NULL;
         if (snr)
         {
            QObject::disconnect(snr, SIGNAL(activated(int)), this, SLOT(readyRead(int)));
            delete snr;
         }
         snr = NULL;
         deadSocket = TRUE;
//         delete this;
      }
   }

   /*
   ** If there is still a socket notifier, turn it off if there is no data
   ** left to send, either binary or string.
   */
   if (snw && !isBinary() && snw)
   {
      tf = !writeBuffer.isEmpty();
      if (dbg) fprintf(stderr, "xSocketTCP::readyWrite(%d:%d):setting snw->Enabled to %d\n",
                               sfd, fd, tf);
      if (dbg) fflush(stderr);
      snw->setEnabled(tf);
   }
   else if (snw && isBinary() && bCnt == 0)
   {
      if (dbg > 3) fprintf(stderr, "xSocketTCP::readyWrite(%d:%d):Deleting binary buffer\n", sfd,fd);
      if (dbg > 3) fflush(stderr);
      if (bBuf != NULL)
         delete bBuf;
      bBuf = bPtr = NULL;
      bCnt = 0;
      bMode = FALSE;
      snw->setEnabled(FALSE);
      emit binaryWriteReady();
   }

   if (dbg > 3) fprintf(stderr, "xSocketTCP::readyWrite(%d:%d):Done\n", sfd,fd);
   if (dbg > 3) fflush(stderr);
}

struct sockaddr_in xSocketTCP::socketName(struct sockaddr_in *p)
{
   struct sockaddr_in addr;
   int x;

   if (dbg) fprintf(stderr, "xSocketTCP::socketName(%d):Enter deadSocket = %d\n",
                            fd, deadSocket);
   if (deadSocket)
   {
      if (dbg) fprintf(stderr, "xSocketTCP::socketName(%d):Dead Socket!!\n", fd);
      return(addr);
   }

   memset(&addr, 0, sizeof(sockaddr_in));
   if (p != NULL)
   {
      memset(p, 0, sizeof(sockaddr_in));
      p->sin_family = AF_INET;
//      p->sin_port = 0;
//      p->sin_addr.s_addr = htonl(INADDR_ANY);
      if (fd >= 0)
      {
         x = sizeof(sockaddr_in);
         if (getsockname(fd, (struct sockaddr *)p, (socklen_t *)&x) < 0)
            perror("Retrieving socket name");
         else
         {
            if (dbg) fprintf(stderr, "xSocketTCP::socketName(%d):1-Retrieved %d byte of %d\n",
                             fd,  x, sizeof(addr));
            if (dbg) fprintf(stderr, "xSocketTCP::socketName(%d):1-Host: %d, Port: %d\n",
                             fd, p->sin_addr.s_addr, p->sin_port);
         }
         return(addr);
      }
   }
   else
   {
      addr.sin_family = AF_INET;
//      addr.sin_port = 0;
//      addr.sin_addr.s_addr = htonl(INADDR_ANY);
      if (fd >= 0)
      {
         x = sizeof(addr);
         if (getsockname(fd, (struct sockaddr *)&addr, (socklen_t *)&x) < 0)
            perror("Retrieving socket name");
         else
         {
            if (dbg) fprintf(stderr, "xSocketTCP::socketName(%d):2-Retrieved %d byte of %d\n",
                             fd, x, sizeof(addr));
            if (dbg) fprintf(stderr, "xSocketTCP::socketName(%d):2-Host: %d, Port: %d\n",
                             fd, addr.sin_addr.s_addr, addr.sin_port);
         }
         return(addr);
      }
   }
   return(addr);
}

void xSocketTCP::off()
{
   if (dbg) fprintf(stderr, "xSocketTCP::off():Turning Socket Off!\n");
   if (dbg) fflush(stderr);
   socketOff = TRUE;
}

void xSocketTCP::sendToSocket(xSocketBuffer sb)
{
   if (dbg > 1) fprintf(stderr, "xSocketTCP::sendToSocket(xSocketBuffer):Enter- %d, 0x%x, 0x%x\n",
                             writeBuffer.isEmpty(), (int)bBuf, (int)snw);
   if (dbg > 1) fflush(stderr);
   if (deadSocket)
      return;
   if (writeBuffer.isEmpty() == TRUE && bBuf == NULL && snw != NULL)
   {
      if (dbg > 1) fprintf(stderr, "xSocketTCP::sendToSocket(xSocketBuffer):Sending %d bytes\n", sb.n);
      if (dbg > 1) fflush(stderr);
      bMode = TRUE;
      bBuf = new char[sb.n];
      bPtr = bBuf;
      if (dbg > 1) fprintf(stderr, "xSocketTCP::sendToSocket(xSocketBuffer):bBuf = %lx, bPtr = %lx, bCnt = %d\n",
                           (unsigned long)bBuf, (unsigned long)bPtr, bCnt);
      if (dbg > 1) fflush(stderr);
      bCnt = sb.n;
      memcpy(bBuf, sb.buf, sb.n);
      snw->setEnabled(TRUE);
   }
   if (dbg > 1) fprintf(stderr, "xSocketTCP::sendToSocket(xSocketBuffer):Exit\n");
   if (dbg > 1) fflush(stderr);
}

void xSocketTCP::sendToSocket(const char *pText)
{
   bool tf;
   int sfd = fd;

   if (bMode || deadSocket)
      return;

   if (!socketOff && snw != NULL && snr != NULL)
   {
      tf = isBinary();
      if (dbg) fprintf(stderr, "xSocketTCP::sendToSocket(%d:%d):bMode = %d\n",
                               sfd, fd, tf);
      if (dbg) fflush(stderr);
//      if (connectFlag == TRUE && fd >= 0)
      if (fd >= 0)
      {
         if (dbg) fprintf(stderr, "xSocketTCP::sendToSocket(%d:%d):Adding |%s| to writeBuffer\n",
                          sfd, fd, pText);
         if (dbg) fflush(stderr);
         writeBuffer += pText;
      }
      tf = !writeBuffer.isEmpty();
      if (dbg) fprintf(stderr, "xSocketTCP::sendToSocket(%d:%d):setting snw->Enabled to %d\n",
                               sfd, fd, tf);
      if (dbg) fflush(stderr);
      snw->setEnabled(tf);
   }
   else
   {
      if (dbg) fprintf(stderr, "xSocketTCP::sendToSocket(%d:%d):Read or Write slot not connected!\n",
                                sfd, fd);
      if (dbg) fflush(stderr);
   }
}

void xSocketTCP::stopConnect()
{
   int x = fd;

   if (deadSocket)
      return;
   if (dbg) fprintf(stderr, "xSocketTCP::stopConnect(%d):Enter\n", fd);
   if (dbg) fflush(stderr);
   pTimer->stop();
   QObject::disconnect(pTimer, SIGNAL(timeout()), this, SLOT(timeout()));
   disconnectTCP();
   if (snc)
   {
      QObject::disconnect(snc, SIGNAL(activated(int)), this, SLOT(retryConnect(int)));
      delete snc;
      errno = ECONNABORTED;
      emit status(errno);
   }
   snc = NULL;
   if (dbg) fprintf(stderr, "xSocketTCP::stopConnect(%d):Exit\n", x);
   if (dbg) fflush(stderr);
   deadSocket = TRUE;
//   delete this;
}

int xSocketTCP::disconnectTCP()
{
   int rv = 0, x = fd, sock;

   if (dbg) fprintf(stderr, "xSocketTCP::disconnectTCP(%d):Enter\n", fd);
   if (dbg) fflush(stderr);
   if (snw != NULL)
   {
      if (dbg) fprintf(stderr, "xSocketTCP::disconnectTCP():Disconnecting snw\n");
      if (dbg) fflush(stderr);
      QObject::disconnect(snw, SIGNAL(activated(int)), this, SLOT(readyWrite(int)));
      delete snw;
   }
   snw = NULL;
   if (snr != NULL)
   {
      if (dbg) fprintf(stderr, "xSocketTCP::disconnectTCP():Disconnecting snr\n");
      if (dbg) fflush(stderr);
      QObject::disconnect(snr, SIGNAL(activated(int)), this, SLOT(readyRead(int)));
      delete snr;
   }
   snr = NULL;
   if (sna != NULL)
   {
      if (dbg) fprintf(stderr, "xSocketTCP::disconnectTCP():Disconnecting sna\n");
      if (dbg) fflush(stderr);
      QObject::disconnect(sna, SIGNAL(activated(int)), this, SLOT(readyAccept(int)));
      sock = sna->socket();
      close(sock);
      delete sna;
   }
   sna = NULL;
   if (snc != NULL)
   {
      if (dbg) fprintf(stderr, "xSocketTCP::disconnectTCP():Disconnecting snc\n");
      if (dbg) fflush(stderr);
      QObject::disconnect(snc, SIGNAL(activated(int)), this, SLOT(retryConnect(int)));
      delete snc;
   }
   snc = NULL;
   if (fd >= 0)
   {
      if (dbg) fprintf(stderr, "xSocketTCP::disconnectTCP():Shutting down Socket!\n");
      if (dbg) fflush(stderr);
      rv = shutdown(fd, 2);
   }
   if (dbg) fprintf(stderr, "xSocketTCP::disconnectTCP():Calling readyRead()\n");
   if (dbg) fflush(stderr);
   connectFlag = FALSE;
   fd = -1;
   deadSocket = TRUE;
   if (dbg) fprintf(stderr, "xSocketTCP::disconnectTCP():Exit(%d)\n", x);
   if (dbg) fflush(stderr);
   return(rv);
}

//#include "xSocket.moc"

