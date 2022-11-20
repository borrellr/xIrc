#ifndef XIRCCHANNELQUERY_H
#define XIRCCHANNELQUERY_H
#include "channeldialog.h"
#include "xIrcSocket.h"

#define xCQ_Clear    2
#define xCQ_Names    3
#define xCQ_Join     4

class xChannelQuery : public channelDialog
{
   Q_OBJECT
public:
   xChannelQuery(QWidget *parent = 0, const char *name = 0);
   ~xChannelQuery();
   enum  chanResults { Accepted, Rejected, Clear, Names, Join, DccChat };

signals:
   void ircMessageOut(xIrcMessage *pMsg);
   void hasResult(int results);

private slots:
   void names();
   void dccChat();
   void joinChannel();
};

#endif
