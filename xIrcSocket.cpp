/***************************************************************************
**    xIrcSocket.cpp  $Revision: 1.4 $ - $Name:  $ 
**    xSocket Class to parse IRC Messages
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
#include <xDefaults.h>
#include "xIrcCommands.h"
#include "xIrcSocket.h"

static int dbg = 0;

extern xDefaults Defaults;
extern xIrcCommands ircResponses;

static int stdColors[16][3] = 
{
   {  0,   0,   0},  /* 0: Black       */
   {  0,   0, 128},  /* 1: Blue        */
   {  0, 128,   0},  /* 2: Grean       */
   {  0, 128, 128},  /* 3: Cyan        */
   {128,   0,   0},  /* 4: Red         */
   {128,   0, 128},  /* 5: Purple      */
   {128, 128,   0},  /* 6: Brown       */
   {204, 204, 204},  /* 7: Lt Gray     */
   {128, 128, 128},  /* 8: Gray        */
   {  0,   0, 255},  /* 9: Lt Blue     */
   {  0, 255,   0},  /* A: Lt Green    */
   {  0, 255, 255},  /* B: lt Cyan     */
   {255,   0,   0},  /* C: Lt Red      */
   {255,   0, 255},  /* D: Pink        */
   {255, 255,   0},  /* E: Yellow      */
   {255, 255, 255},  /* F: White       */
};

static int mircColors[16][3] = 
{
   {255, 255, 255},  /* 0: White       */
   {  0,   0,   0},  /* 1: Black       */
   {  0,   0, 100},  /* 2: Navy Blue   */
   {  0, 180,   0},  /* 3: Green       */
   {180,   0,   0},  /* 4: Red         */
   {128, 100,   0},  /* 5: Brown       */
   {128,   0, 128},  /* 6: Purple      */
   {220, 145,   0},  /* 7: Orange      */
   {255, 255,   0},  /* 8: Yellow      */
   {138, 255,   0},  /* 9: lt.Green    */
   {  0, 128, 128},  /* A: Teal        */
   {  0, 255, 255},  /* B: Cyan        */
   {  0,   0, 255},  /* C: Lt.Blue     */
   {255,   0, 255},  /* D: lt.Purple   */
   {128, 128, 128},  /* E: Grey        */
   {190, 190, 190},  /* F: lt.Grey     */
};

xIrcSocket::xIrcSocket(xWidgetResInfo *pPRes, QObject *pParent, const char *pHost,
                       const char *pService, QObject *pSocketDialog, 
                       const char *pSlotStatus, const char *pSlotProgress, 
                       int &err) :
            xSocketTCP(pPRes, pParent, pHost, pService, pSocketDialog,
                       pSlotStatus, pSlotProgress, serr)
{
   if(dbg) {
      fprintf(stderr, "The hostname is [%s]\n", pHost);
      fprintf(stderr, "The port num is [%s]\n", pService);
      fprintf(stderr, "The err  val is [%d]\n", serr);
      fflush(stderr);
   }
   if ((err = serr) == 0)
   {
      connect(this, SIGNAL(socketDataOut(QString)), 
              this, SLOT(socketDataIn(QString)));
      connect(this, SIGNAL(readFromSocket(char *)), 
              this, SLOT(rawSocketDataIn(char *)));
      connect(this, SIGNAL(sendToServer(const char *)), 
              this, SLOT(sendToSocket(const char *)));
   }
}

xIrcSocket::~xIrcSocket()
{
//   QObject::disconnect(this);
}

