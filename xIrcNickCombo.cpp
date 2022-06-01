/***************************************************************************
**    xIrcNickBox.cpp  $Revision: 1.2 $ - $Name:  $ 
**    xComboBox for Handling Nicks and addresses
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
#include <stdio.h>
#include "xIrcNickCombo.h"

static int dbg = 0;

xIrcNickCombo::xIrcNickCombo(xWidgetResInfo *pPRes, QWidget *pParent,
                             const char *pName, int type) :
               xEditList(pPRes, pParent, pName), xIrcNickList()
{
   wdtRes = new xWidgetResInfo(pPRes, QString(""), QString(""));
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   xIrcNickList::pList = NULL;
   nameType = type;
   connect(this, SIGNAL(gotEntry(const char *)), 
           this, SLOT(gotSelected(const char *)));
}

xIrcNickCombo::~xIrcNickCombo()
{
//   disconnect(this);
   if (dbg) fprintf(stdout, "xIrcNickCombo::~xIrcNickCombo():Here\n");
   if (dbg) fflush(stdout);
}

void xIrcNickCombo::merge(const char *pNick, char delim)
{
   if (dbg) fprintf(stdout, "xIrcNickCombo::merge(char *):Enter\n");
   if (dbg) fflush(stdout);
   xIrcNickList::merge(pNick, delim);
   xEditList::insertItemsSort(pNick);
   if (dbg) fprintf(stdout, "xIrcNickCombo::merge(char *):Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcNickCombo::merge(xIrcNickListEntry *pEntry)
{
   if (dbg) fprintf(stdout, "xIrcNickCombo::merge(pEntry):Enter\n");
   if (dbg) fflush(stdout);
   xIrcNickList::merge(pEntry);
   xEditList::insertItemsSort(pEntry->getNick());
   if (dbg) fprintf(stdout, "xIrcNickCombo::merge(pEntry):Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcNickCombo::remove(const char *pNick, bool delflag, char delim)
{
   int idx;

   if ((idx = getIndex(pNick)) >= 0)
      removeItem(idx);
   xIrcNickList::remove(pNick, delflag, delim);
}

void xIrcNickCombo::remove(xIrcNickListEntry *pEntry, bool delflag)
{
   int idx;

   if ((idx = getIndex(pEntry->getNick())) >= 0)
      removeItem(idx);
   xIrcNickList::remove(pEntry, delflag);
}

void xIrcNickCombo::clearList()
{
   xIrcNickListEntry *pEntry, *pEntry1;
   
   if (dbg) fprintf(stdout, "xIrcNickCombo::clearList():Enter\n");
   if (dbg) fflush(stdout);
   for (pEntry = xIrcNickList::pList; pEntry; pEntry = pEntry1)
   {
      pEntry1 = pEntry->next();
      pEntry->unlink();
      delete pEntry;
   }
   xIrcNickList::pList = NULL;
   xEditList::clear();
   if (dbg) fprintf(stdout, "xIrcNickCombo::clearList():Exit\n");
   if (dbg) fflush(stdout);
}

void xIrcNickCombo::purgeList()
{
   const char *cp;
   int x;

   for (x = 0; (unsigned int)x < xEditList::pList->count(); x++)
   {
      cp = xEditList::string(x);
      if (cp)
      {
         if (!is(cp))
            removeItem(x);
      }
   }
}

xIrcNickListEntry *xIrcNickCombo::selected()
{
   xIrcNickListEntry *pEntry;
   const char *cp;
   int x;
   
   if (dbg) fprintf(stdout, "xIrcNickCombo::selected():Enter\n");
   if (dbg) fflush(stdout);
   pEntry = NULL;
   if (strlen(cp = pEdit->text()) == 0)
   {
      if (dbg) fprintf(stdout, "xIrcNickCombo::selected():No Edit Text getting current item\n");
      if (dbg) fflush(stdout);
      if ((x = xEditList::pList->currentItem()) >= 0)
      {
         if (dbg) fprintf(stdout, "xIrcNickCombo::selected():Have current item\n");
         if (dbg) fflush(stdout);
         cp = xEditList::string(x);
         if (dbg) fprintf(stdout, "xIrcNickCombo::selected():name = |%s|\n", cp);
         if (dbg) fflush(stdout);
         pEdit->setText(cp);
         pEntry = xIrcNickList::find(cp);
      }
   }
   else
   {
      if (dbg) fprintf(stdout, "xIrcNickCombo::selected():Have Edit Text\n");
      if (dbg) fflush(stdout);
      pEdit->setText(cp);
      pEntry = xIrcNickList::find(cp); 
   }
   if (dbg) fprintf(stdout, "xIrcNickCombo::selected():Exit(%s)\n", pEntry ? "Found" : "Not Found" );
   if (dbg) fflush(stdout);
   return(pEntry);
};

void xIrcNickCombo::gotSelected(const char *pName)
{
   xIrcNickListEntry *pEntry;
   const char *cp;
   
   cp = pName;
   if ((pEntry = xIrcNickList::find(pName)) != NULL)
   {
      cp = pEntry->getNick();
      pEdit->setText(cp);
   }
   emit nickSelected(cp);
}

void xIrcNickCombo::resize(int width, int height)
{
   pEdit->QWidget::resize(width, pEdit->size().height());
   xEditList::pList->resize(width, height);
   fitFrame();
}
   
//#include "xIrcNickCombo.moc" 
