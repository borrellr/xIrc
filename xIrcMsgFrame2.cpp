/***************************************************************************
**    xIrcMsgFrame2.cpp  $Revision: 1.2 $ - $Name:  $ 
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
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <qkeycode.h>
#include <qmenubar.h>
#include <xApp.h>
#include "xDefaults.h"
#include "xIrcNickQuery.h"
#include "xIrcLineEditQuery.h"
#include "xIrcMsgDispatch.h"
#include "xIrcMsgFrame.h"
#include "xIrcChannelQuery.h"

static int dbg = 0;

extern xApplication *pApp;
extern xDefaults Defaults;
extern xIrcCommands ircResponses;
extern xChannelQuery *ChanQuery;
extern xIrcNickQuery *NickQuery;
extern xIrcLineEditQuery *KickQuery;
extern xIrcMsgDispatch Dispatcher;

bool xIrcMessageFrame::isMsg(int cmd, char *pStr)
{ 
   return(ircResponses.is(cmd, pStr));
}

void xIrcMessageFrame::modeRespIn(xIrcMessage *pMsg)
{
   const char *str;
   QString strTmp = "";
   
   str = pMsg->msgStr;
   if (dbg) fprintf(stdout, "xIrcMessageFrame::modeRespIn():Mode = %d\n", str[1]);
   if (dbg) fflush(stdout);
   switch (str[1])
   {
      case 'o':
         if (str[0] == '+')
            strTmp.sprintf("[B]Operator status given to %s by %s\n",
                           &str[3], (const char *)pMsg->srcNick);
         else
            strTmp.sprintf("[B]Operator status taken from %s by %s\n",
                           &str[3], (const char *)pMsg->srcNick);
         break;
         
      case 'p':
      /*
         if (str[0] == '+')
            privateChannel();
         else
            publicChannel();
      */
      
      default:
         strTmp.sprintf("[B]%s changed Mode to %s\n",
                        (const char *)pMsg->srcNick, str);
   }
   pMsgFrame->pWin->putString(strTmp);
}

