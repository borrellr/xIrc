/***************************************************************************
**    xMisc.cpp  $Revision: 1.1.1.1 $ - $Name:  $ 
**    Misc utility routines
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <qlist.h>
#include <qwidget.h>
#include <qpalette.h>
#include <qregexp.h>
#include "xMisc.h"
#include "xDialog.h"
#include "xResources.h"

static int dbg = 0;

char strtoc(const char *pStr, const char **rtnStr)
{
   char rv, ch;
   int base;

   if (*pStr == '\\')
   {
      pStr++;
      if (toupper(*pStr) == 'X')
      {
         base = 16;
         pStr++;
      }
      else if (*pStr == '0')
         base = 8;
      else if (isdigit(*pStr))
         base = 10;
      else
      {
         rv = *(pStr++);
         if (rtnStr != NULL)
            *rtnStr = pStr;
         return(rv);
      }

      for (rv = 0; *pStr;)
      {         
         if (base == 16)
         {
            if (dbg) printf("strtoc():base 16, ch = 0x%x, %c\n\r", *pStr, *pStr);
            if (isxdigit(*pStr))
            {
               rv *= 16;
               if ((ch = toupper(*pStr) - '0') > 9)
                  rv += ch - 7;
               else
                  rv += ch;
               if (dbg) printf("strtoc():base 16, rslt = 0x%x\n\r", rv);
               pStr++;
            }
            else
               break;
         }
         else if (base == 8)
         {
            if (dbg) printf("strtoc():base 8, ch = 0x%x, %c\n\r", *pStr, *pStr);
            if (*pStr >= '0' && *pStr <= '7')
            {
               rv *= 8;
               rv += *pStr - '0';
               if (dbg) printf("strtoc():base 8, rslt = 0%x\n\r", rv);
               pStr++;
            }
            else
               break;
         }
         else
         {
            if (dbg) printf("strtoc():base 10, ch = 0x%x, %c\n\r", *pStr, *pStr);
            if (isdigit(*pStr))
            {
               rv *= 10;
               rv += *pStr - '0';
               if (dbg) printf("strtoc():base 10, rslt = 0x%x\n\r", rv);
               pStr++;
            }
            else
               break;
         }
         if (dbg) printf("strtoc():exit(%d)\n", rv);
      }
   }
   else
      rv = *(pStr++);

   if (rtnStr != NULL)
      *rtnStr = pStr;
   return(rv);
}

/*
** This function sets the color of the QColor object refrenced by
** 'color' to that pointed to by pColorStr. pColorStr may either point
** to an RGB value or a color name
*/
void setStrColor(QColor &color, const char *pColorStr)
{
   unsigned long rgbColor;
   char *cp;

   if (dbg) fprintf(stdout, "setStrColor():Setting Color |%s|\n", pColorStr);   
   if (dbg) fflush(stdout);
   rgbColor = strtol(pColorStr, &cp, 0);
   if (cp != pColorStr)
   {
      if (dbg) fprintf(stdout, "setStrColor():Is Value\n");   
      color.setRgb(rgbColor);
   }
   else
   {
      if (dbg) fprintf(stdout, "setStrColor():Is Named\n");   
      color.setNamedColor(pColorStr);
   }
}

