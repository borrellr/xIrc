/***************************************************************************
**    xMultiLinWin.cpp  $Revision: 1.3 $ - $Name:  $ 
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
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <qpainter.h>
#include "xMisc.h"
#include "xResources.h"
#include "xMultiLineWin.h"

static int dbg = 0;

#define WIN_MARGIN   2

static int rgbtbl[16][3] =
{
   {  0,   0,   0},  /* 0: Black       */
   {  0,   0, 128},  /* 1: Blue        */
   {  0, 128,   0},  /* 2: Grean       */
   {  0, 128, 128},  /* 3: Cyan        */
   {128,   0,   0},  /* 4: Red         */
   {128,   0, 128},  /* 5: Purple      */
   {128, 128,   0},  /* 6: Brown       */
   {204, 204, 204},  /* 7: Lt Gray     */
   {128, 128, 128},  /* 8: Gray        */
   {  0,   0, 255},  /* 9: Lt Blue     */
   {  0, 255,   0},  /* A: Lt Green    */
   {  0, 255, 255},  /* B: lt Cyan     */
   {255,   0,   0},  /* C: Lt Red      */
   {255,   0, 255},  /* D: Pink        */
   {255, 255,   0},  /* E: Yellow      */
   {255, 255, 255},  /* F: White       */
};

static const char *pInitialResources[] =
{
   "*MultiLine.Window.Font.Family: Fixed",
   "*MultiLine.Window.HighColor: white",
   "*MultiLine.Window.TextColor: grey72",
   "*MultiLine.Window.Background: #001bbd",
   NULL
};

xMultiLineWin::xMultiLineWin(xWidgetResInfo *pPRes, QWidget *pParent,
                             const char *pName,
                             int width, int height,
                             int bufHeight) :
                  QWidget(pParent, pName, (WFlags)0), xMultiLineBuf(bufHeight)
{
   botLineOfs = 0;
   windowHeight = height;
   windowWidth = width;
   bufferLen = bufHeight;
   char size[20];
   const char *pHighLight;

   wdtRes = new xWidgetResInfo(pPRes, QString("multiline.window"), QString("MultiLine.Window"));
   Resources->setWidgetInit(pInitialResources);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   setFocusPolicy(QWidget::NoFocus);

   f = font();

   sprintf(size, "%d", font().pointSize());
   if ((pHighLight = Resources->get(wdtRes, "highcolor", "HighColor"))
       == NULL)
      pHighLight = "yellow";

   boldColor.setNamedColor(pHighLight);

   QFontMetrics fm = QWidget::fontMetrics();
   setGeometry(0, 0, fm.maxWidth() * width + (WIN_MARGIN * 2), 
                     fm.lineSpacing() * height);
   setSizeIncrement(fm.maxWidth(), fm.lineSpacing());
   setMinimumSize(fm.maxWidth() * 10, fm.lineSpacing() * 3);
}

xMultiLineWin::~xMultiLineWin()
{
}

bool xMultiLineWin::putWord(QString *pWord, QString *pLine, const char ch, int *pCnt)
{
   bool rv = FALSE;

   if (dbg) fprintf(stdout, "MultiLineWin::putWord():Enter\n");
   if (dbg) fflush(stdout);
   if ((signed)(*pCnt) < windowWidth)
   {
      if (dbg) fprintf(stdout, "MultiLineWin::putWord():adding word: |%s| to line\n",
         (const char *)*pWord);
      *pLine += *pWord;
      if (dbg) fprintf(stdout, "MultiLineWin::putWord():line now: |%s|\n",
         (const char *)*pLine);
      if (dbg) fflush(stdout);
   }
   else
   {
      if (dbg) fprintf(stdout, "MultiLineWin::putWord():Line to long for one line\n");
      if (dbg) fprintf(stdout, "MultiLineWin::putWord():adding line |%s| to screen\n", 
                               (const char *)*pLine);
      if (dbg) fflush(stdout);
      if (getLinesUsed() < windowHeight || botLineOfs == getLinesUsed())
         botLineOfs = (botLineOfs == bufferLen - 1) ? bufferLen - 1 : botLineOfs + 1;
      if (dbg) fprintf(stdout, "MultiLineWin::putWord():Lines used = %d, botLineOfs = %d\n",
                               getLinesUsed(), botLineOfs);
      if (ch == '\0' && strlen(*pWord) > 0)
         *pLine += '\xff';
      rv = xMultiLineBuf::putString(*pLine);
      if (testForScroll())
      {
         if (dbg) fprintf(stdout, "MultiLineWin::putWord():Need to scroll 1 line\n");
         if (dbg) fflush(stdout);
         scrollWindowImage(1);
      }
      *pLine = *pWord;
      *pCnt = strlen(*pWord);
   }
   *pWord = "";
   if (ch == '\0')
   {
      if (strlen(*pLine) > 0)
      {
         *pLine += '\xff';
         if (dbg) fprintf(stdout, "MultiLineWin::putWord():Reached the end of the string\n");
         if (dbg) fprintf(stdout, "MultiLineWin::putWord():adding line |%s| to screen\n", 
                                  (const char *)*pLine);
         if (getLinesUsed() < windowHeight || botLineOfs == getLinesUsed())
            botLineOfs = (botLineOfs == bufferLen - 1) ? bufferLen - 1 : botLineOfs + 1;

         if (dbg) fprintf(stdout, "MultiLineWin::putWord():Lines used = %d, botLineOfs = %d\n",
                                  getLinesUsed(), botLineOfs);
         rv = xMultiLineBuf::putString(*pLine);
         if (testForScroll())
         {
            if (dbg) fprintf(stdout, "MultiLineWin::putWord():Need to scroll 1 line\n");
            if (dbg) fflush(stdout);
               scrollWindowImage(1);
         }
      }
      rv = TRUE;
   }
   else
      rv = FALSE;
   if (dbg) fprintf(stdout, "MultiLineWin::putWord(%d):Exit\n", rv);
   if (dbg) fflush(stdout);
   return(rv);
}

