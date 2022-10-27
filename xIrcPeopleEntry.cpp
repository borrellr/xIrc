/***************************************************************************
**    xIrcPeopleEntry.cpp  $Revision: 1.4 $ - $Name:  $ 
**    Builds an entry for the People list
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
#include <qt.h>
#include <qregexp.h>
#include <stdio.h>
#include "xIrcConnect.h"
#include "xIrcPeopleEntry.h"

static bool dbg = false;

extern xIrcCommands ircResponses;

xIrcPeopleEntry::xIrcPeopleEntry(xIrcMessage *pMsg, bool atAround)
{
   const char *cp, *cp1;

   if (pMsg->rspCode == 352)
   {
      const char *cp;

      /*
      ** Skip past the user's server name and the Irc server's name and
      ** any spaces in the middle (Namely get the 3rd field of msgStr
      */
      if (dbg) fprintf(stdout, "xIrcPeopleEntry::xIrcPeopleEntry():Gleening Nick from: |%s|\n",
                                (const char *)pMsg->msgStr.latin1());
      if (dbg) fflush(stdout);
      cp = pMsg->msgStr.latin1();
      cp1 = pMsg->dstStr.latin1();
      if (*cp1 == '#')
      {
         if (dbg) fprintf(stdout, "xIrcPeopleEntry::xIrcPeopleEntry():Skipping Username: |%s|\n",
                                  (const char *)cp);
         for (; *cp != ' '; cp++);
      }
      if (dbg) fprintf(stdout, "xIrcPeopleEntry::xIrcPeopleEntry():Skipping WS: |%s|\n",
         (const char *)cp);
      if (dbg) fflush(stdout);
      for (; *cp == ' '; cp++);
      if (dbg) fprintf(stdout, "xIrcPeopleEntry::xIrcPeopleEntry():Skipping Hostname: |%s|\n",
         (const char *)cp);
      if (dbg) fflush(stdout);
      for (; *cp != ' '; cp++);
      if (dbg) fprintf(stdout, "xIrcPeopleEntry::xIrcPeopleEntry():Skipping WS: |%s|\n",
         (const char *)cp);
      if (dbg) fflush(stdout);
      for (; *cp == ' '; cp++);
      if (dbg) fprintf(stdout, "xIrcPeopleEntry::xIrcPeopleEntry():Skipping Server name: |%s|\n",
         (const char *)cp);
      if (dbg) fflush(stdout);
      for (; *cp != ' '; cp++);
      if (dbg) fprintf(stdout, "xIrcPeopleEntry::xIrcPeopleEntry():Skipping WS: |%s|\n",
         (const char *)cp);
      if (dbg) fflush(stdout);
      for (; *cp == ' '; cp++);
      /*
      ** Assume that the 4th field is the nick
      */
      if (dbg) fprintf(stdout, "xIrcPeopleEntry::xIrcPeopleEntry():Nick: |%s|\n",
         (const char *)cp);
      if (dbg) fflush(stdout);
      for (; *cp != ' '; cp++)
         Nick += *cp;
      Mask = buildMask(pMsg, atAround);
   }
   else if (ircResponses.is(pMsg->rspCode, "NOTICE") ||
            ircResponses.is(pMsg->rspCode, "PRIVMSG"))
   {
      Nick = pMsg->srcNick;
      Mask = "";
      for (cp = pMsg->srcAddr.latin1(); *cp != '@'; cp++)
         Mask += *cp;
      if (atAround == TRUE)
         Mask += "@*";
      else
         Mask += " *";
      for (cp = pMsg->srcAddr.latin1(); *cp != '.'; cp++);
      for (cp = pMsg->srcAddr.latin1(); *cp; cp++);
         Mask += *cp;
   }
   Flag = 1;
   State = 0;
}

