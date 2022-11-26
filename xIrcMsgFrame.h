/***************************************************************************
**    xIrcMsgFrame.h  $Revision: 1.2 $ - $Name:  $ 
**    Class to handle chatting w/ a person or channel
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
#ifndef _xIRCMSGFRAME_H
#define _xIRCMSGFRAME_H

#include <qcheckbox.h>
#include <qlineedit.h>
#include <qtimer.h>
#include <qmenubar.h>
#include <xMultiLineFrame.h>
#include <xPshBtnFrame.h>
#include <xEdit.h>
#include <xDialog.h>
#include <xResources.h>
#include "xIrcSocket.h"
#include "xIrcCommands.h"
#include "channickbox.h"
#include "xIrcNickListEntry.h"

#define xCN_Whois   1
#define xCN_Query   2
#define xCN_Invite  3
#define xCN_Kick    4
#define xCN_Clear   5
#define xCN_Ping    6
#define xCN_Close   7

class xIrcLineEdit : public QLineEdit
{
   Q_OBJECT

public:
   xIrcLineEdit(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
                const char *pName = NULL) :
      QLineEdit(pParent, pName)
      {
         wdtRes = new xWidgetResInfo(pPRes, QString(""), QString(""));
         setDefPallet(this, wdtRes);
         setDefFont(this, wdtRes);
      };

private:
   xWidgetResInfo *wdtRes;
};

class xIrcMessageFrame : public xDialog
{
   Q_OBJECT
   
public:   
   xIrcMessageFrame(xWidgetResInfo *pPRes, QWidget *pParent = NULL, const char *pName = NULL);
   ~xIrcMessageFrame();
   void initClass(QWidget *pParent, const char *pName);
//   reset() { pMsgFrame->reset(); };
   
   void link(xIrcMessageFrame *pMsgFrame);
   xIrcMessageFrame *next() { return(pNext); };
   bool isPending(int rspCode);
   bool procServerMsg(xIrcMessage *pMsg);
   bool is(const char *pName);
   virtual void mouseDoubleClickEvent(QMouseEvent *pEvt);
   
signals:
   void ircMessageOut(xIrcMessage *pMsg);
   void ircMsgFrameClosing(xIrcMessageFrame *pMF);
   void textSelected(xMultiLineTextSelection txt);
   void chatRequest();
   void nickSelected(const char *pNick);
   
public slots:
   void ircPrivMessageIn(xIrcMessage *pMsg);
   void ircRespMessageIn(xIrcMessage *pMsg);
   void hideFrame() { hide(); };
   void closeFrame();
   void doPing();
   void clearLine();
   void newName(const char *pName);
   void putString(const char *pStr);
   void procMessage(xIrcMessage *pMsg)
   {
      procServerMsg(pMsg);
   };
   void haveNickSelected(const char *pNick);
   void changeColors(bool f);
      
   
protected slots:
   virtual void gotKeyboardInput();
   void gotNickAction(int action);
   void nickTime();
   void nickPurgeTime();
   void nickButtonPressed(int iBtn);
   void buttonPressed(int);
   void haveTextSelection(xMultiLineTextSelection);
   
protected:
   enum btnCodes
   {
      btnClose, btnPing, btnClear, btnAction, btnList, btnColors
   };
   virtual void resizeEvent(QResizeEvent *pEvt);
   void procCommand(const char *pStr);
   bool isMsg(int cmd, char *pStr);

private:
   QMenuBar          *pMenu;
   xPshBtnFrame      *pButtons;
   xMultiLineFrame   *pMsgFrame;
   xEdit             *pEdit;
   xIrcChanNickBox   *pNicks;
   xIrcMessageFrame  *pNext;
   xIrcMessageFrame  *pPrev;
   xIrcCommands      cmds;
   QTimer            *pNickUpdateTimer;
   QTimer            *pNickPurgeTimer;
   QCheckBox         *pMircColors;
   QCheckBox         *pCtcp2;
   QCheckBox         *pBeepMsg;
   xFrame            *pFrame1;
   xFrame            *pFrame11;
   xFrame            *pFrame12;
   xWidgetResInfo    *wdtRes;
   
   void modeRespIn(xIrcMessage *pMsg);
   void doSpecial(xIrcMessage *pMsg);
   
   int resizeCnt;
   int   oldAttr;
   int   mircColors;
};

#define NICK_UPDATE_TIME   10000
#define NICK_PURGE_TIME    55000

#endif