const QString *xMultiLineWin::putString(const char *pStr)
{
   QString tmpStr, tmpWord, tmpFormat;
   const QString *rv;
   const char *cp;
   char ch;
   int linesUsed, chCnt;
   bool inEscape = FALSE, inFormat = FALSE;

   if (dbg) fprintf(stdout, "MultiLineWin::putString(\"%s\"):Enter\n", pStr);
   if (dbg) fflush(stdout);
   linesUsed = getLinesUsed();
   tmpStr = "";
   tmpWord = "";
   rv = NULL;
   for (chCnt = 0, cp = pStr; ch != '\0'; cp++)
   {
      ch = *cp;
      if (ch != '\r' && ch != '\n')
      {
         if (!inEscape)
         {
            if (ch == '\\')
            {
               inEscape = TRUE;
               tmpWord += ch;
               continue;
            }
            if (inFormat)
            {
               tmpFormat += ch;
               if (ch == ']' || ch == '\0')
               {
                  if (dbg) fprintf(stdout, "MultiLineWin::putString():End of Format line\n");
                  if (dbg) fflush(stdout);
                  if (dbg) fprintf(stdout, "MultiLineWin::putString():Added format: |%s| to line\n",
                                    (const char *)tmpFormat);
                  if (ch != ']')
                     tmpStr += '\\';
                  tmpStr += tmpFormat;
                  tmpFormat = "";
                  inFormat = FALSE;
                  continue;
               }
            }
            else if (ch == '[')
            {
               if (dbg) fprintf(stdout, "MultiLineWin::putString():Start of Format line\n");
               if (dbg) fflush(stdout);
               putWord(&tmpWord, &tmpStr, ch, &chCnt);
               if (dbg) fprintf(stdout, "MultiLineWin::putString():Added word: |%s| to line\n",
                                 (const char *)tmpWord);
               if (dbg) fflush(stdout);
               tmpWord = "";
               tmpFormat += ch;
               inFormat = TRUE;
            }
         }
         if (!inFormat && (isspace(ch) || ch == '\0'))
         {
            inEscape = FALSE;
            if (dbg) fprintf(stdout, "MultiLineWin::putString():Reached end of a word or the string\n");
            if (dbg) fflush(stdout);
            if (ch == '\t')
               ch = ' ';
            if (ch != '\0')
            {
               tmpWord += ch;
               chCnt++;
            }
            if (dbg) fprintf(stdout, "MultiLineWin::putString():chCnt = %d, word length = %d, Window Width = %d, |%s|\n",
                             chCnt, strlen(tmpWord), windowWidth, 
                             (const char *)tmpWord);
            if (putWord(&tmpWord, &tmpStr, ch, &chCnt) == TRUE)
               break;
         }
         else if (!inFormat)
         {
            chCnt++;
            tmpWord += ch;
            inEscape = FALSE;
         }
      }
   }
   if (getLinesUsed() != linesUsed)
      emit linesUsedChanged(getLinesUsed());
   if (getLinesUsed() <= windowHeight)
      updateImage(getLinesUsed() - linesUsed);
   if (dbg) fprintf(stdout, "MultiLineWin::putString():Exit!!\n");
   if (dbg) fflush(stdout);
   return(rv);
}

