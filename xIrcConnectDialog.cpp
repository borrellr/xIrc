#include <qstring.h>
#include <qstringlist.h>
#include <qcombobox.h>
#include "xIrcServerEntry.h"
#include "xIrcConnectDialog.h"

xIrcConnectDialog::xIrcConnectDialog(QWidget *parent, const char *name)
    : serverConnectDialog (parent, name)
{
   portComboBox->setDuplicatesEnabled(false);
   listItems.clear();
}

xIrcConnectDialog::~xIrcConnectDialog()
{
}

void xIrcConnectDialog::initEntry(xIrcServerEntry *entry)
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
   {
      QStringList portList = QStringList::split(",", tmpStr);
      portComboBox->insertStringList(portList);
   }
}

void xIrcConnectDialog::appendLine()
{
   if (listItems.isEmpty()) {
      listItems.append(portLine->text());
   } else {
      QStringList list = listItems.grep(portLine->text());
      if (list.isEmpty()) {
         listItems.append(portLine->text());
      }
   }
}

xIrcServerEntry* xIrcConnectDialog::getEntry()
{
   xIrcServerEntry *entry = new xIrcServerEntry();

   entry->setGroup(groupEdit->text().latin1());
   entry->setCountry(countryEdit->text().latin1());
   entry->setState(stateEdit->text().latin1());
   entry->setCity(cityEdit->text().latin1());
   entry->setServer(serverEdit->text().latin1());
   entry->setPorts(portComboBox->currentText().latin1());

//   entry->showEntries();

   return entry;
}
