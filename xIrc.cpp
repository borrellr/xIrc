/***************************************************************************
**    xIrc.cpp  $Revision: 1.9 $ - $Name:  $ 
**    IRC Client Main Line code
**
**    Copyright (C) 1997 1998 Joseph Croft <joe@croftj.net>
** Copyright (C) 1995, 1996  Joseph Croft <jcroft@unicomp.net>  
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 1, or (at your option)
** any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
 ***************************************************************************/
#include <qapplication.h>

#include <qpainter.h>
#include <qwindowdefs.h>
#include <qfontinfo.h>
#include <qpixmap.h>
#include <qbuffer.h>
#include <qimage.h>
#include <qstring.h>
#include <qobject.h>
#include <qevent.h>
#include <qtimer.h>
#include <qkeycode.h>

#include <qlineedit.h>
#include <qlabel.h>
#include <qdialog.h>
#include <qmessagebox.h>
#include <qfiledialog.h>

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#include <xGauge.h>

#include "xIrcConnect.h"
#include "xIrcMsgDispatch.h"
#include "xIrcNickQuery.h"
#include "xIrcNickCombo.h"
#include "xIrcLineEditQuery.h"
#include "xIrcServerQuery.h"
#include "xIrcChannelQuery.h"
#include <xApp.h>
#include <xResources.h>
#include <xDefaults.h>
#include <xMisc.h>
#include "MyApp.h"

static bool dbg = FALSE;

xIrcConnect *pTWindow = NULL;
xChannelQuery *ChanQuery = NULL;
xIrcNickQuery *NickQuery = NULL;
xIrcLineEditQuery *KickQuery = NULL;
xIrcLineEditQuery *QuitQuery = NULL;
xServerQuery *ServQuery = NULL;
xWidgetResInfo appRes(NULL, QString("xirc"), QString("XIRC"));
QPixmap *AppPixMap;

xIrcMsgDispatch Dispatcher;

xDefaults Defaults;
xApplication *pApp = NULL;

QEvent qEvt(QEvent::User);

static const char *pInitialResources[] =
{
   "XIRC*PixMap: xIrc.ppm",
   "XIRC*PixMapPath: ./;~/;/usr/local/lib/xIrc;",
   "XIRC*Font.Family: Helvetica",
   "XIRC*Font.Size: 12",
   "XIRC*Font.Weight: normal",
   "XIRC*Menu.Font.Weight: Bold",
   "XIRC*Popup.Font.Weight: Bold",
   "XIRC*PushBtn.Font.Weight: Bold",
   "XIRC*PushBtn.Background: #afafaf",
   "XIRC*Background: 0xc3c3c3",
   "XIRC*Forground: black",
   "XIRC*BaseColor: light yellow",
   "XIRC*TextColor: black",
   "XIRC*AutoFocus: false",
   "XIRC*MessageBox.Background: #a00000",
   "XIRC*MessageBox.Forground: Yellow",
   "XIRC*MessageBox.BaseColor: #a00000",
   "XIRC*MessageBox.TextColor: Yellow",
   "XIRC*MessageBox.Font.Weight: Bold",
   "XIRC*MessageBox.Font.Size: 18",
   "XIRC*ListBox.BaseColor: #b8b8b8",
   "XIRC*MultiLine.Window.Background: #000090",
   "XIRC*MultiLine.Window.HighColor: white",
   "XIRC*MultiLine.Window.TextColor: grey72",
   "XIRC*MessageDialog.Columns: 60",
   "XIRC*MessageDialog.Lines: 7",
   "XIRC*MessageDialog.PushBtn.Background: #afafaf",
   "XIRC*MessageDialog*MultiLine.Window.Background: #bfbfbf",
   "XIRC*MessageDialog*MultiLine.Window.BaseColor: #9f9f9f",
   "XIRC*MessageDialog*MultiLine.Window.TextColor: Navy Blue",
   "XIRC*MultiLine.Window.Font.Family: Fixed",
   "XIRC*DccChat.Input.Font.Family: Fixed",
   "XIRC*DccChat.Columns: 80",
   "XIRC*DccChat.Lines: 11",
   "XIRC*DccChat.MircColors: TRUE",
   "XIRC*DccChat.CTCP2: False",
   "XIRC*DccChat.Beep: FALSE",
   "XIRC*MsgChat.Input.Font.Family: Fixed",
   "XIRC*MsgChat.Columns: 80",
   "XIRC*MsgChat.Lines.Channel: 22",
   "XIRC*MsgChat.Lines.Nick: 11",
   "XIRC*MsgChat.MircColors: TRUE",
   "XIRC*MsgChat.CTCP2: False",
   "XIRC*MsgChat.Beep: FALSE",
   "XIRC*ServerDialog.Servers.Background: #bfbfbf",
   "XIRC*ServerDialog.Font.Family: Fixed",
   "XIRC*ServerDialog.ImportFile: servers.ini",
   "XIRC*ServerDialog.ImportPath: ./",
   "XIRC*ServerDialog.ImportFilter: *.ini",
   "XIRC*ServerDialog.Filename: servers.dat",
   "XIRC*ServerDialog.Path: /usr/local/lib/xIrc",
   "XIRC*ServerDialog.Filter: .*",
   NULL
};

