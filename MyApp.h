#ifndef MYAPP_H
#define MYAPP_H

#include <xApp.h>
#include <qevent.h>

class MyApp : public xApplication
{
public:
   MyApp(int argc, char **argv);
   bool notify(QObject *pRcvr, QEvent *pEvt);
};

#endif
