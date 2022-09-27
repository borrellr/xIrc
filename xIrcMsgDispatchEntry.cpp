/***************************************************************************
** xIrcDispatchEntry.h Message Dispatch Entry class used for keeping commands
** and thier responses going to the right widgets.
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
*/
#include <qt.h>
#include "ircreply.h"
#include "xIrcCommands.h"
#include "xIrcMsgDispatchEntry.h"

extern xIrcCommands ircResponses;

#ifdef VERSION
#undef VERSION
#endif

static int dbg = 0;

static int rspOper[] = { RPL_YOUREOPER, 0 };
static int rspMode[] = { RPL_CHANNELMODEIS, RPL_ENDOFBANLIST, 0 };
static int rspTopic[] = { RPL_TOPIC, RPL_NOTOPIC, 0 };
static int rspNames[] = { RPL_NAMEREPLY, RPL_ENDOFNAMES, 0 };
static int rspList[] = { RPL_LIST, RPL_LISTSTART, RPL_LISTEND, 0 };
static int rspInvite[] = { RPL_INVITING, RPL_AWAY, 0 };
static int rspVersion[] = { RPL_VERSION, 0 };
static int rspStats[] = { RPL_STATSCLINE, RPL_STATSILINE, RPL_STATSQLINE, RPL_STATSLINKINFO,
  RPL_STATSHLINE, RPL_STATSNLINE, RPL_STATSKLINE, RPL_STATSLLINE,
  RPL_STATSUPTIME, RPL_STATSOLINE, RPL_ENDOFSTATS, 0 };
static int rspLinks[] = { RPL_LINKS, RPL_ENDOFLINKS, 0 };
static int rspTime[] = { RPL_TIME, 0 };
static int rspTrace[] = { RPL_TRACECONNECTING, RPL_UNKNOWN, RPL_TRACEUSER, RPL_TRACESERVICE,
  RPL_TRACECLASS, RPL_TRACEHANDSHAKE, RPL_TRACEOPERATOR, RPL_TRACESERVER,
  RPL_TRACENEWTYPE, 0 };
static int rspAdmin[] = { RPL_ADMINME, RPL_ADMINLOC2, RPL_ADMINLOC1, RPL_ADMINEMAIL, 0 };
static int rspInfo[] = { RPL_INFO, RPL_ENDOFINFO, 0 };
static int rspPrivMsg[] = { RPL_AWAY, 0 };
static int rspNotice[] = { RPL_AWAY, 0 };
static int rspWho[] = { RPL_WHOREPLY, RPL_ENDOFWHO, 0 };
static int rspWhoIs[] = { RPL_WHOISUSER, RPL_WHOISCHANNELS, RPL_AWAY, RPL_WHOISIDLE, RPL_ENDOFWHOIS,
  RPL_WHOISSERVER, RPL_WHOISOPERATOR, 0 };
static int rspWhoWas[] = { RPL_WHOWASUSER, RPL_ENDOFWHOWAS, RPL_WHOISSERVER, 0 };
static int rspAway[] = { RPL_UNAWAY, RPL_UNAWAY, 0 };
static int rspRehash[] = { RPL_REHASHING, 0 };
static int rspSummoning[] = { RPL_SUMMONING, 0 };
static int rspUsers[] = { RPL_USERSTART, RPL_NOUSERS, RPL_USERS, 0 };
static int rspIson[] = { RPL_ISON, 0 };
static int rspMap[] = { RPL_MAP, RPL_ENDOFMAP, 0 };
static int rspSignOff[] = { RPL_SIGNOFF, 0 };

static int *pResponseList[] = 
{
   NULL, rspOper, rspMode, rspTopic, rspNames, rspList, rspInvite,
   rspVersion, rspStats, rspLinks, rspTime, rspTrace, rspAdmin,
   rspInfo, rspPrivMsg, rspNotice, rspWho, rspWhoIs, rspWhoWas,
   rspAway, rspRehash, rspSummoning, rspUsers, rspIson, rspMap,
   rspSignOff
};

