/***************************************************************************
**    xFrame.cpp  $Revision: 1.5 $ - $Name:  $ 
**    Self fitting Frame class
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
#include <string.h>
#include <qkeycode.h>
#include <qsize.h>
#include "xTable.h"
#include "xFrame.h"

static int dbg = 0;

/*
static int keyTable[] =
{
   Key_F1, Key_F24, -1,
   Key_A, Key_Z, AltButton,
   Key_0, Key_9, AltButton | ControlButton,
   Key_A, Key_Z, AltButton,
   Key_Up, Key_Up, -1,
   Key_Down, Key_Down, -1,
   Key_Next, Key_Next, -1,
   Key_Prior, Key_Prior, -1,
   Key_Tab, Key_Tab, -1,
   Key_Backtab, Key_Backtab, -1,
   Key_Return, Key_Return, -1,
   -1, -1, -1
};
*/

xFrame::xFrame(xWidgetResInfo *pPRes, QWidget *pParent, const char *pName) :
        QFrame(pParent, pName)
{
   if (dbg) fprintf(stdout, "xFrame::xFrame():Enter\n");
   if (dbg) fflush(stdout);
   fc = 0;
   wdtRes = new xWidgetResInfo(pPRes, QString(""), QString(""));
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
   if (dbg) fprintf(stdout, "xFrame::xFrame():Done setting font and palette\n");
   if (dbg) fflush(stdout);

   widgetAlgn = xALIGN_Vert;
   widgetResz = xSPACE_ToFit;
   widgetEqualize = TRUE;
   widgetSpac = 10;
   widgetVMarg = 10;
   widgetHMarg = 10;
   
   widgetListLength = 10;
   widgetCount = 0;
   fitFrameRecurse = 0;
   setFocusPolicy(QWidget::StrongFocus);
   fitted = FALSE;
   if (dbg) fprintf(stdout, "xFrame::xFrame():Exit\n");
   if (dbg) fflush(stdout);
}

xFrame::xFrame(xWidgetResInfo *pPRes, int align, int resz, int space,
               QWidget *pParent, const char *pName) :
        QFrame(pParent, pName)
{
   if (dbg) fprintf(stdout, "xFrame::xFrame():Enter\n");
   wdtRes = new xWidgetResInfo(pPRes, QString(""), QString(""));
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   fc = 0;
//   initMetaObject();
   widgetAlgn = align;
   widgetResz = resz;
   widgetSpac = space;
   widgetVMarg = 10;
   widgetHMarg = 10;
   widgetEqualize = TRUE;
   
   widgetListLength = 10;
   widgetCount = 0;
   fitFrameRecurse = 0;
   setFocusPolicy(StrongFocus);
   fitted = FALSE;
   if (dbg) fprintf(stdout, "xFrame::xFrame():Exit\n");
}

xFrame::~xFrame()
{
   widgetList.setAutoDelete(FALSE);
}

void xFrame::setAutoDelete(bool f)
{
   widgetList.setAutoDelete(f);
}

void xFrame::coupleWidgets(QWidget *pWdt1, QWidget *pWdt2, int side)
{
   if (dbg) fprintf(stdout, "xFrame::coupleWidgets():Enter\n");
   if (dbg) fflush(stdout);
   if (pWdt1->name() != NULL)
      couplings.insert(pWdt1->name(), new xWdtSpec(pWdt2, side));
   else
      fprintf(stderr, "xFrame::coupleWidgets: Unnamed Widgets cannot be coupled\n");
   fitted = FALSE;
   if (dbg) fprintf(stdout, "xFrame::coupleWidgets():Exit\n");
   if (dbg) fflush(stdout);
}
   
int xFrame::addWidget(QWidget *pWidget, QWidget *pNeighbor, int relativePos)
{
   unsigned int iOfs;
   int rv;
   QWidget *pNext;
   
   if (dbg) fprintf(stdout, "xFrame::addWdiget(%s:%s):Enter, fc = %d\n", pWidget->className(), pWidget->name(), fc);
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xFrame::addWidget():pWidget = %d\n", (int)pWidget);
   if (dbg) fflush(stdout);
   rv = 0;
   
   iOfs = 0;
   pNext = NULL;
   if (pNeighbor != NULL && !widgetList.isEmpty())
   {
      for (pNext = widgetList.first(); pNext != NULL; pNext = widgetList.next())
      {
         if (pNext == pNeighbor)
            break;
         iOfs++;
      }
   }
   if (pNext == NULL)
      widgetList.append(pWidget);
   else
   {
      iOfs = (relativePos == xPOS_Right && iOfs < widgetList.count()) ? 
             iOfs++ : iOfs;
      widgetList.insert(iOfs, pWidget);
   }
   if (dbg) fprintf(stdout, "xFrame::addWidget():exit(%d)\n", rv);
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xFrame::addWdiget(%s:%s):Exit, fc = %d\n", pWidget->className(), pWidget->name(), fc);
   if (dbg) fflush(stdout);
   fitted = FALSE;
   return(rv);
}