bool xMultiLineWin::testForScroll()
{
   if (dbg) fprintf(stdout, "MultiLineWin::testForScroll():Enter- lines used: %d, buffer len: %d\n", 
                            getLinesUsed(), bufferLen);
   if ((getLinesUsed() > windowHeight && botLineOfs == getLinesUsed()) || 
       (getLinesUsed() >= bufferLen - 1))
      return(TRUE);
   else
      return(FALSE);
}
     
void xMultiLineWin::scrollWindow(int lines)
{
   int x;
   
   x = botLineOfs;
   if (lines < 0)
   {
      lines = abs(lines);
      botLineOfs = (botLineOfs - lines < windowHeight) ? windowHeight : botLineOfs - lines;
   }
   else
      botLineOfs = (botLineOfs + lines > getLinesUsed()) ? 
                     getLinesUsed() : botLineOfs + lines;
   botLineOfs = (botLineOfs == bufferLen - 1) ? bufferLen - 1 : botLineOfs;
   scrollWindowImage(botLineOfs - x);
}

void xMultiLineWin::scrollToLine(int line)
{
   int x;

   if (dbg) fprintf(stdout, "MultiLineWin::scrollToLine():Enter, line = %d\n", line);
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "MultiLineWin::scrollToLine():botLineOfs = %d\n", botLineOfs);
   if (dbg) fflush(stdout);
   x = line - (botLineOfs - windowHeight);
   if (dbg) fprintf(stdout, "MultiLineWin::scrollToLine():x = %d\n", x);
   if (dbg) fflush(stdout);
   scrollWindow(x);
   if (dbg) fprintf(stdout, "MultiLineWin::scrollToLine(%d):Exit\n", line);
   if (dbg) fflush(stdout);
}

void xMultiLineWin::updateImage(int lines)
{
   QFontMetrics fm = QWidget::fontMetrics();
   QRect textRect;
   int textLineSpacing, textWindowWidth;
   
   if (dbg) fprintf(stdout, "MultiLineWin::updateImage(%d):Enter\n", lines);
   if (dbg) fflush(stdout);
   textLineSpacing = fm.lineSpacing();
   textWindowWidth = windowWidth * fm.maxWidth();
   textRect.setRect(0, (botLineOfs - lines) * textLineSpacing,
                    textWindowWidth, lines * textLineSpacing);
   
   if (lines > 0)
   {
      if (dbg) fprintf(stdout, "MultiLineWin::updateImage():Repainting the screen!\n");
      if (dbg) fflush(stdout);
      repaint(textRect, TRUE);
   }
   if (dbg) fprintf(stdout, "MultiLineWin::updateImage(%d):Exit\n", lines);
   if (dbg) fflush(stdout);
}

void xMultiLineWin::scrollWindowImage(int lines)
{
   QFontMetrics fm = QWidget::fontMetrics();
   QRect textRect;
   int textLineSpacing, textWindowWidth;
   int srcX = 0, srcY = 0, dstX = 0, dstY = 0;
   
   if (dbg) fprintf(stdout, "MultiLineWin::scrollWindowImage(%d):Enter\n", lines);
   
// if (dbg) fprintf(stdout, "MultiLineWin::scrollWindow():Scrolling %d lines\n", x);
   textLineSpacing = fm.lineSpacing();
   textWindowWidth = windowWidth * fm.maxWidth();
   if (lines > 0)
   {
      dstX = dstY = srcX = 0;
      srcY = lines * textLineSpacing;
      textRect.setRect(0, (windowHeight - lines) * textLineSpacing, 
                        textWindowWidth, lines * textLineSpacing);
   }
   else if (lines < 0)
   {
      lines = abs(lines);
      srcX = dstX = srcY = 0;
      dstY = lines * textLineSpacing;
      textRect.setRect(0, 0, textWindowWidth, lines * textLineSpacing);
   }
   
   if (lines != 0)
   {
      if (dbg) fprintf(stdout, "MultiLineWin::scrollWindow():moving %d to %d\n", srcY, dstY);
      bitBlt(this, dstX, dstY, this, srcX, srcY,
             textWindowWidth, (windowHeight - lines) * textLineSpacing);
      repaint(textRect, TRUE);
   }
   if (dbg) fprintf(stdout, "MultiLineWin::scrollWindow(%d):Exit\n", lines);
}
   
