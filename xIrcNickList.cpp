/***************************************************************************
**    xIrcNickList.cpp  $Revision: 1.1.1.1 $ - $Name:  $ 
**    Class for handling lists of Nick names
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
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <qt.h>
#include "xIrcNickList.h"

static int dbg = 0;

xIrcNickList::xIrcNickList()
{
   pList = NULL;
}

xIrcNickList::~xIrcNickList()
{
   xIrcNickListEntry *pEntry, *pEntry1;

   if (dbg) fprintf(stdout, "xIrcNickList::~xIrcNickList():Enter\n");
   if (dbg) fflush(stdout);
   if (pList)
   {
      for (pEntry = pList; pEntry != NULL; pEntry = pEntry1)
      {
         pEntry1 = pEntry->next();
         delete pEntry;
      }
   }
   if (dbg) fprintf(stdout, "xIrcNickList::~xIrcNickList():Exit\n");
   if (dbg) fflush(stdout);
}

bool xIrcNickList::is(const char *pName, bool byNick)
{
   xIrcNickListEntry *pEntry = NULL;
   int rv;

   if (dbg) fprintf(stdout, "xIrcNickList::is(char *):Enter\n");
   if (dbg) fflush(stdout);
   rv = FALSE;
   if (pList)
   {
      if (dbg > 2) fprintf(stdout, "xIrcNickList::is(char *):Testing first name\n");
      if (dbg > 2) fflush(stdout);
      for (pEntry = pList; pEntry != NULL; pEntry = pEntry->next())
      {
         if ((rv = pEntry->is(pName, byNick)) != FALSE)
            break;
         if (dbg > 2) fprintf(stdout, "xIrcNickList::is(char *):Moving to next name\n");
         if (dbg > 2) fflush(stdout);
      }
      if (dbg > 2) fprintf(stdout, "xIrcNickList::is(char *):Found match or hit end\n");
      if (dbg > 2) fflush(stdout);
      if (rv != TRUE || pEntry == NULL)
         rv = FALSE;
   }
   if (dbg) fprintf(stdout, "xIrcNickList::is(char *):Exit(%s)\n", rv ? "TRUE" : "FALSE");
   if (dbg) fflush(stdout);
   return(rv);
}

xIrcNickListEntry *xIrcNickList::find(const char *pName, bool byNick)
{
   xIrcNickListEntry *pEntry = NULL, *rv;

   rv = NULL;
   if (pList)
   {
      for (pEntry = pList; pEntry != NULL; pEntry = pEntry->next())
      {
         if (pEntry->is(pName, byNick))
         {
            rv = pEntry;
            break;
         }
      }
   }
   return(rv);
}

void xIrcNickList::merge(const char *pName, char delim)
{
   xIrcNickListEntry *pEntry, *pEntry1;
   QString tmpName;
   const char *cp;

   if (dbg) fprintf(stdout, "xIrcNickList::merge(char *):Enter\n");
   if (dbg) fflush(stdout);
   for (cp = pName; *cp;)
   {
      if (dbg) fprintf(stdout, "xIrcNickList::merge(char *):Splitting out next name from string\n");
      if (dbg) fflush(stdout);
      for (tmpName = ""; *cp && *cp != delim && !isspace(*cp); cp++)
         tmpName += *cp;
      if (dbg) fprintf(stdout, "xIrcNickList::merge(char *):Testing if the name exists\n");
      if (dbg) fflush(stdout);
      if (!is(tmpName))
      {
         if (dbg) fprintf(stdout, "xIrcNickList::merge(char *):Creating new name %s\n", (const char *)tmpName);
         if (dbg) fflush(stdout);
         pEntry = new xIrcNickListEntry(tmpName);
         if (pList == NULL)
            pList = pEntry;
         else
         {
            if (dbg) fprintf(stdout, "xIrcNickList::merge(char *):Searching loc for new name\n");
            if (dbg) fflush(stdout);
            for (pEntry1 = pList; pEntry1->next() != NULL; pEntry1 = pEntry1->next())
               ;
/*
            {
               if (strcmp(pEntry1->getNick(), tmpName) > 0)
               {
                  if (dbg) fprintf(stdout, "xIrcNickList::merge(char *):Linking in new name\n");
                  if (dbg) fflush(stdout);
                  pEntry->link(pEntry1);
                  break;
               }
            }
            if (pEntry1->next() == NULL)
            {
               if (dbg) fprintf(stdout, "xIrcNickList::merge(char *):Linking in new name at end of list\n");
               if (dbg) fflush(stdout);
               if (strcmp(pEntry1->getNick(), tmpName) > 0)
               {
                  pEntry1->link(pEntry);
                  if (pEntry1 == pList)
                     pList = pEntry;
               }
               else
                  pEntry->link(pEntry1);
            }
*/
            pEntry->link(pEntry1);
         }
      }
      if (dbg) fprintf(stdout, "xIrcNickList::merge(char *):Moving to next name\n");
      if (dbg) fflush(stdout);
      while (*cp && (*cp == delim || isspace(*cp)))
         cp++;
   }
   if (dbg) fprintf(stdout, "xIrcNickList::merge(char *):Exit\n");
}