void xFrame::resizeEvent(QResizeEvent *pEvt)
{
   QSize sizeTmp;
   
   if (dbg) fprintf(stdout, "xFrame::resizeEvent(%s:%s):Enter, fc = %d\n", className(), name(), fc);
   if (dbg) fflush(stdout);
   sizeTmp = pEvt->size();
//   fitFrame(&sizeTmp);
   QFrame::resizeEvent(pEvt);
   if (dbg) fprintf(stdout, "xFrame::resizeEvent(%s:%s):Exit, fc = %d\n", className(), name(), fc);
   if (dbg) fflush(stdout);
}

void xFrame::resize(QSize *frameSize)
{
   if (dbg) fprintf(stdout, "xFrame::resize(%s:%s):Enter\n", className(), name());
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xFrame::resize(QSize*):frameSize.w = %d, .h = %d\n",
                    frameSize->width(), frameSize->height());
   if (dbg) fflush(stdout);
   fitFrame(frameSize);
   if (dbg) fprintf(stdout, "xFrame::resize(QSize*):Calling QWidget::resize()\n");
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xFrame::resize(%s:%s):fc = %d\n", className(), name(), fc);
   if (dbg) fflush(stdout);
   QWidget::resize(*frameSize);
   if (dbg) fprintf(stdout, "xFrame::resize(%s:%s):Exit\n", className(), name());
   if (dbg) fflush(stdout);
}

