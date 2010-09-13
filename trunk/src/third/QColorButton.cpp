/*
* Copyright (C) 2008/07/02 Florian Becker <fb@pyades.com>
*
* The ownership of this document rests with the Pyades Technologies GmbH.
* It is strictly prohibited to change, sell or publish it in any way. In
* case you have access to this document, you are obligated to ensure its
* nondisclosure. Noncompliances will be prosecuted.
*
* Diese Datei ist Eigentum der Pyades Technologies GmbH. Ändern, verkaufen
* oder auf eine andere Weise verbreiten und Öffentlich machen ist strikt
* untersagt. Falls Sie Zugang zu dieser Datei haben, sind Sie verpflichtet
* alles Mögliche für deren Geheimhaltung zu tun. Zuwiderhandlungen werden
* strafrechtlich verfolgt.
*/

/* qt header */
#include <QColorDialog>

/* local header */
#include "QColorButton.h"

QColorButton::QColorButton( QWidget *parent )
    : QPushButton( parent ),
    mEnableColorText( true ),
    mEnableToolTip( true ) {

  connect( this, SIGNAL( clicked() ), SLOT( slotChangeColor() ) );
  setCurrentColor( "#ffffff" );
}

void QColorButton::setCurrentColor( const QColor &currentColor ) {

  if ( mCurrentColor == currentColor )
    return;

  mCurrentColor = currentColor;
  if ( mEnableColorText )
    setColorText( mCurrentColor.name() );
  if ( mEnableToolTip )
    setToolTip( mCurrentColor.name() );
  QPixmap pix( 16, 16 );
  pix.fill( mCurrentColor );
  setIcon( pix );
}

void QColorButton::setText( const QString &text ) {

  if ( mRealText == text )
    return;

  mRealText = text;
  QPushButton::setText( mRealText );
}

void QColorButton::enableColorText( bool enableColorText ) {

  if ( mEnableColorText == enableColorText )
    return;

  mEnableColorText = enableColorText;
  if ( mEnableColorText )
    setColorText( mCurrentColor.name() );
  else
    setText( mRealText );
}

void QColorButton::enableToolTip( bool enableToolTip ) {

  if ( mEnableToolTip == enableToolTip )
    return;

  mEnableToolTip = enableToolTip;
  if ( mEnableToolTip )
    setToolTip( mCurrentColor.name() );
  else
    setToolTip( "" );
}

void QColorButton::setColorText( const QString &colorText ) {

  if ( text() == colorText )
    return;

  QPushButton::setText( colorText );
}

void QColorButton::slotChangeColor() {

  QColor newColor = QColorDialog::getColor( mCurrentColor, this );
  if ( newColor.isValid() )
    setCurrentColor( newColor );
}
