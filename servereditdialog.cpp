/****************************************************************************
** Form implementation generated from reading ui file 'servereditdialog.ui'
**
** Created: Thu Oct 20 20:51:42 2022
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "servereditdialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "servereditdialog.ui.h"

/*
 *  Constructs a serverEditDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
serverEditDialog::serverEditDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "serverEditDialog" );
    setSizeGripEnabled( TRUE );

    QWidget* privateLayoutWidget = new QWidget( this, "layout3" );
    privateLayoutWidget->setGeometry( QRect( 10, 250, 190, 50 ) );
    layout3 = new QHBoxLayout( privateLayoutWidget, 11, 6, "layout3"); 

    buttonOk = new QPushButton( privateLayoutWidget, "buttonOk" );
    buttonOk->setEnabled( FALSE );
    buttonOk->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, buttonOk->sizePolicy().hasHeightForWidth() ) );
    buttonOk->setMinimumSize( QSize( 80, 30 ) );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    layout3->addWidget( buttonOk );

    buttonCancel = new QPushButton( privateLayoutWidget, "buttonCancel" );
    buttonCancel->setMinimumSize( QSize( 80, 30 ) );
    buttonCancel->setAutoDefault( TRUE );
    layout3->addWidget( buttonCancel );

    QWidget* privateLayoutWidget_2 = new QWidget( this, "layout2" );
    privateLayoutWidget_2->setGeometry( QRect( 10, 10, 190, 230 ) );
    layout2 = new QGridLayout( privateLayoutWidget_2, 1, 1, 11, 6, "layout2"); 

    serverEdit = new QLineEdit( privateLayoutWidget_2, "serverEdit" );

    layout2->addWidget( serverEdit, 4, 1 );

    stateLabel = new QLabel( privateLayoutWidget_2, "stateLabel" );

    layout2->addWidget( stateLabel, 2, 0 );

    cityLabel = new QLabel( privateLayoutWidget_2, "cityLabel" );

    layout2->addWidget( cityLabel, 3, 0 );

    groupLabel = new QLabel( privateLayoutWidget_2, "groupLabel" );

    layout2->addWidget( groupLabel, 0, 0 );

    countryEdit = new QLineEdit( privateLayoutWidget_2, "countryEdit" );

    layout2->addWidget( countryEdit, 1, 1 );

    portEdit = new QLineEdit( privateLayoutWidget_2, "portEdit" );

    layout2->addWidget( portEdit, 5, 1 );

    serverLabel = new QLabel( privateLayoutWidget_2, "serverLabel" );

    layout2->addWidget( serverLabel, 4, 0 );

    groupEdit = new QLineEdit( privateLayoutWidget_2, "groupEdit" );

    layout2->addWidget( groupEdit, 0, 1 );

    countryLabel = new QLabel( privateLayoutWidget_2, "countryLabel" );

    layout2->addWidget( countryLabel, 1, 0 );

    cityEdit = new QLineEdit( privateLayoutWidget_2, "cityEdit" );

    layout2->addWidget( cityEdit, 3, 1 );

    portLabel = new QLabel( privateLayoutWidget_2, "portLabel" );

    layout2->addWidget( portLabel, 5, 0 );

    stateEdit = new QLineEdit( privateLayoutWidget_2, "stateEdit" );

    layout2->addWidget( stateEdit, 2, 1 );
    languageChange();
    resize( QSize(214, 305).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( groupEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( enableOkButton() ) );
    connect( countryEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( enableOkButton() ) );
    connect( stateEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( enableOkButton() ) );
    connect( cityEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( enableOkButton() ) );
    connect( serverEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( enableOkButton() ) );
    connect( portEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( enableOkButton() ) );

    // tab order
    setTabOrder( groupEdit, countryEdit );
    setTabOrder( countryEdit, stateEdit );
    setTabOrder( stateEdit, cityEdit );
    setTabOrder( cityEdit, serverEdit );
    setTabOrder( serverEdit, portEdit );
    setTabOrder( portEdit, buttonOk );
    setTabOrder( buttonOk, buttonCancel );
}

/*
 *  Destroys the object and frees any allocated resources
 */
serverEditDialog::~serverEditDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void serverEditDialog::languageChange()
{
    setCaption( tr( "serverEditDialog" ) );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( tr( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
    stateLabel->setText( tr( "<p align=\"right\">State: </p>" ) );
    cityLabel->setText( tr( "<p align=\"right\">City: </p>" ) );
    groupLabel->setText( tr( "<p align=\"right\">Group: </p>" ) );
    serverLabel->setText( tr( "<p align=\"right\">Server: </p>" ) );
    countryLabel->setText( tr( "<p align=\"right\">Country: </p>" ) );
    portLabel->setText( tr( "<p align=\"right\">Port: </p>" ) );
}

