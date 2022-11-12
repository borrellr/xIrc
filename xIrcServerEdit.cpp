#include <qstring.h>
#include <qstringlist.h>
#include <qcombobox.h>
#include "servereditdialog.h"
#include "xIrcServerEntry.h"
#include "xIrcServerEdit.h"
#include <iostream>

xIrcServerEdit::xIrcServerEdit(QWidget *parent, const char *name)
    : serverEditDialog (parent, name)
{
}

xIrcServerEdit::~xIrcServerEdit()
{
}

void xIrcServerEdit::initEntry(xIrcServerEntry *entry)
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

xIrcServerEntry* xIrcServerEdit::getEntry()
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
