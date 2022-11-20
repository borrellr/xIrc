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
#include <qlineedit.h>
#include <qlistbox.h>

void channelDialog::setText( const QString &text )
{
   channelEdit->setText(text);
}

const char *channelDialog::editText()
{
    return channelEdit->text().latin1();
}

void channelDialog::enableButtons()
{
    QString str = channelEdit->text();
    if (str.isEmpty()) {
       okButton->setEnabled(false);
       joinButton->setEnabled(false);
       chatButton->setEnabled(false);
       dccChatButton->setEnabled(false);
       clearButton->setEnabled(false);
    } else {
       okButton->setEnabled(true);
       joinButton->setEnabled(true);
       chatButton->setEnabled(true);
       dccChatButton->setEnabled(true);
       clearButton->setEnabled(true);
    }
}

void channelDialog::insertItems( const QString &channels )
{
   QStringList strList;
   if (!channels.isEmpty()) {
      strList = QStringList::split(" ", channels);
      if (strList.isEmpty())
         channelBox->insertItem("#chatzone");
      else
         channelBox->insertStringList(strList);
   }
}

void channelDialog::updateItems(QListBoxItem *item)
{
   QString text(item->text());

   if (!text.isEmpty())
      channelEdit->setText(text);
}