//
// This member function parses the Private messages is they
// are special, ei, ones that begin with a ^A. On exiting, if
// pRtn is not NULL, it will be set past the command found unless
// it is not recognized,  in which case pRtn will be set to the
// original value of cp.
//
// pMsg->pmsgType will be set to one of the values of the 
// enum xIrcPrvMsgType to indicate the type of private message
// found.
//
void xIrcSocket::parseMsgSpecial(xIrcMessage *pMsg, const char *pStr,
                     const char **pRtn)
{
   QString  tmpStr = "";
   const char *pSave, *pSave1;
   xIrcMessage msg;

   if (dbg) fprintf(stderr, "xIrcSocket::parseMsgSpecial(): Enter\n");
   if (dbg) fflush(stderr);
   *pRtn = NULL;
   if (strlen(pStr) == 0 || *pStr == '\n')
   {
      if (dbg) fprintf(stderr, "xIrcSocket::parseMsgSpecial(): Exit, NULL String\n");
      if (dbg) fflush(stderr);
      *pRtn = pStr;
      return;
   }

   msg.fd = pMsg->fd;
   msg.rspCode = pMsg->rspCode;
   msg.rspStr = pMsg->rspStr;
   msg.srcNick = pMsg->srcNick;
   msg.srcAddr = pMsg->srcAddr;
   msg.dstStr = pMsg->dstStr;
   msg.msgStr = pMsg->msgStr;
   msg.rawMsg = pMsg->rawMsg;

   pSave = pStr;
   while (*pStr && !isspace(*pStr) && *pStr >= ' ')
      tmpStr += toupper(*(pStr++));

   if (strcmp(tmpStr.latin1(), "PING") == 0)
      msg.pmsgTyp = ipmPing;
   else if (strcmp(tmpStr.latin1(), "ACTION") == 0) 
      msg.pmsgTyp = ipmAction;
   else if (strcmp(tmpStr.latin1(), "VERSION") == 0) 
      msg.pmsgTyp = ipmVersion;
   else if (strcmp(tmpStr.latin1(), "USERINFO") == 0) 
      msg.pmsgTyp = ipmUserInfo;
   else if (strcmp(tmpStr.latin1(), "CLIENTINFO") == 0) 
      msg.pmsgTyp = ipmClientInfo;
   else if (strcmp(tmpStr.latin1(), "SOURCE") == 0) 
      msg.pmsgTyp = ipmSource;
   else if (strcmp(tmpStr.latin1(), "FINGER") == 0) 
      msg.pmsgTyp = ipmFinger;
   else if (strcmp(tmpStr.latin1(), "ERRMSG") == 0) 
      msg.pmsgTyp = ipmErrMsg;
   else if (strcmp(tmpStr.latin1(), "TIME") == 0) 
      msg.pmsgTyp = ipmTime;
   else if (strcmp(tmpStr.latin1(), "DCC") == 0)
   {
      pSave1 = pStr;
      while (*pStr && isspace(*pStr)) pStr++;
      tmpStr = "";
      while (*pStr && !isspace(*pStr))
         tmpStr += toupper(*(pStr++));
      if (strcmp(tmpStr.latin1(), "SEND") == 0)
         msg.pmsgTyp = ipmDCCSend;
      else if (strcmp(tmpStr.latin1(), "CHAT") == 0)
         msg.pmsgTyp = ipmDCCChat;
      else
      {
         pStr = pSave1;
         msg.pmsgTyp = ipmDCC;
      }
   }
   else
      *pRtn = pSave;

   if (*pRtn == NULL)
   {
      if (dbg) fprintf(stderr, "xIrcSocket::parseMsgSpecial(): Saving message portion of the string\n");
      if (dbg) fflush(stderr);
      tmpStr = "";
      while (*pStr != '\x0' && *pStr != '\x01')
         tmpStr += *(pStr++);
      if (dbg) fprintf(stderr, "xIrcSocket::parseMsgSpecial(): Translating attributes in message\n");
      if (dbg) fflush(stderr);
      msg.msgStr = translateMessage(tmpStr.latin1(), this, &msg);
      if (*pStr == '\x01')
          pStr++;
   
      if (dbg) fprintf(stderr, "xIrcSocket::parseMsgSpecial(): Sending ipm Message type %d\n",
                        msg.pmsgTyp);
      if (dbg) fflush(stderr);
      emit ircServerMessage(&msg);
      *pRtn = pStr;
   }
   if (dbg) fprintf(stderr, "xIrcSocket::parseMsgSpecial(): Exit- remaining line:|%s|\n",
                           *pRtn);
   if (dbg) fflush(stderr);
}

