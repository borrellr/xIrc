#include <qlineedit.h>
#include <qpushbutton.h>
#include "xIrcKickDialog.h"

xIrcKickDialog::xIrcKickDialog(QWidget *parent, const char *name) :
    kickDialog(parent, name)
{
   connect(defaultButton, SIGNAL(clicked()), this, SLOT(setDefaultMsg()));
}

void xIrcKickDialog::setKickMsg(QString& qMsg)
{
   kickMsg = qMsg;
}

void xIrcKickDialog::setDefaultMsg()
{
   kickLineEdit->setText(kickMsg);
}

QString xIrcKickDialog::text()
{
   return kickLineEdit->text();
}
