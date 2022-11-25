/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/
#include <qmessagebox.h>

const char * xIrcBanDialog::nick()
{
   return nickEdit->text().latin1();
}


const char * xIrcBanDialog::chan()
{
   return chanEdit->text().latin1();
}


const char * xIrcBanDialog::mask()
{
   return maskEdit->text().latin1();
}


const char * xIrcBanDialog::text()
{
   return messEdit->text().latin1();
}


void xIrcBanDialog::setNick( const QString &str )
{
   nickEdit->setText(str);
}


void xIrcBanDialog::setText( const QString &str )
{
   messEdit->setText(str);
}


void xIrcBanDialog::setChan( const QString &str )
{
   chanEdit->setText(str);
}


void xIrcBanDialog::setMask( const QString &str )
{
   maskEdit->setText(str);
}


void xIrcBanDialog::setDefaultMsg( const QString &str )
{
   if (!str.isEmpty()) {
      messEdit->setText(str);
      defaultMsg = str;
   }
}


void xIrcBanDialog::banSlot()
{
   if (maskEdit->text().isEmpty())
      QMessageBox::warning(this, "Error", "No Ban Mask Specified");
   else
      done(Accepted);
}


void xIrcBanDialog::kickSlot()
{
   if (maskEdit->text().isEmpty())
      QMessageBox::warning(this, "Error", "No Ban Mask Specified");
   else
      done(Kick);
}


void xIrcBanDialog::unbanSlot()
{
   if (maskEdit->text().isEmpty())
      QMessageBox::warning(this, "Error", "No Ban Mask Specified");
   else
      done(UnBan);
}


void xIrcBanDialog::defaultSlot()
{
   messEdit->setText(defaultMsg);
}