void xFrame::fitFrame(const QSize *frameSize, bool force)
{
   int maxSize = 0, offset, totalSize = 0, currentSize;
   int frameH = 0, frameW = 0, offsetV, offsetH;
   int x = 0, x1, x2, x3, x4, x5;
   QListIterator<QWidget> it(widgetList);
   QWidget *pWidget = it.current();

   if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Enter\n", fc, className(), name());
   if (dbg) fflush(stdout);
/*
   if (dbg) fprintf(stdout, "xFrame::fitFram(%s:%s):Algn = %x\n", className(), name(), widgetAlgn);
   if (dbg) fprintf(stdout, "xFrame::fitFram(%s:%s):Spac = %x\n", className(), name(), widgetSpac);
   if (dbg) fprintf(stdout, "xFrame::fitFram(%s:%s):LstL = %x\n", className(), name(), widgetListLength);
   if (dbg) fprintf(stdout, "xFrame::fitFram(%s:%s):Cnt  = %x\n", className(), name(), widgetCount);
   if (dbg) fprintf(stdout, "xFrame::fitFram(%s:%s):VMrg = %x\n", className(), name(), widgetVMarg);
   if (dbg) fprintf(stdout, "xFrame::fitFram(%s:%s):HMrg = %x\n", className(), name(), widgetHMarg);
   if (dbg) fprintf(stdout, "xFrame::fitFram(%s:%s):Resz = %x\n", className(), name(), widgetResz);
   if (dbg) fprintf(stdout, "xFrame::fitFram(%s:%s):Eqal = %x\n", className(), name(), widgetEqualize);
   if (dbg) fprintf(stdout, "xFrame::fitFram(%s:%s):fFR  = %x\n", className(), name(), fitFrameRecurse);
   if (dbg) fprintf(stdout, "xFrame::fitFram(%s:%s):fc   = %x\n", className(), name(), fc);
//   memset(buf, 0, sizeof(buf));
//   memcpy(buf, &fitFrameRecurse, 128);
//   if (dbg) fprintf(stdout, "xFrame::fitFram(%s:%s):buf = |%s|\n", className(), name(), buf);
//   if (dbg) fflush(stdout);
   if (fc > 10)
      fc = 0;
   if (widgetEqualize > 1)
      widgetEqualize = 0;
*/
   if (force != TRUE && (fc > 1 || fitted == TRUE))
   {
/*
      if (dbg) fprintf(stdout, "xFrame::fitFram(%s:%s):Algn = %x\n", className(), name(), widgetAlgn);
      if (dbg) fprintf(stdout, "xFrame::fitFram(%s:%s):Spac = %x\n", className(), name(), widgetSpac);
      if (dbg) fprintf(stdout, "xFrame::fitFram(%s:%s):LstL = %x\n", className(), name(), widgetListLength);
      if (dbg) fprintf(stdout, "xFrame::fitFram(%s:%s):Cnt  = %x\n", className(), name(), widgetCount);
      if (dbg) fprintf(stdout, "xFrame::fitFram(%s:%s):VMrg = %x\n", className(), name(), widgetVMarg);
      if (dbg) fprintf(stdout, "xFrame::fitFram(%s:%s):HMrg = %x\n", className(), name(), widgetHMarg);
      if (dbg) fprintf(stdout, "xFrame::fitFram(%s:%s):Resz = %x\n", className(), name(), widgetResz);
      if (dbg) fprintf(stdout, "xFrame::fitFram(%s:%s):Eqal = %x\n", className(), name(), widgetEqualize);
      if (dbg) fprintf(stdout, "xFrame::fitFram(%s:%s):fFR  = %x\n", className(), name(), fitFrameRecurse);
      if (dbg) fprintf(stdout, "xFrame::fitFram(%s:%s):Exit\n", className(), name());
      if (dbg) fflush(stdout);
*/
      return;
   }
   fc++;
   widgetCount = widgetList.count();
   move(0, 0);
   
   if (frameSize != NULL)
   {
      if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):frameSize.w = %d, .h = %d\n", fc, className(), name(),
                       frameSize->width(), frameSize->height());
      if (dbg) fflush(stdout);
   }
   
   //
   // Start by fitting any child frames that have been added
   // to this frame. Always allow the children to size themselves.
   // 
   if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Frame Name: |%s|\n", fc, className(), name(), name());
   if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Frame Class: |%s|\n", fc, className(), name(), className());
   if (parent() != NULL)
   {
      if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Parents Frame Name: |%s|\n", fc, className(), name(), parent()->name());
      if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Parents Frame Class: |%s|\n", fc, className(), name(), parent()->className());
      if (dbg) fflush(stdout);
   }
   for (x = 0, it.toFirst(); it.current(); ++it, x++)
   {
      pWidget = it.current();
      if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Testing to fit child frame %d\n",
                               fc, className(), name(), x);
      if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Frame %d Name: |%s|\n",
                               fc, className(), name(), x, pWidget->name());
      if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Frame %d Class: |%s|\n",
                               fc, className(), name(), x, pWidget->className());
      if (dbg) fflush(stdout);
      if (pWidget->inherits("xFrame"))
      {
         if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Widget %d is child frame\n", fc, className(), name(), x);
         if (dbg) fflush(stdout);
         if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Resizing child frame\n", fc, className(), name());
         ((xFrame *)pWidget)->fitFrame();
         if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Done!\n", fc, className(), name());
      }
      else if (pWidget->inherits("xTable"))
      {
         if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Widget %d is child table\n", fc, className(), name(), x);
         if (dbg) fflush(stdout);
         if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Resizing child frame\n", fc, className(), name());
         ((xTable *)pWidget)->fitFrame();
         if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Done!\n", fc, className(), name());
      }
   }
      
   //
   // Now see who's the largest of the bunch. This will keep them 
   // from looking jagged as they go down or across
   //
   if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Finding largest frame\n", fc, className(), name());
   if (dbg) fflush(stdout);
   for (x = 0, maxSize = 0, it.toFirst(); it.current(); ++it, x++)
   {
      pWidget = it.current();
      if (widgetAlgn == xALIGN_Vert)
         maxSize = (maxSize > pWidget->size().width()) ? maxSize :
                                                         pWidget->size().width();
      else
         maxSize = (maxSize > pWidget->size().height()) ? maxSize :
                                                          pWidget->size().height();
      if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Widget[%d].w = %d .h = %d\n", fc, className(), name(), x,
                                pWidget->size().width(),
                                pWidget->size().height());
   }
   if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):maxSize = %d\n", fc, 
                            className(), name(), maxSize);
   if (dbg) fflush(stdout);
   
   //
   // Now make every one the same size as the largest Object
   // If we resize a xFrame then fit it also
   //
   if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):sizing frames\n", fc, className(), name());
   if (dbg) fflush(stdout);
   for (x = 0, it.toFirst(); widgetEqualize && it.current(); ++it, x++)
   {
      pWidget = it.current();
      if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Resizing Widget %d\n", fc, className(), name(), x);
      if (widgetAlgn == xALIGN_Vert)
         pWidget->QWidget::resize(maxSize, pWidget->size().height());
      else
         pWidget->QWidget::resize(pWidget->size().width(), maxSize);
         
      if (pWidget->inherits("xFrame"))
      {
         QSize size;
         if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Widget %d is child frame\n", fc, className(), name(), x);
         if (dbg) fflush(stdout);
         size = pWidget->size();
         ((xFrame *)pWidget)->fitFrame(&size);
      }
   }
   
   //
   // Okay hear comes the hard part! Fit them into the window
   // Using the constraints given!
   //
   // Start by getting the total width (height) needed by all the
   // Widgets in the frame.
   //
   if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Tabulating required width/height\n",
                             fc, className(), name());
   if (dbg) fflush(stdout);
   for (totalSize = 0, it.toFirst(); it.current(); ++it)
   {
      pWidget = it.current();
      if (widgetAlgn == xALIGN_Vert)
         totalSize += pWidget->size().height();
      else
         totalSize += pWidget->size().width();
      if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):TotalSize: %d\n",
                                fc, className(), name(), totalSize);
      if (dbg) fflush(stdout);
   }

   //
   // Okay, if we can resize the frame, then we will use the given
   // parameters. Otherwise, fit them in to suit, if the frame is too
   // small just ignore the request!
   //
   ///////////////////////////////////////////////////////////////////////
   //
   // Okay, position the Widgets starting at the top (vert) or left (horz)
   // margins spacing them as requested.
   //
   offsetH = frameWidth() + widgetHMarg;
   offsetV = frameWidth() + widgetVMarg;
   
   if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Adjusting widget positions\n", fc, className(), name());
   if (dbg) fflush(stdout);
   if (frameSize == NULL)
   {
      for (x = 0, it.toFirst();
           it.current();
           ++it, x++)
      {
         pWidget = it.current();
         if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):moving widget %d to h=%d, v=%d\n",
                                  fc, className(), name(), x, offsetH, offsetV);
         if (dbg) fflush(stdout);
         pWidget->move(offsetH, offsetV);
            
         if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):calculating offset for next widget\n",
                                  fc, className(), name());
         if (dbg) fflush(stdout);
         if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Widget size: h=%d X w=%d\n",
                                  fc, className(), name(),
                                  pWidget->size().height(),
                                  pWidget->size().width());
         if (dbg) fflush(stdout);
         if (widgetAlgn == xALIGN_Vert)
            offsetV += pWidget->size().height();
         else
            offsetH += pWidget->size().width();
            
         if (x < widgetCount - 1)
         {
            if (widgetAlgn == xALIGN_Vert)
               offsetV += widgetSpac;
            else
               offsetH += widgetSpac;
         }
      }
      offset = (widgetAlgn == xALIGN_Vert) ? offsetV + widgetVMarg :
                                                  offsetH + widgetHMarg;
      
      if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Total width/height = %d\n",
                               fc, className(), name(), offset);
      if (dbg) fflush(stdout);
      
      //
      // Now resize the frame to fit snuggly around the Widgets, also set the
      // minimum size of the frame to the size calculated
      //
      if (widgetAlgn == xALIGN_Vert)
      {
         if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Setting frame size to: %d X %d\n",
                          fc, className(), name(),
                          maxSize + (widgetHMarg * 2), offset);
         if (dbg) fflush(stdout);
         frameH = offset + (frameWidth() * 2);
         frameW = maxSize + (widgetHMarg * 2) +
                  (frameWidth() * 2);
      }
      else
      {
         if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Setting frame size to: %d X %d\n", fc, className(), name(),
                          offset, maxSize + (widgetVMarg * 2));
         if (dbg) fflush(stdout);
         frameW = offset + (frameWidth() * 2);
         frameH = maxSize + (widgetVMarg * 2) +
                  (frameWidth() * 2);
      }
      if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Setting frame size to: w=%d X h=%d\n", fc, className(), name(),
                       frameW, frameH);
      if (dbg) fflush(stdout);
      QWidget::resize(frameW, frameH);
   }
   
   //
   // Okay, the frame is being resized! The default is to position the
   // widgets in the frame to fit within the given margins. Otherwise, if 
   // specified, the widgets will be resized to fit evenly across the
   // frame preserving the spacing and margins as best we can.
   //
   else
   {
      //
      // Here we calculate the distance to seperate the Widgets to
      // evenly space them across (or down) the frame, whose 
      // size is specified by frameSize, taking into account
      // the Margins set by wigetVMarg and widgetHMarg.
      //
      if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Frame is being resized\n", fc, className(), name());
      if (dbg) fflush(stdout);
      
      currentSize = (widgetAlgn == xALIGN_Vert) ? frameSize->height() :
                                                   frameSize->width();
      x3 = 2 * ((widgetAlgn == xALIGN_Vert) ? widgetVMarg : widgetHMarg);
      x4 = 2 * frameWidth();
      x1 = currentSize - (totalSize + x3 + x4);
      if (widgetResz == xSPACE_ToFit)
         x2 = x1 / (widgetCount + 1);
      else
         x2 = 0;
      
      if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):currentSize = %d, totalSize = %d, x1 = %d, x2 = %d, x3 = %d, x4 = %d\n",
                                fc, className(), name(),
                                currentSize, totalSize, x1, x2, x3, x4);
      
      if (widgetResz != xSPACE_ToFit)
      {
         if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Sizing Widgets to fit\n", fc, className(), name());
         currentSize = (widgetAlgn == xALIGN_Vert) ? frameSize->width() :
                                                      frameSize->height();
         x3 = 2 * ((widgetAlgn == xALIGN_Vert) ? widgetHMarg : widgetVMarg);
         x4 = 2 * frameWidth();
         x5 = (widgetAlgn == xALIGN_Vert) ? widgetList.first()->size().width() :
                                            widgetList.first()->size().height();
         x1 = currentSize - (x5 + x3 + x4);
         if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):currentSize = %d, totalSize = %d, x1 = %d, x3 = %d, x4 = %d, x5 = %d\n", fc, className(), name(),
                                   currentSize, totalSize, x1, x3, x4, x5);
      }
      
      offset = frameWidth() + x2;
      offset += (widgetAlgn == xALIGN_Vert) ? widgetVMarg : widgetHMarg;
      for (x = 0, it.toFirst();
           it.current();
           ++it, x++)
      {
         pWidget = it.current();
         if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Offset to widget %d = %d\n", 
                                  fc, className(), name(),
                                  x, offset);
         
         if (widgetResz != xSPACE_ToFit)
         {
            if (widgetAlgn == xALIGN_Vert)
            {
               pWidget->resize(pWidget->size().width() + x1,
                               pWidget->size().height());
            }
            else
            {
               pWidget->resize(pWidget->size().width(),
                               pWidget->size().height() + x1);
            }
            if (pWidget->inherits("xFrame"))
            {
               if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Widget %d is child frame\n", fc, className(), name(), x);
               if (dbg) fflush(stdout);
               ((xFrame *)pWidget)->fitFrame();
            }
         }
         
         /*
         ** If the Widget is coupled to another, line it up w/ the other
         ** as specified
         */
         xWdtSpec *pWdtSpec = NULL;
         if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Testing if widget |%s| is coupled\n", 
                                  fc, className(), name(),
                                  pWidget->name());
         if (dbg) fflush(stdout);
         if (pWidget->name() != NULL &&
            (pWdtSpec = couplings[pWidget->name()]) != NULL)
         {
            if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Widget |%s| is coupled to Widget |%s|\n", 
                                  fc, className(), name(),
                                  pWidget->name(), pWdtSpec->pWdt->name());
            if (dbg) fflush(stdout);
            if (pWdtSpec->side == xCOUPLE_Left)
            {
               if (widgetAlgn == xALIGN_Vert)
               {
                  offset = pWdtSpec->pWdt->y();
                  if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Widget |%s| new offset is %d\n", 
                                        fc, className(), name(),
                                        pWidget->name(), offset);
                  if (dbg) fflush(stdout);
                  pWidget->move(widgetHMarg, offset);
               }
               else
               {
                  offset = pWdtSpec->pWdt->x();
                  if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Widget |%s| new offset is %d\n", 
                                        fc, className(), name(),
                                        pWidget->name(), offset);
                  if (dbg) fflush(stdout);
                  pWidget->move(offset, widgetVMarg);
               }
            }
            else
            {
               if (widgetAlgn == xALIGN_Vert)
               {
                  offset = (pWdtSpec->pWdt->y() + pWdtSpec->pWdt->height()) -
                           pWidget->height();
                  if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Widget |%s| new offset is %d\n", 
                                        fc, className(), name(),
                                        pWidget->name(), offset);
                  if (dbg) fflush(stdout);
                  pWidget->move(widgetHMarg, offset);
               }
               else
               {
                  offset = (pWdtSpec->pWdt->x() + pWdtSpec->pWdt->width()) -
                           pWidget->width();
                  if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Widget |%s| new offset is %d\n", 
                                        fc, className(), name(),
                                        pWidget->name(), offset);
                  if (dbg) fflush(stdout);
                  pWidget->move(offset, widgetVMarg);
               }
            }
         }

         /*
         ** Otherwise, move it to the current offset
         */
         else if (widgetAlgn == xALIGN_Vert)
         {
            if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Widget |%s| is not coupled\n", 
                                  fc, className(), name(),
                                  pWidget->name());
            if (dbg) fflush(stdout);
            pWidget->move(widgetHMarg, offset);
         }
         else
         {
            if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Widget |%s| is not coupled\n", 
                                  fc, className(), name(),
                                  pWidget->name());
            if (dbg) fflush(stdout);
            pWidget->move(offset, widgetVMarg);
         }
         
         /*
         ** Figure out the position of the next widget
         */   
         offset += (widgetAlgn == xALIGN_Vert) ?
            pWidget->size().height() : pWidget->size().width();
            
         if (widgetResz != xSPACE_Resize)
            offset += x2;
         else
            offset += widgetSpac;
         fitted = TRUE;
      }
   }