xIrcPeopleEntry::xIrcPeopleEntry(const char *pNick, const char *pMask, const char *pMsg,
                                 int flag)
{
   Nick = pNick;
   Mask = pMask;
   Message = pMsg;
   Flag = flag;
   State = 0;

   if (dbg) fprintf(stdout, "xIrcPeopleEntry::xIrcPeopleEntry():Nick = |%s|\n",
                             (const char *)Nick.latin1());
   if (dbg) fprintf(stdout, "xIrcPeopleEntry::xIrcPeopleEntry():Mask = |%s|\n",
                             (const char *)Mask.latin1());
   if (dbg) fprintf(stdout, "xIrcPeopleEntry::xIrcPeopleEntry():Message = |%s|\n",
                             (const char *)Message.latin1());
   if (dbg) fprintf(stdout, "xIrcPeopleEntry::xIrcPeopleEntry():Flag = |%d|\n",
                             Flag);
}

xIrcPeopleEntry::xIrcPeopleEntry(xIrcPeopleEntry &e)
{
   Mask = e.mask();
   Nick = e.nick();
   Message = e.message();
   Flag = e.flag();
   State = 0;
   if (dbg) fprintf(stdout, "xIrcPeopleEntry::xIrcPeopleEntry():Nick = |%s|\n",
                             (const char *)Nick.latin1());
   if (dbg) fprintf(stdout, "xIrcPeopleEntry::xIrcPeopleEntry():Mask = |%s|\n",
                             (const char *)Mask.latin1());
}

int xIrcPeopleEntry::compare(xIrcPeopleEntry *e)
{
   return(qstricmp(Nick.latin1(), e->nick().latin1()));
}

