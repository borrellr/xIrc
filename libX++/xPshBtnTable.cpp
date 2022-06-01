/***************************************************************************
**    xPshBtnTable.cpp  $Revision: 1.2 $ - $Name:  $ 
**    xFrame for handling 2 Dimensional Tables of buttons
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
#include <stdlib.h>
#include <qfont.h>
#include "xPshBtnTable.h"

static int dbg = 0;

xPshBtnTable::xPshBtnTable(xWidgetResInfo *pPRes, QWidget *pParent,
                           const char *pName) :
              xFrame((wdtRes = new xWidgetResInfo(pPRes, QString(""),
                                                  QString(""))),
                     pParent, pName)
{
   wdtResPrv = new xWidgetResInfo(pPRes);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

//   setFont(QFont("helvetica", 12, QFont::Bold));
   setAlignment(xALIGN_Horz);
   setMargins(3, 3);
   setResizeMode(xSPACE_Resize);
   btnList.setAutoDelete(TRUE);
}

xPshBtnTable::~xPshBtnTable()
{
   xPshBtnFrame *pBtnFrame;

   for (xPshBtn *pBtn = btnList.first(); pBtn; pBtn = btnList.first())
      btnList.remove(pBtn);

   for (pBtnFrame = btnFrameList.first(); pBtnFrame; pBtnFrame = btnFrameList.first())
   {
      btnFrameList.remove(pBtnFrame);
      delete pBtnFrame;
   }
}

void xPshBtnTable::arrangeButtons(int cols)
{
   xPshBtn *pBtn, *pBtnNew;
   xPshBtnFrame *pBtnFrame;
   int btnCnt = 0, maxHeight = 0, maxWidth = 0;
   int diffColRow = 0, extraBtns = 0, bestFit, x, y, xm, ym;

   //
   // Start by getting a button count and the width
   // of the widest button
   //
   if (dbg) fprintf(stdout, "xPshBtnTable::arrangeButtons():Counting Buttons\n");
   for (pBtn = btnList.first(); pBtn; pBtn = btnList.next())
   {
      btnCnt++;
      maxWidth = (pBtn->width() > maxWidth) ? pBtn->width() : maxWidth;
      maxHeight = (pBtn->height() > maxHeight) ? pBtn->height() : maxHeight;
   }
   if (dbg) fprintf(stdout, "xPshBtnTable::arrangeButtons():have %d Buttons\n", btnCnt);

   //
   // Set all buttons to the width of the widest button
   //
   for (pBtn = btnList.first(); pBtn; pBtn = btnList.next())
      pBtn->resize(maxWidth, maxHeight);

   //
   // If they didn't specify the number of columns, do your
   // best at figuring out how many
   //
   if (cols == 0)
   {
      diffColRow = extraBtns = 65536;
      bestFit = 1;
      for (x = 2; x < btnCnt; x++)
      {
         if (dbg) fprintf(stdout, "xPshBtnTable::arrangeButtons():Trying %d Rows\n",
                                  x);
         cols = btnCnt / x;
         y = btnCnt % x; 
         if (dbg) fprintf(stdout, "xPshBtnTable::arrangeButtons():Makes %d Cols w/ %d left over\n",
                                   cols, y);
         if (y <= extraBtns)
         {
            if (dbg) fprintf(stdout, "xPshBtnTable::arrangeButtons():abs(cols - x): %d < diffColRow: %d\n",
                                      abs(cols - x), diffColRow);
            if (abs(cols - x) <= diffColRow)
            {
               diffColRow = abs(cols - x);
               bestFit = cols;
               if (dbg) fprintf(stdout, "xPshBtnTable::arrangeButtons():diffColRow = %d\n",
                                         diffColRow);
               if (dbg) fprintf(stdout, "xPshBtnTable::arrangeButtons():Best fit yet!\n");
            }
            extraBtns = y;
         }
      }
      cols = bestFit;
      if (dbg) fprintf(stdout, "xPshBtnTable::arrangeButtons():Settled on %d Cols\n",
                                cols);
   }

   //
   // Now delete any existing Button frames by first unlinking its
   // buttons. Then create as many Button frames as columns and
   // Place the buttons into them
   //
   if (dbg) fprintf(stdout, "xPshBtnTable::arrangeButtons():Deleting existing frames\n");
   for (pBtnFrame = btnFrameList.last(); pBtnFrame;)
   {
      btnFrameList.remove(pBtnFrame);
      delete pBtnFrame;
   }

   //
   // Now create all new frames to hold the buttons we have
   //
   if (dbg) fprintf(stdout, "xPshBtnTable::arrangeButtons():Adding Buttons\n");
   if (dbg) fflush(stdout);
   for (x = 0, pBtn = btnList.first(); x < cols && pBtn != 0; x++)
   {
      if (dbg) fprintf(stdout, "xPshBtnTable::arrangeButtons():New button Frame %d\n", x);
      if (dbg) fflush(stdout);
      pBtnFrame = new xPshBtnFrame(wdtResPrv, this);
      pBtnFrame->setAlignment(xALIGN_Vert);
      margins(ym, xm);
      pBtnFrame->setMargins(0, xm);
      btnFrameList.append(pBtnFrame);
      if (dbg) fprintf(stdout, "xPshBtnTable::arrangeButtons():Adding Buttons to frame\n");
         if (dbg) fflush(stdout);
      for (y = 0; y < btnCnt / cols && pBtn; y++, pBtn = btnList.next())
      {
         if (dbg) fprintf(stdout, "xPshBtnTable::arrangeButtons():Adding Button %d to frame\n", y);
         if (dbg) fflush(stdout);
         pBtnNew = pBtnFrame->addButton(pBtn->text(), pBtn->id());
         pBtnNew->resize(maxWidth, pBtn->height());
         if (dbg) fprintf(stdout, "xPshBtnTable::arrangeButtons():Button Added!\n");
         if (dbg) fflush(stdout);
      }
      if (extraBtns > 0 && pBtn != NULL)
      {
         if (dbg) fprintf(stdout, "xPshBtnTable::arrangeButtons():Adding Extra Button %d to frame\n",
                                  extraBtns);
         if (dbg) fflush(stdout);
         pBtnNew = pBtnFrame->addButton(pBtn->text(), pBtn->id());
         pBtnNew->resize(maxWidth, pBtn->height());
         if (dbg) fprintf(stdout, "xPshBtnTable::arrangeButtons():Extra Button Added!\n");
         if (dbg) fflush(stdout);
         extraBtns--;
         pBtn = btnList.next();
      }
      if (dbg) fprintf(stdout, "xPshBtnTable::arrangeButtons():Connecting Button Frame signals\n");
      if (dbg) fflush(stdout);
      connect(pBtnFrame, SIGNAL(clicked(int)), this, SLOT(buttonClicked(int)));
      if (dbg) fprintf(stdout, "xPshBtnTable::arrangeButtons():Adding Button Frame to this\n");
      if (dbg) fflush(stdout);
      pBtnFrame->fitFrame();
      addWidget(pBtnFrame);
   }
   if (dbg) fprintf(stdout, "xPshBtnTable::arrangeButtons():Fitting this frame!\n");
   if (dbg) fflush(stdout);
   fitFrame();
   if (dbg) fprintf(stdout, "xPshBtnTable::arrangeButtons():Exit\n");
   if (dbg) fflush(stdout);
}

void xPshBtnTable::addButton(xPshBtn *pBtn, int id)
{
   pBtn->setId(id);
   addButton(pBtn);
}

void xPshBtnTable::addButton(xPshBtn *pButton)
{
   btnList.append(pButton);
//   pButton->setFont(font());
//   addWidget(pButton);
}

xPshBtn *xPshBtnTable::addButton(const char *pBtnName, int id)
{
   xPshBtn *pBtn;
   
   if ((pBtn = new xPshBtn(wdtRes, NULL, pBtnName, pBtnName, id)) != NULL)
   {
      addButton(pBtn);
   }
   return(pBtn);
}

void xPshBtnTable::rmvButton(int id)
{
   xPshBtn *pBtn;

   for (pBtn = btnList.first(); pBtn; pBtn = btnList.next())
   {
      if (id == pBtn->id())
         btnList.removeRef(pBtn);
   }
}

void xPshBtnTable::buttonClicked(int id)
{
   emit clicked(id);
}

//#include "xPshBtnTable.moc"