void xIrcMessageFrame::ircRespMessageIn(xIrcMessage *pMsg)
{
   char str[1024], tmpStr[20], *cp;
   const char *pStr, *cp1;
   xIrcMessage msg;
   
   if (dbg) fprintf(stdout, "xIrcMessageFrame::ircRespMessageIn():Got Response in!\n");
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xIrcMessageFrame::ircRespMessageIn():Response code = %d\n", pMsg->rspCode);
   if (dbg) fflush(stdout);
   
   if (pMsg->rspCode == 366)
      return;
      
   if (pMsg->rspCode == 353)
   {
      QString tmpDst, tmpName;
      const char *cp;

      if (dbg) fprintf(stdout, "xIrcMessageFrame::ircRespMessageIn():Testing Names response\n");
      if (dbg) fflush(stdout);
      for (cp = pMsg->dstStr, tmpDst = ""; *cp; cp++)
         tmpDst += toupper(*cp);
      for (cp = (char *)name(), tmpName = ""; *cp; cp++)
         tmpName += toupper(*cp);
      if (strcmp(tmpName, tmpDst) == 0 && pNicks && pNicks->isVisible())
      {
         pNicks->setNicks(pMsg->msgStr);
         return;
      }
   }

   pMsg->msgStr.stripWhiteSpace();
   if (dbg) fprintf(stdout, "xIrcMessageFrame::ircRespMessageIn():Done simplifying WS\n");
   if (dbg) fflush(stdout);
   
   if (isMsg(pMsg->rspCode, "MODE"))
   {
      if (dbg) fprintf(stdout, "xIrcMessageFrame::ircRespMessageIn():MODE Response!!!\n");
      if (dbg) fflush(stdout);
      modeRespIn(pMsg);
   }
   else if (isMsg(pMsg->rspCode, "JOIN"))
   {
      sprintf(str, "[B]*** %s (%s) has joined the channel\n",
                    (const char *)pMsg->srcNick, (const char *)pMsg->srcAddr);
      pMsgFrame->pWin->putString(str);
   }
   else if (isMsg(pMsg->rspCode, "PART"))
   {
      sprintf(str, "[B]*** %s has left the channel\n", (const char *)pMsg->srcNick);
      pMsgFrame->pWin->putString(str);
   }
   else if (isMsg(pMsg->rspCode, "NOTICE"))
   {
      sprintf(str, "[B]NOTICE[/B]:%s- %s\n", 
              (const char *)pMsg->srcNick, (const char *)pMsg->msgStr);
      pMsgFrame->pWin->putString(str);
   }
   else if (isMsg(pMsg->rspCode, "NICK"))
   {
      sprintf(str, "[B]*** %s is now known as %s\n",
              (const char *)pMsg->srcNick, (const char *)pMsg->dstStr);
      pMsgFrame->pWin->putString(str);
   }
   else if (isMsg(pMsg->rspCode, "KICK"))
   {
      pStr = pMsg->msgStr;
      while (isspace(*pStr)) pStr++;
      for (cp = tmpStr, cp1 = pStr; *cp1 && *cp1 != ':' && !isspace(*cp1); cp1++)
      {
         *(cp++) = toupper(*cp1);
         *cp = '\0';
      }
      while (*cp1 && *cp1 == ':' && isspace(*cp1)) cp1++;
      strcpy(str, NickQuery->text());
      for (cp = str; *cp; cp++)
         *cp = toupper(*cp);
      for (cp = tmpStr; *cp; cp++)
         *cp = toupper(*cp);
      if (strcmp(tmpStr, str) == 0)
      {
         sprintf(str, "[B]*** You have been kicked by %s!!! (%s)\n",
                 (const char *)pMsg->srcNick, cp1);
         pMsgFrame->pWin->putString(str);
         msg.rspCode = ircResponses.code("NICK");
         msg.dstStr = "";
         msg.msgStr = NickQuery->text();
         msg.msgStr += "_";
         Dispatcher.dispatchMsg(this, SLOT(ircRespMessageIn(xIrcMessage*)), &msg);
//         emit ircMessageOut(&msg);
         msg.rspCode = ircResponses.code("JOIN");
         msg.dstStr = name();
         msg.msgStr = "";
         Dispatcher.dispatchMsg(this, SLOT(ircRespMessageIn(xIrcMessage*)), &msg);
//         emit ircMessageOut(&msg);
         if ((cp = (char *)Defaults.get("KICKED_YOU_RESPONSE")) != NULL)
         {
            msg.rspCode = ircResponses.code("PRIVMSG");
            msg.dstStr = name();
            msg.msgStr = pMsg->srcNick;
            msg.msgStr = ", ";
            msg.msgStr += cp;
            Dispatcher.dispatchMsg(this, SLOT(ircRespMessageIn(xIrcMessage*)), &msg);
//            emit ircMessageOut(&msg);
         }
         msg.rspCode = ircResponses.code("NICK");
         msg.dstStr = "";
         msg.msgStr = NickQuery->text();
         Dispatcher.dispatchMsg(this, SLOT(ircRespMessageIn(xIrcMessage*)), &msg);
//         emit ircMessageOut(&msg);
      }
      else
      {
         sprintf(str, "[B]*** %s has been kicked by %s!!! (%s)\n",
                 tmpStr, (const char *)pMsg->srcNick, cp1);
         pMsgFrame->pWin->putString(str);
         for (cp = str, cp1 = pMsg->msgStr; *cp1 && !isspace(*cp1); cp1++, cp)
         {
            *(cp++) = *cp1;
            *cp = '\0';
         }
         if ((cp = (char *)Defaults.get("KICKED_OTHER_RESPONSE")) != NULL)
         {
            msg.rspCode = ircResponses.code("PRIVMSG");
            msg.dstStr = str;
            msg.msgStr = cp;
            Dispatcher.dispatchMsg(this, SLOT(ircRespMessageIn(xIrcMessage*)), &msg);
//            emit ircMessageOut(&msg);
         }
      }
   }
   else if (isMsg(pMsg->rspCode, "QUIT"))
   {
      sprintf(str, "[B]*** %s <%s> has quit!! (%s)\n",
                    (const char *)pMsg->srcNick, (const char *)pMsg->srcAddr, (const char *)pMsg->msgStr);
      pMsgFrame->pWin->putString(str);
   }
   else if (isMsg(pMsg->rspCode, "TOPIC"))
   {
      sprintf(str, "[B]*** %s has changed the topic to: %s\n",
                    (const char *)pMsg->srcNick, (const char *)pMsg->msgStr);
      pMsgFrame->pWin->putString(str);
   }
   else if (pMsg->rspCode == 353)
   {
      sprintf(str, "Members of %s: %s\n",
                   (const char *)pMsg->dstStr,
                   (const char *)pMsg->msgStr);
      pMsgFrame->pWin->putString(str);
   }
   else if (pMsg->rspCode == 322)
   {
      sprintf(str, "Chan: [B]%s[/B] : %s\n", 
                  (const char *)pMsg->dstStr, (const char *)pMsg->msgStr); 
      pMsgFrame->pWin->putString(str);
   }
   else
   {
      if (dbg) fprintf(stdout, "xIrcMessageFrame::ircRespMessageIn():Building response!\n");
      if (dbg) fflush(stdout);
      sprintf(str, "%s: %s: %s\n", ircResponses.text(pMsg->rspCode), 
             (const char *)pMsg->dstStr, (const char *)pMsg->msgStr);
      if (dbg) fprintf(stdout, "xIrcMessageFrame::ircRespMessageIn():string = |%s|!\n", str);
      if (dbg) fflush(stdout);
      pMsgFrame->pWin->putString(str);
   }
}