typedef enum 
{
   OPER=1, MODE, TOPIC, NAMES, LIST, INVITE, VERSION, STATS, LINKS,
   TIME, TRACE, ADMIN, INFO, PRIVMSG, NOTICE, WHO, WHOIS, WHOWAS, AWAY, REHASH,
   SUMMONING, USERS, ISON, MAP
} IrcCommandRef;


typedef struct
{
   IrcCommandRef  cmdRef;
   char           *cmdName;
   int            *replies;
} IrcCommands;

static const IrcCommands cmdList[] =
{
   { OPER, "OPER", pResponseList[OPER] },
   { MODE, "MODE", pResponseList[MODE] },
   { TOPIC, "TOPIC", pResponseList[TOPIC] },
   { NAMES, "NAMES", pResponseList[NAMES] },
   { LIST, "LIST", pResponseList[LIST] },
   { INVITE, "INVITE", pResponseList[INVITE] },
   { VERSION, "VERSION", pResponseList[VERSION] },
   { STATS, "STATS", pResponseList[STATS] },
   { LINKS, "LINKS", pResponseList[LINKS] },
   { TIME, "TIME", pResponseList[TIME] },
   { TRACE, "TRACE", pResponseList[TRACE] },
   { ADMIN, "ADMIN", pResponseList[ADMIN] },
   { INFO, "INFO", pResponseList[INFO] },
   { PRIVMSG, "PRIVMSG", pResponseList[PRIVMSG] },
   { NOTICE, "NOTICE", pResponseList[NOTICE] },
   { WHO, "WHO", pResponseList[WHO] },
   { WHOIS, "WHOIS", pResponseList[WHOIS] },
   { WHOWAS, "WHOWAS", pResponseList[WHOWAS] },
   { AWAY, "AWAY", pResponseList[AWAY] },
   { REHASH, "REHASH", pResponseList[REHASH] },
   { SUMMONING, "SUMMONING", pResponseList[SUMMONING] },
   { USERS, "USERS", pResponseList[USERS] },
   { ISON, "ISON", pResponseList[ISON] },
   { MAP, "MAP", pResponseList[MAP] },
   { (IrcCommandRef)0, NULL, NULL }
};

typedef struct
{
   int            rspCode;
   bool           terminating;
   bool           display;
   char           *parseStr;
} IrcResponseParse;

