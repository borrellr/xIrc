/*
** xIrcMsgDispatch.cpp Message Dispatch Entry class used for dispatching
** and thier responses going to the right widgets.
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
*/
#include "ircreply.h"
#include "xIrcMsgDispatch.h"

static int dbg = 0;

xIrcMsgDispatch::xIrcMsgDispatch()
{
   if (dbg) fprintf(stdout, "xIrcMsgDispatch::xIrcMsgDispatch():Enter\n");
   pSocket = NULL;
   if (dbg) fprintf(stdout, "xIrcMsgDispatch::xIrcMsgDispatch():Exit\n");
}

xIrcMsgDispatch::~xIrcMsgDispatch()
{
}

void xIrcMsgDispatch::setSocket(xIrcSocket *s)
{
   pSocket = s;
   if (pSocket == NULL && dspList.count() > 0)
      dspList.clear();      
}

bool xIrcMsgDispatch::outstanding(xIrcMessage *pMsg)
{
   bool rv = FALSE;
   xIrcMsgDispatchListIterator it(dspList);
   xIrcMsgDispatchEntry *pEnt;

   for (; (pEnt = it.current()) != NULL; ++it)
   {
      if (pEnt->sent() && pEnt->message()->rspCode == pMsg->rspCode)
      {
         rv = TRUE;
         break;
      }
   }
   return(rv);
}
   
void xIrcMsgDispatch::dispatchMsg(QObject *pObj, const char *pSlot, xIrcMessage *pMsg)
{
   xIrcMsgDispatchEntry *pEnt;
   int accepted;

   if (dbg) fprintf(stdout, "xIrcMsgDispatch::dispatchMsg():Enter\n");
   if (dbg) fflush(stdout);
   pEnt = new xIrcMsgDispatchEntry(pObj, pSlot, pMsg, accepted);
   if (dbg) fprintf(stdout, "xIrcMsgDispatch::dispatchMsg():accepted = %d\n", accepted);
   if (dbg) fflush(stdout);
   if (accepted == 1)
   {
      if (dbg) fprintf(stdout, "xIrcMsgDispatch::dispatchMsg():Adding Message to list\n");
      if (dbg) fflush(stdout);
      dspList.append(pEnt);
   }
   if (pSocket != NULL && (outstanding(pMsg) == FALSE || accepted == 0))
   {
      if (dbg) fprintf(stdout, "xIrcMsgDispatch::dispatchMsg():Sending Message\n");
      if (dbg) fflush(stdout);
      pEnt->setSent(TRUE);
      pSocket->sendIrcServerMessage(pMsg);
   }
   if (accepted <= 0)
   {
      if (dbg) fprintf(stdout, "xIrcMsgDispatch::dispatchMsg():Deleting Dispatch Entry\n");
      if (dbg) fflush(stdout);
      delete pEnt;
   }
   if (dbg) fprintf(stdout, "xIrcMsgDispatch::dispatchMsg():Exit- List Count = %d\n",
                            dspList.count());
   if (dbg) fflush(stdout);
}

bool xIrcMsgDispatch::proccessResponse(xIrcMessage *pMsg)
{
   xIrcMsgDispatchListIterator it(dspList);
   xIrcMsgDispatchEntry *pEnt, *pEnt1;

   if (dbg) fprintf(stdout, "xIrcMsgDispatch::proccessResponse():Enter\n");
   if (dbg) fflush(stdout);
   if (pMsg->rspCode >= 400 && pMsg->rspCode < 600)
   {
      if (dbg) fprintf(stdout, "xIrcMsgDispatch::proccessResponse():Have Error Response %d\n",
                              pMsg->rspCode);
      if (dbg) fflush(stdout);
      for (; (pEnt = it.current()) != NULL; ++it)
      {
         if (pEnt->sent() == TRUE)
         {
            if (dbg) fprintf(stdout, "xIrcMsgDispatch::proccessResponse():Dispatching error to first entry in list\n");
            if (dbg) fflush(stdout);
            pEnt->dispatchResponse(pMsg);
            if (dbg) fprintf(stdout, "xIrcMsgDispatch::proccessResponse():Removing 1st entry from list\n");
            if (dbg) fflush(stdout);
            dspList.remove(pEnt);
            break;
         }
      }
      if (dbg) fprintf(stdout, "xIrcMsgDispatch::proccessResponse():Exit- List Count = %d\n",
                               dspList.count());
      if (dbg) fflush(stdout);
      return(FALSE);
   }
      
   for (; (pEnt = it.current()) != NULL; ++it)
   {
      if (pEnt->sent() == TRUE && pEnt->is(pMsg))
      {
         if (pEnt->dispatchResponse(pMsg) == FALSE)
         {
            if (dbg) fprintf(stdout, "xIrcMsgDispatch::proccessResponse():Removing Entry from List\n");
            if (dbg) fflush(stdout);
            dspList.remove(pEnt);
            if (pSocket)
            {
               if (dbg) fprintf(stdout, "xIrcMsgDispatch::proccessResponse():Checking for next item in list to send\n");
               if (dbg) fflush(stdout);
               for (it.toFirst(); (pEnt1 = it.current()) != NULL; ++it)
               {
                  if (pEnt1->sent() == FALSE && outstanding(pEnt->message()) == FALSE)
                  {
                     if (dbg) fprintf(stdout, "xIrcMsgDispatch::proccessResponse():Found one! Sending it\n");
                     if (dbg) fflush(stdout);
                     pEnt1->setSent(TRUE);
                     pSocket->sendIrcServerMessage(pEnt1->message());
                     break;
                  }
               }
            }
         }
         break;
      }
   }
   if (dbg) fprintf(stdout, "xIrcMsgDispatch::proccessResponse():Exit- List Count = %d\n",
                            dspList.count());
   if (dbg) fflush(stdout);
   if (pEnt != NULL && pEnt->dispatch() == FALSE)
     return(FALSE);
   else
      return((pEnt == NULL) ? FALSE : TRUE);
}