//
// This member function parses the string pointed to by
// pText and emits a ircServerMessage signal passing the pointer
// to a xIrcMessage Structure to be handled by the higher levels.
//
void xIrcSocket::socketDataIn(QString msgStr)
{
   xIrcMessage msg;
   QString strTmp;
   bool isNumber;
   const char *cp;
   const char *pText = msgStr.latin1();
   int x;
   
   if (dbg) fprintf(stderr, "xIrcSocket::socketDataIn(): Enter\n");
   if (dbg) fflush(stderr);
   if (dbg) fprintf(stderr, "xIrcSocket::socketDataIn():Have Raw message: |%s|\n",
                             pText);
   if (dbg) fflush(stderr);
   msg.srcNick = "";
   msg.srcAddr = "";
   msg.msgStr  = "";
   msg.rawMsg = pText;
   msg.rspCode = -1;
   //
   // Start by getting the nick, if present, then the address of the messages
   // source
   //
   if (dbg) fprintf(stderr, "xIrcSocket::socketDataIn(): Getting Nick\n");
   if (dbg) fflush(stderr);
   cp = pText;
   if (*pText != ':')
   {
      for (; *cp && *cp != '!' && *cp != ' '; cp++)
         strTmp += *(cp);
      msg.rspStr = strTmp;
   
      x = strTmp.toInt(&isNumber);
      if (isNumber)
      {
         if (dbg) fprintf(stderr, "xIrcSocket::socketDataIn(): Response was a code\n");
         if (dbg) fflush(stderr);
         msg.rspCode = x;
      }
      else
      {
         if (dbg) fprintf(stderr, "xIrcSocket::socketDataIn(): Response was a string\n");
         if (dbg) fflush(stderr);
         msg.rspCode = ircResponses.code(strTmp.latin1());
      }
      strTmp = "";
   }
   
   for (cp++; *cp && *cp != '!' && *cp != ' '; cp++)
      strTmp += *(cp);
   if (*cp == '!')
   {
      if (dbg) fprintf(stderr, "xIrcSocket::socketDataIn(): Getting Address\n");
      if (dbg) fflush(stderr);
      msg.srcNick = strTmp;
      strTmp = "";
      for (cp = cp+1; *cp && *cp != ' '; cp++)
         strTmp += *(cp);
   }      
   msg.srcAddr = strTmp;
   strTmp = "";
   
   //
   // Next either a numeric or text response will follow
   // Gather it, if its numeric, get its value and stuff it in msg.rspCode
   // Otherwise, get its text
   //
   if (msg.rspCode < 0)
   {
      if (dbg) fprintf(stderr, "xIrcSocket::socketDataIn(): Getting Response\n");
      if (dbg) fflush(stderr);
      for (cp++; *cp && *cp != ' '; cp++)
         strTmp += *(cp);
      msg.rspStr = strTmp;
      
      x = strTmp.toInt(&isNumber);
      if (isNumber)
      {
         if (dbg) fprintf(stderr, "xIrcSocket::socketDataIn(): Response was a code\n");
         if (dbg) fflush(stderr);
         msg.rspCode = x;
      }
      else
      {
         if (dbg) fprintf(stderr, "xIrcSocket::socketDataIn(): Response was a string\n");
         if (dbg) fflush(stderr);
         msg.rspCode = ircResponses.code(strTmp.latin1());
      }
      strTmp = "";
   }
   
   //
   // Now get the destination for the message, it should either be
   // a nick, or a channel name
   //
   if (dbg) fprintf(stderr, "xIrcSocket::socketDataIn(): Getting Destination\n");
   if (dbg) fflush(stderr);
   if (msg.rspCode >= 300 && msg.rspCode <= 369)
   {
      //
      // If this is a response in the range of 300 - 369, assume that the
      // Destination is the first word of the message and skip the destination
      // specified in the destination field
      //
      for (cp++; *cp && *cp != ':' && *cp != ' '; cp++);
      while (isspace(*cp) || *cp == ':' || *cp == '=' || *cp == '*' || *cp == '@') cp++;
      for (; *cp && *cp != ':' && *cp != ' '; cp++)
      {
         if (*cp != '\r' && *cp != '\n')
            strTmp += *(cp);
      }
      msg.dstStr = strTmp;
   }
   else
   {
      //
      // Otherwise, just use the destination as specified
      //
      while (*cp == ' ' || *cp == ':' || *cp == '=') cp++;
      for (; *cp && *cp != ':' && *cp != ' '; cp++)
      {
         if (*cp != '\r' && *cp != '\n')
            strTmp += *(cp);
      }
      msg.dstStr = strTmp;
   }
   strTmp = "";
   
   //
   // Just gather the remainder of the line and assume that it is the
   // message. This way it can be parsed as needed by the higher levels.
   // In any case, drop and '\r' encountered
   //
   if (dbg) fprintf(stderr, "xIrcSocket::socketDataIn(): Getting Message\n");
   if (dbg) fflush(stderr);
   while (isspace(*cp)) cp++;
   if (*cp == ':')
      while (isspace(*(++cp)));
   
   msg.pmsgTyp = ipmMessage;
   if(*cp)
      strTmp = translateMessage(cp, this, &msg);
   /*
   ** Send the message, if any, to the receiver!
   */
   if ((!ircResponses.is(msg.rspCode, (char *)"PRIVMSG") && !ircResponses.is(msg.rspCode, (char *)"NOTICE")) ||
        strlen(strTmp.latin1()) > 1)
   {
      msg.pmsgTyp = ipmMessage;
      msg.msgStr = strTmp;
      if (dbg) fprintf(stderr, "xIrcSocket::socketDataIn(): Sending data on via ircServerMessage\n");
      if (dbg) fflush(stderr);
      emit ircServerMessage(&msg);
      if (dbg) fprintf(stderr, "xIrcSocket::socketDataIn(): Exit\n");
      if (dbg) fflush(stderr);
   }
}