static const IrcResponseParse responseParseStrings[] =
{
   { RPL_ISON, FALSE, TRUE, "*: %n+" },
   { RPL_AWAY, FALSE, TRUE, "%n *" },
   { RPL_WHOISUSER, FALSE, TRUE, "%n *" },
   { RPL_WHOISSERVER, FALSE, TRUE, "%n *" },
   { RPL_WHOISOPERATOR, FALSE, TRUE, "%n *" },
   { RPL_WHOISIDLE, FALSE, TRUE, "%n *" },
   { RPL_WHOISCHANNELS, FALSE, TRUE, "%n *" },
   { RPL_WHOWASUSER, FALSE, TRUE, "%n *" },
   { RPL_ENDOFWHOIS, TRUE, FALSE, "*" },
   { RPL_ENDOFWHOWAS, TRUE, FALSE, "*" },
   { RPL_LISTSTART, FALSE, TRUE, "%c *" },
   { RPL_LIST, FALSE, TRUE, "%c *" },
   { RPL_LISTEND, TRUE, FALSE, "*" },
   { RPL_NOTOPIC, TRUE, TRUE, "%c *" },
   { RPL_TOPIC, TRUE, TRUE, "%c *" },
   { RPL_INVITING, TRUE, TRUE, "%c %n" },
   { RPL_SUMMONING, TRUE, TRUE, "%u *" },
   { RPL_VERSION, FALSE, TRUE, "*" },
   { RPL_WHOREPLY, FALSE, TRUE, "%c %u %h %s %n *" },
   { RPL_ENDOFWHO, TRUE, FALSE, "*" },
   { RPL_NAMEREPLY, FALSE, TRUE, "%c *" },
   { RPL_ENDOFNAMES, TRUE, FALSE, "%c *" },
   { RPL_LINKS, FALSE, TRUE, "*" },
   { RPL_ENDOFLINKS, TRUE, FALSE, "*" },
   { RPL_BANLIST, FALSE, TRUE, "%c *" },
   { RPL_ENDOFBANLIST, TRUE, FALSE, "%c *" },
   { RPL_INFO, FALSE, TRUE, "*" },
   { RPL_ENDOFINFO, TRUE, FALSE, "*" },
   { RPL_MOTDSTART, FALSE, TRUE, "*" },
   { RPL_MOTD, FALSE, TRUE, "*" },
   { RPL_ENDOFMOTD, TRUE, FALSE, "*" },
   { RPL_YOUREOPER, TRUE, TRUE, "*" },
   { RPL_REHASHING, TRUE, TRUE, "*" },
   { RPL_TIME, TRUE, TRUE, "*" },
   { RPL_USERSTART, FALSE, TRUE, "*" },
   { RPL_USERS, FALSE, TRUE, "*" },
   { RPL_ENDOFUSERS, TRUE, FALSE, "*" },
   { RPL_NOUSERS, FALSE, TRUE, "*" },
   { RPL_TRACELINK, FALSE, TRUE, "*" },
   { RPL_TRACECONNECTING, FALSE, TRUE, "*" },
   { RPL_TRACEHANDSHAKE, FALSE, TRUE, "*" },
   { RPL_UNKNOWN, FALSE, TRUE, "*" },
   { RPL_TRACEOPERATOR, FALSE, TRUE, "*" },
   { RPL_TRACEUSER, FALSE, TRUE, "*" },
   { RPL_TRACESERVER, FALSE, TRUE, "*" },
   { RPL_TRACENEWTYPE, FALSE, TRUE, "*" },
   { RPL_TRACELOG, FALSE, TRUE, "*" },
   { RPL_STATSLINKINFO, FALSE, TRUE, "*" },
   { RPL_STATSCOMMANDS, FALSE, TRUE, "*" },
   { RPL_STATSCLINE, FALSE, TRUE, "*" },
   { RPL_STATSILINE, FALSE, TRUE, "*" },
   { RPL_STATSKLINE, FALSE, TRUE, "*" },
   { RPL_STATSYLINE, FALSE, TRUE, "*" },
   { RPL_ENDOFSTATS, TRUE, FALSE, "*" },
   { RPL_STATSLLINE, FALSE, TRUE, "*" },
   { RPL_STATSUPTIME, FALSE, TRUE, "*" },
   { RPL_STATSOLINE, FALSE, TRUE, "*" },
   { RPL_STATSHLINE, FALSE, TRUE, "*" },
   { RPL_STATSUMODEIS, FALSE, TRUE, "*" },
   { RPL_LUSERCLIENT, FALSE, TRUE, "*" },
   { RPL_LUSEROP, FALSE, TRUE, "*" },
   { RPL_LUSERUNKNOWN, FALSE, TRUE, "*" },
   { RPL_LUSERCHANNELS, FALSE, TRUE, "*" },
   { RPL_LUSERME, FALSE, TRUE, "*" },
   { RPL_ADMINME, FALSE, TRUE, "*" },
   { RPL_ADMINLOC1, FALSE, TRUE, "*" },
   { RPL_ADMINLOC2, FALSE, TRUE, "*" },
   { RPL_ADMINEMAIL, FALSE, TRUE, "*" },
   { RPL_MAP, FALSE, TRUE, "*" },
   { RPL_ENDOFMAP, TRUE, FALSE, "*" },
   { RPL_SIGNOFF, FALSE, TRUE, "*" },
   { 0, 0, 0, NULL }
};

