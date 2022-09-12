/***************************************************************************
**    xEditList.cpp $Revision: 1.2 $ - $Name:  $
**    QListBox w/ Edit field
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
#include <ctype.h>
#include <stdio.h>
#include <qglobal.h>
#include <qkeycode.h>
#include "xEditList.h"

static int dbg = 0;

xEditList::xEditList(xWidgetResInfo *pPRes, QWidget *pParent, 
                     const char *pName,
                     int cols, int rows, bool _unique) :
            xFrame((wdtRes = new xWidgetResInfo(pPRes, QString(""),
                                 QString(""))),
                   pParent, pName)
{
   cols++;
   rows++;
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   unique = _unique;
   if (dbg) fprintf(stdout, "xEditList::xEditList():Enter\n");
   if (dbg) fflush(stdout);
   if ((pList = new xListBox(wdtRes, this)) != NULL)
   {
      QFontMetrics fm = pList->QWidget::fontMetrics();
      
//      pList->QWidget::setFont(QFont("helvetica", 12));
      pList->setScrollBar(TRUE);
      pList->QWidget::resize(fm.maxWidth() * 30, (fm.lineSpacing() * 4));
      if (dbg) fprintf(stdout, "xEditList::xEditList():list.w = %d, .h = %d\n",
                       pList->size().width(), pList->size().height());
      if (dbg) fflush(stdout);
      connect(pList, SIGNAL(selected(int)), this, SLOT(recvdSelect(int)));
      connect(pList, SIGNAL(highlighted(int)), this, SLOT(recvdHighlighted(int)));
   }
   
   if ((pEdit = new xLineEdit(wdtRes, this)) != NULL)
   {
      QFontMetrics fm = pEdit->QWidget::fontMetrics();
      if (dbg) fprintf(stdout, "xEditList::xEditList():edit.w = %d, .h = %d\n",
                       pEdit->size().width(), pEdit->size().height());
//      pEdit->QWidget::setFont(QFont("helvetica", 12));
//      pEdit->QWidget::resize(fm.maxWidth() * 30, fm.lineSpacing() + 7);
      pEdit->QWidget::resize(fm.maxWidth() * 30, pEdit->height());
      if (dbg) fprintf(stdout, "xEditList::xEditList():New edit.w = %d, .h = %d\n",
                       pEdit->size().width(), pEdit->size().height());
      pEdit->setLabel(" ");
      pEdit->setMargins(0,0);
   
      setMargins(0,0);
      if (dbg) fprintf(stdout, "xEditList::xEditList():edit.w = %d, .h = %d\n",
                       pEdit->size().width(), pEdit->size().height());
      if (dbg) fflush(stdout);
      connect(pEdit, SIGNAL(returnPressed()), this, SLOT(recvdReturn()));
   }
   addWidget(pEdit);
   addWidget(pList);
   setWidgetSpacing(0);
   fitFrame();
   if (dbg) fprintf(stdout, "xEditList::xEditList():edit.w = %d, .h = %d\n",
                    pEdit->size().width(), pEdit->size().height());
   if (dbg) fprintf(stdout, "xEditList::xEditList():list.w = %d, .h = %d\n",
                    pList->size().width(), pList->size().height());
   if (dbg) fflush(stdout);
}

xEditList::~xEditList()
{
   if (pList)
      delete pList;
   if (pEdit)
      delete pEdit;
}

void xEditList::insertItemsSort(const char *pItems, char delim)
{
   QString pStr;
   int x;
   
   if (dbg) fprintf(stdout, "xEditList::insertItemsSort(%s, %d)\n", 
                            pItems, delim);
   x = 0;  
   for (;;)
   {
      if (!x && isspace(*pItems))
      {
         pItems++;
         continue;
      }
      else
      {
         x = 1;
         if (*pItems == delim || *pItems == '\0')
         {
            if (dbg) fprintf(stdout, "xEditList::insertItemsSort():Testing for Nick |%s|\n", (const char *)pStr); 
            if (!unique || getIndex((const char *)pStr) < 0)
            {
               if (dbg) fprintf(stdout, "xEditList::insertItemsSort():Adding Nick |%s|\n", (const char *)pStr); 
               pList->insertItem((const char *)pStr);
               pList->sort();
            }
            x = 0;
            pStr = "";
            if (!(*pItems))
               break;
         }
         else
            pStr += *(pItems++);
      }
   }
   if (dbg) fprintf(stdout, "xEditList::insertItemsSort():Exit\n");
}

void xEditList::insertItems(const char *pItems, char delim, int index)
{
   QString pStr;
   int x;
   
   if (dbg) fprintf(stdout, "xEditList::insertItems(%s, %d, %d)\n", 
                            pItems, delim, index);
   if (dbg) fflush(stdout);
   x = 0;
   pStr = "";  
   for (;;)
   {
      if (!x && isspace(*pItems))
      {
         pItems++;
         continue;
      }
      else
      {
         x = 1;
         if ((delim == ' ' && isspace(*pItems)) || *pItems == delim || *pItems == '\0')
         {
            if (dbg) fprintf(stdout, "xEditList::insertItems():Testing for |%s|\n", (const char *)pStr);
            if (dbg) fflush(stdout);
            if (!unique || getIndex((const char *)pStr) < 0)
            {
               if (dbg) fprintf(stdout, "xEditList::insertItems():Inserting |%s|\n", (const char *)pStr);
               if (dbg) fflush(stdout);
               pList->insertItem((const char *)pStr);
               if (dbg) fprintf(stdout, "xEditList::insertItems():Done w/ Insert, starting over w/ |%s|\n", pItems);
               if (dbg) fflush(stdout);
            }
            x = 0;
            pStr = "";
            if (!(*pItems))
               break;
            pItems++;
         }
         else
            pStr += *(pItems++);
      }
   }
   if (dbg) fprintf(stdout, "xEditList::insertItems:Exit"); 
   if (dbg) fflush(stdout);
}

int xEditList::getIndex(const char *pString)
{
   int x, cnt;
   const char *cp;

   if (pList)
      cnt = pList->count();
   else
      return(-1);
   for (x = 0; x < cnt; x++)
   {
      if ((cp = string(x)) != NULL)
      {
         if (strcmp(cp, pString) == 0)
            break;
      }
   }
   return((x == cnt) ? -1 : x);
}

void xEditList::recvdReturn()
{
   int index;
   
   if (strlen(pEdit->text()) > 0)
      emit gotEntry((const char *)pEdit->text());
   else
   {
      index = pList->currentItem();
#ifdef QT_PRE096
      pEdit->setText(pList->string(index));
#else
      pEdit->setText(pList->text(index));
#endif
      emit gotEntry((const char *)pEdit->text());
   }
}

void xEditList::recvdSelect(int index)
{
#ifdef QT_PRE096
   pEdit->setText(pList->string(index));
#else
   pEdit->setText(pList->text(index));
#endif
   emit gotEntry((const char *)pEdit->text());
}

const char *xEditList::string(int x)
{
#ifdef QT_PRE096
   return(pList->string(x)); 
#else
   return(pList->text(x)); 
#endif
}
void xEditList::keyPressEvent(QKeyEvent *pEvt)
{
   unsigned int x;
   
   if (dbg) fprintf(stdout, "xListBox::keyPressEvent():Got key - 0x%d\n", pEvt->key());
   if (pEvt->key() == Key_Up)
   {
      if (dbg) fprintf(stdout, "xListBox::keyPressEvent():Key_Up!!\n");
      if ((x = (unsigned int)pList->currentItem()) > 0)
         pList->setCurrentItem(--x);
   }
   else if (pEvt->key() == Key_Down)
   {
      if (dbg) fprintf(stdout, "xListBox::keyPressEvent():Key_Down!!\n");
      if ((x = (unsigned int)pList->currentItem()) < pList->count())
         pList->setCurrentItem(++x);
   }
   xFrame::keyPressEvent(pEvt);
}

void xEditList::resizeEvent(QResizeEvent *pEvt)
{
   QSize s = pEvt->size();
   int h, w;
   
   if (dbg > 1) fprintf(stdout, "xListBox::resizeEvent():Enter\n");
   h = s.height() - pEdit->size().height() - (lineWidth() * 2);
   w = s.width() - (lineWidth() * 2);
   h = (h <= 0) ? 5 : h;
   w = (w <= 0) ? 5 : w;
   s.setHeight(h);
   s.setWidth(w);
//   pList->resize(s);
//   fitFrame();
   if (dbg > 1) fprintf(stdout, "xListBox::resizeEvent():Exit\n");
}

//#include "xEditList.moc"

