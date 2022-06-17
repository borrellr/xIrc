/***************************************************************************
**    xMisc.h  $Revision: 1.1.1.1 $ - $Name:  $ 
**    Misc Utility routines
**
**    Copyright (C) 1997 1998 Joseph Croft <joe@croftj.net>
**    Copyright (C) 1996 Joseph Croft <jcroft@unicomp.net>
**
**    This library is free software; you can redistribute it and/or
**    modify it under the terms of the GNU Library General Public
**    License as published by the Free Software Foundation; either
**    version 2 of the License, or (at your option) any later version.
**
**    This library is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**    Library General Public License for more details.
**
**    You should have received a copy of the GNU Library General Public
**    License along with this library; if not, write to the Free
**    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
 ***************************************************************************/

#ifndef _XMISC_H
#define _XMISC_H

#include <qcolor.h>
#include <qfont.h>
#include <qlist.h>
#include <qstring.h>

class xWidgetResInfo
{
public:
   xWidgetResInfo(xWidgetResInfo *pRes);
   xWidgetResInfo(xWidgetResInfo &res);
   xWidgetResInfo(xWidgetResInfo *pPRes, QString _nameStr, QString _classStr);
   QString getName() { return(nameStr); };
   QString getClass() { return(classStr); };

private:
   QString nameStr;
   QString classStr;
};

char strtoc(const char *cp, const char **cpp);
void setStrColor(QColor &color, const char *pColorStr);
void setStrFont(QFont &font, const char *pFamily, const char *pSize = NULL, 
        const char *pWeight = NULL);
QFont setStrFont(const char *pFamily, const char *pSize, 
        const char *pWeight);

void buildList(QList<QString> &list, const char *pTests);
bool testString(QList<QString> &list, QString &str);
bool inList(QList<QString> &list, QString string);

void setDefPallet(QWidget *pWdt, xWidgetResInfo *wdtRes);
void setDefFont(QWidget *pWdt, xWidgetResInfo *wdtRes);

#endif
