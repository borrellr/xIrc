/***************************************************************************
**    xSQLField.cpp $Revision: 1.2 $ - $Name:  $ 
**    SQL Edit Field
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
#include <qkeycode.h>
#include <qregexp.h> 
#include "xMisc.h"
#include "xResources.h"
#include "xSQLField.h"

static int dbg = 0;
static const char *emptyStr = "";

static const char *pInitialResources[] =
{
   "*PhoneField.Validate: \" *1?([0-9][0-9][0-9])[ -]?[0-9][0-9][0-9]-[0-9][0-9][0-9][0-9].*\":"
   "\" *1?-?[0-9][0-9][0-9]-[0-9][0-9][0-9]-[0-9][0-9][0-9][0-9].*\":"
   "\" *[0-9][0-9][0-9]-[0-9][0-9][0-9][0-9].*\"",

   "*NameField.Validate: \" *[A-Z]'?[a-zA-Z]*, .*\"",

   "*CurrencyField.Validate: \" *[+-]?\\\\\\\\$?[0-9]*\\\\\\\\.[0-9][0-9]\":"
   "\" *[+-]?\\\\\\\\$?[0-9]*\\\\\\\\.[0-9]\":"
   "\" *[+-]?\\\\\\\\$?[0-9]*\\\\\\\\.\":"
   "\" *[+-]?\\\\\\\\$?[0-9]*\"",

   "*IntField.Validate: \" *[+-]?[0-9]+\"",

   "*FloatField.Validate: \" *[+-]?[0-9]*\\\\\\\\.?[0-9]*\"",

   "*ComputationField.Validate: \".*\"",

   "*ZipField.Validate: \" *[0-9][0-9][0-9][0-9][0-9]\":"
   "\" *[0-9][0-9][0-9][0-9][0-9]-[0-9][0-9][0-9][0-9]\"",

   "*DateField.Validate: \" *[0-9][0-9]?/[0-9][0-9]?/[0-9][0-9]?;MDY\"",

   "*TimeField.Validate: \" *[0-9][0-9]\\\\:[0-9][0-9][AP]?;12\"",

   "*YesNoField.Validate: \" *[YN]\"",
   NULL
};

typedef struct
{
   char  monthStr[4];
   char  monthNum[3];
} monthType;

static monthType monthTbl[12] =
{
   {  "Jan", "01" },
   {  "Feb", "02" },
   {  "Mar", "03" },
   {  "Apr", "04" },
   {  "May", "05" },
   {  "Jun", "06" },
   {  "Jul", "07" },
   {  "Aug", "08" },
   {  "Sep", "09" },
   {  "Oct", "10" },
   {  "Nov", "11" },
   {  "Dec", "12" }
};

xSQLField::xSQLField(xWidgetResInfo *pPRes, QWidget *pParent,
                     xSQLBase *pSqlBase, int db, xSQLFieldDef *pField) :
           xEdit(wdtRes = new xWidgetResInfo(pPRes, 
                                   pSqlBase->getFieldName(pField->name), 
                                             QString("Field")),
                 pParent, pField->name)
{
   const char *ccp;
   int len;

   if (dbg) fprintf(stdout, "xSQLField::xSQLField():Enter\n");
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xSQLField::xSQLField():Label = |%s|\n",
                             pField->label);
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xSQLField::xSQLField():Name  = |%s|\n", 
                             pField->name);
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xSQLField::xSQLField():Len   = %d\n",
                             pField->len);
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xSQLField::xSQLField():Link  = |%s|\n",
                             pField->link);
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xSQLField::xSQLField():Type  = %d\n",
                             pField->type);
   if (dbg) fflush(stdout);

   wdtValidate = NULL;
   Resources->setWidgetInit(pInitialResources);
   edtWidth = pField->len;
   setWidth(edtWidth);
   setDefPallet(this, wdtRes);
   setDefFont(this, wdtRes);
   setDefValidation(pField->type);
   labelStr = pField->label;
   connect(this, SIGNAL(returnPressed()), 
           this, SLOT(haveReturn()));

   autoSelect = FALSE;
   if ((ccp = Resources->get(wdtRes, "autoselect", "AutoSelect")) != NULL)
   {
      QString s(ccp);
      if (strcmp(s.upper(), "TRUE") == 0)
         autoSelect = TRUE;
   }

   type = pField->type;
   pBase = pSqlBase;
   len = pField->len;

   if (dbg) fprintf(stdout, "xSQLField::xSQLField():Fetching Fields for table |%s|\n",
                            (const char *)tableName());
   if (dbg) fflush(stdout);
   if (pField->name != NULL && strlen(pField->name) > 0)
   {
      if (fieldType() != sqlComputation && 
          fieldType() != sqlDate &&
          fieldType() != sqlCurrency &&
          fieldType() != sqlFloat)
      {
         if (dbg) fprintf(stdout, "xSQLField::xSQLField():Getting length of %s\n", pField->name);
         if (dbg) fflush(stdout);
         if ((len = pBase->fieldLen(db, fieldName(), tableName())) >= 0)
         {
            if (dbg) fprintf(stdout, "xSQLField::xSQLField():maxLength = %d\n", len);
            if (dbg) fflush(stdout);
            setMaxLength(len);
//            flags = field->flags;
         }
         else
         {
            if (dbg) fprintf(stdout, "xSQLField::xSQLField():maxLength = INFINITE!!!\n");
            if (dbg) fflush(stdout);
         }
      }
      else
         setEnabled(FALSE);
   }
   if (dbg) fprintf(stdout, "xSQLField::xSQLField():Exit\n");
   if (dbg) fflush(stdout);
}

xSQLField::~xSQLField()
{
   if (wdtRes != NULL)
      delete wdtRes;
}

const char *xSQLField::defaultText()
{
   const char *ccp;

   if ((ccp = Resources->get(wdtRes, "default", "Default")) != NULL)
   {
      QString strTmp;
      char ch;

      ch = *ccp;
      if (ch == '"' || ch == '\'')
      {
         ccp++;
         while (*ccp != ch)
            strTmp += *(ccp++);
      }
      else
         strTmp = ccp;
      if (dbg) fprintf(stdout, "xSQLField::xSQLField():Setting default to \"%s\"\n",
                               (const char *)strTmp);
      if (dbg) fflush(stdout);
   }
   else
      ccp = emptyStr;
   return(ccp);
}

const char *xSQLField::text()
{
   if (dbg) fprintf(stdout, "xSQLField::text():Enter: type = %d\n",
                            fieldType());
   if (dbg) fflush(stdout);
   if (fieldType() == sqlYesNo)
   {
      char buf[256];
      char *cp;
      if (dbg) fprintf(stdout, "xSQLField::text():Have sqlYesNo type\n");
      if (dbg) fflush(stdout);
      strcpy(buf, xEdit::text());
      if (dbg) fprintf(stdout, "xSQLField::text():Got text!\n");
      if (dbg) fflush(stdout);
      for (cp = buf; *cp; cp++)
         *cp = toupper(*cp);
      if (dbg) fprintf(stdout, "xSQLField::text():Putting Upper case back!\n");
      if (dbg) fflush(stdout);
      setText(buf);
   }
   if (dbg) fprintf(stdout, "xSQLField::text():Exiting\n");
   if (dbg) fflush(stdout);
   return(xEdit::text());
}

void xSQLField::setText(const char *pText)
{
   if (dbg) fprintf(stdout, "xSQLField::setText():Enter\n");
   if (dbg) fflush(stdout);
   xEdit::setText(pText);
   if (dbg) fprintf(stdout, "xSQLField::setText():Exit\n");
   if (dbg) fflush(stdout);
}

void xSQLField::setAutoSelect(bool f)
{
   autoSelect = f;
}

bool xSQLField::isKey()
{
   if (fieldType() == sqlComputation)
      return((bool)IS_PRI_KEY(flags));
   else
      return(FALSE);
}

bool xSQLField::canBeNull()
{
   if (fieldType() == sqlComputation)
      return((bool)IS_NOT_NULL(flags));
   else
      return(TRUE);
}

QString xSQLField::tableName()
{
   QString rv;
   const char *cp;

   for (cp = name(); *cp && *cp != '.'; cp++)
      rv += *cp;
   rv = rv.lower();
   return(rv);
}

QString xSQLField::fieldName()
{
   QString rv;
   const char *cp;

   for (cp = name(); *cp && *cp != '.'; cp++);
   if (*cp)
      rv = cp + 1;
   else
      rv = cp;
   rv = rv.lower();
   return(rv);
}

void xSQLField::setValidations(QList<QString> &list)
{
   QListIterator<QString> it(list);
   for (; it.current() != NULL; ++it)
   {
      validateList.append(new QString(*it.current()));
      if (dbg) fprintf(stdout, "xSQLField::setValidations():list item =\'%s\'\n",
                      (const char *)*it.current());
      if (dbg) fflush(stdout);
   }
}

bool xSQLField::valid()
{
   bool rv;
   QString tmpStr(text());
   if (dbg)
   {
      QListIterator<QString> it(validateList);
      for (; it.current() != NULL; ++it)
      {
         if (dbg) fprintf(stdout, "xSQLField::valid():Validation: \'%s\'\n",
                         (const char *)*it.current());
         if (dbg) fflush(stdout);
      }
      fflush(stdout);
   }
   if (fieldType() != sqlComputation)
   {
      if ((rv = testString(validateList, tmpStr)) != TRUE)
      {
         if (dbg) fprintf(stdout, "xSQLField::valid():Not Valid, see if we are using the default string\n");
         if (dbg) fflush(stdout);
         if (strcmp(text(), defaultText()) == 0)
         {
            if (dbg) fprintf(stdout, "xSQLField::valid():Yes, we are, bless it as valid\n");
            if (dbg) fflush(stdout);
            rv = TRUE;
         }
      }
   }
   else
      rv = TRUE;
   return(rv);
}

bool xSQLField::validate(const char *pStr)
{
   QString tmpStr(pStr);
   if (dbg)
   {
      QListIterator<QString> it(validateList);
      for (; it.current() != NULL; ++it)
      {
         fprintf(stdout, "xSQLField::validate():list item =\'%s\'\n",
                         (const char *)*it.current());
      }
   }
//   if (fieldType() == sqlComputation)
   if (fieldType() != sqlComputation)
      return(testString(validateList, tmpStr));
   else
      return(TRUE);
}

bool xSQLField::compare(const char *pStr)
{
   return(strcmp(text(), pStr));
}

bool xSQLField::compare(QString &str)
{
   return(compare((const char *)str));
}

void xSQLField::setEditWidth(int wdt)
{
   edtWidth = wdt;
   setWidth(edtWidth);
}

void xSQLField::setName(const char *pName)
{
   int len;

   if (dbg) fprintf(stdout, "xSQLField::setName():Enter\n");
   xEdit::setName(pName);
   if (pName != NULL && strlen(pName) > 0)
   {
      if ((len = pBase->fieldLen(db, pName, tableName())) >= 0)
      {
         if (dbg) fprintf(stdout, "xSQLField::setName():maxLength = %d\n", len);
         setMaxLength(len);
//         flags = field->flags;
      }
      else
         if (dbg) fprintf(stdout, "xSQLField::setName():maxLength = INFINITE!!!\n");
   }
   if (dbg) fprintf(stdout, "xSQLField::setName():Exit\n");
}

void xSQLField::setDefValidation(int t)
{
   QList<QString> vList;

   if (wdtValidate)
   {
      delete wdtValidate;
      wdtValidate = NULL;
   }
   switch (t)
   {
      case (int)sqlPhone:
         wdtValidate = new xWidgetResInfo(wdtRes, QString("phoneField"),
                                                  QString("PhoneField"));
         break;

      case (int)sqlName:
         wdtValidate = new xWidgetResInfo(wdtRes, QString("nameField"),
                                                  QString("NameField"));
         break;

      case (int)sqlCurrency:
         wdtValidate = new xWidgetResInfo(wdtRes, QString("currencyField"),
                                                  QString("CurrencyField"));
         break;

      case (int)sqlInteger:
         wdtValidate = new xWidgetResInfo(wdtRes, QString("intField"),
                                                  QString("IntField"));
         break;

      case (int)sqlFloat:
         wdtValidate = new xWidgetResInfo(wdtRes, QString("floatField"),
                                                  QString("FloatField"));
         break;

      case (int)sqlZip:
         wdtValidate = new xWidgetResInfo(wdtRes, QString("zipField"),
                                                  QString("ZipField"));
         break;

      case (int)sqlDate:
         wdtValidate = new xWidgetResInfo(wdtRes, QString("dateField"),
                                                  QString("DateField"));
         break;

      case (int)sqlYesNo:
         wdtValidate = new xWidgetResInfo(wdtRes, QString("yesnoField"),
                                                  QString("YesNoField"));
         break;
   }
   if (wdtValidate)
   {
      buildList(vList, Resources->get(wdtValidate, "validate","Validate"));
      setValidations(vList);
   }
}

void xSQLField::haveReturn()
{
   if (dbg) fprintf(stdout, "xSQLField::haveReturn():Enter\n");
   if (dbg) fflush(stdout);
   if (valid() == TRUE)
   {
      if (dbg) fprintf(stdout, "xSQLField::haveReturn():Emiting newData()\n");
      if (dbg) fflush(stdout);
      emit newData(this);
   }
   else
   {
      if (dbg) fprintf(stdout, "xSQLField::haveReturn():Emiting invalidEntry()\n");
      if (dbg) fflush(stdout);
      emit invalidEntry(this);
   }
   if (dbg) fprintf(stdout, "xSQLField::haveReturn():Exit\n");
   if (dbg) fflush(stdout);
}

void xSQLField::keyPressEvent(QKeyEvent *pEvt)
{
   if (dbg) fprintf(stdout, "xSQLField::keyPressEvent():Enter\n");
   if (dbg) fflush(stdout);
   if (pEvt->key() == Key_Tab)
   {
      if (dbg) fprintf(stdout, "xSQLField::keyPressEvent():Have Tab Key\n");
      if (dbg) fflush(stdout);
      if (valid() == TRUE)
      {
         if (dbg) fprintf(stdout, "xSQLField::keyPressEvent():Sending it on to xEdit\n");
         if (dbg) fflush(stdout);
         xEdit::keyPressEvent(pEvt);
      }
      else
      {
         if (dbg) fprintf(stdout, "xSQLField::keyPressEvent():Emitting invalidEntry()\n");
         if (dbg) fflush(stdout);
         emit invalidEntry(this);
      }
   }
   if (pEvt->key() == Key_Escape)
   {
      if (dbg) fprintf(stdout, "xSQLField::keyPressEvent():Have Escape Key\n");
      if (dbg) fflush(stdout);
      emit entryAbandoned(this);
   }
   else
   {
      if (dbg) fprintf(stdout, "xSQLField::keyPressEvent():Sending it on to xEdit\n");
      if (dbg) fflush(stdout);
      xEdit::keyPressEvent(pEvt);
   }
}

void xSQLField::focusInEvent(QFocusEvent *pEvt)
{
   QString strTmp;

   xEdit::focusInEvent(pEvt);
   strTmp = text();
   setText(strTmp.stripWhiteSpace());
   if (autoSelect == TRUE)
      selectAll();
}

void xSQLField::focusOutEvent(QFocusEvent *pEvt)
{
   deselect();
   xEdit::focusOutEvent(pEvt);
}

bool xSQLField::isComputation()
{
   return( (fieldType() == sqlComputation) ? TRUE : FALSE);
}

void xSQLField::setEnabled(bool f)
{
   const char *ccp;

   if (dbg) fprintf(stdout, "xSQLField::setEnabled():Enter\n");
   if (dbg) fflush(stdout);
   xEdit::setEnabled(f);
   if (dbg) fprintf(stdout, "xSQLField::setEnabled():Checking for defaults\n");
   if (dbg) fflush(stdout);
   if (f = TRUE &&
       (ccp = Resources->get(wdtRes, "default", "Default")) != NULL)
   {
      QString strTmp;
      char ch;

      ch = *ccp;
      if (ch == '"' || ch == '\'')
      {
         ccp++;
         while (*ccp != ch)
            strTmp += *(ccp++);
      }
      else
         strTmp = ccp;
      if (dbg) fprintf(stdout, "xSQLField::setEnabled():Setting default to \"%s\"\n",
                               (const char *)strTmp);
      if (dbg) fflush(stdout);
      setText(strTmp);
      if (dbg) fprintf(stdout, "xSQLField::setEnabled():Default set to \"%s\"\n",
                               text());
      if (dbg) fflush(stdout);
   }
   if (dbg) fprintf(stdout, "xSQLField::setEnabled():Enter\n");
   if (dbg) fflush(stdout);
}

//#include "xSQLField.moc"

