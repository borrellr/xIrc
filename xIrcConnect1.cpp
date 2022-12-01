/***************************************************************************
**    xIrcConnect1.cpp  $Revision: 1.21 $ - $Name:  $ 
**    Class for making the server connections
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

#include <pwd.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <qt.h>
#include <qglobal.h>
#include <qapplication.h>
#include <qkeycode.h>
#include <qsize.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <xApp.h>
#include "xIrcChannelQuery.h"
#include "xIrcLineEditQuery.h"
#include "xIrcMsgDispatch.h"
#include "xIrcNickQuery.h"
#include "xIrcDccQuery.h"
#include "xIrcServerQuery.h"
#include "xIrcInviteBox.h"
#include "xIrcIgnoreQuery.h"
#include "xDefaults.h"
#include "xIrcConnect.h"

#define VERSION "2.3.8"
//static int dbg = 0;
#define dbg 0

extern xApplication *pApp;
extern xDefaults Defaults;
extern xIrcNickQuery *NickQuery;
extern xChannelQuery *ChanQuery;
extern xServerQuery *ServQuery;
extern xIrcLineEditQuery *QuitQuery;
extern xIrcCommands ircResponses;
extern QPixmap *AppPixMap;
extern xIrcMsgDispatch Dispatcher;

static const char *pInitialResources[] =
{
   "DCC.DirSave: ~/",
   "DCC.DirOpen: ~/",
   NULL
};

xIrcConnect::xIrcConnect(xWidgetResInfo *pPRes, QWidget *parent,
                         const char *name, WFlags iFlags,
                         int width, int height, int maxLines) :
              xDialog(wdtRes = new xWidgetResInfo(pPRes, QString("main"),
                                                  QString("Main")),
                      parent, name, FALSE, iFlags)
{
   struct passwd *pPasswdEnt;
   char buf[80];
   const char *pDirSave, *pDirOpen;

   if (dbg) fprintf(stdout, "xIrcConnect::xIrcConnect():Enter\n");
   if (dbg) fflush(stdout);
   Resources->setWidgetInit(pInitialResources);
   wdtPrv = new xWidgetResInfo(pPRes);
   if (dbg) fprintf(stdout, "xIrcConnect::xIrcConnect():Settingv colors\n");
   if (dbg) fflush(stdout);
   setDefPallet(this, wdtRes);
   if (dbg) fprintf(stdout, "xIrcConnect::xIrcConnect():Setting fonts\n");
   if (dbg) fflush(stdout);
   setDefFont(this, wdtRes);
   if (dbg) fprintf(stdout, "xIrcConnect::xIrcConnect():Setting Popup ResInfo\n");
   if (dbg) fflush(stdout);

   if (dbg) fprintf(stdout, "xIrcConnect::xIrcConnect():Initializing Variables\n");
   if (dbg) fflush(stdout);
   iFlags = 0;
   resizeCnt = -2;
   quitFlag = FALSE;
   serverFlag = FALSE;
   banFlag = FALSE;
   ignoreFlag = FALSE;
   pIrcMsgFrames = NULL;
//   pSocket = new xIrcSocket(this);
   if (dbg) fprintf(stdout, "xIrcConnect::xIrcConnect():Setting Socket to Null\n");
   if (dbg) fflush(stdout);
   pSocket = NULL;
   Dispatcher.setSocket(pSocket);

   if (dbg) fprintf(stdout, "xIrcConnect::xIrcConnect():Creating Main Window\n");
   if (dbg) fflush(stdout);
   pMainWin = new xMultiLineFrame(wdtRes, this, NULL, width, height, maxLines);
   if (AppPixMap != NULL)
      setIcon(*AppPixMap);
   setFocusPolicy(StrongFocus);
   pPasswdEnt = getpwuid(getuid());
   userName = pPasswdEnt->pw_name;
   realName = pPasswdEnt->pw_gecos;
   gethostname(buf, sizeof(buf));
   hostName = buf;

   if (dbg) fprintf(stdout, "xIrcConnect::xIrcConnect():Creating SocketBox\n");
   if (dbg) fflush(stdout);
   pSocketBox = new xSocketDialog(wdtPrv, NULL);

   if (dbg) fprintf(stdout, "xIrcConnect::xIrcConnect():Creating BanBox\n");
   if (dbg) fflush(stdout);
   pBanBox = new xIrcBanDialog(wdtPrv);
   if (dbg) fprintf(stdout, "xIrcConnect::xIrcConnect():Creating NickAction\n");
   if (dbg) fflush(stdout);
   pNickAction = NULL;
   pNickAction = new xIrcNickActionQuery(wdtPrv);

   pIgnore = new xIrcIgnoreQuery(wdtPrv);
   pNotify = new xIrcNotifyQuery(wdtPrv);
   pNotify->shutDown();
   pNotifyList = new xIrcNotifyTable(wdtPrv);

   connect(pNotify, SIGNAL(updateList(xIrcPeopleList*)),
           pNotifyList, SLOT(update(xIrcPeopleList*)));
   //pNotifyList->show(); // disble

   pLabel = new QLabel(this);

   pDirSave = Resources->get(wdtRes, "dcc.dirsave", "DCC.DirSave");
   pSaveDialog = new QFileDialog(pDirSave, QString::null, this);
   pSaveDialog->setCaption("xIrc- Dcc File Receive");
   pDirOpen = Resources->get(wdtRes, "dcc.diropen", "DCC.DirOpen");
   pOpenDialog = new QFileDialog(pDirOpen, QString::null, this);
   pOpenDialog->setCaption("xIrc- Dcc File Send");

   InitializeMenu();
/*
   pButtons = new xPshBtnFrame(wdtRes, this);
   pButtons->setFrameStyle(QFrame::Panel | QFrame::Raised);
   pButtons->setAlignment(xALIGN_Horz);
   pButtons->setResizeMode(xSPACE_Resize);
   pButtons->setWidgetSpacing(2);
   pButtons->setMargins(2, 2);
   pButtons->addButton("Server Selection", btnServer);
   pButtons->addButton("Channels / People", btnChannel);
   pButtons->addButton("Nick Actions", btnAction);
   pButtons->addButton("Your Nick", btnNick);
   pButtons->addButton("Quit", btnQuit);
   connect(pButtons, SIGNAL(clicked(int)), this, SLOT(buttonPressed(int)));
*/

   if (dbg) fprintf(stdout, "xIrcConnect::xIrcConnect():Connecting xNickActionQuery Signals/slots\n");
   if (dbg) fflush(stdout);
   connect(pNickAction, SIGNAL(done(xMultiLineTextSelection)), 
          this, SLOT(nickActionHandler(xMultiLineTextSelection)));
   connect(pMainWin, SIGNAL(textSelected(xMultiLineTextSelection)),
           pNickAction, SLOT(wakeUp(xMultiLineTextSelection)));
   connect(pNotifyList, SIGNAL(selected(const char*)),
           pNickAction, SLOT(wakeUp(const char*)));

   connect(ServQuery, SIGNAL(msgToSocket(const char *)),
           this, SLOT(sendMsgToSocket(const char *)));
   connect(ChanQuery, SIGNAL(hasResult(int)),
           this, SLOT(gotChanBoxResult(int)));
   pButtons = NULL;
