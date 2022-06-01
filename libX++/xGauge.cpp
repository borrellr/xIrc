/***************************************************************************
**    xGauge.cpp  $Revision: 1.3 $ - $Name:  $ 
**    Progress Indicator, looks sort of like a thermometer
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
#include "xGauge.h"
#include <qlineedit.h>
#include <qpainter.h>
#include <qpen.h>

extern xDefaults Defaults;

static int dbg = 0;

static const char *pInitialResources[] =
{
   "*BaseHighlight: red",
   "*Gradient: TRUE",
   NULL
};

xGauge::xGauge(xWidgetResInfo *pPRes, QWidget *pParent, const char *pName) :
        QWidget(pParent, pName)
{
   orient = Vertical;
   initGauge(pPRes);
}

xGauge::xGauge(xWidgetResInfo *pPRes, Orientation orientation,
               QWidget *pParent, const char *pName) :
        QWidget(pParent, pName)
{
   orient = orientation;
   initGauge(pPRes);
}

void xGauge::initGauge(xWidgetResInfo *pPRes)
{
   const char *cp;

   if (dbg) fprintf(stdout, "xGauge::xGauge():Enter\n");
   thick = 4;

   wdtRes = new xWidgetResInfo(pPRes, QString("gauge"), QString("Gauge"));
   Resources->setWidgetInit(pInitialResources);
   if (dbg) fprintf(stdout, "xGauge::xGauge():Width = %d, height = %d\n",
                            width(), height());
   if (dbg) fflush(stdout);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);

   cp = Resources->get(wdtRes, "basehighlight", "BaseHighlight");
   setStrColor(baseHighlight, cp);
   resize(12, 100);
   if (dbg) fprintf(stdout, "xGauge::xGauge():Exit\n");
}

void xGauge::setLineThickness(int t)
{
   thick = t;
   update();
}

void xGauge::paintEvent(QPaintEvent *pEvt)
{
   QPainter p;
   QRect r(pEvt->rect());
   p.begin(this);
   if (orient == Vertical)
      paintGaugeVert(&p, r);
   p.end();
}

void xGauge::paintGaugeVert(QPainter *p, QRect r)
{
   QPalette pl = palette();
   QColorGroup cg = palette().normal();
   QColor bgd = cg.dark();
   QColor bgm = cg.mid();
   QColor bgl = cg.light();
   QColor fc(red);
   QRect r1;
   int n, n1;
   double v, v1;
   
   r1.setLeft(r.left() + ((r.width() - thick) / 2));
   r1.setRight(r.right() - ((r.width() - thick) / 2));
   r1.setTop(r.top());
   r1.setBottom(r.bottom());
   int thick2 = (thick / 2);
   p->setBrush(bgm);
   p->setPen(QPen(bgd));
   for (n = 1; n < 4; n++)
      p->drawLine(r1.left() - thick, r1.top() + ((r1.height() / 4) * n),
                  r1.right() + thick, r1.top() + ((r1.height() / 4) * n));
   p->setPen(QPen(bgl));
   for (n = 1; n < 4; n++)
      p->drawLine(r1.left() - thick, r1.top() + ((r1.height() / 4) * n) + 1,
                  r1.right() + thick, r1.top() + ((r1.height() / 4) * n) + 1);

   p->setPen(QPen(bgd));
   p->drawChord(r1.left(), r1.top(),
             thick, thick,
             0 * 16, 180 * 16);

   p->setPen(QPen(bgl));
   p->drawChord(r1.right() - thick, r1.bottom() - thick,
             thick, thick,
             180 * 16, 180 * 16);

   p->setBrush(bgm);
   p->fillRect(r1.left(), r1.top() + thick2,
            thick, r1.height() - thick, QBrush(bgm));

   p->setPen(QPen(bgd));
   p->drawLine(r1.left(), r1.bottom() - (thick2),
              r1.left(), r1.top() + (thick2));


   p->setPen(QPen(bgl));
   p->drawLine(r1.right(), r1.bottom() - (thick2),
              r1.right(), r1.top() + (thick2));

   v = (double)(r1.height() - thick) / (double)maxValue();

   v1 = v * (double)value() + 0.5;
   n = (int)v1;

   n1 = r1.height() - n;
   fprintf(stderr, "value = %d, max = %d, height = %d\n", value(), maxValue(), r1.height());
   fprintf(stderr, "v = %f, v1 = %f, n = %d, n1 = %d\n", v, v1, n, n1);
   p->setPen(QPen(fc));
   p->fillRect(r1.left() + 1, r1.top() + n1,
            thick - 2, n - thick2, QBrush(fc));

}

//      drawLine(r.left(), r.bottom() - (thick / 2),
//               r.left(), r.top() + (thick / 2));

void xGauge::valueChanged()
{
   update();
}
void xGauge::rangeChanged()
{
   update();
}
void xGauge::stepChanged()
{
   update();
}

//#include "xGauge.moc"
