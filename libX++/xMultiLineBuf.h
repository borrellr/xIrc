/***************************************************************************
**    xMultiLineBuf.h  $Revision: 1.1.1.1 $ - $Name:  $ 
**    Class for manipulating Multiple lines of text
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
#ifndef _xMULTILINEBUF_H
#define _xMULTILINEBUF_H

#include <qstring.h>

class xMultiLineBuf
{
public:
   xMultiLineBuf(int height);
   virtual ~xMultiLineBuf();
   
   virtual const QString   *putString(const char *pStr);
   const QString           *getNext(const QString *pStr, int &changed);
   const QString           *getPrev(const QString *pStr, int &changed);
   const QString           *pointLine(int ofs, int &retOfs, int &retLines);
   int                     getLinesUsed();

protected:
   const QString  *nextLine(QString *pStr, bool used = FALSE);
   const QString  *prevLine(const QString *pStr, bool used = FALSE);
   const QString  *lastLine();
   
private:
   QString     *pBeg;
   QString     *pEnd;
   QString     *pHead;
   QString     *pTail;
};

#endif