/*
   connect(pSocket, SIGNAL(socketClosed()),
           this, SLOT(socketClosed()));
   connect(pSocket, SIGNAL(ircServerMessage(xIrcMessage *)),
           this, SLOT(recvMessage(xIrcMessage *)));
   connect(this, SIGNAL(textOut(const char *)), 
           pSocket, SLOT(sendToSocket(const char *)));
   connect(ChanQuery, SIGNAL(ircMessageOut(xIrcMessage*)),
           pSocket, SLOT(sendIrcServerMessage(xIrcMessage*)));
   connect(pSocket, SIGNAL(connecting(const char *)),
           pSocketBox, SLOT(connectMessage(const char *)));
   connect(pSocketBox, SIGNAL(stopConnection()),
           pSocket, SLOT(stopConnect()));
   connect(pSocketBox, SIGNAL(nextConnection()),
           pSocket, SLOT(tryNextConnection()));
   addWidget(pButtons);
*/
   addWidget(pLabel);
   addWidget(pMainWin);
   setMargins(0,0);
   setWidgetSpacing(0);
   initFrame();
}

xIrcConnect::~xIrcConnect() 
{
   xIrcMessageFrame *pM, *pM1;

//   disconnect(this);
   if (dbg) fprintf(stdout, "xIrcConnect::~xIrcConnect():Enter\n");
   if (dbg) fprintf(stdout, "xIrcConnect::~xIrcConnect():Deleting Socket\n");
   if (dbg) fflush(stdout);
   if (pSocket)
      delete pSocket;
   Dispatcher.setSocket(NULL);
   pNotify->shutDown();
   if (dbg) fprintf(stdout, "xIrcConnect::~xIrcConnect():Deleting Buttons\n");
   if (dbg) fflush(stdout);
   if (pButtons)
      delete pButtons;

   if (dbg) fprintf(stdout, "xIrcConnect::~xIrcConnect():Nick Action Query\n");
   if (dbg) fflush(stdout);
   if (pNickAction)
      delete pNickAction;

   if (pIrcMsgFrames)
   {
      if (dbg) fprintf(stdout, "xIrcConnect::~xIrcConnect():Deleting Message Frames\n");
      if (dbg) fflush(stdout);
      for (pM = pIrcMsgFrames; pM != NULL; pM = pM1)
      {
         if (dbg) fprintf(stdout, "xIrcConnect::~xIrcConnect():Pointing to next Message Frame\n");
         if (dbg) fflush(stdout);
         pM1 = pM->next();
         if (dbg) fprintf(stdout, "xIrcConnect::~xIrcConnect():Deleting Message Frame\n");
         if (dbg) fflush(stdout);
         delete pM;
      }
   }
   if (dbg) fprintf(stdout, "xIrcConnect::~xIrcConnect():Main Window\n");
   if (dbg) fflush(stdout);
   if (pMainWin)
      delete pMainWin;
   if (dbg) fprintf(stdout, "xIrcConnect::~xIrcConnect():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcConnect::InitializeMenu()
{
   xWidgetResInfo wdtTmp(wdtRes, QString("menu"), QString("Menu"));
   xWidgetResInfo wdtPopTmp(&wdtTmp, QString("popup"), QString("Popup"));

   pNickMenu = new QPopupMenu;
   setDefFont(pNickMenu, &wdtPopTmp);
   setDefPallet(pNickMenu, &wdtPopTmp);
   pNickMenu->insertItem("Show Folks Online", this, SLOT(showNotifyList()));
   pNickMenu->insertItem("Action", this, SLOT(showNickAction()));
   pNickMenu->insertItem("Change Nick", this, SLOT(newNick()));
   setDefFont(pNickMenu, &wdtPopTmp);
   setDefPallet(pNickMenu, &wdtPopTmp);

   pListMenu = new QPopupMenu;
   setDefFont(pListMenu, &wdtPopTmp);
   setDefPallet(pListMenu, &wdtPopTmp);
   pListMenu->insertItem("Server List", this, SLOT(newServer()));
   pListMenu->insertItem("Ignore List", this, SLOT(showIgnore()));
   pListMenu->insertItem("Notify List", this, SLOT(showNotify()));
   setDefFont(pListMenu, &wdtPopTmp);
   setDefPallet(pListMenu, &wdtPopTmp);

   pFileMenu = new QPopupMenu;
   setDefFont(pFileMenu, &wdtPopTmp);
   setDefPallet(pFileMenu, &wdtPopTmp);
   pFileMenu->insertItem("Connect Server", this, SLOT(newServer()));
   pFileMenu->insertItem("Open Channel", this, SLOT(newChannel()));
   pFileMenu->insertSeparator();
   pFileMenu->insertItem("Exit", this, SLOT(quitIrc()));
   setDefFont(pFileMenu, &wdtPopTmp);
   setDefPallet(pFileMenu, &wdtPopTmp);

   pHelpMenu = new QPopupMenu;
   CHECK_PTR(pHelpMenu);
   pHelpMenu->insertItem("&About", this, SLOT(about()));
   setDefFont(pHelpMenu, &wdtPopTmp);
   setDefPallet(pHelpMenu, &wdtPopTmp);

   pMenu = new QMenuBar(this);
   setDefFont(pMenu, &wdtPopTmp);
   setDefPallet(pMenu, &wdtPopTmp);
   pMenu->insertItem("File", pFileMenu);
   pMenu->insertItem("Nick", pNickMenu);
   pMenu->insertItem("Lists", pListMenu);
   pMenu->insertSeparator();
   pMenu->insertItem("Help", pHelpMenu);
   setDefFont(pMenu, &wdtTmp);
   setDefPallet(pMenu, &wdtTmp);
}

void xIrcConnect::about()
{
   QMessageBox::about(this, "About xIrc",
                  "Version: " VERSION "\n"
                  "License: GPL\n"
                  "Copyright: 1997-2001\n\n"
                  "Maintained by Robert Borrell\n"
                  "Developed by Joe Croft");
}

void xIrcConnect::showNickAction()
{
   pNickAction->show();
}

void xIrcConnect::showIgnore()
{
   pIgnore->show();
}

void xIrcConnect::showNotify()
{
   pNotify->show();
}

void xIrcConnect::showNotifyList()
{
   if (pNotifyList->isVisible() != TRUE)
      pNotifyList->show();
   else
      pNotifyList->hide();
}

void xIrcConnect::buttonPressed(int btn) 
{
   switch(btn)
   {
      case btnServer:
         newServer();
         break;

      case btnChannel:
         newChannel();
         break;

      case btnAction:
         pNickAction->show();
         break;

      case btnNick:
         newNick();
         break;

      case btnQuit:
         quitIrc();
         break;
   }
}

void xIrcConnect::quitIrc()
{
   char buf[256];
   
   if (QuitQuery->exec() != QDialog::Rejected)
   {
      if (pSocket != NULL)
      {
         if (dbg) fprintf(stdout, "xIrcConnect::quitIrc():Disconnecting from Server\n");
         if (dbg) fflush(stdout);
         sprintf(buf, "QUIT :%s\n", QuitQuery->text());
         sendMsgToSocket(buf);
         pSocket->off();
         quitFlag = TRUE;
      }
      else
         pApp->quit();
   }
}

void xIrcConnect::connStatus(int err)
{
   if (err) {
      fprintf(stderr, "Connection failed..\n");
      fflush(stderr);
      failedConnection(err);
   }
   else {
      if(dbg) {
         fprintf(stderr, "Connection successful..\n");
         fflush(stderr);
      }
      goodConnection(err);
   }
}

void xIrcConnect::goodConnection(int sock)
{
   const char *pRealName, *pUserName, *pHostName, *cp;
   xIrcMessageFrame *pMsgFrame;
   QString strTmp;
   char buf[256];
//   int x;

   if (dbg) fprintf(stdout, "xIrcConnect::goodConnection():Success!!!\n");
   if (dbg) fflush(stdout);
   if (dbg)
   {
      struct sockaddr_in addr;
      addr = pSocket->socketName();

/*      fprintf(stdout, "xIrcConnect::goodConnection():My IP is:%s (%ld %d)\n",
                      inet_ntoa(addr.sin_addr), 
                      htonl(addr.sin_addr.s_addr), addr.sin_port);*/
   }
   pSocketBox->connected(QString("Connected: Waiting Response from Host").latin1());
// pMainWin->pWin->putString("Connected!!!\n");

/*
   disconnect(pSocket, SIGNAL(connAccepted(int)),      
              this, SLOT(goodConnection(int)));
   disconnect(pSocket, SIGNAL(connFailed(int)),      
              this, SLOT(failedConnection(int)));
*/
   sprintf(buf, "NICK %s\n", NickQuery->text());

   sock++;
   sendMsgToSocket(buf);
   if ((pRealName = Defaults.get("REALNAME")) == NULL)
      pRealName = realName.latin1();
   if ((cp = Defaults.get("EMAIL_ADDR")) == NULL)
   {
      pHostName = "dummy.hostname.org";
      pUserName = userName.latin1();
   }
   else
   {
      for (strTmp = ""; *cp && *cp != '@'; cp++)
         strTmp += *cp;
      pUserName = strTmp.latin1();
      if (*cp)
         pHostName = ++cp;
      else
         pHostName = "dummy.hostname.org";
   }
   sprintf(buf, "USER %s %s %s :%s\n", pUserName, pHostName,
                                      ServQuery->server(), pRealName);
   sendMsgToSocket(buf);

   for (pMsgFrame = pIrcMsgFrames; pMsgFrame != NULL; pMsgFrame = pMsgFrame->next())
   {
      cp = (char *)pMsgFrame->name();
      if (strlen(cp) > 0 && *cp == '#')
      {
         sprintf(buf, "JOIN %s\n", pMsgFrame->name());
         sendMsgToSocket(buf);
      }
   }
   sprintf(buf, "xIrc - %s", NickQuery->text());
   setCaption(buf);
   Dispatcher.setSocket(pSocket);
}

void xIrcConnect::failedConnection(int err)
{
   char buf[512];

   if (dbg) fprintf(stdout, "IrcConnect::failedConnection():Failed!!!\n");
   if (dbg) fflush(stdout);
   pSocketBox->hide();
   if (dbg) fprintf(stdout, "IrcConnect::failedConnection():Discconecting signals\n");
   if (dbg) fflush(stdout);
/*
   disconnect(pSocket, SIGNAL(connAccepted(int)),      
              this, SLOT(goodConnection(int)));
   disconnect(pSocket, SIGNAL(connFailed(int)),      
              this, SLOT(failedConnection(int)));
*/
   disconnect(pSocket, SIGNAL(socketClosed()),
           this, SLOT(socketClosed()));
   disconnect(pSocket, SIGNAL(ircServerMessage(xIrcMessage *)),
           this, SLOT(recvMessage(xIrcMessage *)));
   disconnect(this, SIGNAL(textOut(const char *)), 
           pSocket, SLOT(sendToSocket(const char *)));
   disconnect(this, SIGNAL(msgOut(xIrcMessage *)), 
           pSocket, SLOT(sendIrcServerMessage(xIrcMessage *)));
   disconnect(ChanQuery, SIGNAL(ircMessageOut(xIrcMessage*)),
           pSocket, SLOT(sendIrcServerMessage(xIrcMessage*)));
   disconnect(pSocketBox, SIGNAL(stopConnection()),
           this, SLOT(stopConnection()));
/*
   disconnect(pSocket, SIGNAL(connecting(const char *)),
           pSocketBox, SLOT(connectMessage(const char *)));
   disconnect(pSocketBox, SIGNAL(nextConnection()),
           pSocket, SLOT(tryNextConnection()));
*/
   if (dbg) fprintf(stdout, "IrcConnect::failedConnection():Showing Error\n");
   if (dbg) fflush(stdout);
   sprintf(buf, "[B]***Connection Failed: %s\n", strerror(err));
   pMainWin->pWin->putString(buf);
   delete pSocket;
   pSocket = NULL;
   Dispatcher.setSocket(pSocket);
   pNotify->shutDown();
//   if (dbg) fprintf(stdout, "IrcConnect::failedConnection():Calling newServer\n");
//   if (dbg) fflush(stdout);
//   newServer();
}

void xIrcConnect::newServer()
{
   int btn, err;
   char buf[512];

   if (dbg) fprintf(stdout, "xIrcConnect::newServer():Enter\n");
   if (dbg) fflush(stdout);
   if ((btn = ServQuery->exec()) != QDialog::Rejected)
   {
      if (pSocket != NULL && (btn == xServerQuery::Disconnect ||
                                      btn == xServerQuery::Accepted))
      {
         if (dbg) {
             fprintf(stderr, "xIrcConnect::newServer():Disconnecting from Current Socket\n");
             fflush(stderr);
         }
         sprintf(buf, "QUIT :Changing Servers\n");
         sendMsgToSocket(buf);
         disconnect(pSocket, SIGNAL(socketClosed()),
                 this, SLOT(socketClosed()));
         disconnect(pSocket, SIGNAL(ircServerMessage(xIrcMessage *)),
                 this, SLOT(recvMessage(xIrcMessage *)));
/*
         disconnect(this, SIGNAL(textOut(const char *)), 
                 pSocket, SLOT(sendToSocket(const char *)));
         disconnect(this, SIGNAL(msgOut(xIrcMessage *)), 
                 pSocket, SLOT(sendIrcServerMessage(xIrcMessage *)));
         disconnect(ChanQuery, SIGNAL(ircMessageOut(xIrcMessage*)),
                 pSocket, SLOT(sendIrcServerMessage(xIrcMessage*)));
         disconnect(pSocketBox, SIGNAL(stopConnection()),
                 pSocket, SLOT(stopConnect()));
*/
/*
         disconnect(pSocket, SIGNAL(connecting(const char *)),
                 pSocketBox, SLOT(connectMessage(const char *)));
         disconnect(pSocketBox, SIGNAL(nextConnection()),
                 pSocket, SLOT(tryNextConnection()));
*/
         if (dbg) fprintf(stderr, "xIrcConnect::newServer():Disconnection done\n");
         if (dbg) fflush(stderr);
      }
      if (btn == xServerQuery::Accepted)
      {
         if (dbg) {
            fprintf(stderr, "xIrcConnect::newServer():Creating new socket\n");
            fflush(stderr);
         }
         pSocket = new xIrcSocket(wdtPrv, this,
                                  ServQuery->server(), ServQuery->port(),
                                  pSocketBox, SLOT(connStatus(int)),
                                  SLOT(connectMessage(const char *)), err);
         CHECK_PTR(pSocket);
         if (dbg) {
            fprintf(stderr, "xIrcConnect::newServer():New socket created.\n");
            fflush(stderr);
         }
         Dispatcher.setSocket(pSocket);
         pNotify->shutDown();
         if (err == 0)
         {
            if(dbg) fprintf(stderr, "Connecting signals...\n");
            connect(pSocket, SIGNAL(socketClosed()),
                    this, SLOT(socketClosed()));
            connect(pSocket, SIGNAL(ircServerMessage(xIrcMessage *)),
                    this, SLOT(recvMessage(xIrcMessage *)));
            connect(this, SIGNAL(textOut(const char *)), 
                    pSocket, SLOT(sendToSocket(const char *)));
            connect(this, SIGNAL(msgOut(xIrcMessage *)), 
                    pSocket, SLOT(sendIrcServerMessage(xIrcMessage *)));
            connect(ChanQuery, SIGNAL(ircMessageOut(xIrcMessage*)),
                    pSocket, SLOT(sendIrcServerMessage(xIrcMessage*)));
            connect(pSocketBox, SIGNAL(stopConnection()),
                    this, SLOT(stopConnection()));
            pSocket->connectToHost();
         }
/*
         connect(pSocket, SIGNAL(connecting(const char *)),
                 pSocketBox, SLOT(connectMessage(const char *)));
         connect(pSocketBox, SIGNAL(stopConnection()),
                 pSocket, SLOT(stopConnect()));
         connect(pSocketBox, SIGNAL(nextConnection()),
                 pSocket, SLOT(tryNextConnection()));

         connect(pSocket, SIGNAL(connAccepted(int)),      
                 this, SLOT(goodConnection(int)));
         connect(pSocket, SIGNAL(connFailed(int)),      
                 this, SLOT(failedConnection(int)));
         if (dbg) fprintf(stderr, "xIrcConnect::newServer():calling xSocket::connectTo\n");
         if (dbg) fflush(stderr);
         if (pSocket->connectTo(ServQuery->server(), ServQuery->port()) == FALSE)
*/
         else
         {
            if (dbg) fprintf(stderr, "xIrcConnect::newServer():Got an error\n");
            if (dbg) fflush(stderr);
/*
            disconnect(pSocket, SIGNAL(connAccepted(int)),      
                       this, SLOT(goodConnection(int)));
            disconnect(pSocket, SIGNAL(connFailed(int)),      
                       this, SLOT(failedConnection(int)));
            pSocketBox->hide();
            QMessageBox::warning(this, "Error", "Connection Failed");
*/
            if (dbg) fprintf(stderr, "xIrcConnect::newServer():Failed!!!\n");
            if (dbg) fflush(stderr);
            delete pSocket;
            pSocket = NULL;
            Dispatcher.setSocket(pSocket);
            pNotify->shutDown();
            newServer();
         }
         if (dbg) fprintf(stderr, "xIrcConnect::newServer():Waiting connection\n");
         if (dbg) fflush(stderr);
      }
      else
      {
         pSocket = NULL;
         Dispatcher.setSocket(pSocket);
         pNotify->shutDown();
      }
   }
}

void xIrcConnect::newNick()
{
   char buf[256];

   for (;;)
   {
      NickQuery->exec();
      if (NickQuery->result() == QDialog::Accepted)
      {
         nickName = NickQuery->text();
         sprintf(buf, "NICK %s\n", NickQuery->text());
         sendMsgToSocket(buf);
         break;
      }
      else if((strlen(nickName.latin1()) == 0) && (NickQuery->result() != Rejected))
         QMessageBox::warning(this, "Error", "You must choose a NickName");
      else
         break;
   }
   if(NickQuery->result() == QDialog::Accepted) {
      sprintf(buf, "xIrc - %s", NickQuery->text());
      setCaption(buf);
   }
} 

void xIrcConnect::stopConnection()
{
   
   disconnect(pSocket, SIGNAL(socketClosed()),
              this, SLOT(socketClosed()));
   disconnect(pSocket, SIGNAL(ircServerMessage(xIrcMessage *)),
              this, SLOT(recvMessage(xIrcMessage *)));
   disconnect(this, SIGNAL(textOut(const char *)), 
              pSocket, SLOT(sendToSocket(const char *)));
   disconnect(this, SIGNAL(msgOut(xIrcMessage *)), 
              pSocket, SLOT(sendIrcServerMessage(xIrcMessage *)));
   disconnect(ChanQuery, SIGNAL(ircMessageOut(xIrcMessage*)),
              pSocket, SLOT(sendIrcServerMessage(xIrcMessage*)));
   disconnect(pSocketBox, SIGNAL(stopConnection()),
              this, SLOT(stopConnection()));
   pSocket->stopConnect();
   delete pSocket;
   pSocket = NULL;
   Dispatcher.setSocket(pSocket);
   pNotify->shutDown();
}

void xIrcConnect::socketClosed()
{
      char buf[512];

      if (dbg) fprintf(stdout, "xIrcConnect::socketClosed():Enter\n");
      if (dbg) fflush(stdout);
      serverFlag = FALSE;
      if (dbg) fprintf(stdout, "xIrcConnect::socketClosed():Disconnecting signals\n");
      if (dbg) fflush(stdout);
/*
      disconnect(pSocket, SIGNAL(connAccepted(int)),      
                 this, SLOT(goodConnection(int)));
      disconnect(pSocket, SIGNAL(connFailed(int)),      
                 this, SLOT(failedConnection(int)));
*/
      disconnect(pSocket, SIGNAL(socketClosed()),
              this, SLOT(socketClosed()));
      disconnect(pSocket, SIGNAL(ircServerMessage(xIrcMessage *)),
              this, SLOT(recvMessage(xIrcMessage *)));
      disconnect(this, SIGNAL(textOut(const char *)), 
              pSocket, SLOT(sendToSocket(const char *)));
      disconnect(this, SIGNAL(msgOut(xIrcMessage *)), 
              pSocket, SLOT(sendIrcServerMessage(xIrcMessage *)));
      disconnect(ChanQuery, SIGNAL(ircMessageOut(xIrcMessage*)),
              pSocket, SLOT(sendIrcServerMessage(xIrcMessage*)));
      disconnect(pSocketBox, SIGNAL(stopConnection()),
              this, SLOT(stopConnection()));
/*
      disconnect(pSocket, SIGNAL(connecting(const char *)),
              pSocketBox, SLOT(connectMessage(const char *)));
      disconnect(pSocketBox, SIGNAL(nextConnection()),
              pSocket, SLOT(tryNextConnection()));
*/
      delete pSocket;
      pSocket = NULL;
      Dispatcher.setSocket(pSocket);
      pNotify->shutDown();
      if (dbg) fprintf(stdout, "xIrcConnect::socketClosed():Signals disconnected\n");
      if (dbg) fflush(stdout);
      if (quitFlag)
         pApp->quit();
      else
      {
         pApp->beep();
         sprintf(buf, "[B]***Connection Closed!!\n");
         pMainWin->pWin->putString(buf);
//         newServer();
      }
      if (dbg) fprintf(stdout, "xIrcConnect::socketClosed():Exit\n");
      if (dbg) fflush(stdout);
};

void xIrcConnect::joinChannel(const char *channel)
{
    const char *cp;
    xIrcMessageFrame *pMsg;
    char buf[256];

    if (dbg) fprintf(stdout, "xIrcConnect::gotChannelBoxResults():Creating Message Frame\n");
    if (dbg) fflush(stdout);
    if ((pMsg = makeNewMsgFrame(NULL, channel)) != NULL)
    {
       if (dbg) fprintf(stdout, "xIrcConnect::gotChannelBoxResults():Showing Message Frame\n");
       if (dbg) fflush(stdout);

       pMsg->show();
       if (dbg) fprintf(stdout, "xIrcConnect::gotChannelBoxResults():Getting channel name\n");
       if (dbg) fflush(stdout);

       cp = channel;
       if (*cp == '#')
       {
          if (dbg) fprintf(stdout, "xIrcConnect::gotChannelBoxResults():Joining channel\n");
          if (dbg) fflush(stdout);
          sprintf(buf, "JOIN %s\n", channel);
          sendMsgToSocket(buf);
//        connect(pMsg, SIGNAL(nickSelected(xMultiLineTextSelection)), 
//            pNickAction, SLOT(wakeUp(xMultiLineTextSelection)));
       }
    }     
}

void xIrcConnect::gotChanBoxResult(int result)
{
   xIrcDccChatFrame *pDccChatFrame;
   char buf[256];
   int err;
   
   if (dbg) fprintf(stdout, "xIrcConnect::gotChannelBoxResults():Enter\n");
   if (dbg) fflush(stdout);
   if (pSocket == NULL)
   {
      QMessageBox::warning(this, "Error", "Not Connected to a Server");
      return;
   }

   if (result == xChannelQuery::Accepted)
   {
      if (findMsgFrame(ChanQuery->editText()) == NULL)
      {
         joinChannel(ChanQuery->editText());
      }
      else
         QMessageBox::warning(this, "Error", "Channel already opened");
   }
   else if (result == xChannelQuery::DccChat)
   {
      struct sockaddr_in addr = pSocket->socketName();
      sprintf(buf, "%d", addr.sin_addr.s_addr);
      if ((pDccChatFrame = new xIrcDccChatFrame(wdtPrv, NULL, 
                                                ChanQuery->editText())) != NULL)
      {
         connect(pDccChatFrame, SIGNAL(initiateDCCChat(xIrcDccChatFrame*)),
                 this, SLOT(initiateDCCChat(xIrcDccChatFrame*)));
         if ((err = pDccChatFrame->acceptdcc()) == 0)
         {
            connect(pDccChatFrame, SIGNAL(textSelected(xMultiLineTextSelection)),
                    pNickAction, SLOT(wakeUp(xMultiLineTextSelection)));
            pDccChatFrame->show();
         }
         else
         {
            sprintf(buf, "Error Accepting Connection from %s:%s",
                         ChanQuery->editText(), strerror(err));
            QMessageBox::warning(this, "Error", buf);
            disconnect(pDccChatFrame, SIGNAL(initiateDCCChat(xIrcDccChatFrame*)),
                       this, SLOT(initiateDCCChat(xIrcDccChatFrame*)));
            delete pDccChatFrame;
         }
      }
   }
   if (dbg) fprintf(stdout, "xIrcConnect::gotChannelBoxResults():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcConnect::newChannel()
{
   ChanQuery->show();
}

bool xIrcConnect::isMsg(int cmd, char *pStr)
{
   return(ircResponses.is(cmd, pStr));
}

void xIrcConnect::sendMsgToSocket(const char *pBuf)
{
   emit textOut(pBuf);
}

void xIrcConnect::sendMessage(xIrcMessage *pMsg)
{
   Dispatcher.dispatchMsg(NULL, NULL, pMsg);
}

void xIrcConnect::recvMessage(xIrcMessage *pMsg)
{
   xIrcMessage msgOut;
   
   if (isMsg(pMsg->rspCode, (char *)"PING"))
   {
      msgOut.rspCode = ircResponses.code("PONG");
      msgOut.dstStr = "";
      msgOut.msgStr = pMsg->srcAddr;
      if (pSocket != NULL)
         pSocket->sendIrcServerMessage(&msgOut);
   }
   else
      gotResponse(pMsg);
}


void xIrcConnect::showResponse(xIrcMessage *pMsg)
{
   int pos;
   char buf[1024];
   QStringList tmpStrList, tmpStrList1;
   QString tmpStr, tmpStr2;

#if 0
   printf("%d %s", pMsg->rspCode, (const char *)pMsg->rawMsg);
#endif

   switch(pMsg->rspCode) {
   case 25:
      sprintf(buf, "*** Mode Change \"%s\"", (const char*)pMsg->msgStr.latin1());
      break;
   case 27:
      tmpStrList = QStringList::split(":", pMsg->rawMsg);
      tmpStr2 = "*** " + pMsg->srcNick + " is now known as " + tmpStrList[1];
      sprintf(buf, "%s", (const char*)tmpStr2.latin1());
      break;
   case 29:
      sprintf(buf, "*** Left Channel %s", (const char*)pMsg->dstStr.latin1());
      break;
   case 33:
      tmpStr = pMsg->rawMsg;
      pos = tmpStr.find(":", 1);
      tmpStr.remove(0, ++pos);
      tmpStr2 = "*** Signoff " + pMsg->srcNick + ": " + tmpStr;
      sprintf(buf, "%s", (const char*)tmpStr2.latin1());
      break;
   case 311:
      tmpStrList = QStringList::split(":",  pMsg->rawMsg);
      tmpStrList1 = QStringList::split(" ", tmpStrList[0]);
      tmpStr2 = "*** " + tmpStrList1[3] + " is " + tmpStrList1[4] + "@";
      tmpStr2 += tmpStrList1[5] + "(" + tmpStrList[1] + ")";
      sprintf(buf, "%s", (const char *)tmpStr2.latin1());
      break;
   case 312:
      tmpStr = pMsg->rawMsg;
      pos = tmpStr.find(":", 1);
      tmpStr.remove(0, ++pos);
      tmpStrList = QStringList::split(":", pMsg->rawMsg);
      tmpStrList1 = QStringList::split(" ", tmpStrList[0]);
      tmpStr2 = "*** on irc server " + tmpStrList1[4] + "(" + tmpStr + ")";
      sprintf(buf, "%s", (const char *)tmpStr2.latin1());
      break;
   case 314:
      tmpStrList = QStringList::split(":",  pMsg->rawMsg);
      tmpStrList1 = QStringList::split(" ", tmpStrList[0]);
      tmpStr2 = "*** " + tmpStrList1[3] + " was " + tmpStrList1[4] + "@";
      tmpStr2 += tmpStrList1[5] + "(" + tmpStrList[1] + ")";
      sprintf(buf, "%s", (const char *)tmpStr2.latin1());
      break;
   case 318:
      tmpStrList = QStringList::split(":", pMsg->rawMsg);
      tmpStrList1 = QStringList::split(" ", tmpStrList[0]);
      tmpStr2 = "*** " + tmpStrList1[3] + " " + tmpStrList[1];
      sprintf(buf, "%s", (const char *)tmpStr2.latin1());
      break;
   case 319:
      tmpStr = pMsg->rawMsg;
      pos = tmpStr.find(":", 1);
      tmpStr.remove(0, ++pos);
      tmpStr2 = "*** on irc channel(s) : " + tmpStr;
      sprintf(buf, "%s", (const char *)tmpStr2.latin1());
      break;
   case 369:
      tmpStrList = QStringList::split(":", pMsg->rawMsg);
      tmpStrList1 = QStringList::split(" ", tmpStrList[0]);
      tmpStr2 = "*** " + tmpStrList1[3] + " " + tmpStrList[1];
      sprintf(buf, "%s", (const char *)tmpStr2.latin1());
      break;
   default:
      sprintf(buf, "*** %s", (const char *)pMsg->msgStr.latin1());
   }
   pMainWin->pWin->putString(buf);
}

void xIrcConnect::showError(xIrcMessage *pMsg)
{
   int rspCode = pMsg->rspCode;
//   if (dbg) fprintf(stdout, "xIrcConnect::showError():Enter:pMsg = %d\n", (int)pMsg);
//   if (dbg) fflush(stdout);
   if (rspCode == 431 || rspCode == 432 || rspCode == 433)
      gotErrorAck(rspCode);
   else
      QMessageBox::warning(this, "Error", (const char *)pMsg);

   if (dbg) fprintf(stdout, "xIrcConnect::showError():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcConnect::gotErrorAck(int rspCode)
{
   char buf[200];
   const char *cp;
   
   if (dbg) fprintf(stdout, "xIrcConnect::gotErrorAck():Enter: rspCode = %d\n", rspCode);
   if (dbg) fflush(stdout);
   if (rspCode == 431 || rspCode == 432 || rspCode == 433)
   {
      if (dbg) fprintf(stdout, "xIrcConnect::gotErrorAck():Have Nick Problem, are we auto??\n");
      if (dbg) fflush(stdout);
      if (NickQuery->autoSelectNick())
      {
         if (dbg) fprintf(stdout, "xIrcConnect::gotErrorAck():Get Next Nick\n");
         if (dbg) fflush(stdout);
         if ((cp = NickQuery->nextNick()) != NULL)
         {
            if (dbg) fprintf(stdout, "xIrcConnect::gotErrorAck():Got it!\n");
            if (dbg) fflush(stdout);
            sprintf(buf, "NICK %s\n", cp);
            sendMsgToSocket(buf);
            NickQuery->setText(cp);
            if (dbg) fprintf(stdout, "xIrcConnect::gotErrorAck():Exit\n");
            if (dbg) fflush(stdout);
            return;
         }
      }
      if (dbg) fprintf(stdout, "xIrcConnect::gotErrorAck():Querying for Nick\n");
      if (dbg) fflush(stdout);
      for (;;)
      {
         NickQuery->exec();
         if (strlen(NickQuery->text()) > 0)
         {
            nickName = NickQuery->text();
            break;
         }
         else
            QMessageBox::warning(this, "Error", "You must choose a NickName");
      }
      sprintf(buf, "NICK %s\n", NickQuery->text());
      sendMsgToSocket(buf);
   }
   sprintf(buf, "xIrc - %s", NickQuery->text());
   setCaption(buf);
   if (dbg) fprintf(stdout, "xIrcConnect::gotErrorAck():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcConnect::keyboardInput()
{
   if (dbg) fprintf(stdout, "xIrcConnect::keyboardInput():Got Enter!!!!\n");
   if (dbg) fflush(stdout);
//   pMainWin->pWin->putString(pEdit->text());
//   pSocket->sendToSocket(pText);
   if (dbg) fprintf(stdout, "xIrcConnect::keyboardInput():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcConnect::resizeEvent(QResizeEvent *pEvt)
{
//   pEvt++;
   QSize sizeFrame, sizeNewFrame, sizeTerm, s;

   if (dbg) fprintf(stdout, "xIrcConnect::resizeEvent():Enter - %d\n", resizeCnt);
   if (dbg) fflush(stdout);
   if (resizeCnt >= 0)
      resizeCnt++;
//   if (resizeCnt < 2 && resizeCnt >= 0)
   {
      if (dbg) fprintf(stdout, "xIrcConnect::resizeEvent():Resizing Frame - %d\n", resizeCnt);
      if (dbg) fflush(stdout);
      sizeFrame = pEvt->size();
      sizeTerm = sizeFrame;
      if (resizeCnt >= 0)
      {
         if (dbg) fprintf(stdout, "xIrcConnect::resizeEvent():Resizing Terminal Frame - %d\n", resizeCnt);
         if (dbg) fflush(stdout);
         sizeTerm.setHeight(sizeFrame.height() - pLabel->size().height());
/*
         sizeTerm.setHeight(sizeFrame.height() - pButtons->size().height());
*/
         pMainWin->resize(sizeTerm);
      }

      s = pLabel->size();
/*
      s = pButtons->size();
*/
      s.setWidth(sizeTerm.width());
      pLabel->resize(s);
/*
      pButtons->resize(&s);
*/

      sizeNewFrame = pMainWin->size();
      sizeNewFrame.setHeight(sizeNewFrame.height() + pLabel->size().height());
/*
      sizeNewFrame.setHeight(sizeNewFrame.height() + pButtons->size().height());
*/
      if (sizeNewFrame != sizeFrame)
      {
         if (dbg) fprintf(stdout, "xIrcConnect::resizeEvent():Fittinging Frame - %d\n", resizeCnt);
         if (dbg) fflush(stdout);
         resize(&sizeNewFrame);
         initFrame(&sizeNewFrame);
      }
      if (dbg) fprintf(stdout, "xIrcConnect::resizeEvent():Fittinging Frame - %d\n", resizeCnt);
      if (dbg) fflush(stdout);
      if (dbg) fprintf(stdout, "xIrcConnect::resizeEvent():Done -  %d\n", resizeCnt);
      if (dbg) fflush(stdout);
   }
//   else if (resizeCnt < 0)
   if (resizeCnt < 0)
      resizeCnt++;
   if (resizeCnt > 0)
      resizeCnt--;
      
   if (dbg) fprintf(stdout, "xIrcConnect::resizeEvent():Exit-  %d\n", resizeCnt);
   if (dbg) fflush(stdout);
}