xIrcMsgDispatchEntry::xIrcMsgDispatchEntry(QObject *pObj, const char *pSlot, 
                                           xIrcMessage *pMsg, int &accepted) :
                        QObject(NULL, NULL)
{
   const char *response;
   const IrcCommands *pCmdList;

   if (dbg) fprintf(stdout, "xIrcMsgDispatchEntry::xIrcMsgDispatchEntry():Enter\n");
   if (dbg) fflush(stdout);
   pObject = pObj;
   slot = pSlot;

   msg.fd      = pMsg->fd;
   msg.pmsgTyp = pMsg->pmsgTyp;
   msg.rspCode = pMsg->rspCode;
   msg.rspStr  = pMsg->rspStr;
   msg.srcNick = pMsg->srcNick;
   msg.srcAddr = pMsg->srcAddr;
   msg.dstStr  = pMsg->dstStr;
   msg.msgStr  = pMsg->msgStr;
   msg.rawMsg  = pMsg->rawMsg;

   if (dbg) fprintf(stdout, "xIrcMsgDispatchEntry::xIrcMsgDispatchEntry():rspCode = %d\n",
                      msg.rspCode);
   if (dbg) fflush(stdout);
   sentFlag = FALSE;

   pResponseList = NULL;
   response = ircResponses.name(pMsg->rspCode);
   if (dbg) fprintf(stdout, "xIrcMsgDispatchEntry::xIrcMsgDispatchEntry():response = |%s|\n",
                           response);
   if (dbg) fflush(stdout);
   for (pCmdList = cmdList; pCmdList->cmdName; pCmdList++)
   {
      if (dbg) fprintf(stdout, "xIrcMsgDispatchEntry::xIrcMsgDispatchEntry():Testing against |%s|\n",
                                 pCmdList->cmdName);
      if (dbg) fflush(stdout);
      if (strcmp(response, pCmdList->cmdName) == 0)
      {
         if (dbg) fprintf(stdout, "xIrcMsgDispatchEntry::xIrcMsgDispatchEntry():Have Match!\n");
         if (dbg) fflush(stdout);
         pResponseList = pCmdList->replies;
         if (dbg) 
         {
            fprintf(stdout, "xIrcMsgDispatchEntry::xIrcMsgDispatchEntry():Responses = |");
            for (int *p = pResponseList; *p != 0; p++)
               fprintf(stdout, "%d, ", *p);
            fprintf(stdout, "|\n");
            if (dbg) fflush(stdout);
         }
         break;
      }
   }
   if (pResponseList)
   {
      if (dbg) fprintf(stdout, "xIrcMsgDispatchEntry::xIrcMsgDispatchEntry():Have Response List!\n");
      if (dbg) fflush(stdout);
      if (pCmdList->cmdRef != PRIVMSG && pCmdList->cmdRef != NOTICE)
      {
         if (dbg) fprintf(stdout, "xIrcMsgDispatchEntry::xIrcMsgDispatchEntry():Not privmsg or notice!\n");
         if (dbg) fflush(stdout);
         accepted = 1;
         if (pObject != NULL)
            connect(this, SIGNAL(gotResponse(xIrcMessage*)), pObject, (const char*)slot);
      }
      else
         accepted = 0;
   }
   else
      accepted = -1;
   if (dbg) fprintf(stdout, "xIrcMsgDispatchEntry::xIrcMsgDispatchEntry():Exit- Accepted = %d\n",
                           accepted);
   if (dbg) fflush(stdout);
}

xIrcMsgDispatchEntry::~xIrcMsgDispatchEntry()
{
}

bool xIrcMsgDispatchEntry::is(xIrcMessage *pMsg)
{
   int *ip;

   if (dbg) fprintf(stdout, "xIrcMsgDispatchEntry::is():Enter- Response Code = %d\n",
                            pMsg->rspCode);
   if (dbg) fflush(stdout);
   for (ip = pResponseList; *ip; ip++)
   {
      if (dbg) fprintf(stdout, "xIrcMsgDispatchEntry::is():Testing against %d\n", *ip);
      if (dbg) fflush(stdout);
      if (pMsg->rspCode == *ip)
      {
         if (dbg) fprintf(stdout, "xIrcMsgDispatchEntry::is():Have Match!\n");
         if (dbg) fflush(stdout);
         break;
      }
   }
   if (dbg) fprintf(stdout, "xIrcMsgDispatchEntry::is():Exit\n");
   if (dbg) fflush(stdout);
   return(*ip == 0 ? FALSE : TRUE);
}

bool xIrcMsgDispatchEntry::dispatchResponse(xIrcMessage *pMsg)
{
   bool rv = TRUE;

   for (const IrcResponseParse *pRsp = responseParseStrings; pRsp->rspCode != 0; pRsp++) {
      if (pRsp->rspCode == pMsg->rspCode)
      {
//         if (pRsp->display)
         emit gotResponse(pMsg);
         if (pRsp->terminating)
            rv = FALSE;
      }
   }
   return(rv);
}
