/***************************************************************************
**    xResources.cpp  $Revision: 1.2 $ - $Name:  $ 
**    X Resource handling class
**
**    Copyright (C) 1997 1998 Joseph Croft <joe@croftj.net>
**    Copyright (C) 1996 Joseph Croft <jcroft@unicomp.net>
**
**    This library is free software; you can redistribute it and/or
**    modify it under the terms of the GNU Library General Public
**    License as published by the Free Software Foundation; either
**    version 2 of the License, or (at your option) any later version.
**
**    This library is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**    Library General Public License for more details.
**
**    You should have received a copy of the GNU Library General Public
**    License along with this library; if not, write to the Free
**    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
 ***************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <qapplication.h>
#include "xResources.h"

static int dbg = 0;

xResources *Resources;

/*
** This constructor Builds an X resource database from pDefaults, 
** the $HOME/.Xdefaults database, /usr/lib/X11/app-defaults/<appname> and 
** any appropriate command line options as specified through pOptDescList. 
**
** The order in which they are read are, pDefaults, $HOME/.Xdefaults,
** the apps-default file, then the command line options.
**
** pDefaults is a string array of entries w/ each string representing one 
** resource entry in the standard format used in the files. The array of
** strings should be terminated w/ a NULL pointer
**
** pOptDescList and tblCount are explained on the man page for XrmInitialize
** Suffice it to say here that they are used to describe how to process the
** command line arguments.
**
** argc and argv are pointers for returning the unprocessed command line 
** entries. The command line entries are gotten from the QApplication object
** for the program using argc() and argv() member functions. It is mandatory
** that the QApplication object is created prior to the creation of this 
** object for the propper handling of any command line parameters.
** After the creation of this object, any remaining command line processing
** should use the values returned by this constructor.
**
*/
xResources::xResources(const char *pDefaults[], XrmOptionDescList *pOptDescList,
                       int tblCount, int *argc, char **argv, const char *optsFile)
{
   XrmDatabase defDB = NULL, cmdDB = NULL, appDB = NULL, optDB = NULL;
   char *cp, fname[256], appName[256];

   if (dbg) fprintf(stdout, "xResources::xResources::():Enter!!!\n");
   if (dbg) fflush(stdout);
   XrmInitialize();
   rDB = NULL;

   /*
   ** Start by building a database using pDefaults
   */
   if (dbg) fprintf(stdout, "xResources::xResources::():Adding in Defaults\n");
   if (dbg) fflush(stdout);
   for (defDB = NULL; *pDefaults != NULL; pDefaults++)
      XrmPutLineResource(&defDB, *pDefaults);
   if (dbg) fprintf(stdout, "xResources::xResources::():Done\n");
   if (dbg) fflush(stdout);

   /*
   ** Now Build a database of the .Xdefaults in the hame directory
   */
   if (dbg) fprintf(stdout, "xResources::xResources::():Getting HOME\n");
   if (dbg) fflush(stdout);
   if ((cp = getenv("HOME")) == NULL)
      cp = "";
   strcpy(fname, cp);
   strcat(fname, "/.Xdefaults");
   if (dbg) fprintf(stdout, "xResources::xResources::():HOME Xdefaults = |%s|\n",
                             fname);
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xResources::xResources::():Maybe calling XrmGetFileDatabase\n");
   if (dbg) fflush(stdout);
   rDB = XrmGetFileDatabase(fname);
   if (dbg) fprintf(stdout, "xResources::xResources::():done...\n");
   if (dbg) fflush(stdout);

   if (optsFile)
      optDB = XrmGetFileDatabase(optsFile);

   /*
   ** Now Build a database of the appliation defaults
   */
   for (cp = argv[0] + strlen(argv[0]);
        cp != argv[0] && *cp != '/';
        cp--);
   if (*cp == '/')
      cp++;
   strcpy(appName, cp);
   strcpy(fname, "/usr/lib/X11/app-defaults/");
   if (dbg) fprintf(stdout, "xResources::xResources::():app-defaults from |%s|\n",
                             fname);
   if (dbg) fflush(stdout);
   strcat(fname, appName);
   if (dbg) fprintf(stdout, "xResources::xResources::():app-defaults from |%s|\n",
                             fname);
   if (dbg) fflush(stdout);
   appDB = XrmGetFileDatabase(fname);

   /*
   ** Lastly Build a database using the command line options
   */
   if (dbg) fprintf(stdout, "xResources::xResources::():Adding args!\n");
   if (dbg) fflush(stdout);
   XrmParseCommand(&cmdDB, *pOptDescList, tblCount, appName, argc, argv);
   if (dbg) fprintf(stdout, "xResources::xResources::():Merging Resources!\n");
   if (dbg) fflush(stdout);

   XrmCombineDatabase(defDB, &rDB, FALSE);
//   XrmMergeDatabases(xDefDB, &rDB);
   XrmCombineDatabase(appDB, &rDB, FALSE);
//   XrmMergeDatabases(appDB, &rDB);
   if (optDB != NULL)
      XrmCombineDatabase(optDB, &rDB, TRUE);
   XrmMergeDatabases(cmdDB, &rDB);

//   if (dbg) fprintf(stdout, "xResources::xResources::():Destroying temp databases!\n");
//   if (dbg) fflush(stdout);
//   XrmDestroyDatabase(defDB);
//   XrmDestroyDatabase(xDefDB);
//   XrmDestroyDatabase(appDB);
//   XrmDestroyDatabase(cmdDB);
   
   if (dbg) fprintf(stdout, "xResources::xResources::():Exit!!!\n");
   if (dbg) fflush(stdout);
}

