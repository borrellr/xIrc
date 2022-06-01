/***************************************************************************
**    xMultiLineEdit.cpp  $Revision: 1.2 $ - $Name:  $ 
**    QMultiLineEdit Replacement
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
#include "xMultiLineEdit.h"

extern xDefaults Defaults;

static int dbg = 0;

static const char *pInitialResources[] =
{
/*
   "*MultiLineEdit.Background: #c3c3c3",
   "*MultiLineEdit.Foreground: yellow",
   "*MultiLineEdit.BaseColor: light yellow",
   "*MultiLineEdit.BaseHighlight: blue",
   "*MultiLineEdit.TextColor: Black",
   "*MultiLineEdit.TextHighlight: White",
*/
   NULL
};

xMultiLineEdit::xMultiLineEdit(xWidgetResInfo *pPRes, QWidget *pParent, const char *pName) :
        QMultiLineEdit(pParent, pName)
{
   QPalette p;
   QColor baseColor;
//   QColor baseHighlight;
   QColor textColor;
//   QColor textHighlight;
   QColor textEnabled;
   QColorGroup n, a, d;
   const char *cp, *cp1;

   if (dbg) fprintf(stdout, "xMultiLineEdit::xMultiLineEdit():Enter\n");
   Resources->setWidgetInit(pInitialResources);
   wdtRes = new xWidgetResInfo(pPRes, QString("multieditedit"), QString("MultiLineEdit"));
   if (dbg) fprintf(stdout, "xMultiLineEdit::xMultiLineEdit():Width = %d, height = %d\n",
                            width(), height());
   if (dbg) fflush(stdout);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
//   QMultiLineEdit::setFont(font());
   if (dbg) fprintf(stdout, "xMultiLineEdit::xMultiLineEdit():Defaults Set\n");
   if (dbg) fflush(stdout);
   
   if (dbg) fprintf(stdout, "xMultiLineEdit::xMultiLineEdit():Setting Colors and such\n");
   if (dbg) fflush(stdout);
   p = palette();
   cp = Resources->get(wdtRes, "basecolor", "BaseColor");
   setStrColor(baseColor, cp);

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
                  d.mid(), d.dark(), textColor, baseColor);
   paletteEnabled.setNormal(n1);
   paletteEnabled.setActive(a1);
   paletteEnabled.setDisabled(d1);

   QColorGroup n2(n.foreground(), n.background(),  n.light(), 
                  n.mid(), n.dark(), textColor, baseColor);
   QColorGroup a2(a.foreground(), a.background(),  a.light(), 
                  a.mid(), a.dark(), textColor, baseColor);
   QColorGroup d2(d.foreground(), d.background(),  d.light(), 
                  d.mid(), d.dark(), textColor, baseColor);
   paletteDisabled.setNormal(n2);
   paletteDisabled.setActive(a2);
   paletteDisabled.setDisabled(d2);

   if (dbg) fprintf(stdout, "xMultiLineEdit::xMultiLineEdit():Getting Font Metrics\n");
   if (dbg) fflush(stdout);
   QFontMetrics fm = fontMetrics();
   if (dbg) fprintf(stdout, "xMultiLineEdit::xMultiLineEdit():Setting to Width = %d, height = %d\n",
                            width(), fm.lineSpacing() + 8);
   resize(width(), fm.lineSpacing() + 8);
   if (dbg) fprintf(stdout, "xMultiLineEdit::xMultiLineEdit():New Width = %d, height = %d\n",
                            width(), height());
//   frameStyleEnabled = QFrame::Panel | QFrame::Sunken;
//   frameStyleDisabled = QFrame::Panel | QFrame::Plain;
   setEnabled(TRUE);
   QMultiLineEdit::setReadOnly(FALSE);
   setFocusPolicy(QWidget::StrongFocus);
   if (dbg) fprintf(stdout, "xMultiLineEdit::xMultiLineEdit():Exit\n");
}

void xMultiLineEdit::setFrameStyles(int styleEnabled, int styleDisabled)
{
   if (styleEnabled != -1)
      frameStyleEnabled = styleEnabled;
   if (styleDisabled != -1)
      frameStyleDisabled = styleDisabled;
//   setFrameStyle((enabled == TRUE) ?
//                 frameStyleEnabled : frameStyleDisabled);
}

void xMultiLineEdit::setHeight(int h)
{
   QFontMetrics fm = fontMetrics();
   if (dbg) fprintf(stdout, "xMultiLineEdit::setWidth(%d):Enter\n", h);
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xMultiLineEdit::setWidth():maxWidth = %d, width = %d\n",
                            fm.maxWidth(), fm.maxWidth() * h);
   if (dbg) fflush(stdout);
   resize(width(),  fm.lineSpacing() * h + 8);
   if (dbg) fprintf(stdout, "xMultiLineEdit::setWidth():Exit\n");
   if (dbg) fflush(stdout);
}

void xMultiLineEdit::setWidth(int width)
{
   QFontMetrics fm = fontMetrics();
   if (dbg) fprintf(stdout, "xMultiLineEdit::setWidth(%d):Enter\n", width);
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xMultiLineEdit::setWidth():maxWidth = %d, width = %d\n",
                            fm.maxWidth(), fm.maxWidth() * width);
   if (dbg) fflush(stdout);
   resize(fm.maxWidth() * width + 8,  height());
   if (dbg) fprintf(stdout, "xMultiLineEdit::setWidth():Exit\n");
   if (dbg) fflush(stdout);
}

void xMultiLineEdit::setEnabled(bool f)
{
   if (dbg) fprintf(stdout, "xMultiLineEdit::setEnabled():Enter\n");
   if (dbg) fflush(stdout);
   enabled = f;
   if (f == FALSE)
   {
      if (dbg) fprintf(stdout, "xMultiLineEdit::setEnabled():palette disabled, framestyle disabled\n");
      if (dbg) fflush(stdout);
      QMultiLineEdit::setPalette(paletteDisabled);
      setPalette(paletteDisabled);
//      setFrameStyle(frameStyleDisabled);
   }
   else
   {
      if (dbg) fprintf(stdout, "xMultiLineEdit::setEnabled():palette enabled, framestyle enabled\n");
      if (dbg) fflush(stdout);
      QMultiLineEdit::setPalette(paletteEnabled);
      setPalette(paletteEnabled);
//      setFrameStyle(frameStyleEnabled);
   }
   if (f == FALSE && hasFocus() == TRUE)
      clearFocus();
   QMultiLineEdit::setReadOnly(!f);
   if (f)
      setFocusPolicy(QWidget::StrongFocus);
   else
      setFocusPolicy(QWidget::NoFocus);
   if (dbg) fprintf(stdout, "xMultiLineEdit::setEnabled():Exit\n");
   if (dbg) fflush(stdout);
}

//#include "xMultiLineEdit.moc"
