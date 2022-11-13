#include <qlineedit.h>
#include <qpushbutton.h>
#include "xIrcQuitDialog.h"

xIrcQuitDialog::xIrcQuitDialog(xDefaults &def)
{
   QString qMsg = def.get("QuitMessage");
   setQuitMsg(qMsg);
   setDefaultMsg();
   connect(defaultButton, SIGNAL(clicked()), this, SLOT(setDefaultMsg()));
}

void xIrcQuitDialog::setQuitMsg(QString& qMsg)
{
   quitMsg = qMsg;
}

void xIrcQuitDialog::setDefaultMsg()
{
   quitLineEdit->setText(quitMsg);
}

QString xIrcQuitDialog::text()
{
   return quitLineEdit->text();
}