void xMultiLineWin::resizeEvent(QResizeEvent *pEvt)
{
   QFontMetrics fm = QWidget::fontMetrics();
   QSize newSize, s;
   int width, height;
   
   if (dbg > 2) fprintf(stdout, "xMultiLineWin::resizeEvent():Enter\n");
   if (dbg > 2) fflush(stdout);
   newSize = pEvt->size();
   if (newSize.isValid())
   {
      if (dbg > 2) fprintf(stdout, "xMultiLineWin::resizeEvent():Getting new Height / width info\n");
      if (dbg > 2) fflush(stdout);
      height = (int)(newSize.height() / fm.lineSpacing());
      width  = (int)(newSize.width() / fm.maxWidth());
      if (dbg > 2) fprintf(stdout, "xMultiLineWin::resizeEvent():Setting Term buf\'s Height / width info\n");
      if (dbg > 2) fflush(stdout);
      windowHeight = height;
      windowWidth = width;
      s.setHeight(height * fm.lineSpacing());
      s.setWidth(width * fm.maxWidth() + WIN_MARGIN);
      if (s != newSize)
         resize(s);
      emit changePageSize(height);
   }
   if (dbg > 2) fprintf(stdout, "xMultiLineWin::resizeEvent():Calling original resizeEvent\n");
   QWidget::resizeEvent(pEvt);
   if (dbg > 2) fprintf(stdout, "xMultiLineWin::resizeEvent():Exit\n");
}

