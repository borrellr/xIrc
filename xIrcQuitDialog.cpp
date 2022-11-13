#include <qlineedit.h>
#include <qpushbutton.h>
#include "xIrcQuitDialog.h"

xIrcQuitDialog::xIrcQuitDialog(QWidget *parent, const char *name) :
    quitDialog(parent, name)
{
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
