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
#include "xIrcServerEntry.h"

void xIrcServerEdit::enableOkButton()
{
   buttonOk->setEnabled(isEnable());
}

bool xIrcServerEdit::isEnable()
{
   return (!groupEdit->text().isEmpty() && !countryEdit->text().isEmpty() &&
     !cityEdit->text().isEmpty() && !serverEdit->text().isEmpty() &&
     !portEdit->text().isEmpty());
}



void xIrcServerEdit::initEntry( xIrcServerEntry *entry )
{
   QString tmpStr;

   tmpStr = entry->group();
   if (!tmpStr.isEmpty())
      groupEdit->setText(tmpStr);

   tmpStr = entry->country();
   if (!tmpStr.isEmpty())
      countryEdit->setText(tmpStr);

   tmpStr = entry->state();
   if (!tmpStr.isEmpty())
      stateEdit->setText(tmpStr);

   tmpStr = entry->city();
   if (!tmpStr.isEmpty())
      cityEdit->setText(tmpStr);

   tmpStr = entry->server();
   if (!tmpStr.isEmpty())
      serverEdit->setText(tmpStr);

   tmpStr = entry->ports();
   if (!tmpStr.isEmpty())
      portEdit->setText(tmpStr);
}


xIrcServerEntry * xIrcServerEdit::getEntry()
{
   xIrcServerEntry *entry = new xIrcServerEntry();

   entry->setGroup(groupEdit->text().latin1());
   entry->setCountry(countryEdit->text().latin1());
   entry->setState(stateEdit->text().latin1());
   entry->setCity(cityEdit->text().latin1());
   entry->setServer(serverEdit->text().latin1());
   entry->setPorts(portEdit->text().latin1());

   return entry;
}