void xMultiLineWin::paintEvent(QPaintEvent *pEvt)
{
   QPainter p;
   QRect    r;
   QFont fnt;
   int      x, y, curLine, numLines = 0;
   int      topLine, botLine;
   int      haveNewLine;
   bool inEscape = FALSE;
   bool inFormat = FALSE;
   bool inFormatNOT = FALSE;
   bool inFormatIgnore = FALSE;
   bool resetMode = FALSE;
   const QString  *pStr;
   const char *cp;
   struct PaintAttr attr;
   
   if (dbg > 2) fprintf(stdout, "MultiLineWin::repaint():Enter\n");
   r = pEvt->rect();
   topLine = r.top();
   botLine = r.bottom();
   if (dbg > 2) fprintf(stdout, "MultiLineWin::repaint():topLine = %d, botLine = %d\n", topLine, botLine);
   
   p.begin(this);
   p.setBackgroundMode(BGMode(OpaqueMode));
   attr.bold = FALSE;
   attr.underline = FALSE;
   attr.italic = FALSE;
   attr.strikeout = FALSE;
   attr.inverse = FALSE;
   attr.font = 0;
   attr.linespacing = 0;
   attr.fg = colorGroup().text();
   attr.bg = colorGroup().background();
   setAttributes(&p, &attr); 
   if (dbg > 2) fprintf(stdout, "MultiLineWin::repaint():textLineSpacing = %d, textBaseLine = %d\n", 
                        attr.linespacing, attr.baseline);

   x = (((getLinesUsed() - 1) - botLineOfs) + windowHeight < 0) ? 
       0 : ((getLinesUsed() - 1) - botLineOfs) + windowHeight;
   pStr = pointLine(x, x, numLines);
   if (dbg > 2) fprintf(stdout, "MultiLineWin::repaint():offset = %d, Lines returned = %d\n", 
                                 x, numLines);
// numLines = pTermBuf->getTopLines(pText, windowHeight);
   haveNewLine = TRUE;
   for (x = curLine = 0; 
        haveNewLine && numLines && curLine < numLines;
        curLine++, pStr = getNext(pStr, haveNewLine))
   {
      if (!haveNewLine)
         continue;
      if (resetMode)
      {
         attr.bold = FALSE;
         attr.underline = FALSE;
         attr.italic = FALSE;
         attr.strikeout = FALSE;
         attr.inverse = FALSE;
         attr.font = 0;
         attr.fg = colorGroup().text();
         attr.bg = colorGroup().background();
         attr.linespacing = 0;
         setAttributes(&p, &attr); 
         resetMode = FALSE;
      }
      if (dbg > 2) fprintf(stdout, "MultiLineWin::repaint():Line offset = %d, textBaseLine = %d\n", 
                                    curLine * attr.linespacing, attr.baseline);
      if (dbg > 2) fprintf(stdout, "MultiLineWin::repaint():Drawing text: |%s|\n", 
            (const char *)*pStr);
      for (y = WIN_MARGIN, cp = (const char *)*pStr; *cp && resetMode == FALSE; cp++)
      {
         if (!inEscape)
         {
            /*
            ** if Escaping next char, just show the next char
            */
            if (*cp == '\\')
            {
               inEscape = TRUE;
               continue;
            }
            /*
            ** If begining of a format change, go into format change mode
            */
            else if (*cp == '[')
            {
               inFormat = TRUE;
               continue;
            }
            /*
            ** If end of line, reset all attributes to normal
            */
            else if ((unsigned char)(*cp) == 0xff)
            {
               resetMode = TRUE;
               continue;
            }
            /*
            ** If in format change mode and they want out, get out!
            */
            else if ((inFormat || inFormatNOT || inFormatIgnore) && *cp == ']')
            {
               inFormat = FALSE;
               inFormatNOT = FALSE;
               inFormatIgnore = FALSE;
               setAttributes(&p, &attr); 
               continue;
            }
            /*
            ** If in format change mode, parse the change accordingly
            */
            if (inFormat)
            {
               /*
               ** These Turn on attributes, including colors
               */
               inFormatIgnore = TRUE;
               inFormat = FALSE;
               if (*cp == 'C')
               {
                  if (dbg > 2) fprintf(stdout, "MultiLineWin::repaint():Have Color Spec\n");
                  cp = parseColor(++cp, &attr);
                  if (dbg > 2) fprintf(stdout, "MultiLineWin::repaint():Complete Color Spec Remaining line = |%s|\n",
                                             cp);
               }
               else if (*cp == 'B')
                  attr.bold = TRUE;
               else if (*cp == 'U')
                  attr.underline = TRUE;
               else if (*cp == 'I')
                  attr.italic = TRUE;
               else if (*cp == 'V')
                  attr.inverse = TRUE;
               else if (*cp == 'S')
                  attr.strikeout = TRUE;
               else if (*cp == 'F')
                  attr.font++;
               else if (*cp == 'N')
               {
                  attr.bold = FALSE;
                  attr.underline = FALSE;
                  attr.italic = FALSE;
                  attr.strikeout = FALSE;
                  attr.inverse = FALSE;
                  attr.font = 0;
                  attr.fg = colorGroup().text();
                  attr.bg = colorGroup().background();
               }
               /*
               ** Ones preceded by a bar negates the atrribute, show we are
               ** turning off the attribute
               */
               else if (*cp == '/')
               {
                  inFormatNOT = TRUE;
                  inFormat = FALSE;
               }
               else
               {
                  if (dbg > 2) fprintf(stdout, "MultiLineWin::repaint():Ignoring format spec |%c|, 0x%2x\n",
                                                *cp, *cp);
                  drawChar(&p, y, x + attr.baseline, '[');
                  drawChar(&p, y, x + attr.baseline, *cp);
                  inFormat = FALSE;
                  inFormatIgnore = FALSE;
                  inFormatNOT = FALSE;
               }
            }
            /*
            ** If the attribute is being turned off, turn it off!
            */
            else if (inFormatNOT)
            {
               if (*cp == 'C')
               {
                  attr.fg = colorGroup().text();
                  attr.bg = colorGroup().background();
               }
               else if (*cp == 'B')
                  attr.bold = FALSE;
               else if (*cp == 'U')
                  attr.underline = FALSE;
               else if (*cp == 'I')
                  attr.italic = FALSE;
               else if (*cp == 'V')
                  attr.inverse = FALSE;
               else if (*cp == 'S')
                  attr.strikeout = FALSE;
               else if (*cp == 'F')
                  attr.font--;
               else if (*cp == 'N')
               {
                  attr.bold = FALSE;
                  attr.underline = FALSE;
                  attr.italic = FALSE;
                  attr.strikeout = FALSE;
                  attr.font = 0;
                  attr.fg = colorGroup().text();
                  attr.bg = colorGroup().background();
               }
               inFormatNOT = FALSE;
               inFormatIgnore = TRUE;
            }
            /*
            ** Draw any non format chars here
            */
            else if (!inFormatIgnore)
               if (x + attr.linespacing >= topLine && x <= botLine)
                  drawChar(&p, y, x + attr.baseline, *cp);
         }
         /*
         ** Draw escaped characters here
         */
         else
         {
            if (x + attr.linespacing >= topLine && x <= botLine)
               drawChar(&p, y, x + attr.baseline, *cp);
            inEscape = FALSE;
         }
      }
      if (x > botLine)
         break;
      x += attr.linespacing;
   }
   p.end();
   
   if (dbg > 2) fprintf(stdout, "MultiLineWin::repaint():Exit\n");
}

