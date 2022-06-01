/***************************************************************************
**    xEdit.cpp  $Revision: 1.3 $ - $Name:  $ 
**    QEdit w/ Macro Replacement
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
#include "xDefaults.h"
#include "xEdit.h"
#include <qlineedit.h>

extern xDefaults Defaults;

static int dbg = 0;

static const char *pInitialResources[] =
{
   "*BaseColor: light yellow",
   "*BaseHighlight: dark blue",
   "*TextColor: black",
   "*TextHighlight: White",
   NULL
};

xEdit::xEdit(xWidgetResInfo *pPRes, QWidget *pParent, const char *pName) :
        QLineEdit(pParent, pName)
{
   QPalette p;
   QColor baseColor;
   QColor baseHighlight;
   QColor textColor;
   QColor textHighlight;
   QColor textEnabled;
   QColorGroup n, a, d;
   const char *cp, *cp1;

   if (dbg) fprintf(stdout, "xEdit::xEdit():Enter\n");
   wdtRes = new xWidgetResInfo(pPRes, QString("input"), QString("Input"));
   Resources->setWidgetInit(pInitialResources);
   if (dbg) fprintf(stdout, "xEdit::xEdit():Width = %d, height = %d\n",
                            width(), height());
   if (dbg) fflush(stdout);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
   if (dbg) fprintf(stdout, "xEdit::xEdit():Defaults Set\n");
   if (dbg) fflush(stdout);
   
   highlight = FALSE;

   if (dbg) fprintf(stdout, "xEdit::xEdit():Setting Colors and such\n");
   if (dbg) fflush(stdout);
   p = palette();
   cp = Resources->get(wdtRes, "basecolor", "BaseColor");
   setStrColor(baseColor, cp);

   cp = Resources->get(wdtRes, "basehighlight", "BaseHighlight");
   setStrColor(baseHighlight, cp);

   cp = Resources->get(wdtRes, "texthighlight", "TextHighlight");
   setStrColor(textHighlight, cp);

   cp = Resources->get(wdtRes, "textcolor", "TextColor");
   setStrColor(textColor, cp);

   cp1 = Resources->get(wdtRes, "textenabled", "TextEnabled");
   if (cp1 != NULL)
      setStrColor(textEnabled, cp1);
   else
      setStrColor(textEnabled, cp);

   n = p.normal();
   a = p.active();
   d = p.disabled();
   QColorGroup n1(n.foreground(), n.background(),  n.light(), 
                  n.mid(), n.dark(), textEnabled, baseColor);
   QColorGroup a1(a.foreground(), a.background(),  a.light(), 
                  a.mid(), a.dark(), textEnabled, baseColor);
   QColorGroup d1(d.foreground(), d.background(),  d.light(), 
                  d.mid(), d.dark(), textColor, n.background());
   paletteEnabled.setNormal(n1);
   paletteEnabled.setActive(a1);
   paletteEnabled.setDisabled(d1);

   QColorGroup n2(n.foreground(), n.background(),  n.light(), 
                  n.mid(), n.dark(), textColor, n.background());
   QColorGroup a2(a.foreground(), a.background(),  a.light(), 
                  a.mid(), a.dark(), textColor, n.background());
   QColorGroup d2(d.foreground(), d.background(),  d.light(), 
                  d.mid(), d.dark(), textColor, n.background());
   paletteDisabled.setNormal(n2);
   paletteDisabled.setActive(a2);
   paletteDisabled.setDisabled(d2);

   QColorGroup n3(n.foreground(), n.background(),  n.light(), 
                  n.mid(), n.dark(), textHighlight, baseHighlight);
   QColorGroup a3(a.foreground(), a.background(),  a.light(), 
                  a.mid(), a.dark(), textHighlight, baseHighlight);
   QColorGroup d3(d.foreground(), d.background(),  d.light(), 
                  d.mid(), d.dark(), textHighlight, baseHighlight);
   paletteHighlight.setNormal(n3);
   paletteHighlight.setActive(a3);
   paletteHighlight.setDisabled(d3);

   if (dbg) fprintf(stdout, "xEdit::xEdit():Getting Font Metrics\n");
   if (dbg) fflush(stdout);
   QFontMetrics fm = fontMetrics();
   if (dbg) fprintf(stdout, "xEdit::xEdit():Setting to Width = %d, height = %d\n",
                            width(), fm.lineSpacing() + 8);
   resize(width(), fm.lineSpacing() + 8);
   if (dbg) fprintf(stdout, "xEdit::xEdit():New Width = %d, height = %d\n",
                            width(), height());
//   frameStyleEnabled = QFrame::Panel | QFrame::Sunken;
//   frameStyleDisabled = QFrame::Panel | QFrame::Plain;
   setEnabled(TRUE);
   QLineEdit::setEnabled(TRUE);
   setFocusPolicy(QWidget::StrongFocus);
   if (dbg) fprintf(stdout, "xEdit::xEdit():Exit\n");
}

void xEdit::setFrameStyles(int styleEnabled, int styleDisabled)
{
   if (styleEnabled != -1)
      frameStyleEnabled = styleEnabled;
   if (styleDisabled != -1)
      frameStyleDisabled = styleDisabled;
//   setFrameStyle((enabled == TRUE) ?
//                 frameStyleEnabled : frameStyleDisabled);
}

void xEdit::setWidth(int width)
{
   QFontMetrics fm = fontMetrics();
   if (dbg) fprintf(stdout, "xEdit::setWidth(%d):Enter\n", width);
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xEdit::setWidth():maxWidth = %d, width = %d\n",
                            fm.maxWidth(), fm.maxWidth() * width);
   if (dbg) fflush(stdout);
   resize(fm.maxWidth() * width + 8,  fm.lineSpacing() + 8);
   if (dbg) fprintf(stdout, "xEdit::setWidth():Exit\n");
   if (dbg) fflush(stdout);
}

void xEdit::setEnabled(bool f)
{
   if (dbg) fprintf(stdout, "xEdit::setEnabled():Enter\n");
   if (dbg) fflush(stdout);
   enabled = f;
   if (f == FALSE && highlight == FALSE)
   {
      if (dbg) fprintf(stdout, "xEdit::setEnabled():palette disabled, framestyle disabled\n");
      if (dbg) fflush(stdout);
      setPalette(paletteDisabled);
//      setFrameStyle(frameStyleDisabled);
   }
   else if (f == FALSE && highlight == TRUE) 
   {
      if (dbg) fprintf(stdout, "xEdit::setEnabled():palette highlight, framestyle disabled\n");
      if (dbg) fflush(stdout);
      setPalette(paletteHighlight);
//      setFrameStyle(frameStyleDisabled);
   }
   else
   {
      if (dbg) fprintf(stdout, "xEdit::setEnabled():palette enabled, framestyle enabled\n");
      if (dbg) fflush(stdout);
      setPalette(paletteEnabled);
//      setFrameStyle(frameStyleEnabled);
   }
//   if (f == FALSE && hasFocus() == TRUE)
//      clearFocus();
   if (f == TRUE)
   {
      setFocusPolicy(QWidget::StrongFocus);
      if (dbg) fprintf(stdout, "xEdit::setEnabled():Focus Enabled\n");
      if (dbg) fflush(stdout);
   }
   else
   {
      setFocusPolicy(QWidget::NoFocus);
      clearFocus();
      if (dbg) fprintf(stdout, "xEdit::setEnabled():Focus Disabled\n");
      if (dbg) fflush(stdout);
   }
   if (dbg) fprintf(stdout, "xEdit::setEnabled():Exit\n");
   if (dbg) fflush(stdout);
//   QLineEdit::setEnabled(f);
}

void xEdit::setHighlight(bool f)
{
   if (dbg) fprintf(stdout, "xEdit::setHighlight():Enter\n");
   if (dbg) fflush(stdout);
   highlight = f;
   if (f == FALSE && enabled == FALSE)
   {
      if (dbg) fprintf(stdout, "xEdit::setHighlight():palette disabled, framestyle disabled\n");
      if (dbg) fflush(stdout);
      setPalette(paletteDisabled);
//      setFrameStyle(frameStyleDisabled);
   }
   else if (enabled == TRUE)
   {
      if (dbg) fprintf(stdout, "xEdit::setHighlight():palette enabled, framestyle enabled\n");
      if (dbg) fflush(stdout);
      setPalette(paletteEnabled);
//      setFrameStyle(frameStyleEnabled);
   }
   else
   {
      if (dbg) fprintf(stdout, "xEdit::setHighlight():palette highlight, framestyle enabled\n");
      if (dbg) fflush(stdout);
      setPalette(paletteHighlight);
//      setFrameStyle(frameStyleEnabled);
   }
   if (dbg) fprintf(stdout, "xEdit::setHighlight():Exit\n");
   if (dbg) fflush(stdout);
}

QString xEdit::macroReplace(const char *cp)
{
   QString rv, strTmp1, strTmp2;
   char ch, ch1, ch2;
   int base;

   if (dbg) fprintf(stdout, "xEdit::macroReplace():Enter\n");
   if (cp == NULL)
   {
      rv = "";
      return(rv);
   }
   for (ch = ' ', rv = ""; *cp;)
   {
      if (ch != '\\' && ch != '$' && *cp == '$')
      {
         if (*(cp + 1) != '$')
         {
            if (dbg) fprintf(stdout, "xEdit::macroReplace():Test for Macro in: |%s|\n", cp);
            for (strTmp1 = "", cp++; *cp && isalnum(*cp) || *cp == '.' || *cp == '_'; cp++)
               strTmp1 += *cp;
            if (dbg) fprintf(stdout, "xEdit::macroReplace():Tag =  |%s|\n", (const char *)strTmp1);
            if ((strTmp2 = macroReplace(Defaults.get(strTmp1))) != NULL)
            {
               if (dbg) fprintf(stdout, "xEdit::macroReplace():Found Entry!! = |%s|\n", (const char *)strTmp1);
               rv += strTmp2;
            }
            if (dbg) fprintf(stdout, "xEdit::macroReplace():Remaining string to test = |%s|\n", cp);
         }
         else
            ch = *(cp++);
      }
      else if (ch != '\\' && *cp == '\\')
      {
         if (dbg) fprintf(stdout, "xEdit::macroReplace():Found escape char!\n");
         ch = *(cp++);
      }
      else if (ch == '\\')
      {
         if (dbg) fprintf(stdout, "xEdit::macroReplace():Interpreting escape char!\n");
         switch (toupper(*cp))
         {
            case 'X':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
               if (dbg) fprintf(stdout, "xEdit::macroReplace():Found Number!!!!!\n");
               if (toupper(*cp) == 'X')
               {
                  base = 16;
                  cp++;
               }
               else if (*cp == '0')
                  base = 8;
               else
                  base = 10;
               for (ch1 = 0; base != 0;)
               {
                  if (base == 16)
                  {
                     if (isxdigit(*cp))
                     {
                        ch1 *= 16;
                        if ((ch2 = toupper(*cp) - '0') > 9)
                           ch1 += ch2 - 7;
                        else
                           ch1 += ch2;
                        cp++;
                     }
                     else
                        base = 0;
                  }
                  else if (base == 8)
                  {
                     if (dbg) printf("ctoi():base 8, ch = 0x%x, %c\n\r", ch, ch);
                     if (*cp >= '0' && *cp <= '7')
                     {
                        ch1 *= 8;
                        ch1 += *cp - '0';
                        cp++;
                        if (dbg) printf("ctoi():base 8, rslt = 0%x\n\r", ch1);
                     }
                     else
                        base = 0;
                  }
                  else
                  {
                     if (dbg) printf("ctoi():base 10, *cp = 0x%x, %c\n\r", *cp, *cp);
                     if (isdigit(*cp))
                     {
                        ch1 *= 10;
                        ch1 += *cp - '0';
                        cp++;
                        if (dbg) printf("ctoi():base 10, rslt = 0x%x\n\r", ch1);
                     }
                     else
                        base = 0;
                  }
               }
               ch = ch1;
               rv += ch1;
               break;

            default:
               if (dbg) fprintf(stdout, "xEdit::macroReplace():Found defaults!!!!!!!!\n");
               rv += ch;
               ch = ' ';
               rv += *(cp++);
               break;
         }
      }
      else
      {
         ch = *cp;
         rv += *(cp++);
      }
   }
   if (dbg) fprintf(stdout, "xEdit::macroReplace():Exit |%s|\n", (const char *)rv);
   return(rv);
}

const char *xEdit::text()
{
   const char *cp;
   QString strTmp;

   if (dbg) fprintf(stdout, "xEdit::text():Enter\n");
   if (dbg) fflush(stdout);
   txtStr = "";
   cp = QLineEdit::text();
   if (strlen(cp) > 0)
   {
      if (dbg) fprintf(stdout, "xEdit::text():Intermediate cp = |%s|\n", cp);
      if (dbg) fflush(stdout);
      strTmp = cp;
      txtStr = Defaults.expandEntry(strTmp);
   }
   if (dbg) fprintf(stdout, "xEdit::text():Exit |%s|\n", (const char *)txtStr);
   if (dbg) fflush(stdout);
   return((const char *)txtStr);
}

void xEdit::mousePressEvent(QMouseEvent *pEvt)
{
   xMouseEvent evt(this, pEvt);
   if (dbg) fprintf(stdout, "xEdit::mousePressEvent():Enter\n");
   QLineEdit::mousePressEvent(pEvt);
   emit mousePressed(&evt);
   if (dbg) fprintf(stdout, "xEdit::mousePressEvent():Exit\n");
}

void xEdit::mouseReleaseEvent(QMouseEvent *pEvt)
{
   xMouseEvent evt(this, pEvt);
   if (dbg) fprintf(stdout, "xEdit::mouseReleaseEvent():Enter\n");
   QLineEdit::mouseReleaseEvent(pEvt);
   emit mouseReleased(&evt);
   if (dbg) fprintf(stdout, "xEdit::mouseReleaseEvent():Exit\n");
}

void xEdit::mouseDoubleClickEvent(QMouseEvent *pEvt)
{
   xMouseEvent evt(this, pEvt);
   if (dbg) fprintf(stdout, "xEdit::mouseDoubleClickEvent():Enter\n");
   QLineEdit::mouseDoubleClickEvent(pEvt);
   emit mouseDoubleClicked(&evt);
   if (dbg) fprintf(stdout, "xEdit::mouseDoubleClickEvent():Exit\n");
}

void xEdit::mouseMoveEvent(QMouseEvent *pEvt)
{
   xMouseEvent evt(this, pEvt);
   if (dbg) fprintf(stdout, "xEdit::mouseEvent():Enter\n");
   QLineEdit::mouseMoveEvent(pEvt);
   emit mouseMoved(&evt);
   if (dbg) fprintf(stdout, "xEdit::mouseEvent():Exit\n");
}

//#include "xEdit.moc"
