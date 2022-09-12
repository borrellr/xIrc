/***************************************************************************
**    xDefaults.cpp  $Revision: 1.2 $ - $Name:  $ 
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
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <qregexp.h>
#include "xMisc.h"
#include "xDefaults.h"

static int dbg = 0;

xDefaults::xDefaults()
{
   callBack = NULL;
}

QString xDefaults::expandEntry(QString &strVal)
{
   xDefltEntry *pEntry;
   QRegExp aliasExp("\\$[a-zA-Z_]+[a-zA-Z0-9_]*"),
           callbackExp1("%[a-zA-Z]"),
           callbackExp2("\\\\[a-wyzA-WYZ]"),
           decEscExp("\\\\[0-9]+"),
           hexEscExp("\\\\[xX][a-fA-F0-9]+");
   QString strTmp1, strRtn;
   char ch;
   const char *cp;

   if (dbg) fprintf(stdout, "xDefaults::expandEntry():Enter\n"); 
   if (dbg) fflush(stdout);
   /*
   ** Start by replacing any Aliases found in the string
   */
   for (strRtn = strVal; strRtn.find(aliasExp) >= 0;)
   {
      int ofs, len;

      if (dbg) fprintf(stdout, "xDefaults::expandEntry():Found Macro!\n"); 
      if (dbg) fflush(stdout);
      ofs = aliasExp.match(strRtn, 0, &len);
      QString strTmp1(strRtn.mid(ofs + 1, len - 1));
      if ((pEntry = findEntry(strTmp1)) != NULL)
      {
         if (dbg) fprintf(stdout, "xDefaults::expandEntry():Replacing |%s| with default\n", 
                                  (const char *)strTmp1); 
         if (dbg) fflush(stdout);
         strRtn.replace(ofs, len, pEntry->strVal);
      }
      else if ((cp = getenv(strTmp1)) != NULL)
      {
         if (dbg) fprintf(stdout, "xDefaults::expandEntry():Replacing |%s| with Envoronment\n", 
                                  (const char *)strTmp1); 
         if (dbg) fflush(stdout);
         strRtn.replace(ofs, len, cp);
      }
      else
         strRtn.replace(aliasExp, "");
   }
   if (dbg) fprintf(stdout, "xDefaults::expandEntry():String is now |%s|\n", 
                             (const char *)strRtn); 
   if (dbg) fflush(stdout);
   /*
   ** Now do any %c expressions requiring the callback
   while (strRtn.find(callbackExp1) >= 0)
   {
      int ofs, len;

      if (dbg) fprintf(stdout, "xDefaults::expandEntry():Found *percent* declaration!\n"); 
      if (dbg) fflush(stdout);
      ofs = callbackExp1.match(strRtn, 0, &len);
      QString strTmp1(strRtn.mid(ofs, len));
      if (callBack != NULL)
         strRtn.replace(ofs, len, (callBack)(strTmp1));
      else
         strRtn.replace(ofs, len, "");
   }
   if (dbg) fprintf(stdout, "xDefaults::expandEntry():String is now |%s|\n", 
                             (const char *)strRtn); 
   if (dbg) fflush(stdout);
   */

   /*
   ** Now do any \c expressions requiring the callback
   while (strRtn.find(callbackExp2) >= 0)
   {
      int ofs, len;

      if (dbg) fprintf(stdout, "xDefaults::expandEntry():Found \\c declaration!\n"); 
      ofs = callbackExp2.match(strRtn, 0, &len);
      QString strTmp1(strRtn.mid(ofs, len));
      if (callBack != NULL)
         strRtn.replace(ofs, len, (callBack)(strTmp1));
      else
         strRtn.replace(ofs, len, "");
   }
   if (dbg) fprintf(stdout, "xDefaults::expandEntry():String is now |%s|\n", 
                             (const char *)strRtn); 
   */

   /*
   ** Do any decimal escape sequences
   */
   while (strRtn.find(decEscExp) >= 0)
   {
      int ofs, len;
      if (dbg) fprintf(stdout, "xDefaults::expandEntry():Found decimal escape!\n"); 
      if (dbg) fflush(stdout);
      ofs = decEscExp.match(strRtn, 0, &len);
      QString strTmp1(strRtn.mid(ofs, len));
      ch = strtoc(strTmp1, NULL);
      strTmp1 = "";
      strTmp1 += ch;
      strRtn.replace(ofs, len, strTmp1);
   }
   if (dbg) fprintf(stdout, "xDefaults::expandEntry():String is now |%s|\n", 
                             (const char *)strRtn); 
   if (dbg) fflush(stdout);

   /*
   ** Finish up with any hex escape sequences
   */
   while (strRtn.find(hexEscExp) >= 0)
   {
      int ofs, len;
      if (dbg) fprintf(stdout, "xDefaults::expandEntry():Found hex escape!\n"); 
      if (dbg) fflush(stdout);
      ofs = hexEscExp.match(strRtn, 0, &len);
      QString strTmp1(strRtn.mid(ofs, len));
      ch = strtoc(strTmp1, NULL);
      strTmp1 = "";
      strTmp1 += ch;
      strRtn.replace(ofs, len, strTmp1);
   }
   if (dbg) fprintf(stdout, "xDefaults::expandEntry():Exit- returning |%s|\n", 
                             (const char *)strRtn); 
   if (dbg) fflush(stdout);
   return(strRtn);
}