void xMultiLineWin::setAttributes(QPainter *p, struct PaintAttr *attr)
{
   int fs, fs1;

   if (dbg) fprintf(stdout, "MultiLineWin::setAttributes():Enter\n");
   if (dbg) fflush(stdout);
   /*
   ** Determine which font to use
   */
   if (dbg) fprintf(stdout, "MultiLineWin::setAttributes():bold      = %d\n", attr->bold);
   if (dbg) fprintf(stdout, "MultiLineWin::setAttributes():underline = %d\n", attr->underline);
   if (dbg) fprintf(stdout, "MultiLineWin::setAttributes():italic    = %d\n", attr->italic);
   if (dbg) fprintf(stdout, "MultiLineWin::setAttributes():strikeout = %d\n", attr->strikeout);
   if (dbg) fflush(stdout);
   p->setFont(f);

   /*
   ** Get and normalize the font size requested
   */
   fs1 = attr->font;
   if (fs1 > 4)
      fs1 = 4;
   if (fs1 < -4)
      fs1 = -4;
   if (dbg) fprintf(stdout, "MultiLineWin::setAttributes():fs1 = %d\n", fs1);
   if (dbg) fflush(stdout);

   /*
   ** Set the font as desired
   */
   QFont fnt = p->font();
   fs = fnt.pointSize();
   if (dbg) fprintf(stdout, "MultiLineWin::setAttributes():pointsize = %d\n", fs + (fs1*2));
   if (dbg) fflush(stdout);
//   fnt.setPointSize(fs + (fs1 * 2));
   fnt.setBold(attr->bold);
   fnt.setItalic(attr->italic);
   fnt.setUnderline(attr->underline);
   fnt.setStrikeOut(attr->strikeout);
   p->setFont(fnt);

   /*
   ** Retrieve the font metrics needed for painting
   */
   QFontMetrics fm = p->fontMetrics();
   if (attr->linespacing < fm.lineSpacing())
      attr->linespacing = fm.lineSpacing();
   attr->baseline = fm.ascent();

   /*
   ** Set the colors as desired
   */
   if (dbg)
   {
      fprintf(stdout, "MultiLineWin::setAttributes():fg = %d/%d/%d\n", 
               attr->fg.red(), attr->fg.green(), attr->fg.blue());
      fprintf(stdout, "MultiLineWin::setAttributes():bg = %d/%d/%d\n", 
               attr->bg.red(), attr->bg.green(), attr->bg.blue());
      fflush(stdout);
   }
   if (attr->inverse)
   {
      p->setPen(attr->bg);
      p->setBackgroundColor(attr->fg);
   }
   else
   {
      p->setPen(attr->fg);
      p->setBackgroundColor(attr->bg);
   }
   if (dbg) fprintf(stdout, "MultiLineWin::setAttributes():Exit\n");
   if (dbg) fflush(stdout);
}

void xMultiLineWin::drawChar(QPainter *p, int &y, int x, int ch)
{
   char buf[2];

   buf[0] = ch;
   buf[1] = '\0';
   p->drawText(y, x, buf);
   y += p->fontMetrics().width(ch);   
}

const char *xMultiLineWin::parseColor(const char *cp, struct PaintAttr *attr)
{
   QColor fg = attr->fg;
   QColor bg = attr->bg;
   while (isspace(*cp)) cp++;
   if (*cp != '-' && *cp != '.')
      fg = gleanColor(&cp);
   else if (*cp != '.')
      fg = colorGroup().text();

   while (isspace(*cp)) cp++;
   if (*cp != '-' && *cp != '.')
      bg = gleanColor(&cp);
   else if (*cp != '.')
      bg = colorGroup().background();
   attr->fg = fg;
   attr->bg = bg;
   return(--cp);
}

QColor xMultiLineWin::gleanColor(const char **p)
{
   int r, g, b, i;
   const char *cp = *p;

   if (dbg) fprintf(stdout, "MultiLineWin::gleanColor():Enter\n");
   if (dbg) fflush(stdout);
   if (*cp != '#')
   {
      if (dbg) fprintf(stdout, "MultiLineWin::gleanColor():Have Index %x\n", toupper(*cp));
      if (dbg) fflush(stdout);
      i = toupper(*cp) - '0';
      if (dbg) fprintf(stdout, "MultiLineWin::gleanColor():i.1 = %x\n", i);
      if (dbg) fflush(stdout);
      if (i > 9)
         i -= 7;
      if (dbg) fprintf(stdout, "MultiLineWin::gleanColor():i = %x\n", i);
      if (dbg) fflush(stdout);
      r = rgbtbl[i][0];
      g = rgbtbl[i][1];
      b = rgbtbl[i][2];
      cp++;
   }
   else
   {
      cp++;
      hex2rgb(&cp, &r, &g, &b);
      if (dbg) fprintf(stdout, "MultiLineWin::gleanColor():r = %d, g = %d, b = %d\n", r, g, b);
      if (dbg) fflush(stdout);
   }
   *p = cp;
   if (dbg) fprintf(stdout, "MultiLineWin::gleanColor():Exit\n");
   if (dbg) fflush(stdout);
   return(QColor(r, g, b));
}