void xIrcSocket::rawSocketDataIn(char *pText)
{
   int crFlag;
   QString tmpBuf;

   if (dbg > 3) fprintf(stderr, "xIrcSocket::rawSocketDataIn(\"%s\"): Enter\n", pText);
   if (dbg > 3) fflush(stderr);
   tmpBuf = "";
   for (crFlag = 0; *pText; pText++)
   {
      if (crFlag)
      {
         if (*pText != '\n')
         {
            if (dbg > 3) fprintf(stderr, "xIrcSocket::rawSocketDataIn(): Terminator 1\n");
            if (dbg > 3) fflush(stderr);
            socketBuffer += '\n';
            tmpBuf = socketBuffer.copy();
            socketBuffer.truncate(0);
            socketBuffer = "";
//            emit socketDataOut(socketBuffer);
//            emit socketDataOut(tmpBuf);
            if (dbg > 3) fprintf(stderr, "xIrcSocket::rawSocketDataIn():socketBuffer set to |%s|\n",
                                 (const char *)socketBuffer.latin1());
            if (dbg > 3) fflush(stderr);
            socketBuffer += *pText;
         }
         else
         {
            if (dbg > 3) fprintf(stderr, "xIrcSocket::rawSocketDataIn(): Terminator 2\n");
            if (dbg > 3) fflush(stderr);
            socketBuffer += *pText;
            tmpBuf = socketBuffer.copy();
            socketBuffer.truncate(0);
            socketBuffer = "";
//            emit socketDataOut(tmpBuf);
//            emit socketDataOut(socketBuffer);
            if (dbg > 3) fprintf(stderr, "xIrcSocket::rawSocketDataIn():socketBuffer set to |%s|\n",
                                 (const char *)socketBuffer.latin1());
            if (dbg > 3) fflush(stderr);
         }
         if (*pText != '\r')
            crFlag = 0;
      }
      else
      {
         if (*pText == '\r')
            crFlag = 1;
         else
            socketBuffer += *pText;
         if (*pText == '\n')
         {
            if (dbg > 3) fprintf(stderr, "xIrcSocket::rawSocketDataIn(): Terminator 3\n");
            if (dbg > 3) fflush(stderr);
            socketBuffer += *pText;
            tmpBuf = socketBuffer.copy();
            socketBuffer.truncate(0);
            socketBuffer = "";
//            emit socketDataOut(socketBuffer);
//            emit socketDataOut(tmpBuf);
            if (dbg > 3) fprintf(stderr, "xIrcSocket::rawSocketDataIn():socketBuffer set to |%s|\n",
                                 (const char *)socketBuffer.latin1());
            if (dbg > 3) fflush(stderr);
         }
      }
   }
   if (strlen(tmpBuf.latin1()) > 0)
      emit socketDataOut(tmpBuf);

   if (dbg > 4) fprintf(stderr, "xIrcSocket::rawSocketDataIn():socketBuffer left as |%s|\n",
                        (const char *)socketBuffer.latin1());
   if (dbg > 4) fflush(stderr);
   if (dbg > 3) fprintf(stderr, "xIrcSocket::rawSocketDataIn(): Exit\n");
   if (dbg > 3) fflush(stderr);
}

