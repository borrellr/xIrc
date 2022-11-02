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
#include <qt.h>
#include "xIrcServerEntry.h"

static bool dbg = false;

xIrcServerEntry::xIrcServerEntry()
{
}

xIrcServerEntry::xIrcServerEntry(char *p)
{
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
   if (dbg) fprintf(stdout, "xIrcServerEntry::xIrcServerEntry():Id = |%s|\n",
                             (const char *)Id.latin1());
   if (dbg) fprintf(stdout, "xIrcServerEntry::xIrcServerEntry():Group = |%s|\n",
                             (const char *)Group.latin1());
   if (dbg) fprintf(stdout, "xIrcServerEntry::xIrcServerEntry():Country = |%s|\n",
                             (const char *)Country.latin1());
   if (dbg) fprintf(stdout, "xIrcServerEntry::xIrcServerEntry():State = |%s|\n",
                             (const char *)State.latin1());
   if (dbg) fprintf(stdout, "xIrcServerEntry::xIrcServerEntry():City = |%s|\n",
                             (const char *)City.latin1());
   if (dbg) fprintf(stdout, "xIrcServerEntry::xIrcServerEntry():Server = |%s|\n", 
                             (const char *)Server.latin1());
   if (dbg) fprintf(stdout, "xIrcServerEntry::xIrcServerEntry():Ports = |%s|\n", 
                             (const char *)Ports.latin1());
   if (dbg) fflush(stdout);
}

xIrcServerEntry::xIrcServerEntry(const char *group, const char *country, const char *state,
                                 const char *city, const char *server, const char *ports)
{
   QString Group(group), Country(country), State(state), City(city),
        Server(server), Ports(ports);
   xIrcServerEntry(Group, Country, State, City, Server, Ports);
}

xIrcServerEntry::xIrcServerEntry(const QString &group, const QString &country,
     const QString &state, const QString &city, const QString &server,
     const QString &ports)
{
   Group = group;
   Country = country;
   State = state;
   City = city;
   Server = server;
   Ports = ports;
   if (dbg) fprintf(stdout, "xIrcServerEntry::xIrcServerEntry():Id = |%s|\n",
                             (const char *)Id.latin1());
   if (dbg) fprintf(stdout, "xIrcServerEntry::xIrcServerEntry():Group = |%s|\n",
                             (const char *)Group.latin1());
   if (dbg) fprintf(stdout, "xIrcServerEntry::xIrcServerEntry():Country = |%s|\n",
                             (const char *)Country.latin1());
   if (dbg) fprintf(stdout, "xIrcServerEntry::xIrcServerEntry():State = |%s|\n",
                             (const char *)State.latin1());
   if (dbg) fprintf(stdout, "xIrcServerEntry::xIrcServerEntry():City = |%s|\n",
                             (const char *)City.latin1());
   if (dbg) fprintf(stdout, "xIrcServerEntry::xIrcServerEntry():Server = |%s|\n", 
                             (const char *)Server.latin1());
   if (dbg) fprintf(stdout, "xIrcServerEntry::xIrcServerEntry():Ports = |%s|\n", 
                             (const char *)Ports.latin1());
   if (dbg) fflush(stdout);
}

xIrcServerEntry::xIrcServerEntry(xIrcServerEntry &e)
{
   Group = e.group();
   Country = e.country();
   State = e.state();
   City = e.city();
   Server = e.server();
   Ports = e.ports();
   if (Group.isEmpty())
      Group = "";
   if (Country.isEmpty())
      Country = "";
   if (State.isEmpty())
      State = "";
   if (City.isEmpty())
      City = "";
   if (Server.isEmpty())
      Server = "";
   if (Ports.isEmpty())
      Ports = "";

   if (dbg) fprintf(stdout, "xIrcServerEntry::xIrcServerEntry():Id = |%s|\n",
                             (const char *)Id.latin1());
   if (dbg) fprintf(stdout, "xIrcServerEntry::xIrcServerEntry():Group = |%s|\n",
                             (const char *)Group.latin1());
   if (dbg) fprintf(stdout, "xIrcServerEntry::xIrcServerEntry():Country = |%s|\n",
                             (const char *)Country.latin1());
   if (dbg) fprintf(stdout, "xIrcServerEntry::xIrcServerEntry():State = |%s|\n",
                             (const char *)State.latin1());
   if (dbg) fprintf(stdout, "xIrcServerEntry::xIrcServerEntry():City = |%s|\n",
                             (const char *)City.latin1());
   if (dbg) fprintf(stdout, "xIrcServerEntry::xIrcServerEntry():Server = |%s|\n", 
                             (const char *)Server.latin1());
   if (dbg) fprintf(stdout, "xIrcServerEntry::xIrcServerEntry():Ports = |%s|\n", 
                             (const char *)Ports.latin1());
   if (dbg) fflush(stdout);
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
   if (dbg) fprintf(stdout, "xIrcServerEntry::parseState():s = |%s|\n", (const char *)s.latin1());
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

bool xIrcServerEntry::compare(xIrcServerEntry *e)
{
   if (Group == e->Group)
   {
      if (Country == e->Country)
      {
         if (State == e->State)
            return(City == e->City);
         else 
            return(State == e->State);
      }
      else
         return(Country == e->Country);
   }
   return(Group == e->Group);
}

void xIrcServerEntry::showEntries()
{
   printf ("     Id is |%s|\n", Id.latin1());
   printf ("  Group is |%s|\n", Group.latin1());
   printf ("Country is |%s|\n", Country.latin1());
   printf ("  State is |%s|\n", State.latin1());
   printf ("   City is |%s|\n", City.latin1());
   printf (" Server is |%s|\n", Server.latin1());
   printf ("  Ports is |%s|\n", Ports.latin1());
}