xDefltEntry *xDefaults::findEntry(QString &strTag)
{
   xDefltEntry *pEntry;

   for (pEntry = defList.first(); pEntry != NULL; pEntry = defList.next())
      if (strTag.upper() == pEntry->strTag)
         break;
   return(pEntry);
}

const char *xDefaults::findEntry(const char *pTag)
{
   xDefltEntry *pEntry;
   QString strTag(pTag);

   if ((pEntry = findEntry(strTag)) != NULL)
      return((const char *)pEntry->strVal);
   else
      return((const char *)NULL);
}

void xDefaults::add(const char *pStr)
{
   xDefltEntry *pEntry, *pEntry1;
   QString strTmp(pStr);

   if ((pEntry1 = makeEntry(strTmp)) != NULL)
   {
      if ((pEntry = findEntry(pEntry1->strTag)) != NULL)
      {
         pEntry->strVal = pEntry1->strVal;
         delete pEntry1;
      }
      else
         defList.append(pEntry1);
   }
}

void xDefaults::add(QString &strTag, QString &strVal)
{
   xDefltEntry *pEntry;

   if ((pEntry = findEntry(strTag)) != NULL)
      pEntry->strVal = strVal;
   else
      defList.append(new xDefltEntry(strTag, strVal));
}

void xDefaults::set(QString strTag, QString strVal)
{
   add(strTag, strVal);
}

void xDefaults::set(const char *pTag, const char *pVal)
{
   QString strTag(pTag), strVal(pVal);

   add(strTag, strVal);
}

void xDefaults::set(const char *pStr)
{
   add(pStr);
}

bool xDefaults::getValStr(QString &strTag, QString &strVal, bool expand)
{
   xDefltEntry *pEntry;
   bool rv;

   if ((pEntry = findEntry(strTag)) != NULL)
   {
      rv = TRUE;
      if (expand)
         strVal = expandEntry(pEntry->strVal);
      else
         strVal = pEntry->strVal;
   }
   else
      rv = FALSE;
   return(rv);
}

const char *xDefaults::get(const char *pTag, bool expand)
{
   static QString strVal;
   xDefltEntry *pEntry;
   QString strTag(pTag);
   const char *rv;

   if ((pEntry = findEntry(strTag)) != NULL)
   {
      if (expand)
      {
         strVal = expandEntry(pEntry->strVal);
         rv = (const char *)strVal;
      }
      else
         rv = (const char *)pEntry->strVal;
   }
   else
      rv = NULL;
   return(rv);
}

xDefltEntry *xDefaults::makeEntry(QString &str)
{
   QString strTag, strVal;
   const char *cp;

   cp = (const char *)str;
   while (isspace(*cp))
      cp++;
   for (cp = str, strTag = ""; cp && *cp != '\0'; cp++)
   {
      if (*cp == '\n' || *cp == '\r' || *cp == '=' || isspace(*cp))
         break;
      strTag += toupper(*cp);
   }
   while (isspace(*cp))
      cp++;
   if (*cp != '=')
      return(new xDefltEntry());

   cp++;
   while (isspace(*cp))
      cp++;
   for (strVal = ""; cp && *cp != '\0'; cp++)
   {
      if (*cp == '\n' || *cp == '\r')
         break;
      else if ((*cp == '$' || *cp == '\\') && *(cp + 1) == '$')
      {
         strVal += '$';
         cp++;
      }
      else if ((*cp == '%' || *cp == '\\') && *(cp + 1) == '%')
      {
         strVal += '$';
         cp++;
      }
      strVal += *cp;
   }
   return(new xDefltEntry(strTag, strVal));
}

void xDefaults::load(FILE *pFile, const char *pDefaults[])
{
   QString strTmp;
   int state;
   char ch, ch1 = ' ';

   if (pDefaults != NULL)
   {
      while (*pDefaults != NULL)
      {
         add(QString(*pDefaults));
         pDefaults++;
      }
   }

   for (strTmp = "", state = 0; pFile != NULL && !feof(pFile);)
   {
      if ((ch = fgetc(pFile)) != EOF)
      {
         switch (state)
         {
            case 0:
               switch (ch)
               {
                  case '\n':
                     add(strTmp);
                     strTmp = "";
                     break;
                  case '\r':
                     break;
                  case '\\':
                     ch1 = ch;
                     state = 1;
                     break;
                  default:
                     strTmp += ch;
               }
               break;
            case 1:
               switch (ch)
               {
                  case '\n':
                     state = 0;
                     break;
                  case '\r':
                     break;
                  default:
                     strTmp += ch1;
                     strTmp += ch;
                     state = 0;
               }
               break;
         }
      }
   }
}

CallBack xDefaults::setCallBack(CallBack pCallBack)
{
   CallBack rv = callBack;
   callBack = pCallBack;
   return(rv);
}

void xDefaults::show()
{
   QPtrListIterator<xDefltEntry> it(defList);
   xDefltEntry *entry;

   while ((entry = it.current()) != 0) {
      ++it;
      printf("Tag: %s Value: %s \n", entry->strTag.ascii(), entry->strVal.ascii());
   }
}
