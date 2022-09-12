/***************************************************************************
**    xDefaults.h  $Revision: 1.1.1.1 $ - $Name:  $ 
**    Reads Defaults file
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
#ifndef _xDDEFAULTS_H
#define _xDDEFAULTS_H

#include <stdio.h>
#include <qstring.h>
#include <qptrlist.h>

class xDefaults;
class xDefltEntry;

class xDefltEntry
{
public:
   xDefltEntry(QString &_strTag, QString &_strVal)
   {
      strTag = _strTag.upper();
      strVal = _strVal;
   }
   xDefltEntry()
   {
      strTag = "";
      strVal = "";
   }
   friend class xDefaults;

private:
   QString  strTag;
   QString  strVal;
};

typedef const char *(*CallBack)(QString);

class xDefaults
{
public:
   xDefaults();
   ~xDefaults() {};

   QString     expandEntry(QString &strVal);
   xDefltEntry *findEntry(QString &strTag);
   const char  *findEntry(const char *Tag);
   void        add(const char *pStr);
   void        add(QString &strTag, QString &strVal);
   void        set(QString strTag, QString strVal);
   void        set(const char *pStr);
   void        set(const char *pTag, const char *pStr);
   const char  *get(const char *pTag, bool expand = TRUE);
   bool        getValStr(QString &strTag, QString &strVal, bool expand = TRUE);
   xDefltEntry *makeEntry(QString &str);
   void        load(FILE *pFile, const char *pDefaults[]);
   CallBack    setCallBack(CallBack pCallBack);

private:
   QPtrList<xDefltEntry>   defList;
   CallBack    callBack;
};

#endif
