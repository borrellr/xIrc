/***************************************************************************
**    xIrcServerEntry.h  $Revision: 1.1.1.1 $ - $Name:  $ 
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
#ifndef XIRCSERVERENTRY_H
#define XIRCSERVERENTRY_H

#include <qstring.h>

class xIrcServerEntry
{
public:
   xIrcServerEntry(char *p);
   xIrcServerEntry(const char *group, const char *country, const char *state,
                   const char *city, const char *server, const char *ports);
   xIrcServerEntry(xIrcServerEntry &e);
   
   QString server() { return(Server); };
   QString group() { return(Group); };
   QString country() { return(Country); };
   QString state() { return(State); };
   QString city() { return(City); };
   QString ports() { return(Ports); };
   QString id() { return(Id); };

   void setGroup(const char *s) { Group = s; };
   void setCountry(const char *s) { Country = s; };
   void setState(const char *s) { State = s; };
   void setCity(const char *s) { City = s; };
   void setServer(const char *s) { Server = s; };
   void setPorts(const char *s) { Ports = s; };

   bool isCountry(QString &s) { return((s == Country) ? TRUE : FALSE); };
   bool isState(QString &s) { return((s == State) ? TRUE : FALSE); };
   bool isCity(QString &s) { return((s == City) ? TRUE : FALSE); };
   bool isId(QString &s) { return((s == Id) ? TRUE : FALSE); };
   int compare(xIrcServerEntry *e);

private:
   char *parseId(char *p, QString &s);
   char *parseGroup(char *p, QString &s);
   char *parseCountry(char *p, QString &s);
   char *parseState(char *p, QString &s);
   char *parseCity(char *p, QString &s);
   char *parseServer(char *p, QString &s);
   char *parsePorts(char *p, QString &s);

   QString     Id;
   QString     Country;
   QString     State;
   QString     City;
   QString     Server;
   QString     Ports;
   QString     Group;
};

#endif