void xMultiLineWin::hex2rgb(const char **p, int *r, int *g, int *b)
{
   int x, y, i;
   const char *cp = *p;
   char rgb[3] = {0, 0, 0}, *rp;

   if (dbg) fprintf(stdout, "MultiLineWin::hex2rgb():Enter\n");
   if (dbg) fflush(stdout);
   for (rp = rgb, x = 0; x < 3; x++, rp++)
   {
      if (dbg) fprintf(stdout, "MultiLineWin::hex2rgb():ch = %c\n", toupper(*cp));
      if (dbg) fflush(stdout);
      for (y = 0; y < 2; y++, cp++)
      {
         if ((i = toupper(*cp) - '0') > 9)
            i -= 7;
         if (y == 0)
            *rp = (i & 0x0f)<<4;
         else
            *rp |= (i & 0x0f);
         if (dbg) fprintf(stdout, "MultiLineWin::hex2rgb():i = %x, rc = %x\n", i, *rp);
         if (dbg) fflush(stdout);
      }
   }
   *r = rgb[0];
   *g = rgb[1];
   *b = rgb[2];
   if (dbg) fprintf(stdout, "MultiLineWin::hex2rgb():r = %x, g = %x, b = %x\n", *r, *g, *b);
   if (dbg) fflush(stdout);
   *p = cp;
   if (dbg) fprintf(stdout, "MultiLineWin::hex2rgb():Exit\n");
   if (dbg) fflush(stdout);
}

void xMultiLineWin::setColors(int *pRgb)
{
   memcpy(rgbtbl, pRgb, sizeof(rgbtbl));
}