void xIrcMessageFrame::doSpecial(xIrcMessage *pMsg)
{
   xIrcMessage msg;
   const char *cp;
   char str[1024], str1[1024];
   char *cp1;
   
   for (cp = (const char *)pMsg->msgStr, cp1 = str; *cp; cp++)
      if (*cp >= ' ')
      {
         *(cp1++) = *cp;
         *cp1 = '\0';
      }

   if (pMsg->pmsgTyp == ipmAction)
   {
      sprintf(str1, "* %s %s", (const char *)pMsg->srcNick, str);
      pMsgFrame->pWin->putString(str1);
   }
   else if (pMsg->pmsgTyp == ipmPing)
   {      
      if (isMsg(pMsg->rspCode, "NOTICE"))
      {
         long pingTime = time(NULL) - atol(str);
         long pingMins = pingTime / 60;
         long pingSecs = pingTime % 60;
         if (pingMins > 0)
            sprintf(str1, "*** Ping from %s: %ld:%02ld minutes\n", 
                           (const char *)pMsg->srcNick,
                           pingMins, pingSecs);
         else
            sprintf(str1, "*** Ping from %s: %ld seconds\n", 
                           (const char *)pMsg->srcNick, pingSecs);
         pMsgFrame->pWin->putString(str1);
      }
      else if (isMsg(pMsg->rspCode, "PRIVMSG"))
      {
         sprintf(str1, "*** Pinged by %s\n", (const char *)pMsg->srcNick);
         pMsgFrame->pWin->putString(str1);
         msg.rspCode = ircResponses.code("NOTICE");
         msg.dstStr = pMsg->srcNick;
         msg.msgStr = "\x01";
         msg.msgStr += "PING ";
         msg.msgStr += str;
         msg.msgStr += "\x01";
         Dispatcher.dispatchMsg(this, SLOT(ircRespMessageIn(xIrcMessage*)), &msg);
//         emit ircMessageOut(&msg);
      }
   }
   else if (pMsg->pmsgTyp == ipmVersion)
   {
      sprintf(str1, "*** Version: %s\n", str);
      pMsgFrame->pWin->putString(str1);
      
   }
   else if (pMsg->pmsgTyp == ipmTime)
   {
      {
         sprintf(str1, "*** Time: %s\n", str);
         pMsgFrame->pWin->putString(str1);
      }
   }
   else if (pMsg->pmsgTyp == ipmErrMsg)
   {
      {
         sprintf(str1, "*** Error Msg: %s\n", str);
         pMsgFrame->pWin->putString(str1);
      }
   }
   else if (pMsg->pmsgTyp == ipmClientInfo)
   {
      {
         sprintf(str1, "*** Client Info: %s\n", str);
         pMsgFrame->pWin->putString(str1);
      }
   }
   else if (pMsg->pmsgTyp == ipmFinger)
   {
      sprintf(str1, "*** Finger: %s\n", str);
      pMsgFrame->pWin->putString(str1);
   }
   else if (pMsg->pmsgTyp == ipmSource)
   {
      sprintf(str1, "*** Source: %s\n", str);
      pMsgFrame->pWin->putString(str1);
   }
   else if (pMsg->pmsgTyp == ipmUserInfo)
   {
      sprintf(str1, "*** UserInfo: %s\n", str);
      pMsgFrame->pWin->putString(str1);
   }
}
   