void xIrcNickList::merge(xIrcNickListEntry *pEntry)
{
   xIrcNickListEntry *pEntry1, *pNewEntry;

   if (dbg) fprintf(stdout, "xIrcNickList::merge(pEntry):Enter\n");
   for (; pEntry != NULL; pEntry = pEntry->next())
   {
      if (!is(pEntry->getNick()))
      {
         pNewEntry = new xIrcNickListEntry(pEntry);
         if (pList == NULL)
            pList = pNewEntry;
         else
         {
            for (pEntry1 = pList; pEntry1->next() != NULL; pEntry1 = pEntry1->next())
               ;
            pEntry1->link(pNewEntry);
         }
      }
   }
   if (dbg) fprintf(stdout, "xIrcNickList::merge(pEntry):Exit\n");
}

void xIrcNickList::remove(xIrcNickListEntry *pEntry, bool delflag)
{
   xIrcNickListEntry *pEntry1 = NULL;

   pEntry1 = pEntry->next();
   if (pEntry == pList)
      pList = pEntry1;
   pEntry->unlink();
   if (delflag && pEntry)
      delete pEntry;
}

void xIrcNickList::remove(const char *pNames, bool delflag, char delim)
{
   xIrcNickListEntry *pEntry = NULL;
   QString tmpName;
   const char *cp = NULL;

   if (dbg) fprintf(stdout, "xIrcNickList::remove(char *):Enter\n");
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xIrcNickList::remove(char *):Testing next name\n");
   if (dbg) fflush(stdout);
   cp = pNames;
   for (tmpName = ""; *cp && *cp != delim && !isspace(*cp); cp++)
      tmpName += *cp;
   if (dbg) fprintf(stdout, "xIrcNickList::remove(char *):Searching for |%s|\n", (const char *)tmpName);
   if (dbg) fflush(stdout);
   if ((pEntry = find(tmpName)) != NULL)
   {
      if (dbg) fprintf(stdout, "xIrcNickList::remove(char *):Found it!!\n");
      if (dbg) fflush(stdout);
      if (pEntry == pList)
      {
         if (dbg) fprintf(stdout, "xIrcNickList::remove(char *):Setting new head of List\n");
         if (dbg) fflush(stdout);
         pList = pEntry->next();
      }
      if (dbg) fprintf(stdout, "xIrcNickList::remove(char *):Unlinking entry!\n");
      if (dbg) fflush(stdout);
      pEntry->unlink();
      if (delflag)
      {
         if (dbg) fprintf(stdout, "xIrcNickList::remove(char *):Deleting entry\n");
         if (dbg) fflush(stdout);
         delete pEntry;
      }
      if (dbg) fprintf(stdout, "xIrcNickList::remove(char *):Done w/ entry!\n");
      if (dbg) fflush(stdout);
   }
}

int xIrcNickList::count()
{
   int rv;
   xIrcNickListEntry *pEntry;

   for (rv = 0, pEntry = pList; pEntry != NULL; pEntry = pEntry->next())
      rv++;
   return(rv);
}

const char *xIrcNickList::getNicks(char delim, bool fullName)
{
   xIrcNickListEntry *pEntry;

   strTmp = "";
   for (pEntry = pList; pEntry != NULL; pEntry = pEntry->next())
   {
      strTmp += pEntry->getNick(fullName);
      if (pEntry->next() != NULL)
         strTmp += delim;
   }
   return(strTmp);
}
const char *xIrcNickList::getAddrs(char delim)
{
   xIrcNickListEntry *pEntry;

   delim++;
   strTmp = "";
   for (pEntry = pList; pEntry != NULL; pEntry = pEntry->next())
   {
      strTmp += pEntry->getAddr();
      if (pEntry->next() != NULL)
         strTmp += ',';
   }
   return(strTmp);
}

xIrcNickListEntry *xIrcNickList::getNext(xIrcNickListEntry *pEntry)
{
   if (pEntry == NULL)
      return(pList);
   else
      return(pEntry->next());
}

