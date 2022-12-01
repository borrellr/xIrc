/***************************************************************************
**    xIrcCommands.cpp  $Revision: 1.1.1.1 $ - $Name:  $ 
**    Class for handling IRC Commands
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
#include "strings.h"
#include "ctype.h"
#include "xIrcCommands.h"

xIrcCommands ircResponses;

typedef struct
{
   int      code;
   char     *text;
} xIrcResponse;

typedef struct
{
   char     *cmd;
   int      *usrFlag;
   char     *text;
} xIrcCmd;

static xIrcResponse rspList[] =
{
   { 301, (const char *)"Away: " },
   { 324, (const char *)"Channel Mode: " },
   { 331, (const char *)"" },
   { 332, (const char *)"Topic: " },
   { 333, (const char *)"Channel Creator: " },
   { 341, (const char *)"Inviting: " },
   { 353, (const char *)"Channel Members: " },
   { 367, (const char *)"Banned: " },
   { 0, (const char *)"" }
};

//
// This is an array of Response strings that can be recieved
// from the server. It must stay in synch with the values set
// in the include file!!
//
static const char *commandList[] = 
{
   (char *)"AWAY",
   (char *)"INVITE",
   (char *)"JOIN",
   (char *)"KICK",
   (char *)"LIST",
   (char *)"MODE",
   (char *)"NAMES",
   (char *)"NICK",
   (char *)"NOTICE",
   (char *)"PART",
   (char *)"PING",
   (char *)"PONG",
   (char *)"PRIVMSG",
   (char *)"QUIT",
   (char *)"TOPIC",
   (char *)"VERSION",
   (char *)"WHO",
   (char *)"WHOIS",
   (char *)"WHOWAS",
   (char *)"FLUSH",
   (char *)"NOTE",
   (char *)"LINKS",
   (char *)"MAP",
   NULL
};

static char emptyStr[] = "";
static QString tmpStr;

xIrcCommands::xIrcCommands()
{
   commandList = ::commandList;
}

int xIrcCommands::code(const char *pCmd)
{
   char buf[20], *cp;
   int x, rv;
   
   for (x = 0, cp = buf; *pCmd && x < 20; x++, pCmd++)
   {
      *(cp++) = toupper(*pCmd);
      *cp = '\0';
   }
   
   for (x = 0; commandList[x] != NULL; x++)
   {
      if (strcmp(commandList[x], buf) == 0)
         break;
   }
   if (commandList[x] != NULL)
      rv = x + IRCRESP_First;
   else
      rv = IRCRESP_Unknown;
   return(rv);
}

char *xIrcCommands::name(int cmd)
{
   char *rv;
   int x;
   
   if ((cmd -= IRCRESP_First) >= 0)
   {
      for (x = 0; commandList[x] != NULL && x < cmd; x++);
   
      if (x == cmd)
         rv = commandList[x];
      else
         rv = emptyStr;
   }
   else
      rv = emptyStr;
   return(rv);
}

const char *xIrcCommands::text(int cmd)
{
   const char *rv;
   char buf[80];
   int x;

   for (x = 0; rspList[x].code != cmd && rspList[x].code != 0; x++);
   if (rspList[x].code != 0)
      rv = rspList[x].text;
   else
   {
      sprintf(buf, "%d::", cmd);
      tmpStr = buf;
      rv = tmpStr.latin1();
   }
   return(rv);
}