void xIrcMessageFrame::ircPrivMessageIn(xIrcMessage *pMsg)
{
   char msgBuf[1024];
   const char *pIn;
   
   if (dbg) fprintf(stdout, "xIrcMessageFrame::ircMessageIn():Got new message in!\n");
   if (dbg) fflush(stdout);
   pIn = pMsg->msgStr;
   if (*pIn < 2 || (pMsg->pmsgTyp != ipmUnknown && pMsg->pmsgTyp != ipmMessage))
      doSpecial(pMsg);
   else
   {
      if (isMsg(pMsg->rspCode, "PRIVMSG"))
      {
         if (dbg) fprintf(stdout, "xIrcMessageFrame::ircMessageIn():Formating Priv Message\n");
         if (dbg) fflush(stdout);
         sprintf(msgBuf, "<%s> %s", (const char *)pMsg->srcNick, (const char *)pMsg->msgStr);
         if (dbg) fprintf(stdout, "xIrcMessageFrame::ircMessageIn():Sending |%s| to TermWin\n", msgBuf);
         if (dbg) fflush(stdout);
         pMsgFrame->pWin->putString(msgBuf);
         if (dbg) fprintf(stdout, "xIrcMessageFrame::ircMessageIn():Done\n");
         if (dbg) fflush(stdout);
      }
      else if (isMsg(pMsg->rspCode, "NOTICE"))
      {
         if (dbg) fprintf(stdout, "xIrcMessageFrame::ircMessageIn():Formating Priv Message\n");
         if (dbg) fflush(stdout);
         sprintf(msgBuf, "<%s> NOTICE: %s", (const char *)pMsg->srcNick, (const char *)pMsg->msgStr);
         if (dbg) fprintf(stdout, "xIrcMessageFrame::ircMessageIn():Sending |%s| to TermWin\n", msgBuf);
         if (dbg) fflush(stdout);
         pMsgFrame->pWin->putString(msgBuf);
         if (dbg) fprintf(stdout, "xIrcMessageFrame::ircMessageIn():Done\n");
         if (dbg) fflush(stdout);
      }
      else
      {
         if (dbg) fprintf(stdout, "xIrcMessageFrame::ircMessageIn():Formating Unknow message\n");
         if (dbg) fflush(stdout);
         sprintf(msgBuf, "<%s> Unknown: ipmTyp = %d, msg = %s", (const char *)pMsg->srcNick, pMsg->pmsgTyp, (const char *)pMsg->msgStr);
         if (dbg) fprintf(stdout, "xIrcMessageFrame::ircMessageIn():Sending |%s| to TermWin\n", msgBuf);
         if (dbg) fflush(stdout);
         pMsgFrame->pWin->putString(msgBuf);
         if (dbg) fprintf(stdout, "xIrcMessageFrame::ircMessageIn():Done\n");
         if (dbg) fflush(stdout);
      }
   }
   if (pBeepMsg->isChecked())
      pApp->beep();
   if (dbg) fprintf(stdout, "xIrcMessageFrame::ircMessageIn():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcMessageFrame::procCommand(const char *pStr)
{
   char buf[1024], buf1[1024], pCmd[20], *cp;
   const char *cp1, *cp2;
   xIrcMessage msg;
   
   for (cp = pCmd, cp1 = pStr; *cp1 && !isspace(*cp1); cp1++)
   {
      *(cp++) = toupper(*cp1);
      *cp = '\0';
   }
   
   while (isspace(*cp1)) cp1++;
   if (strcmp(pCmd, "ME") == 0)
   {
      msg.rspCode = ircResponses.code("PRIVMSG");
      msg.dstStr = name();
      msg.msgStr = "\x01";
      msg.msgStr += "ACTION ";
      msg.msgStr += cp1;
      msg.msgStr += "\x01";
      sprintf(buf, "* %s %s\n", NickQuery->text(), cp1);
      putString(buf);
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
      putString(buf1);
      return;
   }
   else if (strcmp(pCmd, "VERSION") == 0)
   {
      msg.rspCode = ircResponses.code("PRIVMSG");
      if (strlen(cp1) > 0)
         msg.dstStr = cp1;
      else
         msg.dstStr = name();
      msg.msgStr = "\x01";
      msg.msgStr += "VERSION";
      msg.msgStr += "\x01";
      sprintf(buf, "*** Version command sent to %s\n", (const char *)msg.dstStr);
      pMsgFrame->pWin->putString(buf);
   }
   else if (strcmp(pCmd, "USERINFO") == 0)
   {
      msg.rspCode = ircResponses.code("PRIVMSG");
      if (strlen(cp1) > 0)
         msg.dstStr = cp1;
      else
         msg.dstStr = name();
      msg.msgStr = "\x01";
      msg.msgStr += "USERINFO";
      msg.msgStr += "\x01";
      sprintf(buf, "*** UserInfo command sent to %s\n", (const char *)msg.dstStr);
      pMsgFrame->pWin->putString(buf);
   }
   else if (strcmp(pCmd, "CLIENTINFO") == 0)
   {
      for (cp2 = cp1; *cp2 && isspace(*cp2); cp2++);
      for (cp = buf, *cp = '\0'; *cp2 && !isspace(*cp2); cp2++)
      {
         *(cp++) = toupper(*cp2);
         *cp = '\0';
      }
      for (; *cp2 && isspace(*cp2); cp2++);
      msg.rspCode = ircResponses.code("PRIVMSG");
      if (strlen(cp1) > 0)
      {
         msg.dstStr = cp1;
         msg.msgStr = "\x01";
         msg.msgStr += "CLIENTINFO";
         if (strlen(buf) > 0)
         {
            msg.msgStr += " ";
            msg.msgStr += buf;
         }
         msg.msgStr += "\x01";
         sprintf(buf, "*** ClientInfo command sent to %s\n", (const char *)msg.dstStr);
         pMsgFrame->pWin->putString(buf);
      }
      else
      {
         pMsgFrame->pWin->putString("*** ERROR: ClientInfo needs a Nick!\n");
         return;
      }
   }
   else if (strcmp(pCmd, "MSG") == 0)
   {
      for (cp2 = cp1; *cp2 && isspace(*cp2); cp2++);
      for (cp = buf, *cp = '\0'; *cp2 && !isspace(*cp2); cp2++)
      {
         *(cp++) = toupper(*cp2);
         *cp = '\0';
      }
      for (; *cp2 && isspace(*cp2); cp2++);
      if (strlen(cp1) > 0)
      {
         msg.dstStr = buf;
         msg.rspCode = ircResponses.code("PRIVMSG");
         msg.msgStr = cp2;
         sprintf(buf1, "\x02<MSG: %s>\x02 %s\n", buf, cp2);
         putString(buf1);
      }
   }
   else if (strcmp(pCmd, "FINGER") == 0)
   {
      msg.rspCode = ircResponses.code("PRIVMSG");
      if (strlen(cp1) > 0)
         msg.dstStr = cp1;
      else
         msg.dstStr = name();
      msg.msgStr = "\x01";
      msg.msgStr += "FINGER";
      msg.msgStr += "\x01";
      sprintf(buf, "*** Finger command sent to %s\n", (const char *)msg.dstStr);
      pMsgFrame->pWin->putString(buf);
   }
   else if (strcmp(pCmd, "SOURCE") == 0)
   {
      msg.rspCode = ircResponses.code("PRIVMSG");
      if (strlen(cp1) > 0)
         msg.dstStr = cp1;
      else
         msg.dstStr = name();
      msg.msgStr = "\x01";
      msg.msgStr += "SOURCE";
      msg.msgStr += "\x01";
      sprintf(buf, "*** Source command sent to %s\n", (const char *)msg.dstStr);
      pMsgFrame->pWin->putString(buf);
   }
   else if (strcmp(pCmd, "TIME") == 0)
   {
      msg.rspCode = ircResponses.code("PRIVMSG");
      if (strlen(cp1) > 0)
         msg.dstStr = cp1;
      else
         msg.dstStr = name();
      msg.msgStr = "\x01";
      msg.msgStr += "TIME";
      msg.msgStr += "\x01";
      sprintf(buf, "*** Time command sent to %s\n", (const char *)msg.dstStr);
      pMsgFrame->pWin->putString(buf);
   }
   else if (strcmp(pCmd, "PING") == 0)
   {
      sprintf(buf1, "%ld", time(NULL));
      msg.rspCode = ircResponses.code("PRIVMSG");
      if (strlen(cp1) > 0)
         msg.dstStr = cp1;
      else
         msg.dstStr = name();
      msg.msgStr = "\x01";
      msg.msgStr += "PING ";
      msg.msgStr += buf1;
      msg.msgStr += "\x01";
      sprintf(buf, "*** Ping command sent to %s\n", (const char *)msg.dstStr);
      pMsgFrame->pWin->putString(buf);
   }
   else if (strcmp(pCmd, "NAMES") == 0)
   {
      msg.rspCode = ircResponses.code("NAMES");
      if (strlen(cp1) > 0)
      {
         if (*cp1 == '#')
            msg.dstStr = cp1;
         else if (*cp1 == '*')
            msg.dstStr = "";
         else
            return;
      }
      else
         msg.dstStr = name();
      msg.msgStr = "";
      sprintf(buf, "/Names: %s\n", (const char *)msg.dstStr);
      pMsgFrame->pWin->putString(buf);
   }
   else if (strcmp(pCmd, "KICK") == 0)
   {
      for (cp2 = cp1; *cp2 && isspace(*cp2); cp2++);
      for (cp = buf, *cp = '\0'; *cp2 && !isspace(*cp2); cp2++)
      {
         *(cp++) = toupper(*cp2);
         *cp = '\0';
      }
      for (; *cp2 && isspace(*cp2); cp2++);
      
      msg.rspCode = ircResponses.code("KICK");
      msg.dstStr = name();
      msg.dstStr += " ";
      msg.dstStr += buf;
      if (strlen(cp2) > 0)
         msg.msgStr = cp2;
      else
         msg.msgStr = "";
      sprintf(buf, "/Kick: %s\n", (const char *)msg.msgStr);
      pMsgFrame->pWin->putString(buf);
   }
   else if (strcmp(pCmd, "TOPIC") == 0)
   {
      for (; *cp1 && isspace(*cp1); cp1++);
      msg.rspCode = ircResponses.code("TOPIC");
      msg.dstStr = name();
      msg.dstStr += " ";
      if (strlen(cp1) > 0)
      {
         if (*cp1 == '#')
         {
            msg.dstStr = cp1;
            msg.msgStr = "";
         }
         else
            msg.msgStr = cp1;
      }
      else
         msg.msgStr = "";
      sprintf(buf, "/Topic: %s\n", (const char *)msg.msgStr);
      pMsgFrame->pWin->putString(buf);
   }
   else if (strcmp(pCmd, "INVITE") == 0)
   {
      for (cp2 = cp1; *cp2 && isspace(*cp2); cp2++);
      for (cp = buf, *cp = '\0'; *cp2 && !isspace(*cp2); cp2++)
      {
         *(cp++) = toupper(*cp2);
         *cp = '\0';
      }
      for (; *cp2 && isspace(*cp2); cp2++);
      
      msg.rspCode = ircResponses.code("INVITE");
      msg.dstStr = name();
      msg.dstStr += " ";
      msg.dstStr += buf;
      msg.msgStr = cp2;
      sprintf(buf, "/Invite: %s\n", (const char *)msg.msgStr);
      pMsgFrame->pWin->putString(buf);
   }
   else if (strcmp(pCmd, "MODE") == 0)
   {
      msg.rspCode = ircResponses.code("MODE");
      msg.dstStr = name();
      msg.dstStr += " ";
      msg.dstStr += cp1;
      msg.msgStr = "";
      sprintf(buf, "/Mode: %s\n", (const char *)msg.dstStr);
      pMsgFrame->pWin->putString(buf);
   }
   else if (strcmp(pCmd, "NICK") == 0)
   {
      msg.rspCode = ircResponses.code("NICK");
      msg.dstStr = cp1;
      msg.msgStr = "";
      NickQuery->setText(cp1);
      sprintf(buf, "/Nick: %s\n", (const char *)msg.dstStr);
      pMsgFrame->pWin->putString(buf);
   }
   else if (strcmp(pCmd, "QUIT") == 0)
   {
      msg.rspCode = ircResponses.code("QUIT");
      msg.dstStr = " ";
      msg.msgStr = cp1;
      sprintf(buf, "/Quit: %s\n", (const char *)msg.msgStr);
      pMsgFrame->pWin->putString(buf);
   }
   else if (strcmp(pCmd, "AWAY") == 0)
   {
      msg.rspCode = ircResponses.code("AWAY");
      msg.dstStr = " ";
      msg.msgStr = "";
      if (strlen(cp1) > 0)
      {
         msg.msgStr = ":";
         msg.msgStr += cp1;
      }
      sprintf(buf, "/Away: %s\n", (const char *)msg.msgStr);
      putString(buf);
   }
   else if ((msg.rspCode = ircResponses.code(pCmd)) != IRCRESP_Unknown)
   {
      msg.dstStr = cp1;
      msg.msgStr = "";
      sprintf(buf, "/%s: %s\n", pCmd, (const char *)msg.dstStr);
      putString(buf);
   }
   else
   {
//    sprintf(buf, "Unknown Command: /%s %s\n", pCmd, (const char *)msg.dstStr);
      sprintf(buf, "Unknown Command: /%s\n", pCmd);
      pMsgFrame->pWin->putString(buf);
      return;
   }
   Dispatcher.dispatchMsg(this, SLOT(ircRespMessageIn(xIrcMessage*)), &msg);
//   emit ircMessageOut(&msg);
}

void xIrcMessageFrame::nickButtonPressed(int iBtn)
{
   xIrcMessage msg;
   char buf[80], buf1[20];
   xIrcNickListEntry *pEntry = NULL;
   const char *pNick = NULL;

   if (dbg) fprintf(stdout, "xIrcMessageFrame::nickButtonPressed():Enter\n");
   if (dbg) fflush(stdout);
   if (iBtn == xCN_Close)
   {
      if (dbg) fprintf(stdout, "xIrcMessageFrame::nickButtonPressed():Close Btn!\n");
      if (dbg) fflush(stdout);
      pNicks->hide();
   }
   if (strlen(pNicks->text()) == 0 && (pEntry = pNicks->selected()) != NULL)
   {
      if (dbg) fprintf(stdout, "xIrcMessageFrame::nickButtonPressed():Found Selected Nick\n");
      if (dbg) fflush(stdout);
      pNick = pEntry->getNick();
   }
   else
   {
      if (dbg) fprintf(stdout, "xIrcMessageFrame::nickButtonPressed():No Nick Selected\n");
      if (dbg) fflush(stdout);
      pNick = pNicks->text();
      if (strlen(pNick) == 0)
         return;
   }

   if (pNick != NULL)
   {
      if (dbg) fprintf(stdout, "xIrcMessageFrame::nickButtonPressed():Doing action on |%s|\n", pNick);
      if (dbg) fflush(stdout);
      switch (iBtn)
      {
         case xCN_Whois:
            msg.rspCode = ircResponses.code("WHOIS");
            msg.msgStr = pNick;
            msg.dstStr = "";
            break;
        
         case xCN_Invite:
            msg.rspCode = ircResponses.code("INVITE");
            msg.dstStr = name();
            msg.dstStr += ' ';
            msg.dstStr += pNick;
            msg.msgStr = "";
            break;
            
         case xCN_Kick:
            if (KickQuery->exec() != QDialog::Rejected)
            {
               msg.rspCode = ircResponses.code("KICK");
               msg.dstStr = name();
               msg.dstStr += ' ';
               msg.dstStr += pNick;
               msg.msgStr = KickQuery->text();
            }
            else
               return;
            break;
         
         case xCN_Query:
            ChanQuery->setText(pNick);
            emit chatRequest();
            return;
            
         case xCN_Ping:
            sprintf(buf1, "%ld", time(NULL)); 
            msg.rspCode = ircResponses.code("PRIVMSG");
            msg.dstStr = pNick;
            msg.msgStr = "\x01";
            msg.msgStr += "PING ";
            msg.msgStr += buf;
            msg.msgStr += "\x01";
            sprintf(buf, "*** Ping command sent to %s\n", (const char *)msg.dstStr);
            pMsgFrame->pWin->putString(buf);
            break;

         default:
            return;
      }      
      Dispatcher.dispatchMsg(this, SLOT(ircRespMessageIn(xIrcMessage*)), &msg);
//      emit ircMessageOut(&msg);
   }
   if (dbg) fprintf(stdout, "xIrcMessageFrame::nickButtonPressed():Exit\n");
   if (dbg) fflush(stdout);
}
