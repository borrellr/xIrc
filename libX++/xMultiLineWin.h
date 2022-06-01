/***************************************************************************
**    xMultiLinWin.h  $Revision: 1.1.1.1 $ - $Name:  $ 
**    QWidget for manipulating Multiple lines of text in a window
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
#ifndef _xMULTILINEWIN_H
#define _xMULTILINEWIN_H

#include <qwidget.h>
#include "xResources.h"
#include "xMultiLineBuf.h"

struct PaintAttr {
   bool     bold;
   bool     underline;
   bool     italic;
   bool     inverse;
   bool     strikeout;
   int      font;
   int      linespacing;
   int      baseline;
   QColor   fg;
   QColor   bg;
};

class xMultiLineWin : public QWidget, public xMultiLineBuf
{
   Q_OBJECT

public:
   xMultiLineWin(xWidgetResInfo *pPRes, QWidget *pParent = NULL,
                 const char *pName = NULL,
                 int width = 80, int height = 24,
                 int bufHeight = 200);
   ~xMultiLineWin();
   
   virtual const QString   *putString(const char *pStr);
   virtual int             getPosition() { return(botLineOfs); };
   void                    drawChar(QPainter *p, int &y, int x, int ch);
   virtual void            mouseDoubleClickEvent(QMouseEvent *pEvt);
   void                    setColors(int *pRgb);

signals:
   void changePageSize(int height);
   void linesUsedChanged(int lines);
   void textSelected(QString);

public slots:
   void scrollWindow(int lines);
   void scrollToLine(int line);

protected slots:
   void inputText(const char *pText) { putString(pText); };
   void scrollWindowImage(int lines);
   void updateImage(int lines);
   
protected:
   bool putWord(QString *pWord, QString *pLine, const char ch, int *pCnt);
   virtual void   paintEvent(QPaintEvent *pEvt);
   virtual void   resizeEvent(QResizeEvent *pEvt);
   virtual bool   testForScroll();
   
   QFont          f;
   QFont          fontBold;
   QColor         boldColor;
   QColor         fgColor;
   int            botLineOfs;
   int            windowHeight;
   int            windowWidth;
   int            bufferLen;

private:
   const char  *parseColor(const char *cp, PaintAttr *attr);
   void        setAttributes(QPainter *p, struct PaintAttr *attr);
   QColor      gleanColor(const char **p);
   void        hex2rgb(const char **p, int *r, int *g, int *b);

   xWidgetResInfo *wdtRes;
};

#endif