QFont setStrFont(const char *pFamily, const char *pSize, 
        const char *pWeight)
{
   char weight[256], *cp;
   const char *cp1;

   if (dbg) printf("setStrFont()::Enter\n");
   if (pWeight == NULL)
   {
//      sprintf(weight, "%d", font.weight());
//      if (dbg) printf("setStrFont():Using Default Weight:|%s|\n", weight);
      if (dbg) printf("setStrFont():Using Default Weight:|0|\n");
      sprintf(weight, "%d", QFont::Normal);
      pWeight = weight;
   }
   else
   {
      strtol(pWeight, &cp, 0);
      if (pWeight == cp)
      {
         for (cp = weight, cp1 = pWeight; *cp1;)
         {
            *(cp++) = tolower(*cp1++);
            *cp = '\0';
         }
         if (strcmp(weight, "light") == 0)
            sprintf(weight, "%d", QFont::Light);
         else if (strcmp(weight, "normal") == 0)
            sprintf(weight, "%d", QFont::Normal);
         else if (strcmp(weight, "demibold") == 0)
            sprintf(weight, "%d", QFont::DemiBold);
         else if (strcmp(weight, "bold") == 0)
            sprintf(weight, "%d", QFont::Bold);
         else if (strcmp(weight, "black") == 0)
            sprintf(weight, "%d", QFont::Black);
         else 
            sprintf(weight, "%d", QFont::Normal);
         pWeight = weight;
      }
   }

   if (pSize == NULL)
   {
//      sprintf(size, "%d", font.pointSize());
//    if (dbg) printf("setStrFont():Using Default Size:|%s|\n", size);
      if (dbg) printf("setStrFont():Using Default Size:|12|\n");
      pSize = "12";
//      pSize = size;
   }
   if (dbg) printf("setStrFont():Family:|%s|, Size: %d, Weight: %d\n",
                   pFamily, atoi(pSize), atoi(pWeight));
   QFont f(pFamily, atoi(pSize), atoi(pWeight), FALSE);
   if (dbg) printf("setStrFont()::Exit\n");
   return(f);
}

void setDefFont(QWidget *pWdt, xWidgetResInfo *wdtRes)
{
   const char *ccp1, *ccp2, *ccp3;
   QFont defFont;
   char fontWeight[80], fontSize[80];

   if (dbg) printf("setDefFont()::Enter\n");
   if (dbg) fflush(stdout);
   ccp1 = Resources->get(wdtRes, "font.family", "Font.Family");
   ccp2 = Resources->get(wdtRes, "font.size", "Font.Size");
   ccp3 = Resources->get(wdtRes, "font.weight", "Font.Weight");

   if (ccp1 == NULL)
      ccp1 = pWdt->font().family();
   if (ccp2 == NULL)
   {
      sprintf(fontSize, "%d", pWdt->font().pointSize());
      if (dbg) printf("setDefFont()::No Size Specified! using: |%s|\n",
                      fontSize);
      if (dbg) fflush(stdout);
      ccp2 = fontSize;
   }
   if (ccp3 == NULL)
   {
      sprintf(fontWeight, "%d", pWdt->font().weight());
      if (dbg) printf("setDefFont()::No Weight Specified! using: |%s|\n",
                      fontWeight);
      if (dbg) fflush(stdout);
      ccp3 = fontWeight;
   }
   if (dbg) printf("setDefFont()::Family:|%s|, Size|%s|, Weight:|%s|\n",
                   ccp1, ccp2, ccp3);
   if (dbg) fflush(stdout);
   pWdt->setFont(setStrFont(ccp1, ccp2, ccp3));
   if (dbg) printf("setDefFont()::Exit\n");
   if (dbg) fflush(stdout);
}

void setDefPallet(QWidget *pWdt, xWidgetResInfo *wdtRes)
{
   QColor fg, bg, baseColor, textColor;
   const char *ccp1;

   if (dbg) printf("setDefPalette()::Enter\n");
   if (dbg) fflush(stdout);
   if ((ccp1 = Resources->get(wdtRes, "background", "Background")) == NULL)
      ccp1 = "0xc3c3c3";
   setStrColor(bg, ccp1);

   if ((ccp1 = Resources->get(wdtRes, "foreground", "Foreground")) == NULL)
      ccp1 = "black";
   setStrColor(fg, ccp1);

   if ((ccp1 = Resources->get(wdtRes, "basecolor", "BaseColor")) != NULL)
      setStrColor(baseColor, ccp1);
   else
      baseColor = bg;

   if ((ccp1 = Resources->get(wdtRes, "textcolor", "TextColor")) != NULL)
      setStrColor(textColor, ccp1);
   else
      textColor = fg;

   if (dbg) printf("setDefPalette()::Creating Palettes\n");
   if (dbg) fflush(stdout);
   QColorGroup g1(fg, bg, bg.light(), bg.dark(), bg.dark(120),
                 textColor, baseColor);
   QColorGroup g2(fg, bg, bg.light(), bg.dark(), bg.dark(120),
                 textColor, bg);
   QPalette p( g1, g2, g1 );
   if (dbg) printf("setDefPalette()::Setting Widgets Palette\n");
   if (dbg) fflush(stdout);
   pWdt->setPalette(p);
   if (dbg) printf("setDefPalette()::Exit\n");
   if (dbg) fflush(stdout);
}