//   fitFrameRecurse--;
   if (dbg) fprintf(stdout, "xFrame::fitFram.%d(%s:%s):Exit\n", fc, className(), name());
   if (dbg) fflush(stdout);
   fc--;
}

void xFrame::removeWidget(QWidget *pWidget)
{
   widgetList.remove(pWidget);
}

void xFrame::setAlignment(int alignment)
{
   widgetAlgn = alignment;
}

void xFrame::setWidgetSpacing(int spacing)
{
   widgetSpac = spacing;
}

void xFrame::setMargins(int vertMargin, int horzMargin)
{
   widgetVMarg = vertMargin;
   widgetHMarg = horzMargin;
}

void xFrame::margins(int& vertMargin, int& horzMargin)
{
   vertMargin = widgetVMarg;
   horzMargin = widgetHMarg;
}

void xFrame::doubleClickEvent(QMouseEvent *pEvt)
{
   pEvt = pEvt;

/*
   if (dbg) fprintf(stdout, "xFrame::doubleClickEvent(%s):Have Double Click!\n", className());
   for (x = 0; x < widgetCount; x++)
   {
      if (pWidgetList[x]->inherits("QWidget"))
      {
         if (pWidgetList[x]->rect().contains(pEvt->pos(), TRUE))
         {
            if (dbg) fprintf(stdout, "xFrame::doubleClickEvent(%s):Sending Event to child: %d:%s\n",
                                     className(), x, pWidgetList[x]->className());
//            pWidgetList[x]->mouseDoubleClickEvent(pEvt);
         }
      }
   }
   if (dbg) fprintf(stdout, "xFrame::doubleClickEvent(%s):Exit\n", className());
*/
}