/*
** const char *xResources::get(QObj *pObj, const char *pName, const char *pClassName)
**
** This Member function returns a pointer to the value found for the
** X Resource specified by pName and pClassName. 
**
** The Resources name and class name will be appended to the name and
** classname of the object pointed to by pObj appended to the name and
** classname of the application.
**
** If pObj is NULL on entry, pName and pClassName will only use the 
** app name and must otherwise be fully qualified through pName and
** pClassName.
*/
const char *xResources::get(xWidgetResInfo *pRInfo, const char *pResName,
                            const char *pClassName)
{
   QString nameStr(""), classStr("");
   char *cp;
   XrmValue rv;

   if (dbg) fprintf(stdout, "xResources::get():Building name string from |%s|\n",
                            pResName);
   if (dbg) fflush(stdout);
   nameStr = pRInfo->getName() + "." + pResName;
   if (dbg) fprintf(stdout, "xResources::get():Name is |%s|\n",
                            (const char *)nameStr);
   if (dbg) fflush(stdout);
   if (dbg) fprintf(stdout, "xResources::get():Building class string from |%s|\n",
                            pClassName);
   if (dbg) fflush(stdout);
   classStr = pRInfo->getClass() + "." + pClassName;
   if (dbg) fprintf(stdout, "xResources::get():Class Name is |%s|\n",
                            (const char *)classStr);
   if (dbg) fflush(stdout);
   if (XrmGetResource(rDB, (const char *)nameStr, (const char *)classStr,
                      &cp, &rv) == FALSE)
      rv.addr = NULL;
   else
   {
      if (dbg) fprintf(stdout, "xResources::get():Type = |%s|, Str = |%s|\n", 
                                cp, rv.addr);
   }
   if (dbg) fprintf(stdout, "xResources::get():Returning |%s|\n", 
                            (const char*)rv.addr);
   if (dbg) fflush(stdout);
   return((const char*)rv.addr);
}

void xResources::putByName(xWidgetResInfo *pRInfo, const char *pResName,
                           const char *value)
{
   QString nameStr("");
   XrmValue val;

   val.addr = (char *)value;
   val.size = strlen(value);
   if (dbg) fprintf(stdout, "xResources::get():Building name string from |%s|\n",
                            pResName);
   if (dbg) fflush(stdout);
   nameStr = pRInfo->getName() + "." + pResName;
   if (dbg) fprintf(stdout, "xResources::get():Name is |%s|\n",
                            (const char *)nameStr);
   if (dbg) fflush(stdout);
   XrmPutResource(&rDB, (const char *)nameStr, "String", &val);
}

void xResources::putByClass(xWidgetResInfo *pRInfo,
                            const char *pClassName, const char *value)
{
   QString classStr("");
   XrmValue val;

   val.addr = (char *)value;
   val.size = strlen(value);
   if (dbg) fprintf(stdout, "xResources::get():Building class string from |%s|\n",
                            pClassName);
   if (dbg) fflush(stdout);
   classStr = pRInfo->getClass() + "." + pClassName;
   if (dbg) fprintf(stdout, "xResources::get():Class Name is |%s|\n",
                            (const char *)classStr);
   if (dbg) fflush(stdout);
   XrmPutResource(&rDB, (const char *)classStr, "String", &val);
}

void xResources::setWidgetInit(const char *pDefaults[])
{
   XrmDatabase defDB = NULL;

   if (dbg) fprintf(stdout, "xResources::setWdgetInit():Adding in Defaults\n");
   if (dbg) fflush(stdout);
   for (defDB = NULL; *pDefaults != NULL; pDefaults++)
      XrmPutLineResource(&defDB, *pDefaults);
   if (dbg) fprintf(stdout, "xResources::setWdgetInit():Resources Built\n");
   if (dbg) fflush(stdout);
   XrmCombineDatabase(defDB, &rDB, FALSE);
   if (dbg) fprintf(stdout, "xResources::setWdgetInit():Resources Combined\n");
   if (dbg) fflush(stdout);
//   XrmDestroyDatabase(defDB);
   if (dbg) fprintf(stdout, "xResources::setWdgetInit():Done\n");
   if (dbg) fflush(stdout);
}   
