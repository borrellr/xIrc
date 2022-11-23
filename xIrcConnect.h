/***************************************************************************
**    xIrcConnect.h  $Revision: 1.8 $ - $Name:  $ 
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
#ifndef _xIRCCONNECT_H
#define _xIRCCONNECT_H

#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qstring.h>
#include <qfiledialog.h>
#include <xMultiLineFrame.h>
#include <xPshBtnFrame.h>
#include <xSocketDialog.h>
#include "xIrcSocket.h"
#include "xIrcMsgFrame.h"
#include "xIrcIgnoreQuery.h"
#include "xIrcNotifyQuery.h"
#include "xIrcNotifyTable.h"
#include "xIrcCommands.h"
#include "xIrcMsgQuery.h"
#include "xIrcDccChatFrame.h"
#include "xIrcDccFile.h"
#include "xIrcNickActionQuery.h"
#include "xIrcBanDialog.h"

class xIrcConnect : public xDialog
{
   Q_OBJECT

public:   
   xIrcConnect(xWidgetResInfo *pPRes, QWidget *parent=0, const char *name=0, 
               WFlags iFlags = WStyle_MinMax,
               int width = 80, int height = 25, int maxLines = 300);
   ~xIrcConnect();

   QString buildBanMask(xIrcMessage *pMsg);
   void putWindow(const char *pStr)
   {
      pMainWin->pWin->putString(pStr);
   };

signals:
   void textOut(const char *);
   void msgOut(xIrcMessage *pMsg);
   void haveNotification(xIrcMessage *pMsg);

public slots:
   void newServer();
   void sendMessage(xIrcMessage *pMsg);

protected slots:
   void stopConnection();
   void keyboardInput();
   void newChannel();
   void newNick();
   void quitIrc();
   void sendMsgToSocket(const char *pBuf);
   void ircMsgFrameClosing(xIrcMessageFrame *pMF);
   void msgQryHandler(xIrcMsgQuery *pMsgQuery);
   void dccChatHandler(xIrcMessage *pMsg);
   void gotErrorAck(int rspCode);
   void socketClosed();
   void gotChanBoxResult(int result);
   void acceptInvite(QString strChannel);
   void buttonPressed(int btn);
   void nickActionHandler(xMultiLineTextSelection txtSel);
      
private slots:
   void recvMessage(xIrcMessage *pMsg);
   void doNotifyResp(xIrcMessage *pMsg);
   void doIgnoreResp(xIrcMessage *pMsg);
   void doBanResp(xIrcMessage *pMsg);
   void goodConnection(int sock);
   void failedConnection(int err);
   void connStatus(int err);
   void initiateDCCChat(xIrcDccChatFrame*);
   void initiateDCCFile(xIrcDccFile *fFrame);
   void showNickAction();
   void showIgnore();
   void showNotify();
   void showNotifyList();
   void about();
   
protected:
   void              gotPing(xIrcMessage *pMsg);
   void              gotPong(xIrcMessage *pMsg);
   void              gotResponse(xIrcMessage *pMsg);
   bool              doSpecialMessage(xIrcMessage *pMsg);
   void              showResponse(xIrcMessage *pMsg);
   void              showError(xIrcMessage *pMsg);
   xIrcMessageFrame  *findMsgFrame(const char *pName);
   xIrcMessageFrame  *makeNewMsgFrame(QWidget *pParent, const char *pName);
   virtual void      resizeEvent(QResizeEvent *pEvt);
//   xIrcMessageFrame *queryNewMessage(xIrcMsgQuery *pMsgQry,
//                                      xIrcMessage *pMsg);
   
private:
   enum btnVals
   {
      btnServer, btnChannel, btnAction, btnNick, btnQuit
   };

   QPopupMenu           *pListMenu;
   QPopupMenu           *pFileMenu;
   QPopupMenu           *pNickMenu;
   QPopupMenu           *pHelpMenu;
   QMenuBar             *pMenu;

   QLabel               *pLabel;
   xIrcIgnoreQuery      *pIgnore;
   xIrcNotifyQuery      *pNotify;
   xIrcNotifyTable      *pNotifyList;
   xIrcNickActionQuery  *pNickAction;
   xPshBtnFrame         *pButtons;
   xIrcMessageFrame     *pIrcMsgFrames;
   xMultiLineFrame      *pMainWin;
   xIrcSocket           *pSocket;
   xIrcMsgQueryList     msgQueryList;
   xIrcCommands         cmds;
   xSocketDialog        *pSocketBox;
   xIrcBanDialog        *pBanBox;
   QFileDialog          *pSaveDialog;
   QFileDialog          *pOpenDialog;
   QString              nickName;
   QString              realName;
   QString              hostName;
   QString              userName;
   QString              banChan;
   QString              banNick;
   QString              ignoreNick;
   QString              notifyNick;
   bool                 banFlag;
   bool                 ignoreFlag;
   bool                 notifyFlag;
   bool                 notifyQueryFlag;
   bool                 quitFlag;
   bool                 serverFlag;
   int                  resizeCnt;   
   xWidgetResInfo       *wdtRes;
   xWidgetResInfo       *wdtPrv;
   bool  isMsg(int cmd, char *pStr);
   void InitializeMenu();
   void joinChannel(const char *);
   void initializeActions();
   QAction *showNotifyListAct;
   QAction *showNickActionAct;
   QAction *newNickAct;
   QAction *newServerAct;
   QAction *showIgnoreAct;
   QAction *showNotifyAct;
   QAction *connectServerAct;
   QAction *newChannelAct;
   QAction *quitIrcAct;
   QAction *aboutAct;
   QAction *aboutQtAct;
};

#endif