void xMultiLineWin::mouseDoubleClickEvent(QMouseEvent *pEvt)
{
   bool inFormat = FALSE, inEscape = FALSE;
   int botRow, topRow, x, y, curLine, lineSpace, haveNewLine, numLines;
   const char *cp, *pfEnd = NULL;
   QString word;
   const QString *pStr;

   QFontMetrics fm(f);
   lineSpace = fm.lineSpacing();
   if (dbg) fprintf(stdout, "MultiLineWin::mouseDoubleClickEvent():Enter");
   if (dbg) fprintf(stdout, "MultiLineWin::mouseDoubleClickEvent():mouse.x = %d\n", 
                             pEvt->pos().x());
   if (dbg) fprintf(stdout, "MultiLineWin::mouseDoubleClickEvent():mouse.y = %d\n",
                             pEvt->pos().y());
   if (dbg) fprintf(stdout, "MultiLineWin::mouseDoubleClickEvent():lineSpace = %d\n", lineSpace);
   fflush(stdout);
   x = (((getLinesUsed() - 1) - botLineOfs) + windowHeight < 0) ? 
       0 : ((getLinesUsed() - 1) - botLineOfs) + windowHeight;
   pStr = pointLine(x, x, numLines);
   if (dbg) fprintf(stdout, "MultiLineWin::mouseDoubleClickEvent():numLines = %d\n", numLines);
   fflush(stdout);
   haveNewLine = TRUE;
   for (x = curLine = 0; 
        haveNewLine && numLines && curLine < numLines;
        curLine++, pStr = getNext(pStr, haveNewLine))
   {
      topRow = curLine * lineSpace;
      botRow = topRow + lineSpace;
      if (dbg) fprintf(stdout, "MultiLineWin::mouseDoubleClickEvent():curLine = %d, top row = %d, bot row = %d\n",
                       curLine, topRow, botRow);
      fflush(stdout);
      if (pEvt->pos().y() >= topRow &&
          pEvt->pos().y() <= botRow)
      {
         if (dbg) fprintf(stdout, "MultiLineWin::mouseDoubleClickEvent():found row at %d\n", curLine);
         fflush(stdout);
         for (cp = *pStr, y = WIN_MARGIN; *cp != '\0'; cp++)
         {
            if (dbg) fprintf(stdout, "MultiLineWin::mouseDoubleClickEvent():ch = 0x%x,|%c|\n",
                                     *cp, *cp);
            fflush(stdout);
            if (!inFormat)
            {
               if (!inEscape && *cp == '[')
               {
                  inFormat = TRUE;
                  if (dbg) fprintf(stdout, "MultiLineWin::mouseDoubleClickEvent():in format\n");
                  fflush(stdout);
                  continue;
               }
               else if (!inEscape && *cp == '\\')
               {
                  inEscape = TRUE;
                  if (dbg) fprintf(stdout, "MultiLineWin::mouseDoubleClickEvent():in Escape\n");
                  fflush(stdout);
               }
               else
               {
                  inEscape = FALSE;
                  if (dbg) fprintf(stdout, "MultiLineWin::mouseDoubleClickEvent():Out of Escape\n");
                  fflush(stdout);
               }
               if (pEvt->pos().x() >= y && 
                   pEvt->pos().x() <= y + fm.width(*cp))
               {
                  if (dbg) fprintf(stdout, "MultiLineWin::mouseDoubleClickEvent():found col at %d\n", y);
                  fflush(stdout);
                  if (dbg) fprintf(stdout, "MultiLineWin::mouseDoubleClickEvent():pStr = %u, cp = %u, pfEnd = %d\n",
                                             (unsigned)pStr, (unsigned)cp, (unsigned)pfEnd);
                  fflush(stdout);
                  while ((isalnum(*cp) || 
                          *cp == '#' ||
                          *cp == '[' ||
                          *cp == ']' ||
                          *cp == '(' ||
                          *cp == ')' ||
                          *cp == '{' ||
                          *cp == '}' ||
                          *cp == '~' ||
                          *cp == '-' ||
                          *cp == '^' ||
                          *cp == '\'' ||
                          *cp == '`' ||
                          *cp == '_') && 
                          cp != (const char *)*pStr && cp != pfEnd + 1)
                  {
                     if (dbg) fprintf(stdout, "MultiLineWin::mouseDoubleClickEvent():backing up... ch = 0x%x,|%c|\n",
                                              *cp, *cp);
                     cp--;
                  }
                  if (!isalnum(*cp) && 
                          *cp != '#' &&
                          *cp != '[' &&
                          *cp != ']' &&
                          *cp != '(' &&
                          *cp != ')' &&
                          *cp != '{' &&
                          *cp != '}' &&
                          *cp != '~' &&
                          *cp != '-' &&
                          *cp != '^' &&
                          *cp != '\'' &&
                          *cp != '`' &&
                          *cp != '_')
                     cp++;

                  if (dbg) fprintf(stdout, "MultiLineWin::mouseDoubleClickEvent():At Start of word: |%s|\n",
                                   cp);
                  if (*cp == '[' || *cp == '#')
                     word += *(cp++);
                  while (*cp && *cp != '\xff')
                  {
                     if (!inEscape && *cp == '\\')
                         inEscape = TRUE;
   
                     if (!inEscape && *cp == '[')
                        break;
                     else
                         inEscape = FALSE;
   
                     if (isalnum(*cp) || 
                          *cp == '[' ||
                          *cp == ']' ||
                          *cp == '(' ||
                          *cp == ')' ||
                          *cp == '{' ||
                          *cp == '}' ||
                          *cp == '~' ||
                          *cp == '-' ||
                          *cp == '^' ||
                          *cp == '\'' ||
                          *cp == '`' ||
                          *cp == '_')
                     {
                        if (dbg) fprintf(stdout, "MultiLineWin::mouseDoubleClickEvent():Adding %c to word\n",
                                         *cp);
                        word += *cp;
                     }
                     else
                     {
                        if (dbg) fprintf(stdout, "MultiLineWin::mouseDoubleClickEvent():Found word: |%s|\n",
                                         (const char *)word);
                        break;
                     }
                     cp++;
                  }
                  break;
               }
               y += fm.width(*cp);
            }
            else if (*cp == ']')
            {
               inFormat = FALSE;
               pfEnd = cp;
               if (dbg) fprintf(stdout, "MultiLineWin::mouseDoubleClickEvent():out of format\n");
               fflush(stdout);
            }
         }
         if (strlen(word) > 0)
         {
            if (dbg) fprintf(stdout, "MultiLineWin::mouseDoubleClickEvent():Emitting word\n");
            fflush(stdout);
            emit textSelected(word);
         }
         break;
      }
   }
   if (dbg) fprintf(stdout, "MultiLineWin::mouseDoubleClickEvent():Exit\n");
   fflush(stdout);
}


//#include "xMultiLineWin.moc"