char *version = "$Name:  $";
static XrmOptionDescList opts = NULL;

static const char *defaultSpecHandler(QString strSeq)
{
   static QString rv;
   const char *cp = strSeq;

   rv = parseAttr(&cp, TRUE, FALSE);
   return((const char *)rv);
}

static void sigpipe(int s)
{
   s = 0;
   if (dbg) fprintf(stdout, "*** Got SIGPIPE!\n");
}

static void setDefaults()
{
   FILE *pfd;

   if (dbg) fprintf(stdout, "main():Opening defaults\n");   
   if (dbg) fflush(stdout);
   if ((pfd = fopen("xIrc.defaults", "r")) == NULL)
   {
      char dnam[128];
      const char *cp;
      
      if ((cp = getenv("HOME")) != NULL)
      {
         strcpy(dnam, cp);
         strcat(dnam, "/.xIrc");
         if (dbg) fprintf(stdout, "main():Trying defaults file |%s|\n", dnam);   
         if (dbg) fflush(stdout);
         pfd = fopen(dnam, "r");
//         if (dbg) fprintf(stdout, "main():Defaults file pointer = |0x%x|\n", pfd);   
//         if (dbg) fflush(stdout);
      }
   }
   Defaults.setCallBack(defaultSpecHandler);
//   Defaults.setEscapes(defEscapes);
   Defaults.load(pfd, NULL);
   if (dbg) Defaults.show();
}

static void setColors(xResources *r, xApplication *a)
{
   QColor fg, bg, baseColor, textColor;
   const char *ccp1 = NULL;

   if (dbg) fprintf(stdout, "main():Setting Colors and Pallete\n");   
   if (dbg) fflush(stdout);

   if ((ccp1 = r->get(&appRes, "background", "Background")) == NULL)
      ccp1 = "0xc3c3c3";
   setStrColor(bg, ccp1);

   if ((ccp1 = r->get(&appRes, "foreground", "Foreground")) == NULL)
      ccp1 = "black";
   setStrColor(fg, ccp1);

   if ((ccp1 = r->get(&appRes, "basecolor", "BaseColor")) != NULL)
      setStrColor(baseColor, ccp1);
   else
      baseColor = bg;

   if ((ccp1 = r->get(&appRes, "textcolor", "TextColor")) != NULL)
      setStrColor(textColor, ccp1);
   else
      textColor = fg;

   QColorGroup g(fg, bg, bg.light(), bg.dark(), bg.dark(120), fg, bg);
   QPalette p( g, g, g );
   a->setPalette(p);
}

static void setFonts(xResources *r, xApplication *a)
{
   char fontWeight[80], fontSize[80];
   const char *ccp1, *ccp2, *ccp3;

   ccp1 = Resources->get(&appRes, "font.family", "Font.Family");
   ccp2 = Resources->get(&appRes, "font.weight", "Font.Weight");
   ccp3 = Resources->get(&appRes, "font.size", "Font.Size");

   if (ccp1 == NULL)
      ccp1 = a->font().family();
   if (ccp2 == NULL)
   {
      sprintf(fontWeight, "%d", a->font().weight());
      ccp2 = fontWeight;
   }
   if (ccp3 == NULL)
   {
      sprintf(fontSize, "%d", a->font().pointSize());
      ccp3 = fontSize;
   }

   a->setFont(setStrFont(ccp1, ccp3, ccp2));
}

static void setPixMap()
{
   const char *ccp1, *ccp2;
   char *cp, tmpBuf[512], pmBuf[256];

   if (dbg) fprintf(stdout, "main():Getting Pixmap file name\n");   
   AppPixMap = NULL;
   if ((ccp1 = Resources->get(&appRes, "pixmap", "PixMap")) != NULL)
   {
      if ((ccp2 = Resources->get(&appRes, "pixmappath", "PixMapPath")) != NULL)
      {
         strcpy(tmpBuf, ccp2);
         for (cp = tmpBuf; *cp; cp++)
            if (*cp == ';')
               *cp = '\0';
         *(cp + 1) = '\0';
         for (cp = tmpBuf; *cp; cp++)
         {
            if (dbg) fprintf(stdout, "main():Pixmap Path: |%s|\n", cp);
            AppPixMap = new QPixmap;
            strcpy(pmBuf, cp);
            if (pmBuf[strlen(cp) - 1] != '/')
               strcat(pmBuf, "/");
            strcat(pmBuf,ccp1);
            if (dbg) fprintf(stdout, "main():Attempting to open Pixmap: |%s|\n", pmBuf);
            if (!AppPixMap->load(pmBuf))
            {
               if (dbg) fprintf(stdout, "main():Error opening Pixmap: |%s|\n", pmBuf);   
               delete AppPixMap;
               AppPixMap = NULL;
            }
            else
               break;
            while (*cp) cp++;
         }
      }
      else
      {
         delete AppPixMap;
         AppPixMap = NULL;
      }
   }
   else
   {
      delete AppPixMap;
      AppPixMap = NULL;
   }
}

