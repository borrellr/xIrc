/***************************************************************************
**    xIrcNotifyTable.cpp  $Revision: 1.4 $ - $Name:  $ 
**    Frame showing Who's on, who's not
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
#include <stdlib.h>
#include <qt.h>
#include "xIrcCommands.h"
#include "xIrcConnect.h"
#include "xIrcMsgDispatch.h"
#include "xIrcNotifyTable.h"
#include "xIrcPeopleEdit.h"
#include "xDefaults.h"

static bool dbg = false;
                    
static const char *pInitialResources[] =
{
   NULL
};

xIrcNotifyTable::xIrcNotifyTable(xWidgetResInfo *pPRes, QWidget *pParent,
                           const char *pName) :
               xDialog(wdtRes = new xWidgetResInfo(pPRes, QString("notifylist"),
                                                   QString("NotifyList")),
                       pParent, pName, FALSE)
{
   QFontMetrics fm(fontMetrics());

   if (dbg) fprintf(stdout, "xIrcNotifyTable::xNotifyTable():Enter\n");
   if (dbg) fflush(stdout); 
   Resources->setWidgetInit(pInitialResources);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   setCaption("Folks Online");
   mainFrame->setAutoDelete(TRUE);
   pListBox = new xListBox(wdtRes, this);

   pListBox->resize(fm.maxWidth() * 20, fm.lineSpacing() * 15);
   pListBox->repaint(FALSE);
   addWidget(pListBox);
   setMargins(3,3);
   setWidgetSpacing(0);
//   setResizeMode(xSPACE_Resize);
   initFrame();
   
   connect(pListBox, SIGNAL(selected(int)),
           this, SLOT(haveSelection(int)));

   if (dbg) fprintf(stdout, "xIrcNotifyTable::xNotifyTable():Exit\n");
   if (dbg) fflush(stdout); 
}

void xIrcNotifyTable::update(xIrcPeopleList *pList)
{
   xIrcPeopleListIterator it(*pList);

   if (dbg) fprintf(stdout, "xIrcNotifyTable::update():Enter\n");
   if (dbg) fflush(stdout); 
   pListBox->clear();
   for (; it.current() != NULL; ++it)
   {
      xIrcPeopleEntry *p = it.current();
      QString r(p->realNick());

      if (dbg) fprintf(stdout, "xIrcNotifyTable::update():Looking at %s:%s:%s\n", 
                               (const char*)r,
                               (const char*)p->nick(),
                               (const char*)p->mask());
      if (dbg) fflush(stdout); 
      if (dbg) fprintf(stdout, "xIrcNotifyTable::update():Looking at %s:%s:%s\n", 
                               (const char*)r,
                               (const char*)p->nick(),
                               (const char*)p->mask());
      if (dbg) fflush(stdout); 
      if (dbg) fprintf(stdout, "xIrcNotifyTable::update():Looking at %s:%s:%s\n", 
                               (const char*)r,
                               (const char*)p->nick(),
                               (const char*)p->mask());
      if (dbg) fflush(stdout); 
      if (p->state() >= 2)
      {
         QString t((const char*)r);

         t += " (";
         t += p->mask();
         t += ")";
         if (dbg) fprintf(stdout, "xIrcNotifyTable::update():Adding :%s:\n", 
                                  (const char*)r);
//                                  (const char*)p->nick(),
//                                (const char*)p->mask());
         if (dbg) fflush(stdout); 
         pListBox->insertItem(t);
         pListBox->sort();
      }
   }
   pListBox->repaint(FALSE);
   if (dbg) fprintf(stdout, "xIrcNotifyTable::update():Exit\n");
   if (dbg) fflush(stdout); 
}

void xIrcNotifyTable::haveSelection(int x)
{
   const char *cp;

   if (dbg) fprintf(stdout, "xIrcNotifyTable::haveSelected():Enter\n");
   if (dbg) fflush(stdout); 
   cp = pListBox->text(x);
   if (dbg) fprintf(stdout, "xIrcNotifyTable::haveSelected():Have Nick |%s|\n", cp);
   if (dbg) fflush(stdout); 
   emit selected(cp);
   if (dbg) fprintf(stdout, "xIrcNotifyTable::haveSelected():Exit\n");
   if (dbg) fflush(stdout); 
}

void xIrcNotifyTable::resizeEvent(QResizeEvent *pEvt)
{
   QSize newSize = pEvt->size(); //, sizeTW, sizeNewTW, sizeSB;
   int h, w;             

   h = (int)(newSize.height() - (pListBox->frameWidth() * 2.5));
   w = (int)(newSize.width() -  (pListBox->frameWidth() * 2.5));
   pListBox->resize(w, h);
   mainFrame->fitFrame();
}

//#include "xIrcNotifyTable.moc"
