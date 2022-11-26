/***************************************************************************
**    xIrcMsgFrame1.cpp  $Revision: 1.2 $ - $Name:  $ 
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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <qt.h>
#include <qkeycode.h>
#include <xApp.h>
#include "xResources.h"
#include "xIrcMsgDispatch.h"
#include "xIrcMsgFrame.h"
#include "xIrcSocket.h"
#include "nicknameform.h"

static bool dbg = false;

extern xIrcCommands ircResponses;
extern nickNameDialog *NickQuery;
extern QPixmap *AppPixMap;
extern xIrcMsgDispatch Dispatcher;

static const char *pInitialResources[] =
{
   NULL
};

xIrcMessageFrame::xIrcMessageFrame(xWidgetResInfo *pPRes, QWidget *pParent, 
                                   const char *pName) :
               xDialog(wdtRes = new xWidgetResInfo(pPRes, QString("msgchat"),
                                                   QString("MsgChat")),
                       pParent, pName, 0)
{
   if (dbg) fprintf(stdout, "xIrcmessageFrame::xIrcMessageFrame():Enter\n");
   if (dbg) fflush(stdout);
   char *cp, mircColors[256], ctcp2[256], beep[256];
   const char *ccp;
   int chanLines, nickLines, cols, x;

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

   if ((ccp = Resources->get(wdtRes, "lines.channel", "Lines.Channel")) == NULL)
      chanLines = 22;
   else
      chanLines = atoi(ccp);
   if (chanLines < 3)
      chanLines = 22;

   if ((ccp = Resources->get(wdtRes, "lines.nick", "Lines.Nick")) == NULL)
      nickLines = 11;
   else
      nickLines = atoi(ccp);
   if (nickLines < 3)
      nickLines = 11;

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
   pNext = NULL;
   pPrev = NULL;
   if (pName)
      setCaption(pName);
   

   if (dbg) fprintf(stdout, "xIrcmessageFrame::xIrcMessageFrame():Creating Timer\n");
   if (dbg) fflush(stdout);
   pNickUpdateTimer = new QTimer(this);
   pNickPurgeTimer = new QTimer(this);
   
   if (dbg) fprintf(stdout, "xIrcmessageFrame::xIrcMessageFrame():Creating TermFrame\n");
   if (dbg) fflush(stdout);
   pMsgFrame = new xMultiLineFrame(wdtRes, this, NULL, cols, 
                                   (*pName == '#') ? chanLines : nickLines);
   connect(pMsgFrame, SIGNAL(textSelected(xMultiLineTextSelection)),
           this, SLOT(haveTextSelection(xMultiLineTextSelection)));
   if (dbg) fprintf(stdout, "xIrcmessageFrame::xIrcMessageFrame():Creating Button Bar\n");
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
   pButtons->addButton("Ping", btnPing);
   pButtons->addButton("Nick Actions", btnAction);
   if (*pName == '#')
      pButtons->addButton("Nick List", btnList);
   pButtons->addButton("Clear Line", btnClear);
   pButtons->addButton("Show Colors", btnColors);
   pFrame1->addWidget(pButtons);
   connect(pButtons, SIGNAL(clicked(int)), this, SLOT(buttonPressed(int)));
   
   pFrame11 = new xFrame(wdtRes, xALIGN_Horz, xSPACE_Resize, 0, pFrame1);
   pFrame11->setFrameStyle(QFrame::Panel | QFrame::Raised);
//   pFrame11->setFrameStyle(QFrame::Box | QFrame::Plain);
   pFrame11->setMargins(2, 4);
   pFrame11->setWidgetSpacing(5);

//   pFrame12 = new xFrame(wdtRes, xALIGN_Vert, xSPACE_Resize, 0, pFrame1);
//   pFrame12->setMargins(0, 0);
//   pFrame12->setWidgetSpacing(0);

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

   pBeepMsg = new QCheckBox("Beep Message", pFrame11);
   pBeepMsg->setFont(pButtons->font());
   pBeepMsg->adjustSize();
   if (strcmp(beep, "TRUE") == 0)
      pBeepMsg->setChecked(TRUE);

   pFrame11->addWidget(pMircColors);
   pFrame11->addWidget(pCtcp2);
   pFrame11->addWidget(pBeepMsg);
   pFrame11->fitFrame();
//   pFrame12->fitFrame();
   pFrame1->addWidget(pFrame11);
//   pFrame1->addWidget(pFrame12);
   pFrame1->fitFrame();

   if (*pName == '#')
   {
      if (dbg) fprintf(stdout, "xIrcmessageFrame::xIrcMessageFrame():Creating Channel nick box\n");
      if (dbg) fflush(stdout);
      pNicks = new xIrcChanNickBox();
      pNicks->show();
      connect(pNicks, SIGNAL(buttonPressed(int)), this, SLOT(nickButtonPressed(int)));
      connect(pNicks, SIGNAL(nickSelected(const char*)), this, SLOT(haveNickSelected(const char*)));
      
      if (dbg) fprintf(stdout, "xIrcmessageFrame::xIrcMessageFrame():Starting Timer\n");
      if (dbg) fflush(stdout);
      pNickUpdateTimer->start(NICK_UPDATE_TIME);
      pNickPurgeTimer->start(NICK_PURGE_TIME);
      
      if (dbg) fprintf(stdout, "xIrcmessageFrame::xIrcMessageFrame():Connecting Timer\n");
      if (dbg) fflush(stdout);
      connect(pNickUpdateTimer, SIGNAL(timeout()), this, SLOT(nickTime()));
      connect(pNickPurgeTimer, SIGNAL(timeout()), this, SLOT(nickPurgeTime()));
      
      if (dbg) fprintf(stdout, "xIrcmessageFrame::xIrcMessageFrame():Timer Connected\n");
      if (dbg) fflush(stdout);
   }
   else
      pNicks = NULL;
      
   if (dbg) fprintf(stdout, "xIrcmessageFrame::xIrcMessageFrame():Creating Edit Widget\n");
   if (dbg) fflush(stdout);
   pEdit = new xEdit(wdtRes, this);
   pEdit->setMaxLength(350);
   addWidget(pFrame1);
   addWidget(pMsgFrame);
   addWidget(pEdit);
   setMargins(0,0);
   setWidgetSpacing(0);
   initFrame();
   setFocusProxy(pEdit);
   setFocusPolicy(QWidget::StrongFocus);
   connect(pEdit, SIGNAL(returnPressed()), this, SLOT(gotKeyboardInput()));
   if (dbg) fprintf(stdout, "xIrcmessageFrame::xIrcMessageFrame():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcMessageFrame::link(xIrcMessageFrame *pMsg)
{
   if (dbg) fprintf(stdout, "xIrcmessageFrame::~xIrcMessageFrame():Enter\n");
   if (dbg) fflush(stdout);
   pMsg->pNext = pNext;
   if (pNext)
      pNext->pPrev = pMsg;
   pNext = pMsg;
   pMsg->pPrev = this;
   if (dbg) fprintf(stdout, "xIrcmessageFrame::~xIrcMessageFrame():Exit\n");
   if (dbg) fflush(stdout);
}

xIrcMessageFrame::~xIrcMessageFrame()
{
   if (pMsgFrame)
      delete pMsgFrame;
   if (pEdit)
      delete pEdit;
   if (pButtons)
      delete pButtons;
   if (pPrev)
      pPrev->pNext = pNext;
   if (pNext)
      pNext->pPrev = pPrev;
}

void xIrcMessageFrame::buttonPressed(int btn) 
{
   xMultiLineTextSelection msg;

   switch(btn)
   {
      case btnClose:
         closeFrame();
         break;

      case btnPing:
         doPing();
         break;

      case btnAction:
         msg.winName = name();
         msg.text = "";
         emit textSelected(msg);
         break;

      case btnList:
         if (pNicks && pNicks->isVisible())
            pNicks->hide();
         else
            pNicks->show();

      case btnClear:
         clearLine();
         break;

      case btnColors:
         {
            QString s1 = "0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B | C | D | E | F";
            QString s2, s3, s4;
            s2 += "\\C000 \\CX|";
            s2 += "\\C11 1 \\CX|";
            s2 += "\\C22 2 \\CX|";
            s2 += "\\C33 3 \\CX|";
            s2 += "\\C44 4 \\CX|";
            s2 += "\\C55 5 \\CX|";
            s2 += "\\C66 6 \\CX|";
            s2 += "\\C77 7 \\CX|";
            s2 += "\\C88 8 \\CX|";
            s2 += "\\C99 8 \\CX|";
            s2 += "\\Caa A \\CX|";
            s2 += "\\Cbb B \\CX|";
            s2 += "\\Ccc C \\CX|";
            s2 += "\\Cdd D \\CX|";
            s2 += "\\Cee E \\CX|";
            s2 += "\\Cff F \\CX";
            s3 = parseLine(s1.latin1(), pMircColors->isChecked(), pCtcp2->isChecked());
            putString(s3.latin1());
            s3 = parseLine(s2.latin1(), pMircColors->isChecked(), pCtcp2->isChecked());
            putString(s3.latin1());
         }
         break;
   }
}

void xIrcMessageFrame::newName(const char *pName) 
{
   if (*pName == '#')
   {
      if (pNicks)
         delete pNicks;
      pNicks = new xIrcChanNickBox();
      pNicks->show();
      pNickUpdateTimer->start(NICK_UPDATE_TIME);
      pNickPurgeTimer->start(NICK_PURGE_TIME);
      if (dbg) fprintf(stdout, "xIrcmessageFrame::xIrcMessageFrame():Connecting Timer\n");
      if (dbg) fflush(stdout);
      connect(pNickUpdateTimer, SIGNAL(timeout()), this, SLOT(nickTime()));
      connect(pNickPurgeTimer, SIGNAL(timeout()), this, SLOT(nickPurgeTime()));
      connect(pNicks, SIGNAL(buttonPressed(int)), this, SLOT(nickButtonPressed(int)));
      if (dbg) fprintf(stdout, "xIrcmessageFrame::xIrcMessageFrame():Timer Connected\n");
      if (dbg) fflush(stdout);
   }
   else
   {
      if (pNicks)
         delete pNicks;
      pNicks = NULL;
   }
   setName(pName);
   setCaption(pName);
}

void xIrcMessageFrame::closeFrame() 
{
   char *cp;
   xIrcMessage msg;

   cp = (char *)name();
   if (*cp == '#')
   {
      msg.rspCode = ircResponses.code("PART");
      msg.dstStr = cp;
      msg.msgStr = "";
      Dispatcher.dispatchMsg(this, SLOT(ircRespMessageIn(xIrcMessage*)), &msg);
//      emit ircMessageOut(&msg);
      if (dbg) fprintf(stdout, "xIrcmessageFrame::xIrcMessageFrame():Disconnecting Timer\n");
      if (dbg) fflush(stdout);
      disconnect(pNickUpdateTimer, SIGNAL(timeout()), this, SLOT(nickTime()));
      disconnect(pNickPurgeTimer, SIGNAL(timeout()), this, SLOT(nickPurgeTime()));
      if (dbg) fprintf(stdout, "xIrcmessageFrame::xIrcMessageFrame():Timer Disconnected\n");
      if (dbg) fflush(stdout);
      pNickUpdateTimer->stop();
      pNickPurgeTimer->stop();
   }
   
   if (pNicks)
   {
      pNicks->hide();
      delete pNicks;
      pNicks = NULL;
   }
   emit ircMsgFrameClosing(this);
   delete this;
};

bool xIrcMessageFrame::is(const char *pName)
{
   QString tmpNick, tmpName;
   const char *cp;
   bool rv;
   
   if (dbg) fprintf(stdout, "xIrcMessageFrame::is():Enter\n");
   for (cp = pName, tmpNick = ""; *cp; cp++)
      tmpNick += toupper(*cp);
   for (cp = (const char *)name(), tmpName = ""; *cp; cp++)
      tmpName += toupper(*cp);
   if (dbg) fprintf(stdout, "xIrcMessageFrame::is():Testing Name:|%s| to Nick:|%s|\n",
                            (const char *)tmpName.latin1(), (const char *)tmpNick.latin1());
   rv = (tmpName == tmpNick) ? TRUE : FALSE;
   if (dbg) fprintf(stdout, "xIrcMessageFrame::is():Exit- Names %s\n", rv == TRUE ?
                            "Matched!!" : "Didnot Match");
   return(rv);
}

void xIrcMessageFrame::doPing() 
{
   char buf[20], buf1[80];
   xIrcMessage msg;

   sprintf(buf, "%ld", time(NULL));
   msg.rspCode = ircResponses.code("PRIVMSG");
   msg.dstStr = name();
   msg.msgStr = "\x01";
   msg.msgStr += "PING ";
   msg.msgStr += buf;
   msg.msgStr += "\x01";
   sprintf(buf1, "*** Ping command sent to %s\n", (const char *)msg.dstStr.latin1());
   putString(buf1);
   Dispatcher.dispatchMsg(this, SLOT(ircRespMessageIn(xIrcMessage*)), &msg);
//   emit ircMessageOut(&msg);
}

void xIrcMessageFrame::clearLine() 
{
   pEdit->setText("");
}

void xIrcMessageFrame::nickTime()
{
   xIrcMessage msg;

   if (pNicks && pNicks->isVisible())
   {
      if (dbg) fprintf(stdout, "xIrcmessageFrame::nickTime():Enter\n");
      if (dbg) fflush(stdout);
      if (dbg) fprintf(stdout, "xIrcmessageFrame::nickTime():Requesting Name List\n");
      if (dbg) fflush(stdout);
      msg.rspCode = ircResponses.code("NAMES");
      msg.dstStr = name();
      msg.msgStr = "";
      Dispatcher.dispatchMsg(this, SLOT(ircRespMessageIn(xIrcMessage*)), &msg);
//      emit ircMessageOut(&msg);
      if (dbg) fprintf(stdout, "xIrcmessageFrame::nickTime():Exit\n");
      if (dbg) fflush(stdout);
   }
}

void xIrcMessageFrame::nickPurgeTime()
{
   xIrcMessage msg;

   if (pNicks && pNicks->isVisible())
   {
      pNicks->clear();
      if (dbg) fprintf(stdout, "xIrcmessageFrame::nickTime():Enter\n");
      if (dbg) fflush(stdout);
      msg.rspCode = ircResponses.code("NAMES");
      msg.dstStr = name();
      msg.msgStr = "";
      Dispatcher.dispatchMsg(this, SLOT(ircRespMessageIn(xIrcMessage*)), &msg);
//      emit ircMessageOut(&msg);
      if (dbg) fprintf(stdout, "xIrcmessageFrame::nickTime():Exit\n");
      if (dbg) fflush(stdout);
   }
}

void xIrcMessageFrame::gotKeyboardInput()
{
   xIrcMessage msg;
   char buf[512];
   const char *cp;
   QString msgStr;

   if (dbg) fprintf(stdout, "xIrcMessageFrame::gotKeyboardInput():Got input!\n");
   if (dbg) fflush(stdout);
   if (!isVisible())
   {
      show();
      if (pNicks->isVisible())
         pNicks->show();
   }

   strcpy(buf, pEdit->text());
   msgStr = parseLine(buf, pMircColors->isChecked(), pCtcp2->isChecked());
   if ((cp = msgStr.latin1()) == NULL)
   {
      if (dbg) fprintf(stdout, "xIrcMessageFrame::gotKeyboardInput():Blank line!!\n");
      if (dbg) fflush(stdout);
   }
   else if (*cp == '/')
      procCommand(++cp);
   else
   {
      if (!msgStr.isEmpty())
      {
         msg.rspCode = ircResponses.code("PRIVMSG");
         msg.dstStr = name();
         msg.msgStr = msgStr;
         if (dbg) fprintf(stdout, "xIrcMessageFrame::gotKeyboardInput():Sending input\n");
         if (dbg) fflush(stdout);
         Dispatcher.dispatchMsg(this, SLOT(ircRespMessageIn(xIrcMessage*)), &msg);
//         emit ircMessageOut(&msg);
         if (dbg) fprintf(stdout, "xIrcMessageFrame::gotKeyboardInput():Putting it on the screen!\n");
         if (dbg) fflush(stdout);
         sprintf(buf, "-> %s\n", (const char *)msgStr.latin1());
         putString(buf);
      }
   }
   if (dbg) fprintf(stdout, "xIrcMessageFrame::gotKeyboardInput():Clearing the edit field!\n");
   if (dbg) fflush(stdout);
   pEdit->setText("");
   if (dbg) fprintf(stdout, "xIrcMessageFrame::gotKeyboardInput():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcMessageFrame::putString(const char *pStr)
{
   QString s;
   s = translateMessage(pStr);
   pMsgFrame->pWin->putString(s.latin1());
}

bool xIrcMessageFrame::procServerMsg(xIrcMessage *pMsg)
{
   QString tmpDst, tmpSrc, tmpNick, tmpName;
   const char *cp;
   bool rv = FALSE, b = FALSE;
   
   if (dbg) fprintf(stdout, "xIrcMessageFrame::ProcServerMsg(%s):Enter\n", name());
   if (dbg) fflush(stdout);
   
#if 0
   for (cp = NickQuery->text(), tmpNick = ""; *cp; cp++)
      tmpNick += toupper(*cp);
   for (cp = pMsg->srcNick.latin1(), tmpSrc = ""; *cp; cp++)
      tmpSrc += toupper(*cp);
   for (cp = pMsg->dstStr.latin1(), tmpDst = ""; *cp; cp++)
      tmpDst += toupper(*cp);
   for (cp = (char *)name(), tmpName = ""; *cp; cp++)
      tmpName += toupper(*cp);
#else
   tmpNick = NickQuery->text();
   tmpNick.upper();
   tmpSrc = pMsg->srcNick;
   tmpSrc.upper();
   tmpDst = pMsg->dstStr;
   tmpDst.upper();
   tmpName = name();
   tmpName.upper();
#endif
   
   if (dbg) fprintf(stdout, "xIrcMessageFrame::ProcServerMsg():Comparing |%s| & |%s| to |%s|\n", 
                             (const char *)tmpSrc.latin1(), (const char *)tmpDst.latin1(), 
                             (const char *)tmpName.latin1());
   if (dbg) fflush(stdout);
      
   if (
         (tmpNick == tmpDst && tmpSrc == tmpName) || 
         tmpDst == tmpName || 
         (
            pNicks && ((b = pNicks->is(tmpSrc.latin1())) && tmpDst == tmpName)
         ) || (
            pNicks && (b = pNicks->is(tmpDst.latin1())) && 
            (
               (pMsg->rspCode >= 311 && pMsg->rspCode <= 319) ||
               pMsg->rspCode == 301
            )
         ) || (
            pNicks && (b = pNicks->is(tmpSrc.latin1())) && 
            (
               pMsg->rspCode == 27 ||
               pMsg->rspCode == 33
            )
         )
      )
   {
      if (dbg) fprintf(stdout, "xIrcMessageFrame::ProcServerMsg():Found Match!!!!! b = %d\n", b);
      if (dbg) fflush(stdout);
      rv = TRUE;

      if (pMsg->rspCode == 353)
      {
         if (dbg) fprintf(stdout, "xIrcMessageFrame::ProcServerMsg():Adding new nicks to Box\n");
         if (dbg) fflush(stdout);
         if (pNicks && pNicks->isVisible())
            pNicks->setNicks(pMsg->msgStr.latin1());
         else
            ircRespMessageIn(pMsg);
      }
      else if ((pMsg->rspCode >= 300 && pMsg->rspCode <= 369) ||
         (!isMsg(pMsg->rspCode, "PRIVMSG") && !isMsg(pMsg->rspCode, "NOTICE")))
      {
         if (dbg) fprintf(stdout, "xIrcMessageFrame::ProcServerMsg():Sending Response message to Message frame\n");
         if (dbg) fflush(stdout);
         ircRespMessageIn(pMsg);
         if (dbg) fprintf(stdout, "xIrcMessageFrame::ProcServerMsg():Done\n");
         if (dbg) fflush(stdout);
      }
      else if (isMsg(pMsg->rspCode, "PRIVMSG") ||
               isMsg(pMsg->rspCode, "NOTICE"))
      {
         if (pMsg->pmsgTyp == ipmDCC ||
             pMsg->pmsgTyp == ipmDCCChat ||
             pMsg->pmsgTyp == ipmDCCSend ||
             (isMsg(pMsg->rspCode, "PRIVMSG") &&
              (pMsg->pmsgTyp == ipmVersion ||
               pMsg->pmsgTyp == ipmUserInfo ||
               pMsg->pmsgTyp == ipmFinger ||
               pMsg->pmsgTyp == ipmClientInfo ||
               pMsg->pmsgTyp == ipmErrMsg ||
               pMsg->pmsgTyp == ipmSource)))
            rv = FALSE;
         else
            ircPrivMessageIn(pMsg);
      }
   }
   
   if (dbg) fprintf(stdout, "xIrcMessageFrame::ProcServerMsg():Exit\n");
   if (dbg) fflush(stdout);
   return(rv);
}

void xIrcMessageFrame::haveNickSelected(const char *pNick)
{
   xMultiLineTextSelection msg;
   msg.text = pNick;
   msg.winName = name();
   emit textSelected(msg);
}

void xIrcMessageFrame::gotNickAction(int action)
{
   action++;
}

void xIrcMessageFrame::haveTextSelection(xMultiLineTextSelection msg)
{
   if (dbg) fprintf(stdout, "xIrcMessageFrame::havetextSelection():Enter\n");
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xIrcMessageFrame::havetextSelection():winName = |%s|, text = |%s|\n",
                             (const char *)msg.winName.latin1(), (const char *)msg.text.latin1());
   if (dbg) fflush(stdout);
   emit textSelected(msg);
}

void xIrcMessageFrame::resizeEvent(QResizeEvent *pEvt)
{
//   pEvt++;
   QSize sizeFrame, sizeNewFrame, sizeTerm;
   QSize s;

   if (dbg) fprintf(stdout, "xIrcMessageFrame::resizeEvent():Enter - %d\n", resizeCnt);
   if (dbg) fflush(stdout);
   if (resizeCnt >= 0)
      resizeCnt++;
//   if (resizeCnt < 2 && resizeCnt >= 0)
   {
      if (dbg) fprintf(stdout, "xIrcMessageFrame::resizeEvent():Resizing Frame - %d\n", resizeCnt);
      if (dbg) fflush(stdout);
      sizeFrame = pEvt->size();
      sizeTerm = sizeFrame;
      if (resizeCnt >= 0)
      {
         if (dbg) fprintf(stdout, "xIrcMessageFrame::resizeEvent():Resizing Terminal Frame - %d\n", resizeCnt);
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
         if (dbg) fprintf(stdout, "xIrcMessageFrame::resizeEvent():Fittinging Frame - %d\n", resizeCnt);
         if (dbg) fflush(stdout);
         resize(&sizeNewFrame);
         initFrame(&sizeNewFrame);
      }
      if (dbg) fprintf(stdout, "xIrcMessageFrame::resizeEvent():Fittinging Frame - %d\n", resizeCnt);
      if (dbg) fflush(stdout);
      if (dbg) fprintf(stdout, "xIrcMessageFrame::resizeEvent():Done -  %d\n", resizeCnt);
      if (dbg) fflush(stdout);
   }
//   else if (resizeCnt < 0)
   if (resizeCnt < 0)
      resizeCnt++;
   if (resizeCnt > 0)
      resizeCnt--;
      
   if (dbg) fprintf(stdout, "xIrcMessageFrame::resizeEvent():Exit-  %d\n", resizeCnt);
   if (dbg) fflush(stdout);
}

void xIrcMessageFrame::mouseDoubleClickEvent(QMouseEvent *pEvt)
{
   if (dbg) fprintf(stdout, "xIrcMessageFrame::doubleClickEvent():Enter\n");
   if (dbg) fflush(stdout);
   pMsgFrame->mouseDoubleClickEvent(pEvt);
   if (dbg) fprintf(stdout, "xIrcMessageFrame::doubleClickEvent():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcMessageFrame::changeColors(bool f)
{
   setMircColors(pMsgFrame->pWin, f);
}

//#include "xIrcMsgFrame.moc"
