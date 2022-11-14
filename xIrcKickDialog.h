#ifndef XIRCKICKDIALOG_H
#define XIRCKICKDIALOG_H

#include <qstring.h>
#include "kickmsgdialog.h"
#include "xDefaults.h"

class xIrcKickDialog : public kickDialog
{
   Q_OBJECT
public:
   xIrcKickDialog(QWidget *parent = 0, const char *name = 0);
   ~xIrcKickDialog(){};

   void setKickMsg(QString& msg);
   QString text();

public slots:
   void setDefaultMsg();

private:
   QString kickMsg;
   QString currMsg;
};

#endif