void xIrcSocket::sendIrcServerMessage(xIrcMessage *pMsg)
{
   QString  msgStr = "";
   
   if (dbg > 2) fprintf(stderr, "xIrcSocket::sendIrcServerMessage(): Sending message\n");
   if (dbg > 2) fprintf(stderr, "xIrcSocket::sendIrcServerMessage(): Response code = %d\n", pMsg->rspCode);
   if (dbg > 2) fprintf(stderr, "xIrcSocket::sendIrcServerMessage(): Destination =  |%s|\n",
                                (const char *)pMsg->dstStr.latin1());
   if (dbg > 2) fflush(stderr);
   
   msgStr = ircResponses.name(pMsg->rspCode);
   if (strlen(msgStr.latin1()) == 0)
   {
      msgStr.setNum(pMsg->rspCode);
   }
   msgStr += ' ';
   if (strlen((const char *)pMsg->dstStr.latin1()) > 0)
   {
      msgStr += pMsg->dstStr;
      if (strlen((const char *)pMsg->msgStr.latin1()) > 0)
         msgStr += " :";
   }
   if (strlen((const char *)pMsg->msgStr.latin1()) > 0)
      msgStr += pMsg->msgStr;
   msgStr += '\n';
   if (dbg > 2) fprintf(stderr, "xIrcSocket::sendIrcServerMessage(): Sending message %s\n", (const char *)msgStr.latin1());
   if (dbg > 2) fflush(stderr);
   emit sendToServer((const char *)msgStr.latin1());
   if (dbg > 2) fprintf(stderr, "xIrcSocket::sendIrcServerMessage(): Message sent\n");
   if (dbg > 2) fflush(stderr);
}

/*
void xIrcSocket::socketError(int err)
{
   err = 0;
}
*/