xWidgetResInfo::xWidgetResInfo(xWidgetResInfo *pPRes, QString _nameStr, QString _classStr)
{
   if (pPRes != NULL)
   {
      if (strlen(_nameStr) > 0)
         nameStr = pPRes->getName() + ".";
      else
         nameStr = pPRes->getName();
      if (strlen(_classStr) > 0)
         classStr = pPRes->getClass() + ".";
      else
         classStr = pPRes->getClass();
   }
   nameStr +=  _nameStr;
   classStr += _classStr;
}

xWidgetResInfo::xWidgetResInfo(xWidgetResInfo *pRes)
{
   nameStr =  pRes->getName();
   classStr = pRes->getClass();
}

xWidgetResInfo::xWidgetResInfo(xWidgetResInfo &res)
{
   nameStr =  res.getName();
   classStr = res.getClass();
}

void buildList(QPtrList<QString> &list, const char *pTests)
{
   QString tmpStr("");
   int state = 0;
   const char *cp;

   if (dbg) fprintf(stdout, "buildList():Enter\n");
   list.setAutoDelete(TRUE);
   for (cp = pTests; state >= 0 && *cp;)
   {
      if (dbg > 3) fprintf(stdout, "buildList():State %d, chr: '%c'\n",
                                state, *cp);
      switch (state)
      {
         case 0:
            if (*cp != ':' && *cp != '\\' && *cp != ';' && *cp != '"')
               tmpStr += *(cp++);
            else if (*cp == '\\')
            {
               cp++;
               state = 1;
            }
            else if (*cp == ';')
               state = -1;
            else if (*cp == ':')
            {
               if (dbg) fprintf(stdout, "buildList():Adding string |%s|\n",
                                        (const char *)tmpStr);
               list.append(new QString(tmpStr));
               tmpStr = "";
               cp++;
            }
            else
               cp++;
            break;

         case 1:
            tmpStr += *(cp++);
            state = 0;
            break;
      }
   }
   if (strlen(tmpStr) > 0)
   {
      if (dbg) fprintf(stdout, "buildList():Adding Final string: |%s|\n",
                                (const char *)tmpStr);
      list.append(new QString(tmpStr));
   }
}

bool testString(QPtrList<QString> &list, QString &str)
{
   QPtrListIterator<QString> it(list);
   int x, len;

   if (dbg) fprintf(stdout, "testString():Enter\n");
   if (it.current() == NULL || strlen(str) == 0)
      return(TRUE);
   for (; it.current() != NULL; ++it)
   {
      if (dbg) fprintf(stdout, "testString():validating \'%s\' against \'%s\'\n",
                       (const char *)str, (const char *)*it.current());
      QRegExp regexp(*it.current());
      if ((x = regexp.match(str, 0, &len)) >= 0)
      {
         if (dbg) fprintf(stdout, "testString():ofs = %d, len = %d\n", x, len);
         if ((unsigned int)len == strlen(str))
            return(TRUE);
      }
   }
   if (dbg) fprintf(stdout, "testString():Entry failed\n");
   return(FALSE);
}

bool inList(QPtrList<QString> &list, QString string)
{
   bool rv = FALSE;

   if (dbg) fprintf(stdout, "inList():Entry\n");
   for (QPtrListIterator<QString> it(list);
        rv == FALSE && it.current() != NULL; ++it)
   {
      if (dbg) fprintf(stdout, "inList():Comparing |%s| & |%s|\n",
                              (const char *)*it.current(),
                              (const char *)string);
      if (strcmp(*it.current(), string) == 0)
      {
         if (dbg) fprintf(stdout, "inList():Have Match!\n");
         rv = TRUE;
      }
   }
   if (dbg) fprintf(stdout, "inList():Exit(%d)\n", rv);
   return(rv);
}