static void InitializeWindows()
{
   if (dbg) fprintf(stdout, "main():ServQuery\n");   
   if (dbg) fflush(stdout);
   ServQuery = new xServerQuery(&appRes, NULL, "Server Selection");

   if (dbg) fprintf(stdout, "main():Opening channel Query\n");   
   if (dbg) fflush(stdout);
   ChanQuery = new xChannelQuery(&appRes, NULL, "Channels & People");

   if (dbg) fprintf(stdout, "main():NickQuery\n");   
   if (dbg) fflush(stdout);
   NickQuery = new xIrcNickQuery(&appRes, NULL, "Nickname");

   if (dbg) fprintf(stdout, "main():KickQuery\n");   
   if (dbg) fflush(stdout);
   KickQuery = new xIrcLineEditQuery(&appRes, "Enter Kick Message", "", NULL,
                                  "Kick Message", "KICKMESSAGE", &Defaults);

   if (dbg) fprintf(stdout, "main():QuitQuery\n");   
   if (dbg) fflush(stdout);
   QuitQuery = new xIrcLineEditQuery(&appRes, "Enter Quit Message", "", NULL,
                                  "Quit Message", "QUITMESSAGE", &Defaults);
   if (dbg) fprintf(stdout, "main():xIrcConnect\n");   
   if (dbg) fflush(stdout);
//   xIrcServerTable *pTWindow = new xIrcServerTable(&appRes, NULL, "Server Selection Table");
   pTWindow = new xIrcConnect(&appRes);
   
   if (dbg) fprintf(stdout, "main():Showing/Setting/executing app\n");
   if (dbg) fflush(stdout);
}

static void DeleteWindows()
{
   if (pTWindow)
   {
      if (dbg) fprintf(stdout, "main():Deleting pTWindow\n");
      if (dbg) fflush(stdout);
      delete pTWindow;
   }
   if (ChanQuery)
   {
      if (dbg) fprintf(stdout, "main():Deleting ChanQuery\n");
      if (dbg) fflush(stdout);
      delete ChanQuery;
   }
   if (NickQuery)
   {
      if (dbg) fprintf(stdout, "main():Deleting NickQuery\n");
      if (dbg) fflush(stdout);
      delete NickQuery;
   }
   if (KickQuery)
   {
      if (dbg) fprintf(stdout, "main():Deleting KickQuery\n");
      if (dbg) fflush(stdout);
      delete KickQuery;
   }
   if (QuitQuery)
   {
      if (dbg) fprintf(stdout, "main():Deleting QuitQuery\n");
      if (dbg) fflush(stdout);
      delete QuitQuery;
   }
   if (ServQuery)
   {
      if (dbg) fprintf(stdout, "main():Deleting ServQuery\n");
      if (dbg) fflush(stdout);
      delete ServQuery;
   }
   if (dbg) fprintf(stdout, "main():Exiting!\n");
   if (dbg) fflush(stdout);
}

int main(int argc, char **argv)
{
   QFont defFont;
   struct sigaction sa;
   const char *ccp1;
   char buf[256];

   if (dbg) fprintf(stdout, "main():Enter\n");   
   if (dbg) fflush(stdout);

   /*
   ** Personal preference is to initialize each member individualy, but some
   ** flavors of Unix, such as SGI, don't have all the members Linux does.
   ** This should be just as effective though for both systems.
   */
   memset(&sa, 0, sizeof(sa));
   sa.sa_handler = sigpipe;
   sigaction(SIGPIPE, &sa, NULL);

   ccp1 = NULL;
   Resources = new xResources(&ccp1, &opts, 0, &argc, argv);
   Resources->setWidgetInit(pInitialResources);
   MyApp a( argc, argv );
   pApp = &a;

   setDefaults();
   setColors(Resources, pApp);
   setFonts(Resources, pApp);
   setPixMap();
   InitializeWindows();

   a.setMainWidget( pTWindow );
   pTWindow->show();
   for (;;)
   {
      if (NickQuery->exec() == QDialog::Accepted)
         break;
      else
         QMessageBox::warning(pTWindow, "Error", "You must choose a NickName");
   }
   sprintf(buf, "xIrc - %s", NickQuery->text());
   pTWindow->setCaption(buf);
   pTWindow->show();
   pTWindow->newServer();

   a.exec();

   DeleteWindows();
}
