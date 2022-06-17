/***************************************************************************
**    xMultiLineBuf.cpp  $Revision: 1.1.1.1 $ - $Name:  $ 
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
#include <stdio.h>
#include "xMultiLineBuf.h"

static int dbg = 0;

xMultiLineBuf::xMultiLineBuf(int height)
{
   pBeg = new QString[height];
   pEnd = pBeg + height - 1;
   pHead = pTail = pBeg;
}

xMultiLineBuf::~xMultiLineBuf()
{
   if (pBeg)
      delete [] pBeg;
}

const QString *xMultiLineBuf::nextLine(QString *pStr, bool used)
{
   if (used && pHead == pTail)
      return(pHead);
   else if (used && (pStr + 1) == pHead)
      return(pTail);
   else if (pStr == pEnd)
      return(pBeg);
   else
      return(++pStr);
}

const QString *xMultiLineBuf::prevLine(const QString *pStr, bool used)
{
   if (pHead == pTail)
      return(pHead);
   else if (pStr == pTail && used)
   {
      if (pHead == pBeg)
         return(pEnd);
      else
         return(pHead - 1);
   }
   else if (pStr == pBeg)
      return(pEnd);
   else
      return(--pStr);
}

const QString *xMultiLineBuf::getNext(const QString *pStr, int &changed)
{
   changed = TRUE;
//   changed = FALSE;
   if (pHead == pTail)
      return(pStr);
   if (pHead == pTail || (pStr + 1) == pHead || 
       (pStr == pEnd && pHead == pBeg))
      return(pStr);
   else
   {
      changed = TRUE;
      if (pStr == pEnd)
         return(pBeg);
      else
         return(++pStr);
   }
}

const QString *xMultiLineBuf::getPrev(const QString *pStr, int &changed)
{
   changed = FALSE;
   if (pHead == pTail || pStr == pTail)
      return(pStr);
   else
   {
      changed = TRUE;
      if (pStr == pBeg)
         return(pEnd);
      else
         return(--pStr);
   }
}

const QString *xMultiLineBuf::putString(const char *pString)
{
   const QString *rv;
   
   if (dbg) fprintf(stdout, "MultiLineBuf::putString():Adding String: |%s|\n\r", (char *)pString);
   rv = pHead;
   *pHead = pString;
   if (dbg) fprintf(stdout, "MultiLineBuf::putString():*pHead = |%s|\n\r", (const char *)*pHead);
   if (((const QString *)pHead = nextLine(pHead)) == pTail)
      (const QString *)pTail = nextLine(pTail);
   return(rv);
}

const QString *xMultiLineBuf::pointLine(int ofs, int &retOfs, int &retLines)
{
   const QString *rv;
   
   rv = pTail;
   if (pHead == pTail)
   {
      retLines = retOfs = 0;
      return(pHead);
   }
   else if (ofs <= 0)
      rv = lastLine();
   else if (ofs >= getLinesUsed())
   {
      retOfs = 0;
      retLines = getLinesUsed();
      return(pTail);
   }
   else
   {
      if (ofs >= (pHead - pBeg))
         rv = pEnd - (ofs - (pHead - pBeg));
      else
         rv = pHead - (ofs + 1);
      if (rv >= pTail)
         retOfs = rv - pTail;
      else
         retOfs = (rv - pBeg) + (pEnd - pHead);
      retLines = getLinesUsed() - retOfs;
   }
   return(rv);
}

const QString *xMultiLineBuf::lastLine()
{
   if (pHead == pTail)
      return(pHead);
   else if (pHead == pBeg)
      return(pEnd);
   else
      return(pHead - 1);
}

int xMultiLineBuf::getLinesUsed()
{
   if (pHead == pTail)
      return(0);
   else if (pHead > pTail)
      return (pHead - pTail);
   else
      return((pEnd - pTail) + (pHead - pBeg) + 1);
}
