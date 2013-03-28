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


#ifndef FIRSTLAUNCHWIZARD_H
#define FIRSTLAUNCHWIZARD_H

#include <QtGui>
#include "ui_firstlaunchwizard.h"

class FirstLaunchWizard : public QWizard, Ui::FirstLaunchWizard {
Q_OBJECT
public:
    FirstLaunchWizard(QWidget *parent = 0);

    void accept();
};

#endif // FIRSTLAUNCHWIZARD_H
