#include "MyApp.h"

static bool dbg = FALSE;

MyApp::MyApp(int argc, char **argv) :
         xApplication(argc, argv)
{
   if (dbg) fprintf(stdout, "MyApp::MyApp()!!!\n");
   if (dbg) fflush(stdout);
}

bool MyApp::notify(QObject *pRcvr, QEvent *pEvt)
{
   return(xApplication::notify(pRcvr, pEvt));
}

