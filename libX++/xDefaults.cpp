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
   pEntry = new xDefltEntry();

   pEntry->strTag = strTag.upper();
   xDefaultMap::iterator it = defList.find(pEntry->strTag);

   if (it != defList.end())
      pEntry->strVal.append(it.data());   

   return(pEntry);
}

const char *xDefaults::findEntry(const char *pTag)
{
   QString strTag(pTag);
   QString s("");

   strTag = strTag.upper();
   xDefaultMap::iterator it = defList.find(strTag);

   if (it != defList.end())
      s.append(it.data());

   return (const char *)s.latin1();
}

void xDefaults::add(const char *pStr)
{
   QString strTmp(pStr);
   QString key, value;

   key = strTmp.section("=", 0, 0);
   value = strTmp.section("=", 1, 1);
   defList[key.upper()] = value.simplifyWhiteSpace();
}

void xDefaults::add(QString &strTag, QString &strVal)
{
   defList.insert(strTag.upper(), strVal.simplifyWhiteSpace());
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
   strTag = str.section("=", 0, 0);
   strVal = str.section("=", 1, 1);

   return(new xDefltEntry(strTag, strVal));
}

void xDefaults::load(QFile &file, QStringList &Defaults)
{
   QString strTmp;

   if (!Defaults.isEmpty())
   {
   }

   QTextStream stream(&file);
   QString line, line2, key, value;

   while (!stream.atEnd()) {
      line = stream.readLine();
      if (!line.isEmpty()) {
         if (line.endsWith("\\")) {
            line2.append(line);
         } else {
            if (line2.isEmpty()) {
               line = line.simplifyWhiteSpace();
               key = line.section("=", 0, 0);
               value = line.section("=", 1, 1);
               defList[key.upper()] = value.simplifyWhiteSpace();
            } else {
               line2.remove("\\");
               line2 = line2.simplifyWhiteSpace();
               key = line2.section("=", 0, 0);
               value = line2.section("=", 1, 1);
               defList[key.upper()] = value.simplifyWhiteSpace();
               line2 = "";
            }
         }
      }
   }
   file.close();
}

CallBack xDefaults::setCallBack(CallBack pCallBack)
{
   CallBack rv = callBack;
   callBack = pCallBack;
   return(rv);
}

void xDefaults::show()
{
   xDefaultMap::Iterator it;

   printf("Printing Default keys and values:\n");
   for ( it = defList.begin(); it != defList.end(); ++it ) {
       printf("Key: \"%s\" Value: \"%s\"\n", it.key().latin1(), it.data().latin1());
   }
   printf("Printed Default keys and values:\n");
}
