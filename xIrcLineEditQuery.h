/***************************************************************************
**    xIrcLineEditQuery.h  $Revision: 1.2 $ - $Name:  $ 
**    LineEdit with Label, Buttons and Irc translations
**
** Copyright (C) 1997 1998 Joseph Croft <joe@croftj.net>
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
#ifndef _xIRCLINEEDITQUERY_H
#define _xIRCLINEEDITQUERY_H

#include <qstring.h>
#include <xLineEditQuery.h>
#include "xIrcSocket.h"

class xIrcLineEditQuery : public xLineEditQuery
{
public:
   xIrcLineEditQuery(xWidgetResInfo *pPRes, QWidget *pParent = NULL, 
                  const char *pName = NULL,
                  const char *pTag = NULL, xDefaults *pDef = NULL) :
      xLineEditQuery(pPRes, pParent, pName, pTag, pDef) {};

   xIrcLineEditQuery(xWidgetResInfo *pPRes, const char *pText = NULL,
                  const char *pEdtText = NULL, 
                  QWidget *pParent = NULL, const char *pName = NULL,
                  const char *pTag = NULL, xDefaults *pDef = NULL) :
      xLineEditQuery(pPRes, pText, pEdtText, pParent, pName, pTag, pDef) {};

   const char *text() {
      s = parseLine(xLineEditQuery::text(), TRUE, FALSE);
      return(s.isNull() ? "" : (const char *)s); 
   };

private:
   QString  s;
};

#endif