/*
void xFrame::mousePressEvent(QMouseEvent *pEvt)
{
   QFrame::mousePressEvent(pEvt);
}

void xFrame::mouseReleaseEvent(QMouseEvent *pEvt)
{
   QFrame::mouseReleaseEvent(pEvt);
}

void xFrame::mouseDoubleClickEvent(QMouseEvent *pEvt)
{
   QFrame::mouseDoubleClickEvent(pEvt);
}

void xFrame::mouseMoveEvent(QMouseEvent *pEvt)
{
   QFrame::mouseMoveEvent(pEvt);
}

void xFrame::keyPressEvent(QKeyEvent *pEvt)
{
   int *ip;

   for (ip = keyTable; *ip > 0; ip += 3)
   {
      if (*ip <= pEvt->key() && *(ip + 1) >= pEvt->key() &&
          (*(ip + 2) < 0 || *(ip + 2) & pEvt->state() != 0))
         break;
   }

   if (*ip > 0)
      pEvt->ignore();
   else
      QFrame::keyPressEvent(pEvt);
}

void xFrame::keyReleaseEvent(QKeyEvent *pEvt)
{
   int *ip;

   for (ip = keyTable; *ip > 0; ip += 3)
   {
      if (*ip <= pEvt->key() && *(ip + 1) >= pEvt->key() &&
          (*(ip + 2) < 0 || *(ip + 2) & pEvt->state() != 0))
         break;
   }

   if (*ip > 0)
      pEvt->ignore();
   else
      QFrame::keyReleaseEvent(pEvt);
}
*/

//#include "xFrame.moc"

