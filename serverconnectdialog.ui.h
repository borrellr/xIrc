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

enum { Group, Country, State, City, Server, Ports, Max };

void xIrcConnectDialog::initEntry(const QString &entry)
{
   QStringList tmpList = QStringList::split(":", entry);
   QString tmpStr;

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
   if (!tmpStr.isEmpty())
   {
      QStringList portList = QStringList::split(",", tmpStr);
      portComboBox->insertStringList(portList);
   }
}


const QString &xIrcConnectDialog::getEntry()
{
   QStringList tmpList;
   QString *tmpStr = new QString();

   tmpList.append(groupEdit->text());
   tmpList.append(countryEdit->text());
   tmpList.append(stateEdit->text());
   tmpList.append(cityEdit->text());
   tmpList.append(serverEdit->text());
   tmpList.append(portComboBox->currentText());
   *tmpStr = tmpList.join(":");

   return *tmpStr;
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
