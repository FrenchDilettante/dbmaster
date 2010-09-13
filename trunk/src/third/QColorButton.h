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

#ifndef QCOLORBUTTON_H
#define QCOLORBUTTON_H

/* qt header */
#include <QPushButton>

/**
 * Colorbutton, to choose and display the actual color.
 *
 * @brief Choose color via QPushButton and display it.
 * @author Florian Becker (Pyades Technologies GmbH) <fb@pyades.com>
 * @date 2008/07/02
 * @since 1.0.0
 * @version 1.0.0
 */
class QColorButton : public QPushButton {

  Q_OBJECT
  Q_CLASSINFO( "author", "Florian Becker" )
  Q_CLASSINFO( "email", "fb@pyades.com" )
  Q_CLASSINFO( "company", "Pyades Technologies GmbH" )
  Q_CLASSINFO( "link", "http://www.pyades.com" )
  Q_CLASSINFO( "date", "2008/07/02" )

public:
  /**
   * Default constructor for QColorButton.
   *
   * @param parent   Parent widget.
   * @since 1.0.0
   */
  QColorButton( QWidget *parent = 0 );

  /**
   * Set the current color.
   *
   * @param color   The color to set.
   * @since 1.0.0
   */
  void setCurrentColor( const QColor &color );

  /**
   * Return the current color.
   *
   * @return The current color.
   * @since 1.0.0
   */
  inline QColor currentColor() const { return mCurrentColor; }

  /**
   * Set the button text.
   *
   * @param text   The text to set.
   * @since 1.0.0
   */
  void setText( const QString &text );

  /**
   * Enable, to display the user defined text if set, else the color value.
   *
   * @param enableColorText   True, to enable user defined text if set, else
   * the color value - otherwise false. Default is true.
   * @since 1.0.0
   */
  void enableColorText( bool enableColorText );

  /**
   * Is the user defined text if set, else the color value enabled?
   *
   * @return True, if user defined text if set, else the color value is enabled
   * - otherwise false.
   * @since 1.0.0
   */
  inline bool isColorTextEnabled() const { return mEnableColorText; }

  /**
   * Enable, to display the color value as tooltip.
   *
   * @param enableToolTip   True, to enable the color value tooltip - otherwise
   * false. Default is true.
   * @since 1.0.0
   */
  void enableToolTip( bool enableToolTip );

  /**
   * Is the color value tooltip enabled?
   *
   * @return True, if the color value tooltip is enabled - otherwise false.
   * @since 1.0.0
   */
  inline bool isToolTipEnabled() const { return mEnableToolTip; }

private:
  /**
   * Intern function to set the new or changed color value as text.
   *
   * @param colorText   The color value as text.
   * @since 1.0.0
   */
  void setColorText( const QString &colorText );

  /**
   * Is the color text enabled?
   *
   * @since 1.0.0
   */
  bool mEnableColorText;

  /**
   * Is the color tooltip enabled?
   *
   * @since 1.0.0
   */
  bool mEnableToolTip;

  /**
   * The current color.
   *
   * @since 1.0.0
   */
  QColor mCurrentColor;

  /**
   * The real text.
   *
   * @since 1.0.0
   */
  QString mRealText;

private slots:
  /**
   * Internal slot to call the colordialog and change the color.
   *
   * @since 1.0.0
   */
  void slotChangeColor();
};

#endif