bool xIrcMessageList::is(xIrcMessage *pMsg)
{
   xIrcMessage *pMsg1;

   if (dbg) fprintf(stderr,"xIrcMessageList::is(%s): Enter\n",
                           (const char *)pMsg->srcNick.latin1());
   if (dbg) fflush(stderr);
   if ((pMsg1 = at(0)) != NULL)
   {
      if (dbg) fprintf(stderr, "xIrcMessageList::is(%s): Testing\n",
                              (const char *)pMsg1->srcNick.latin1());
      if (dbg) fflush(stderr);
      if (strcmp(pMsg->srcNick.latin1(), pMsg1->srcNick.latin1()) == 0)
      {
         if (dbg) fprintf(stderr,"xIrcMessageList::is(): Exit- Passed!\n");
         if (dbg) fflush(stderr);
         return(TRUE);
      }
   }
   if (dbg) fprintf(stderr,"xIrcMessageList::is(): Exit- Fail\n");
   if (dbg) fflush(stderr);
   return(FALSE);
}

QString translateMessage(const char *cp, xIrcSocket *pSocket, xIrcMessage *pMsg)
{
   int attrList[256];
   QString rv;

   if (dbg) fprintf(stderr,"xIrcSocket::translateMessage(): Enter\n");
   if (dbg) fflush(stderr);
   memset(attrList, 0, sizeof(attrList));
   for (; *cp; cp++)
   {
      /*
      ** Handle any CTCP requests
      */
      if (pSocket != NULL && pMsg != NULL && 
          (ircResponses.is(pMsg->rspCode, (char *)"PRIVMSG") ||
           ircResponses.is(pMsg->rspCode, (char *)"NOTICE")) &&
          *cp == 0x01)
         pSocket->parseMsgSpecial(pMsg, ++cp, &cp);
      /*
      ** Handle any CTCP Formatting here
      */
      else if (*cp == '[')
      {
         rv += "\\[";
      }
      else if (*cp == '\\')
      {
         rv += "\\\\";
      }
      else if (*cp == ('F' - '\x40'))
      {
         cp++;
         char attr = *(cp++);
         rv += '[';

         if (toupper(attr) == 'C')
         {
            QString s;
            while (*cp != ('F' - '\x40') )
               s += *(cp++);
            if (strlen(s.latin1()) == 0)
            {
               rv += '/';
               rv += attr;
            }
            else
            {
               rv += attr;
               rv += s;
            }
            rv += ']';
         }
         else
         {
            attr = toupper(attr);
            if (*cp == '-')
            {
               cp++;
               attrList[attr - 0x40] = 0;
            }
            else if (*cp == '+')
            {
               cp++;
               attrList[attr - 0x40] = 1;
            }
            else
               attrList[attr - 0x40] = (attrList[attr - 0x40] == 1) ? 0 : 1;
            if (dbg) fprintf(stderr,"xIrcSocket::translateMessage(): attrList[%d] = %d\n",
                attr - 0x40, attrList[attr - 0x40]);
            if (attrList[attr - 0x40] == 0)
               rv += '/';
            rv += attr;
            while (*cp != ('F' - '\x40') )
               rv += *(cp++);
            rv += ']';
         }
      }
      else if (*cp == ('B' - '\x40'))
      {
         attrList[int(*cp)] = (attrList[int(*cp)] == 1) ? 0 : 1;
         if (dbg) fprintf(stderr,"xIrcSocket::translateMessage(): attrList[%d] = %d\n",
             *cp, attrList[int(*cp)]);
         if (attrList[int(*cp)] == 1)
            rv += "[B]";
         else
            rv += "[/B]";
      }
      else if (*cp == ('V' - '\x40'))
      {
         attrList[int(*cp)] = (attrList[int(*cp)] == 1) ? 0 : 1;
         if (dbg) fprintf(stderr,"xIrcSocket::translateMessage(): attrList[%d] = %d\n",
             *cp, attrList[int(*cp)]);
         if (attrList[int(*cp)] == 1)
            rv += "[V]";
         else
            rv += "[/V]";
      }
      else if (*cp == ('_' - '\x40'))
      {
         attrList[int(*cp)] = (attrList[int(*cp)] == 1) ? 0 : 1;
         if (dbg) fprintf(stderr,"xIrcSocket::translateMessage(): attrList[%d] = %d\n",
             *cp, attrList[int(*cp)]);
         if (attrList[int(*cp)] == 1)
            rv += "[U]";
         else
            rv += "[/U]";
      }
      else if (*cp == ('O' - '\x40'))
      {
         attrList[int(*cp)] = (attrList[int(*cp)] == 1) ? 0 : 1;
         if (dbg) fprintf(stderr,"xIrcSocket::translateMessage(): attrList[%d] = %d\n",
             *cp, attrList[int(*cp)]);
         if (dbg) fflush(stderr);
         rv += "[N]";
         memset(attrList, 0, sizeof(int)*256);
      }
      else if (*cp == ('C' - '\x40'))
      {
         if (dbg) fprintf(stderr,"xIrcSocket::translateMessage(): Have color mIrc spec\n");
         if (dbg) fflush(stderr);
         rv += "[";
         if (!isdigit(*(cp+1)))
         {
            rv += "/C]";
            if (dbg) fprintf(stderr,"xIrcSocket::translateMessage(): Cancel!\n");
            if (dbg) fflush(stderr);
         }
         else
         {
            char buf[5];
            unsigned int i = 0, x;
            cp++;
            for (i = 0, x = 0; x < 2 && isdigit(*cp); x++, cp++)
            {
               i *= 10;
               i += *cp - '0';
            }
            if (dbg) fprintf(stderr,"xIrcSocket::translateMessage(): i.1 = %x\n", i);
            if (dbg) fflush(stderr);
            i = i % 16;
            if (dbg) fprintf(stderr,"xIrcSocket::translateMessage(): i = %x\n", i);
            if (dbg) fflush(stderr);
            sprintf(buf, "%x", i);
            if (dbg) fprintf(stderr,"xIrcSocket::translateMessage(): buf = |%s|\n", buf);
            if (dbg) fflush(stderr);
            rv += "C ";
            rv += buf;
            if (*cp == ',')
            {
               if (isdigit(*(cp+1)))
               {
                  cp++;
                  for (i = 0, x = 0; x < 2 && isdigit(*cp); x++, cp++)
                  {
                     i *= 10;
                     i += *cp - '0';
                  }
                  if (dbg) fprintf(stderr,"xIrcSocket::translateMessage(): i.1 = %x\n", i);
                  if (dbg) fflush(stderr);
                  i = i % 16;
                  if (dbg) fprintf(stderr,"xIrcSocket::translateMessage(): i = %x\n", i);
                  if (dbg) fflush(stderr);
                  sprintf(buf, " %x", i);
                  if (dbg) fprintf(stderr,"xIrcSocket::translateMessage(): buf = |%s|\n", buf);
                  if (dbg) fflush(stderr);
                  rv += buf;
                  cp--;
               }
               else 
                  rv += " .";
            }
            else 
            {
               rv += " .";
               cp--;
            }
            rv += "]";
         }
      }
      else if (*cp != '\r' && *cp != '\x01')
         rv += *cp;
   }
   if (dbg) fprintf(stderr,"xIrcSocket::translateMessage(\"%s\"): Exit\n", (const char*)rv.latin1());
   if (dbg) fflush(stderr);
   return(rv);
}


