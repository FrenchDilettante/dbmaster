/*
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 */


#ifndef NEWDBWIZARD_H
#define NEWDBWIZARD_H

#include "ui_newdbwizard.h"
#include "ui_ndw_firstpage.h"
#include "ui_ndw_secondpage.h"

class NewDbWizard : public QWizard, Ui::NewDbWizard
{
Q_OBJECT
public:
  enum Pages {
    FirstPage,
    SecondPage,
    OptionsPage
  };
  NewDbWizard(QWidget *parent = 0);

public slots:
  void accept();

protected:
  void setupPages();
};

class NdwFirstPage : public QWizardPage, Ui::NdwFirstPage
{
Q_OBJECT
public:
  NdwFirstPage(QWizard *parent=0);
  void initializePage();
};

class NdwSecondPage : public QWizardPage, Ui::NdwSecondPage
{
Q_OBJECT
public:
	NdwSecondPage(QWizard *parent=0);
	void initializePage();

public slots:
	void browse();

protected slots:
  void test();
  void updateAlias();
};

#endif // NEWDBWIZARD_H
