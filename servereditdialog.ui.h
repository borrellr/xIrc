/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/
#include <qstring.h>
#include <qstringlist.h>

enum { Group, Country, State, City, Server, Ports, Max };

void xIrcServerEdit::enableOkButton()
{
   buttonOk->setEnabled(isEnable());
}

bool xIrcServerEdit::isEnable()
{
   return (!groupEdit->text().isEmpty() && !serverEdit->text().isEmpty() &&
     !portEdit->text().isEmpty());
}



void xIrcServerEdit::initEntry(const QString &entry )
{
   QString tmpStr;
   QStringList tmpList;

   tmpList = QStringList::split(":", entry);

   tmpStr = tmpList[Group];
   if (tmpStr.isEmpty())
      groupEdit->clear();
   else
      groupEdit->setText(tmpStr);

   tmpStr = tmpList[Country];
   if (tmpStr.isEmpty())
      countryEdit->clear();
   else
      countryEdit->setText(tmpStr);

   tmpStr = tmpList[State];
   if (tmpStr.isEmpty())
      stateEdit->clear();
   else
      stateEdit->setText(tmpStr);

   tmpStr = tmpList[City];
   if (tmpStr.isEmpty())
      cityEdit->clear();
   else
      cityEdit->setText(tmpStr);

   tmpStr = tmpList[Server];
   if (tmpStr.isEmpty())
      serverEdit->clear();
   else
      serverEdit->setText(tmpStr);

   tmpStr = tmpList[Ports];
   if (tmpStr.isEmpty())
      portEdit->clear();
   else
      portEdit->setText(tmpStr);
}


const QString &xIrcServerEdit::getEntry()
{
   QStringList entry;
   QString *retStr = new QString();

   entry.append(groupEdit->text());
   entry.append(countryEdit->text());
   entry.append(stateEdit->text());
   entry.append(cityEdit->text());
   entry.append(serverEdit->text());
   entry.append(portEdit->text());

   *retStr = entry.join(":");

   return *retStr;
}
