#include <qpushbutton.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include "xIrcChannelQuery.h"
#include "xIrcCommands.h"
#include "xIrcSocket.h"

extern xIrcCommands ircResponses;

xChannelQuery::xChannelQuery(QWidget *parent, const char *name)
  : channelDialog(parent, name)
{
   connect(chatButton, SIGNAL(clicked()), this, SLOT(names()));
   connect(dccChatButton, SIGNAL(clicked()), this, SLOT(dccChat()));
   connect(okButton, SIGNAL(clicked()), this, SLOT(joinChannel()));
   connect(joinButton, SIGNAL(clicked()), this, SLOT(joinChannel()));
}

xChannelQuery::~xChannelQuery()
{
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