bool xIrcPeopleEntry::is(xIrcMessage *pMsg, int byAddr)
{
   bool rv = FALSE;

   if (dbg) fprintf(stdout, "xIrcPeopleEntry::is():Enter\n");
   if (dbg) fflush(stdout);
   if (pMsg->rspCode == 352)
   {
      if (dbg) fprintf(stdout, "xIrcPeopleEntry::is():Testing Who Message\n");
      if (dbg) fflush(stdout);
      if (byAddr >= 0)
      {
         QRegExp reMask(Mask, FALSE, TRUE);
         reMask.setCaseSensitive(FALSE);
         QString tmpMask(buildMask(pMsg));
         if (dbg) fprintf(stdout, "xIrcPeopleEntry::is():By Addr: |%s| == |%s|?\n",
                         (const char *)tmpMask.latin1(), (const char *)Mask.latin1());
         if (dbg) fflush(stdout);
         if (reMask.match(tmpMask) >= 0)
            rv = TRUE;
      }
      else
      {
         const char *cp;
         QString tmpNick;

         if (dbg) fprintf(stdout, "xIrcPeopleEntry::is():By nick\n");
         if (dbg) fflush(stdout);

         /*
         ** If the destination is a channel name, the 4th field of the message
         ** string has the nick
         */
         cp = pMsg->dstStr.latin1();
         if (*cp == '#')
         {
            for (cp = pMsg->msgStr.latin1(); *cp != ' '; cp++);
            for (; *cp == ' '; cp++);

            for (; *cp != ' '; cp++);
            for (; *cp == ' '; cp++);

            for (; *cp != ' '; cp++);
            for (; *cp == ' '; cp++);
         }

         /*
         ** Assume the cp points to the nick
         */
         for (; *cp != '\0' && *cp != ' '; cp++)
            tmpNick += *cp;
         if (dbg) fprintf(stdout, "xIrcPeopleEntry::is():By Nick: |%s| == |%s|?\n",
                         (const char *)tmpNick.latin1(), (const char *)Nick.latin1());
            if (qstricmp(tmpNick.latin1(), Nick.latin1()) == 0)
            rv = TRUE;
      }
   }
   else if (pMsg->rspCode == 315)
   {
      const char *cp;
      QString tmpMask;

      if (dbg) fprintf(stdout, "xIrcPeopleEntry::is():Testing End of Who Message\n");
      if (dbg) fflush(stdout);

      if (dbg) fprintf(stdout, "xIrcPeopleEntry::is():By nick\n");
      if (dbg) fflush(stdout);
      QString tmpNick;
      for (cp = pMsg->rawMsg.latin1(); *cp != ' '; cp++);
      for (; *cp == ' '; cp++);
      for (; *cp != ' '; cp++);
      for (; *cp == ' '; cp++);
      for (; *cp != ' '; cp++);
      for (; *cp == ' '; cp++);
      /*
      ** Assume that the 4th field is the nick
      */
      for (; *cp != ' '; cp++)
         tmpNick += toupper(*cp);

      if (dbg) fprintf(stdout, "xIrcPeopleEntry::is():By Nick: |%s| == |%s|?\n",
                      (const char *)tmpNick.latin1(), (const char *)Nick.latin1());
      if (byAddr >= 0)
      {
         tmpMask = Mask.upper();
         if (tmpMask.contains(tmpNick, FALSE) > 0)
            rv = TRUE;
      }
      else if (tmpNick == Nick)
         rv = TRUE;
   }
   else if (ircResponses.is(pMsg->rspCode, "NOTICE") ||
            ircResponses.is(pMsg->rspCode, "PRIVMSG"))
   {
      if (dbg) fprintf(stdout, "xIrcPeopleEntry::is():Testing Notice or PrivMsg\n");
      if (dbg) fflush(stdout);
      if (byAddr < 0)
      {
         if (dbg) fprintf(stdout, "xIrcPeopleEntry::is():By Nick: |%s| == |%s|?\n",
                         (const char *)pMsg->srcNick.latin1(), (const char *)Nick.latin1());
         if (dbg) fflush(stdout);
         if (qstricmp(Nick.latin1(), pMsg->srcNick.latin1()) == 0)
            rv = TRUE;
      }
      else
      {
         const char *cp, *cp1;

         cp = pMsg->srcAddr.latin1();
         cp1 = Mask.latin1();
         if (dbg) fprintf(stdout, "xIrcPeopleEntry::is():By Nick: |%s| == |%s|?\n",
                         cp, cp1);
         if (dbg) fflush(stdout);
         for (rv = FALSE; rv != TRUE;)
         {
            if (dbg) fprintf(stdout, "xIrcPeopleEntry::is():By Nick: |%c| == |%c|- ",
                            *cp, *cp1);
            if (dbg) fflush(stdout);
            if (*cp1 != '*')
            {
               if (dbg) fprintf(stdout, "Not in Mask");
               if (dbg) fflush(stdout);
               if (toupper(*cp) != toupper(*cp1))
                  break;
               else
               {
                  if (dbg) fprintf(stdout, " Matched!!\n");
                  if (dbg) fflush(stdout);
                  cp++;
                  cp1++;
               }
            }
            else
            {
               if (*cp != '.')
               {
                  if (dbg) fprintf(stdout, "xIrcPeopleEntry::is():Skipping\n");
                  if (dbg) fflush(stdout);
                  cp++;
               }
               else
               {
                  cp1++;
                  if (dbg) fprintf(stdout, "xIrcPeopleEntry::is():Proceding\n");
                  if (dbg) fflush(stdout);
               }
            }
            if (*cp1 == '\0')
               rv = TRUE;
         }
      }
   }   
   if (dbg) fprintf(stdout, "xIrcPeopleEntry::is():Exit(%d)\n", rv);
   if (dbg) fflush(stdout);
   return(rv);
}

QString xIrcPeopleEntry::buildMask(xIrcMessage *pMsg, bool atAround)
{
   QString rv;
   const char *cp;

   if (pMsg->rspCode == 352)
   {
      cp = pMsg->dstStr.latin1();
      if (*cp != '#')
      {
         rv += pMsg->dstStr.latin1();
         for (cp = pMsg->msgStr.latin1(); *cp == ' '; cp++);
      }
      else
      {
         for (cp = pMsg->msgStr.latin1(); *cp == ' '; cp++);
         for (; *cp != ' '; cp++)
            rv += *cp;
      }
      if (atAround == TRUE)
         rv += "@";
      else
         rv += " ";
      for (; *cp == ' '; cp++);
      if (isdigit(*cp))
      {
         for (; *cp != ' '; cp++)
            rv += *cp;
      }
      else
      {
         char ch = ' ';

         rv += "*";
         for (; *cp != '.'; cp++);
         for (; *cp != ' '; cp++)
         {
            if (isdigit(ch) && isdigit(*cp))
               continue;
            else if (isdigit(*cp))
            {
               ch = *cp;
               rv += '*';
               continue;
            }
            ch = *cp;
            rv += *cp;
         }
      }
   }
   else
      rv = "";
   return(rv);
}

