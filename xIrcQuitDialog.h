#ifndef XIRCQUITDIALOG_H
#define XIRCQUITDIALOG_H

#include <qstring.h>
#include "quitform.h"
#include "xDefaults.h"

class xIrcQuitDialog : public quitDialog
{
   Q_OBJECT
public:
   xIrcQuitDialog(QWidget *parent = 0, const char *name = 0);
   ~xIrcQuitDialog(){};

   void setQuitMsg(QString& msg);
   QString text();

public slots:
   void setDefaultMsg();

private:
   QString quitMsg;
   QString currMsg;
};

#endif
