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
#include <qmessagebox.h>

void xChannelQuery::setText( const QString &text )
{
   channelEdit->setText(text);
}

const char *xChannelQuery::editText()
{
    return channelEdit->text().latin1();
}

void xChannelQuery::enableButtons()
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

void xChannelQuery::insertItems( const QString &channels )
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

void xChannelQuery::updateItems(QListBoxItem *item)
{
   QString text(item->text());

   if (!text.isEmpty())
      channelEdit->setText(text);
}


void xChannelQuery::names()
{
   QString str = channelEdit->text();
   xIrcMessage msg;

   if (!str.isEmpty()) {
      str = str.stripWhiteSpace();
      if (str.startsWith("#"))
         msg.rspCode = ircResponses.code("NAMES");
      else
         msg.rspCode = ircResponses.code("WHOIS");
      msg.dstStr = str.latin1();
      msg.msgStr = "";
      emit ircMessageOut(&msg);
   }
}


void xChannelQuery::dccChat()
{
   QString str = channelEdit->text();

   if (!str.isEmpty()) {
      str = str.stripWhiteSpace();
      if (str.startsWith("#"))
         QMessageBox::warning(this, "Error", "Cannot DCC Chat to a channel");
      else
         emit hasResult(DccChat);
   }
}


void xChannelQuery::joinChannel()
{
   QString str = channelEdit->text();

   if (!str.isEmpty()) {
      str = str.stripWhiteSpace();
      emit hasResult(Accepted);
   }
}
