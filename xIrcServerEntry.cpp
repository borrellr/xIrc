/***************************************************************************
**    xIrcServerEntry.cpp  $Revision: 1.2 $ - $Name:  $ 
**    Parses and builds an entry from a mIrc "servers.ini" file
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
#include <qstringlist.h>
#include "xIrcServerEntry.h"

static int dbg = 1;

xIrcServerEntry::xIrcServerEntry()
{
    if(dbg) printf("Entering xIrcServerEntry::xIrcServerEntry(no args)\n");

    Id = "*";
    Group = "*";
    Country = "*";
    State = "*";
    City = "*";
    Server = "*";
    Ports = "6667";

    if(dbg) printf("Exiting xIrcServerEntry::xIrcServerEntry(no args)\n");
}

xIrcServerEntry::xIrcServerEntry(char *p)
{
   if(dbg) printf("Entering xIrcServerEntry::xIrcServerEntry(arg)\n");

   p = parseId(p, Id);
   p = parseGroup(p, Group);
   p = parseCountry(p, Country);
   p = parseState(p, State);
   p = parseCity(p, City);
   if (City.isEmpty())
   {
      City = State;
      State = "";
   }
   p = parseServer(p, Server);
   p = parsePorts(p, Ports);

   if (dbg) {
      printf("xIrcServerEntry::xIrcServerEntry():Id = |%s|\n", (const char *)Id);
      printf("xIrcServerEntry::xIrcServerEntry():Group = |%s|\n", (const char *)Group);
      printf("xIrcServerEntry::xIrcServerEntry():Country = |%s|\n", (const char *)Country);
      printf("xIrcServerEntry::xIrcServerEntry():State = |%s|\n", (const char *)State);
      printf("xIrcServerEntry::xIrcServerEntry():City = |%s|\n", (const char *)City);
      printf("xIrcServerEntry::xIrcServerEntry():Server = |%s|\n", (const char *)Server);
      printf("xIrcServerEntry::xIrcServerEntry():Ports = |%s|\n", (const char *)Ports);
      printf("Exiting xIrcServerEntry::xIrcServerEntry(arg)\n") ;
   }
}

xIrcServerEntry::xIrcServerEntry(const char *group, const char *country, const char *state,
                                 const char *city, const char *server, const char *ports)
{
   if(dbg) printf("Entering xIrcServerEntry::xIrcServerEntry(many args)\n");

   Group = group;
   Country = country;
   State = state;
   City = city;
   Server = server;
   Ports = ports;
   if (dbg) {
      printf("xIrcServerEntry::xIrcServerEntry():Id = |%s|\n", (const char *)Id);
      printf("xIrcServerEntry::xIrcServerEntry():Group = |%s|\n", (const char *)Group);
      printf("xIrcServerEntry::xIrcServerEntry():Country = |%s|\n", (const char *)Country);
      printf("xIrcServerEntry::xIrcServerEntry():State = |%s|\n", (const char *)State);
      printf("xIrcServerEntry::xIrcServerEntry():City = |%s|\n", (const char *)City);
      printf("xIrcServerEntry::xIrcServerEntry():Server = |%s|\n", (const char *)Server);
      printf("xIrcServerEntry::xIrcServerEntry():Ports = |%s|\n", (const char *)Ports);
      printf("Entering xIrcServerEntry::xIrcServerEntry(many args)\n");
   }
}

xIrcServerEntry::xIrcServerEntry(xIrcServerEntry &e)
{
   if(dbg) printf("Entering xIrcServerEntry::xIrcServerEntry(struct)\n");

   Group = e.group();
   Country = e.country();
   State = e.state();
   City = e.city();
   Server = e.server();
   Ports = e.ports();
   if (strlen(Group) == 0)
      Group = "";
   if (strlen(Country) == 0)
      Country = "";
   if (strlen(State) == 0)
      State = "";
   if (strlen(City) == 0)
      City = "";
   if (strlen(Server) == 0)
      Server = "";
   if (strlen(Ports) == 0)
      Ports = "";

   if (dbg) {
      printf("xIrcServerEntry::xIrcServerEntry():Id = |%s|\n", (const char *)Id);
      printf("xIrcServerEntry::xIrcServerEntry():Group = |%s|\n", (const char *)Group);
      printf("xIrcServerEntry::xIrcServerEntry():Country = |%s|\n", (const char *)Country);
      printf("xIrcServerEntry::xIrcServerEntry():State = |%s|\n", (const char *)State);
      printf("xIrcServerEntry::xIrcServerEntry():City = |%s|\n", (const char *)City);
      printf("xIrcServerEntry::xIrcServerEntry():Server = |%s|\n", (const char *)Server);
      printf("xIrcServerEntry::xIrcServerEntry():Ports = |%s|\n", (const char *)Ports);
      printf("Entering xIrcServerEntry::xIrcServerEntry(struct)\n");
   }
}

xIrcServerEntry::xIrcServerEntry(QString &list)
{
   if(dbg) printf("Entering xIrcServerEntry::xIrcServerEntry(qstring)\n");

    QStringList lines = QStringList::split( ":", list, TRUE);
    Group = lines.first();
    lines.pop_front();
    Country = lines.first();
    lines.pop_front();
    State = lines.first();
    lines.pop_front();
    City = lines.first();
    lines.pop_front();
    Server = lines.first();
    lines.pop_front();
    Ports = lines.first();

   if (dbg) {
      printf("xIrcServerEntry::xIrcServerEntry():Id = |%s|\n", (const char *)Id);
      printf("xIrcServerEntry::xIrcServerEntry():Group = |%s|\n", (const char *)Group);
      printf("xIrcServerEntry::xIrcServerEntry():Country = |%s|\n", (const char *)Country);
      printf("xIrcServerEntry::xIrcServerEntry():State = |%s|\n", (const char *)State);
      printf("xIrcServerEntry::xIrcServerEntry():City = |%s|\n", (const char *)City);
      printf("xIrcServerEntry::xIrcServerEntry():Server = |%s|\n", (const char *)Server);
      printf("xIrcServerEntry::xIrcServerEntry():Ports = |%s|\n", (const char *)Ports);
      printf("Exiting xIrcServerEntry::xIrcServerEntry(qstring)\n");
   }
}

char *xIrcServerEntry::parseId(char *p, QString &s)
{
   s = "";
   for (;*p != '=' && *p != '\0'; p++)
      s += *p;
   if (p != '\0')
      p++;
   return(p);
}

char *xIrcServerEntry::parseGroup(char *p, QString &s)
{
   s = "";
   for (;*p != ':' && *p != '\0'; p++)
      s += *p;
   while (*p != '\0' && (*p == ':' || isspace(*p)))
      p++;
   return(p);
}

char *xIrcServerEntry::parseCountry(char *p, QString &s)
{
   s = "";
   for (;*p != ',' && *p != '\0'; p++)
      s += *p;
   while (*p != '\0' && (*p == ',' || isspace(*p)))
      p++;
   return(p);
}

char *xIrcServerEntry::parseState(char *p, QString &s)
{
   char *p1;

   s = "";
   p1 = strstr((const char *)p, "SERVER:");
   if (dbg) fprintf(stdout, "xIrcServerEntry::parseState():p = |%s|\n", p);
   if (dbg) fprintf(stdout, "xIrcServerEntry::parseState():strstr = |%s|\n", p1);
   if (dbg) fflush(stdout);
   for (;*p != ',' && *p != '\0' && p != p1; p++)
      s += *p;
   if (dbg) fprintf(stdout, "xIrcServerEntry::parseState():s = |%s|\n", (const char *)s);
   if (dbg) fflush(stdout);
   if (p != p1)
   {
      while (*p != '\0' && (*p == ',' || isspace(*p)))
         p++;
   }
   return(p);
}

char *xIrcServerEntry::parseCity(char *p, QString &s)
{
   char *p1;

   s = "";
   p1 = strstr((const char *)p, "SERVER:");
   if (p1 != NULL)
   {
      for (;*p != '\0' && p != p1; p++)
         s += *p;
      if (p == p1)
      {
         for (;*p != ':' && *p != '\0'; p++)
            ;
         if (*p != '\0')
            p++;
      }
   }
   return(p);
}

char *xIrcServerEntry::parseServer(char *p, QString &s)
{
   s = "";
   for (;*p != ':' && *p != '\0'; p++)
      s += *p;
   while (*p != '\0' && (*p == ':' || isspace(*p)))
      p++;
   return(p);
}

char *xIrcServerEntry::parsePorts(char *p, QString &s)
{
   s = "";
   for (;*p != 'G' && *p != '\0'; p++)
      s += *p;
   while (*p != '\0' && (*p == ':' || isspace(*p)))
      p++;
   return(p);
}

int xIrcServerEntry::compare(xIrcServerEntry *e)
{
   if (Group == e->Group)
   {
      if (Country == e->Country)
      {
         if (State == e->State)
            return(strcmp(City, e->City));
         else 
            return(strcmp(State, e->State));
      }
      else
         return(strcmp(Country, e->Country));
   }
   return(strcmp(Group, e->Group));
}

