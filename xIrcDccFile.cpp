/***************************************************************************
**    xIrcDccFile.cpp  $Revision: 1.6 $ - $Name:  $ 
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
#include <qt.h>
#include <xApp.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <qstring.h>
#include <qmessagebox.h>
#include <xSocket.h>
#include "xIrcLineEditQuery.h"
#include "xDefaults.h"
#include "xResources.h"
#include "xIrcDccFile.h"

static int dbg = 0;

extern xApplication *pApp;
extern xDefaults Defaults;
extern QPixmap *AppPixMap;

static const char *pInitialResources[] =
{
   NULL
};

xIrcDccFile::xIrcDccFile(xWidgetResInfo *pPRes, const char *fn,
                                   QWidget *pParent, const char *pName) :
                  xDialog(wdtRes = new xWidgetResInfo(pPRes, QString("dccfile"),
                                                      QString("DccFile")),
                          pParent, pName, 0)
{
   if (dbg) fprintf(stdout, "xIrcDccFile::xIrcDccFile():Enter\n");
   if (dbg) fflush(stdout);
   Resources->setWidgetInit(pInitialResources);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
   if (AppPixMap != NULL)
      setIcon(*AppPixMap);

   resizeCnt = -2;
   if (pName)
      setCaption(pName);

   fileName = fn;

   doSend = FALSE;
   waitingTransmit = FALSE;
   dataAckCnt = 0;
      
   if (dbg) fprintf(stdout, "xIrcDccFile::xIrcDccFile():Creating Timer\n");
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xIrcDccFile::xIrcDccFile():Creating TermFrame\n");
   if (dbg) fflush(stdout);
   pFrame = new xFrame(wdtRes, this);
   pFrame->setAlignment(xALIGN_Horz);
//   pFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
//   pFrame->setMargins(2, 2);
   pFrame->setWidgetSpacing(5);

   pFrame1 = new xFrame(wdtRes, this);
   pFrame1->setAlignment(xALIGN_Vert);
   pFrame1->setFrameStyle(QFrame::Panel | QFrame::Raised);
//   pFrame1->setMargins(2, 2);
   pFrame1->setWidgetSpacing(0);

   pFileName = new QLabel(fn, pFrame1);
   pFileName->setFrameStyle(QFrame::Panel | QFrame::Sunken);
   pFileName->setAlignment(AlignCenter);

   pMsgFrame = new xMultiLineFrame(wdtRes, pFrame1, NULL, 40, 3);

   pEditFrame = new xFrame(wdtRes, pFrame1);
   pEditFrame->setAlignment(xALIGN_Horz);
   pFrame->setMargins(2, 2);

   pBps = new QLineEdit(pEditFrame);
   pBps->setAlignment(xALIGN_Horz);
   pBps->setText("Bytes:");
   pGauge = new xProgressBar(wdtRes, pEditFrame);

   pEditFrame->addWidget(pBps);
   pEditFrame->addWidget(pGauge);

   pFrame1->addWidget(pFileName);
   pFrame1->addWidget(pMsgFrame);
   pFrame1->addWidget(pEditFrame);
   pFrame1->fitFrame();

   pFrame->addWidget(pFrame1);
   pFrame->fitFrame();

   pButtons = new xPshBtnFrame(wdtRes, this);
   pButtons->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pButtons->setAlignment(xALIGN_Horz);
//   pButtons->setResizeMode(xSPACE_Resize);
   pButtons->setWidgetSpacing(0);
//   pButtons->setMargins(2, 2);
   pButtons->addButton("Stop", btnClose);

   connect(pButtons, SIGNAL(clicked(int)), this, SLOT(buttonPressed(int)));
   if (dbg) fprintf(stdout, "xIrcDccFile::xIrcDccFile():Creating Edit Widget\n");
   if (dbg) fflush(stdout);

   addWidget(pFrame);
   addWidget(pButtons);
   setMargins(0,0);
   setWidgetSpacing(0);
   initFrame();
//   pSocket = new xIrcDccSocket();
}

xIrcDccFile::~xIrcDccFile()
{
   if (pSocket)
   {
      if (dbg) fprintf(stdout, "xIrcDccFile::~/xIrcDccFile():Deleting Socket\n");
      if (dbg) fflush(stdout);
      delete pSocket;
   }
   if (dbg) fprintf(stdout, "xIrcDccFile::~/xIrcDccFile():Exit\n");
   if (dbg) fflush(stdout);
}

int xIrcDccFile::connectTo(const char *pNetAddr, int _fd)
{
   char buf[512];
   int x, rv = 0;
   const char *cp;
   QString sLen;

   if (dbg) fprintf(stdout, "xIrcDccFile::connectTo():Responding to DCC File!!!\n");
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xIrcDccFile::connectTo():pNetAddr = |%s|\n", pNetAddr);
   if (dbg) fflush(stdout);

   for (x = 0, cp = pNetAddr; *cp; cp++)
   {
      if (*cp == ' ' && ++x == 3)
      {
         cp++;
         break;
      }
   }
   fSize = atoi(cp);
   if (dbg) fprintf(stdout, "xIrcDccFile::connectTo():File Size: %d\n", fSize);
   if (dbg) fflush(stdout);
   pGauge->setTotalSteps(atoi(cp));
   doSend = FALSE;
   fd = _fd;
   if ((pSocket = new xIrcDccSocket(wdtRes, this, pNetAddr, this, 
                                    SLOT(sockStatus(int)), 
                                    SLOT(sockProgress(const char*)), rv)) ==
        NULL ||
        rv != 0)
   {
      if (dbg) fprintf(stdout, "xIrcDccFile::connectTo():Attempt Failed\n");
      if (dbg) fflush(stdout);
      sprintf(buf, "Error connecting:%s\n", strerror(errno));
      pMsgFrame->pWin->putString(buf);
      pMsgFrame->pWin->putString("You may close this window any time now\n");
   }
   if (rv < 0)
   {
      if (dbg) fprintf(stdout, "xIrcDccFile::connectTo():Deleting socket object\n");
      if (dbg) fflush(stdout);
//      delete pSocket;
//      pSocket = NULL;
      return(errno);
   }
   else
   {
      pTimer = new QTimer(this);
      pTimer->stop();
      waitingTransmit = TRUE;
      connect(pTimer, SIGNAL(timeout()),
              this, SLOT(socketWait()));
      return(0);
   }
}

int xIrcDccFile::accept(int _fd)
{
   char buf[512], buf1[128];
   int rv = 0;

   if (dbg) fprintf(stdout, "xIrcDccFile::accept():Initiating DCC File!!!\n");
   if (dbg) fprintf(stdout, "xIrcDccFile::accept():Calling pSocket->accept()\n");
   if (dbg) fflush(stdout);
   doSend = TRUE;
   fd = _fd;
   if ((pSocket = new xIrcDccSocket(wdtRes, this, NULL,
                                    SLOT(sockStatus(int)), rv)) != NULL &&
       rv == 0)
   {
      if (dbg) fprintf(stdout, "xIrcDccFile::accept():pSocket->accept() exited ok\n");
      struct sockaddr_in addr = pSocket->socketName();
      sprintf(buf1, "%s:%d", inet_ntoa(addr.sin_addr), addr.sin_port);
      if (dbg) 
      {
         fprintf(stdout, "xIrcDccFile::accept():got socket name (%s) %ld:%d\n",
                                inet_ntoa(addr.sin_addr),
                                htonl(addr.sin_addr.s_addr),
                                addr.sin_port);
         fflush(stdout);
      }
      sprintf(buf, "Waiting connection on socket: %s\n", buf1);
      pMsgFrame->pWin->putString(buf);
      emit initiateDCCFile(this);
   }
   if (rv < 0)
   {
//      delete pSocket;
//      pSocket = NULL;
      return(errno);
   }
   else
      return(0);
} 

void xIrcDccFile::sockStatus(int err)
{
   if (err)
      failedConnection(err);
   else
      goodConnection(err);
}

void xIrcDccFile::failedConnection(int err)
{
   char buf[512];

   if (dbg) fprintf(stdout, "IrcDccFile::failedConnection():Failed!!!\n");
   if (dbg) fflush(stdout);
   disconnect(this);
//   delete pSocket;
//   pSocket = NULL;
   sprintf(buf, "\x02Connection Failed: %s\x02", strerror(err));
   pMsgFrame->pWin->putString(buf);
}

void xIrcDccFile::goodConnection(int sock)
{
   if (dbg) fprintf(stdout, "IrcDccFile::goodConnection():Socket Connected!\n");
   if (dbg) fflush(stdout);
   sock++;
   dataAckCnt = 0;
   dataAckBCnt = 0;
   dataAckTmp = 0;
   dataAcked = FALSE;
   waitingTransmit = FALSE;
   connect(pSocket, SIGNAL(readFromSocketBin(xSocketBuffer)),
           this, SLOT(socketIn(xSocketBuffer)));
   connect(pSocket, SIGNAL(socketClosed()),
           this, SLOT(socketClosed()));
   connect(pSocket, SIGNAL(binaryWriteReady()),
           this, SLOT(readyWrite()));
   pMsgFrame->pWin->putString("Connected!!");
   if (doSend)
   {
      int n;
      xSocketBuffer sb;
      char buff[2*1024];
      struct stat fs;

      fstat(fd, &fs);
      pGauge->setTotalSteps(fs.st_size);
      if ((n = read(fd, buff, sizeof(buff))) > 0)
      {
         sb.buf = (unsigned char *)buff;
         sb.n = n;
         pSocket->sendToSocket(sb);
         bCnt = n;
         dataAckCnt = n;
         waitingTransmit = TRUE;
         if (dbg) fprintf(stdout, "IrcDccFile::goodConnection():Sent %d bytes\n", n);
         if (dbg) fflush(stdout);
      }
      else if (n == 0)
      {
         pMsgFrame->pWin->putString("Empty File!");
         pMsgFrame->pWin->putString("You may close this window any time now\n");
      }
      else
      {
         sprintf(buff, "Error reading file:%s\n", strerror(errno));
         pMsgFrame->pWin->putString(buff);
         pMsgFrame->pWin->putString("You may close this window any time now\n");
      }
   }
   if (dbg) fprintf(stdout, "IrcDccFile::goodConnection():exit\n");
   if (dbg) fflush(stdout);
}

void xIrcDccFile::buttonPressed(int btn) 
{
   switch(btn)
   {
      case btnClose:
         closeFrame();
         break;
   }
}

void xIrcDccFile::closeFrame() 
{
   if (dbg) fprintf(stdout, "IrcDccFile::closeFrame():Enter\n");
   if (dbg) fflush(stdout);
   if (pSocket != NULL)
   {
      if (dbg) fprintf(stdout, "IrcDccFile::closeFrame():Calling pSocket->disconnect()\n");
      if (dbg) fflush(stdout);
      QObject::disconnect(pSocket, SIGNAL(readFromSocket(xSocketBuffer)),
              this, SLOT(socketIn(xSocketBuffer)));
      QObject::disconnect(pSocket, SIGNAL(socketClosed()),
              this, SLOT(socketClosed()));
      QObject::connect(pSocket, SIGNAL(binaryWriteReady()),
              this, SLOT(readyWrite()));
      pSocket->disconnectTCP();
   }
   close(fd);
   if (dbg) fprintf(stdout, "IrcDccChatFrame::closeFrame():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcDccFile::socketIn(xSocketBuffer sb)
{
   unsigned int x;
   xSocketBuffer wsb;

   if (dbg > 3) fprintf(stdout, "xIrcDccFile::socketIn(): Enter, doSend = %d, have %d bytes\n", doSend, sb.n);
   if (dbg > 3) fflush(stdout);
   if (doSend == TRUE)
   {
      for (x = 0; x < sb.n; x++)
      {
         dataAckTmp = dataAckTmp * 256;
         dataAckTmp += *sb.buf;
         if (dbg > 3) fprintf(stdout, "xIrcDccFile::socketIn(): dataAckTmp = 0x%x, BCnt = %d, sb = 0x%x, sizof(long) = %d\n",
                              dataAckTmp, dataAckBCnt, *sb.buf, sizeof(long));
         if (dbg > 3) fflush(stdout);
         sb.buf++;
         if (++dataAckBCnt == 4)
         {
            if (dbg > 3) fprintf(stdout, "xIrcDccFile::socketIn(): ntohl:dataAckTmp = 0x%lx, BCnt = %d, dataAckCnt = 0x%x\n",
                                 ntohl(dataAckTmp), dataAckBCnt, dataAckCnt);
            if (dbg > 3) fflush(stdout);
            if (dbg > 3) fprintf(stdout, "xIrcDccFile::socketIn():Got an Ack!\n");
            if (dbg > 3) fflush(stdout);
            dataAckTmp = 0;
            dataAckBCnt = 0;
         }
      }
   }
   else
   {
      if (dbg > 3) fprintf(stdout, "xIrcDccFile::socketIn(): Writing %d bytes to file\n", sb.n);
      if (dbg > 3) fflush(stdout);
      if (write(fd, sb.buf, sb.n) != (int)sb.n)
      {
         char buff[256];
         sprintf(buff, "Error Writing file:%s\n", strerror(errno));
         pMsgFrame->pWin->putString(buff);
         pMsgFrame->pWin->putString("You may close this window any time now\n");
      }
      else
      {
         QString s;
         dataAckBCnt += sb.n;
         if (dbg > 3) fprintf(stdout, "xIrcDccFile::socketIn(): Grand total of %d bytes rceived\n", dataAckBCnt);
         if (dbg > 3) fflush(stdout);
         x = dataAckBCnt;
         if ((dataAckBCnt % 128 == 0 || (int)dataAckBCnt == pGauge->totalSteps()) && 
              waitingTransmit == FALSE)
         {
            if (dbg > 3) fprintf(stdout, "xIrcDccFile::socketIn(): Acking for %d bytes\n", x);
            if (dbg > 3) fflush(stdout);
            x = htonl(x);
            wsb.buf = (unsigned char*)&x;
            wsb.n = sizeof(x);
            pSocket->sendToSocket(wsb);
            waitingTransmit = TRUE;
         }
         s.setNum(dataAckBCnt);
         pBps->setText(s);
         pGauge->setProgress(dataAckBCnt);
      }
   }
         
   if (dbg > 3) fprintf(stdout, "xIrcDccFile::socketIn(): Exit\n");
   if (dbg > 3) fflush(stdout);
}

void xIrcDccFile::readyWrite()
{
   int n;
   xSocketBuffer sb;
   char buff[2048];

   if (dbg > 3) fprintf(stdout, "xIrcDccFile::readyWrite():Enter, doSend = %d, dataAcked = %d\n",
                        doSend, dataAcked);
   if (dbg > 3) fflush(stdout);
   if (pSocket == NULL)
      return;

   if (doSend == TRUE)
   {
      QString s;
      if (dbg > 3) fprintf(stdout, "xIrcDccFile::readyWrite():Reading data from file\n");
      if (dbg > 3) fflush(stdout);
      s.setNum(dataAckCnt);
      pBps->setText(s);
      pGauge->setProgress(dataAckCnt);
      dataAcked = FALSE;
      if ((n = read(fd, buff, sizeof(buff))) > 0)
      {
         sb.buf = (unsigned char *)buff;
         sb.n = n;
         if (dbg > 3) fprintf(stdout, "xIrcDccFile::readyWrite():Sending %d bytes to socket...\n", sb.n);
         if (dbg > 3) fflush(stdout);
         pSocket->sendToSocket(sb);
         bCnt = n;
         dataAckCnt += n;
      }
      else if (n == 0)
      {
         pMsgFrame->pWin->putString("Transfer Complete");
         pMsgFrame->pWin->putString("You may close this window any time now\n");
         pSocket->disconnectTCP();
      }
      else
      {
         sprintf(buff, "Error reading file:%s\n", strerror(errno));
         pMsgFrame->pWin->putString(buff);
         pMsgFrame->pWin->putString("You may close this window any time now\n");
      }
   }
   else
        waitingTransmit = FALSE;

   if (dbg > 3) fprintf(stdout, "xIrcDccFile::readyWrite():Exit\n");
   if (dbg > 3) fflush(stdout);
}

void xIrcDccFile::socketWait()
{
   if (dbg > 3) fprintf(stdout, "xIrcDccFile::socketWait():Enter\n");
   if (dbg > 3) fflush(stdout);
   waitingTransmit = FALSE;
   if (pSocket != NULL && waited == TRUE)
   {
      if (dbg > 3) fprintf(stdout, "xIrcDccFile::socketWait():Sending ack!\n");
      if (dbg > 3) fflush(stdout);
      readyWrite();
   }
   else if (pSocket == NULL)
   {
      if (dbg > 3) fprintf(stdout, "xIrcDccFile::socketWait():Socket Closed!!\n");
      if (dbg > 3) fflush(stdout);
   }
   if (dbg > 3) fprintf(stdout, "xIrcDccFile::socketWait():Exit\n");
   if (dbg > 3) fflush(stdout);
}

void xIrcDccFile::socketClosed()
{
   if (dbg) fprintf(stdout, "xIrcDccFile::socketClosed():Enter\n");
   if (dbg) fflush(stdout);
   pApp->beep();
   QObject::disconnect(pSocket, SIGNAL(readFromSocket(xSocketBuffer)),
           this, SLOT(socketIn(xSocketBuffer)));
   QObject::disconnect(pSocket, SIGNAL(socketClosed()),
           this, SLOT(socketClosed()));
   QObject::connect(pSocket, SIGNAL(binaryWriteReady()),
           this, SLOT(readyWrite()));
   pMsgFrame->pWin->putString("***\x02 Connection Closed!!!\n");
};

//#include "xIrcDccFile.moc"

