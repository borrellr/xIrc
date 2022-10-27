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
#include <qstring.h>
#include "xIrcServerEntry.h"

void serverEditDialog::enableOkButton()
{
   buttonOk->setEnabled(isEnable());
}

bool serverEditDialog::isEnable()
{
   return (!groupEdit->text().isEmpty() && !countryEdit->text().isEmpty() &&
     !cityEdit->text().isEmpty() && !serverEdit->text().isEmpty() &&
     !portEdit->text().isEmpty());
}

