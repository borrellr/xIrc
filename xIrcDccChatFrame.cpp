/***************************************************************************
**    xIrcDccChatFrame.cpp  $Revision: 1.5 $ - $Name:  $ 
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
#include <qt.h>
#include <xApp.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <qstring.h>
#include <qmessagebox.h>
#include <xSocket.h>
#include "xIrcLineEditQuery.h"
#include "xIrcSocket.h"
#include "xDefaults.h"
#include "xResources.h"
#include "xIrcDccChatFrame.h"

static bool dbg = false;

extern xDefaults Defaults;
extern QPixmap *AppPixMap;

static const char *pInitialResources[] =
{
   NULL
};

xIrcDccChatFrame::xIrcDccChatFrame(xWidgetResInfo *pPRes, QWidget *pParent,
                                   const char *pName) :
                  xDialog(wdtRes = new xWidgetResInfo(pPRes, QString("dccchat"),
                                                      QString("DccChat")),
                          pParent, pName, 0)
{
   if (dbg) fprintf(stdout, "xIrcDccChatFrame::xIrcDccChatFrame():Enter\n");
   if (dbg) fflush(stdout);
   char *cp, mircColors[256], ctcp2[256], beep[256];
   const char *ccp;
   int lines, cols, x;

   Resources->setWidgetInit(pInitialResources);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
   if (AppPixMap != NULL)
      setIcon(*AppPixMap);

   if ((ccp = Resources->get(wdtRes, "Columns", "Columns")) == NULL)
      cols = 80;
   else
      cols = atoi(ccp);
   if (cols < 10)
      cols = 80;

   if ((ccp = Resources->get(wdtRes, "lines", "Lines")) == NULL)
      lines = 11;
   else
      lines = atoi(ccp);

   if (lines < 3)
      lines = 11;

   if ((ccp = Resources->get(wdtRes, "mirccolors", "MircColors")) == NULL)
      ccp = "TRUE";
   for (cp = mircColors, x = 0; x < 256 && *ccp != '\0'; ccp++)
      *(cp++) = toupper(*ccp);
   *cp = '\0';

   if ((ccp = Resources->get(wdtRes, "ctcp2", "CTCP2")) == NULL)
      ccp = "FALSE";
   for (cp = ctcp2, x = 0; x < 256 && *ccp != '\0'; x++, ccp++)
      *(cp++) = toupper(*ccp);
   *cp = '\0';

   if ((ccp = Resources->get(wdtRes, "beep", "Beep")) == NULL)
      ccp = "FALSE";
   for (cp = beep, x = 0; x < 256 && *ccp != '\0'; x++, ccp++)
      *(cp++) = toupper(*ccp);
   *cp = '\0';

   resizeCnt = -2;
   if (pName)
      setCaption(pName);
      
   if (dbg) fprintf(stdout, "xIrcDccChatFrame::xIrcDccChatFrame():Creating Timer\n");
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xIrcDccChatFrame::xIrcDccChatFrame():Creating TermFrame\n");
   if (dbg) fflush(stdout);
   pMsgFrame = new xMultiLineFrame(wdtRes, this, NULL, cols, lines);
   connect(pMsgFrame, SIGNAL(textSelected(xMultiLineTextSelection)),
           this, SLOT(haveTextSelection(xMultiLineTextSelection)));
   if (dbg) fprintf(stdout, "xIrcDccChatFrame::xIrcDccChatFrame():Creating Button Bar\n");
   if (dbg) fflush(stdout);
   pFrame1 = new xFrame(wdtRes, xALIGN_Vert, xSPACE_Resize, 0, this);
   pFrame1->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pFrame1->setWidgetSpacing(1);
   pFrame1->setMargins(2, 2);

   pButtons = new xPshBtnFrame(wdtRes, pFrame1);
   pButtons->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pButtons->setAlignment(xALIGN_Horz);
   pButtons->setResizeMode(xSPACE_Resize);
   pButtons->setWidgetSpacing(0);
   pButtons->setMargins(2, 2);
   pButtons->addButton("Close", btnClose);
   pButtons->addButton("Clear Line", btnClear);
   pButtons->addButton("Show Colors", btnColors);
   pFrame1->addWidget(pButtons);

   connect(pButtons, SIGNAL(clicked(int)), this, SLOT(buttonPressed(int)));
   if (dbg) fprintf(stdout, "xIrcDccChatFrame::xIrcDccChatFrame():Creating Edit Widget\n");
   if (dbg) fflush(stdout);
   pFrame11 = new xFrame(wdtRes, xALIGN_Horz, xSPACE_Resize, 0, pFrame1);
//   pFrame11->setFrameStyle(QFrame::Box | QFrame::Plain);
   pFrame11->setMargins(2, 4);
   pFrame11->setWidgetSpacing(5);

   pMircColors = new QCheckBox("mIrc Colors", pFrame11);
   pMircColors->setFont(pButtons->font());
   pMircColors->adjustSize();
   connect(pMircColors, SIGNAL(toggled(bool)),
           this, SLOT(changeColors(bool)));
   if (strcmp(mircColors, "TRUE") == 0)
      pMircColors->setChecked(TRUE);
   changeColors(pMircColors->isChecked());

   pCtcp2 = new QCheckBox("CTCP2", pFrame11);
   pCtcp2->setFont(pButtons->font());
   pCtcp2->adjustSize();
   if (strcmp(ctcp2, "TRUE") == 0)
      pCtcp2->setChecked(TRUE);

   pBeepMsg = new QCheckBox("Beep on Message", pFrame11);
   pBeepMsg->setFont(pButtons->font());
   pBeepMsg->adjustSize();
   if (strcmp(beep, "TRUE") == 0)
      pBeepMsg->setChecked(TRUE);

   pFrame11->addWidget(pMircColors);
   pFrame11->addWidget(pCtcp2);
   pFrame11->addWidget(pBeepMsg);
   pFrame11->fitFrame();
   pFrame1->addWidget(pFrame11);
   pFrame1->fitFrame();

   pEdit = new xEdit(wdtRes, this);
   pEdit->setMaxLength(350);
   setFocusProxy(pEdit);
   setFocusPolicy(QWidget::StrongFocus);
   addWidget(pFrame1);
   addWidget(pMsgFrame);
   addWidget(pEdit);
   setMargins(0,0);
   setWidgetSpacing(0);
   initFrame();
   connect(pEdit, SIGNAL(returnPressed()), this, SLOT(gotKeyboardInput()));
//   pSocket = new xIrcDccSocket();
}

xIrcDccChatFrame::~xIrcDccChatFrame()
{
   if (pSocket)
      delete pSocket;
   if (pMsgFrame)
      delete pMsgFrame;
   if (pEdit)
      delete pEdit;
   if (pButtons)
      delete pButtons;
}

int xIrcDccChatFrame::connectTo(const char *pNetAddr)
{
   char buf[512];
   int rv = 0;

   if (dbg) fprintf(stdout, "xIrcDccChatFrame::connectTo():Responding to DCC Chat!!!\n");
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xIrcDccChatFrame::connectTo():pNetAddr = |%s|\n", pNetAddr);
   if (dbg) fflush(stdout);
   if ((pSocket = new xIrcDccSocket(wdtRes, this, pNetAddr, this, 
                                    SLOT(sockStatus(int)), 
                                    SLOT(sockProgress(const char*)), rv)) ==
        NULL ||
        rv != 0)
   {
      if (dbg) fprintf(stdout, "xIrcDccChatFrame::connectTo():Attempt Failed\n");
      if (dbg) fflush(stdout);
      sprintf(buf, "Error connecting:%s\n", strerror(errno));
      pMsgFrame->pWin->putString(buf);
      pMsgFrame->pWin->putString("You may close this window any time now\n");
   }
   if (rv < 0)
   {
      if (dbg) fprintf(stdout, "xIrcDccChatFrame::connectTo():Deleting socket object\n");
      if (dbg) fflush(stdout);
      delete pSocket;
      pSocket = NULL;
      return(errno);
   }
   else
      return(0);
}

int xIrcDccChatFrame::acceptdcc()
{
   struct sockaddr_in addr;
   char buf[512], buf1[128];
   int rv = 0;

   if (dbg) fprintf(stdout, "xIrcDccChatFrame::accept():Initiating DCC Chat!!!\n");
   if (dbg) fprintf(stdout, "xIrcDccChatFrame::accept():Calling pSocket->accept()\n");
   if (dbg) fflush(stdout);
   if ((pSocket = new xIrcDccSocket(wdtRes, this, NULL,
                                    SLOT(sockStatus(int)), rv)) != NULL &&
       rv == 0)
   {
      if (dbg) fprintf(stdout, "xIrcDccChatFrame::accept():pSocket->accept() exited ok\n");
      addr = pSocket->socketName();
#if 0
      if (dbg) 
      {
         fprintf(stdout, "xIrcDccChatFrame::accept():got socket name (%s) %ld:%d\n",
                                inet_ntoa(addr.sin_addr),
                                htonl(addr.sin_addr.s_addr),
                                addr.sin_port);
         fflush(stdout);
      }
#endif
      sprintf(buf1, "%s:%d", inet_ntoa(addr.sin_addr), addr.sin_port);
      sprintf(buf, "Waiting connection on socket: %s\n", buf1);
      pMsgFrame->pWin->putString(buf);
      emit initiateDCCChat(this);
   }
   if (rv < 0)
   {
      delete pSocket;
      pSocket = NULL;
      return(errno);
   }
   else
      return(0);
} 

void xIrcDccChatFrame::sockStatus(int err)
{
   if (err)
      failedConnection(err);
   else
      goodConnection(err);
}

void xIrcDccChatFrame::failedConnection(int err)
{
   char buf[512];

   if (dbg) fprintf(stdout, "IrcDccChatFrame::failedConnection():Failed!!!\n");
   if (dbg) fflush(stdout);
   disconnect(this);
   delete pSocket;
   pSocket = NULL;
   sprintf(buf, "\x02Connection Failed: %s\x02", strerror(err));
   pMsgFrame->pWin->putString(buf);
}

void xIrcDccChatFrame::goodConnection(int sock)
{
   if (dbg) fprintf(stdout, "IrcDccChatFrame::goodConnection():Socket Connected!\n");
   if (dbg) fflush(stdout);
   sock++;
   connect(this, SIGNAL(textOut(const char*)),
           pSocket, SLOT(sendToSocket(const char*)));
   connect(pSocket, SIGNAL(readFromSocket(char*)),
           this, SLOT(socketIn(char*)));
   connect(pSocket, SIGNAL(socketClosed()),
           this, SLOT(socketClosed()));
   pMsgFrame->pWin->putString("Connected!!");
   if (dbg) fprintf(stdout, "IrcDccChatFrame::goodConnection():exit\n");
   if (dbg) fflush(stdout);
}

void xIrcDccChatFrame::buttonPressed(int btn) 
{
   switch(btn)
   {
      case btnClose:
         closeFrame();
         break;

      case btnClear:
         clearLine();
         break;
   }
}

void xIrcDccChatFrame::closeFrame() 
{
   if (dbg) fprintf(stdout, "IrcDccChatFrame::closeFrame():Enter\n");
   if (dbg) fflush(stdout);
   if (pSocket != NULL)
   {
      if (dbg) fprintf(stdout, "IrcDccChatFrame::closeFrame():Calling pSocket->disconnect()\n");
      if (dbg) fflush(stdout);
      QObject::disconnect(this, SIGNAL(textOut(const char *)),
              pSocket, SLOT(sendToSocket(const char *)));
      QObject::disconnect(pSocket, SIGNAL(readFromSocket(char *)),
              this, SLOT(socketIn(char *)));
      QObject::disconnect(pSocket, SIGNAL(socketClosed()),
              this, SLOT(socketClosed()));
      pSocket->disconnectTCP();
      delete pSocket;
      pSocket = NULL;
   }
   delete this;
   if (dbg) fprintf(stdout, "IrcDccChatFrame::closeFrame():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcDccChatFrame::clearLine() 
{
   pEdit->setText("");
}

void xIrcDccChatFrame::gotKeyboardInput()
{
   char buf[512];
   const char *cp;
   QString msgStr;

   if (dbg) fprintf(stdout, "xIrcDccChatFrame::gotKeyboardInput():Got input!\n");
   if (dbg) fflush(stdout);
   strcpy(buf, pEdit->text());
   if (!isVisible())
      show();
   msgStr = parseLine(buf, pMircColors->isChecked(), pCtcp2->isChecked());
   cp = msgStr.latin1();
   if (*cp == '/')
      procCommand(++cp);
   else
   {
      if (!msgStr.isEmpty())
      {
         if (dbg) fprintf(stdout, "xIrcDccChatFrame::gotKeyboardInput():Sending input\n");
         if (dbg) fflush(stdout);
         sprintf(buf, "%s\n", (const char *)msgStr.latin1());
         emit textOut(buf);
         if (dbg) fprintf(stdout, "xIrcDccChatFrame::gotKeyboardInput():Putting it on the screen!\n");
         if (dbg) fflush(stdout);
         sprintf(buf, "-> %s\n", (const char *)msgStr.latin1());
         msgStr = translateMessage(buf);
         pMsgFrame->pWin->putString(msgStr.latin1());
      }
   }
   if (dbg) fprintf(stdout, "xIrcDccChatFrame::gotKeyboardInput():Clearing the edit field!\n");
   if (dbg) fflush(stdout);
   pEdit->setText("");
   if (dbg) fprintf(stdout, "xIrcDccChatFrame::gotKeyboardInput():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcDccChatFrame::procCommand(const char *pStr)
{
   char buf[1024], buf1[1024], pCmd[20], *cp;
   const char *cp1, *cp2;
   QString msgStr;
   
   if (dbg) fprintf(stdout, "xIrcDccChatFrame::procCommand():Entered w/ |%s|\n", pStr);
   if (dbg) fflush(stdout);
   for (cp = pCmd, cp1 = pStr; *cp1 && !isspace(*cp1); cp1++)
   {
      *(cp++) = toupper(*cp1);
      *cp = '\0';
   }
   if (dbg) fprintf(stdout, "xIrcDccChatFrame::procCommand():Cmd = |%s|\n", pCmd);
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xIrcDccChatFrame::procCommand():cp1 = |%s|\n", cp1);
   if (dbg) fflush(stdout);

   while (isspace(*cp1)) cp1++;
   if (strcmp(pCmd, "ME") == 0)
   {
      if (dbg) fprintf(stdout, "xIrcDccChatFrame::procCommand():Formatting text\n");
      if (dbg) fflush(stdout);
      sprintf(buf, "<== %s\n", cp1);
      if (dbg) fprintf(stdout, "xIrcDccChatFrame::procCommand():Emiting |%s| to socket\n", buf);
      if (dbg) fflush(stdout);
      emit textOut(buf);
      if (dbg) fprintf(stdout, "xIrcDccChatFrame::procCommand():Emiting |%s| to Screen\n", buf);
      if (dbg) fflush(stdout);
      msgStr = translateMessage(buf);
      pMsgFrame->pWin->putString(msgStr.latin1());
      if (dbg) fprintf(stdout, "xIrcDccChatFrame::procCommand():Done!!\n");
      if (dbg) fflush(stdout);
   }
   else if (strcmp(pCmd, "ALIAS") == 0)
   {
      for (cp2 = cp1; *cp2 && isspace(*cp2); cp2++);
      for (cp = buf, *cp = '\0'; *cp2 && !isspace(*cp2); cp2++)
      {
         *(cp++) = toupper(*cp2);
         *cp = '\0';
      }
      for (; *cp2 && isspace(*cp2); cp2++);
      Defaults.set(buf, cp2);
      sprintf(buf1, "*** $%s set to:\"%s\"\n", buf, cp2);
      msgStr = translateMessage(buf1);
      pMsgFrame->pWin->putString(msgStr.latin1());
   }
   if (dbg) fprintf(stdout, "xIrcDccChatFrame::procCommand():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcDccChatFrame::socketIn(char *pText)
{
   QString msgStr;
   char buf[1024];

   if (dbg > 3) fprintf(stdout, "xIrcDccChatFrame::socketIn(): Enter\n");
   if (dbg > 3) fflush(stdout);
   while (*pText)
   {
      if (*pText == '\n')
      {
         socketData += pText;
         if (dbg) fprintf(stdout, "xIrcDccChatFrame::socketIn(): Have String %s\n",
                              (const char *)socketData.latin1());
         if (dbg) fflush(stdout);
         msgStr = translateMessage(socketData.latin1());
         sprintf(buf, "<%s> %s", name(), (const char *)msgStr.latin1());
         pMsgFrame->pWin->putString(buf);
         socketData = "";
         if (pBeepMsg->isChecked())
            qApp->beep();
      }
      else if (*pText != '\r')
         socketData += *pText;
      pText++;
   }
   if (dbg > 3) fprintf(stdout, "xIrcDccChatFrame::socketIn(): Exit\n");
   if (dbg > 3) fflush(stdout);
}

void xIrcDccChatFrame::socketClosed()
{
      if (dbg) fprintf(stdout, "xIrcDccChatFrame::socketClosed():Enter\n");
      if (dbg) fflush(stdout);
      QObject::disconnect(this, SIGNAL(textOut(const char *)),
              pSocket, SLOT(sendToSocket(const char *)));
      QObject::disconnect(pSocket, SIGNAL(readFromSocket(char *)),
              this, SLOT(socketIn(char *)));
      QObject::disconnect(pSocket, SIGNAL(socketClosed()),
              this, SLOT(socketClosed()));
      qApp->beep();
      pMsgFrame->pWin->putString("***\x02 Connection Closed!!!\n");
      delete pSocket;
      pSocket = NULL;
};

void xIrcDccChatFrame::haveTextSelection(xMultiLineTextSelection msg)
{
   if (dbg) fprintf(stdout, "xIrcDccChatFrame::havetextSelection():Enter\n");
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xIrcDccChatFrame::havetextSelection():winName = |%s|, text = |%s|\n",
                             (const char *)msg.winName.latin1(), (const char *)msg.text.latin1());
   if (dbg) fflush(stdout);
   emit textSelected(msg);
}

void xIrcDccChatFrame::resizeEvent(QResizeEvent *pEvt)
{
   QSize sizeFrame, sizeNewFrame, sizeTerm;
   QSize s;

   if (dbg) fprintf(stdout, "xIrcDccChatFrame::resizeEvent():Enter - %d\n", resizeCnt);
   if (dbg) fflush(stdout);
   if (resizeCnt >= 0)
      resizeCnt++;
   if (dbg) fprintf(stdout, "xIrcDccChatFrame::resizeEvent():Resizing Frame - %d\n", resizeCnt);
   if (dbg) fflush(stdout);
   sizeFrame = pEvt->size();
   sizeTerm = sizeFrame;
   if (resizeCnt >= 0)
   {
      if (dbg) fprintf(stdout, "xIrcDccChatFrame::resizeEvent():Resizing Terminal Frame - %d\n", resizeCnt);
      if (dbg) fflush(stdout);
      sizeTerm.setHeight(sizeFrame.height() - (pFrame1->size().height() +
                                               pEdit->size().height()));
      pMsgFrame->resize(sizeTerm);
   }

   s = pFrame1->size();
   s.setWidth(sizeTerm.width());
   pFrame1->resize(&s);

   s = pEdit->size();
   s.setWidth(sizeTerm.width());
   pEdit->resize(s);

   sizeNewFrame = pMsgFrame->size();
   sizeNewFrame.setHeight(sizeNewFrame.height() + pEdit->size().height() +
                       pFrame1->size().height());
   if (sizeNewFrame != sizeFrame)
   {
      if (dbg) fprintf(stdout, "xIrcDccChatFrame::resizeEvent():Fittinging Frame - %d\n", resizeCnt);
      if (dbg) fflush(stdout);
      resize(&sizeNewFrame);
      initFrame(&sizeNewFrame);
   }
   if (dbg) fprintf(stdout, "xIrcDccChatFrame::resizeEvent():Fittinging Frame - %d\n", resizeCnt);
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xIrcDccChatFrame::resizeEvent():Done -  %d\n", resizeCnt);
   if (dbg) fflush(stdout);
   if (resizeCnt < 0)
      resizeCnt++;
   if (resizeCnt > 0)
      resizeCnt--;
      
   if (dbg) fprintf(stdout, "xIrcDccChatFrame::resizeEvent():Exit-  %d\n", resizeCnt);
   if (dbg) fflush(stdout);
}

void xIrcDccChatFrame::changeColors(bool f)
{
   setMircColors(pMsgFrame->pWin, f);
}

//#include "xIrcDccChatFrame.moc"