QString parseLine(const char *cp, bool mircColors, bool ctcp2)
{
   QString strTmp, rv;
   char ch;

   for (; *cp && isspace(*cp); cp++);
//   strTmp = cp;
//   strTmp = Defaults.expandEntry(strTmp);
//   cp = strTmp;
   for (ch = ' '; *cp;)
   {
      if (ch == '\\')
      {
         if (*cp == '\\')
            rv += *(cp++);
         else
            rv += parseAttr(&cp, mircColors, ctcp2);
         ch = ' ';
      }
      else
      {
         ch = *cp;
         if (*cp != '\\')
            rv += *cp;
         cp++;
      }
   }
   return(rv);
}

QString parseAttr(const char **p, bool mircColors, bool ctcp2)
{
   int x;
   QString rv;
   char ch;
   const char *cp;
   QString fgColor, bgColor;

   cp = *p;
   if (dbg) fprintf(stderr, "xIrcmessageFrame::parseAttr(%c):Enter\n", toupper(*cp));
   if (dbg) fflush(stderr);
   switch (toupper(*(cp++)))
   {
      case 'B':
         if (ctcp2 == FALSE)
            rv += 'B' - 0x40;
         else
            rv = "\6B\6";
         break;

      case 'U':
         if (ctcp2 == FALSE)
            rv += '_' - 0x40;
         else
            rv = "\6U\6";
         break;

      case 'V':
         if (ctcp2 == FALSE)
            rv += 'V' - 0x40;
         else
            rv = "\6V\6";
         break;

      case 'N':
         if (ctcp2 == FALSE)
            rv += 'O' - 0x40;
         else
            rv = "\6N\6";
         break;

      case 'C':
         fgColor = "";
         bgColor = "";
         if (toupper(*cp) == 'X')
         {
            if (mircColors == TRUE)
               rv += 'C' - 0x40;
            else
               rv += "\6C\6";
            cp++;
         }
         else
         {
            if (*cp == '#')
            {
               for (x = 0; x < 7 && (ch = *(cp++)) != '\0'; x++)
                  fgColor += ch;
            }
            else
               fgColor += *(cp++);
            if (*cp == '#')
            {
               for (x = 0; x < 7 && (ch = *(cp++)) != '\0'; x++)
                  bgColor += ch;
            }
            else
               bgColor += *(cp++);
            if (dbg) fprintf(stderr, "xIrcmessageFrame::parseAttr():fgColor = |%s|\n", 
                             (const char*)fgColor.latin1());
            if (dbg) fprintf(stderr, "xIrcmessageFrame::parseAttr():bgColor = |%s|\n", 
                             (const char*)bgColor.latin1());
            if (dbg) fflush(stderr);

            if (mircColors == TRUE)
            {
               if (fgColor != "-" && fgColor != ".")
               {
                  const char *pFg = fgColor.latin1();
                  int fg;
                  QString f;
                  rv += 'C' - 0x40;
                  if ((fg = toupper(*pFg) - '0') > 9)
                     fg -= 7;
                  if (dbg) fprintf(stderr, "xIrcmessageFrame::parseAttr():fg = %d, |%s|\n", 
                                   fg, pFg);
                  rv += f.sprintf("%02d", fg);
               }
               if (fgColor != "-" && fgColor != "." && bgColor != "-")
               {
                  const char *pBg = bgColor.latin1();
                  int bg;
                  QString f;
                  if (*pBg != '.')
                  {
                     if ((bg = toupper(*pBg) - '0') > 9)
                        bg -= 7;
                     if (dbg) fprintf(stderr, "xIrcmessageFrame::parseAttr():bg = %d, |%s|\n", 
                                      bg, pBg);
                     rv += f.sprintf(",%02d", bg);
                  }
               }
            }
            else
               rv = "\6C" + fgColor + bgColor + "\6";
            fgColor = "";
            bgColor = "";
         }
         break;

      default:
         rv += *cp;
   }
   *p = cp;
   if (dbg) fprintf(stderr, "xIrcmessageFrame::parseAttr(%s):Exit, remaining line |%s|\n",
                    (const char*)rv.latin1(), cp);
   if (dbg) fflush(stderr);
   return(rv);
}

void setMircColors(xMultiLineWin *p, bool f)
{
   if (f)
      p->setColors((int*)mircColors);
   else
      p->setColors((int*)stdColors);
}
//#include "xIrcSocket.moc"
