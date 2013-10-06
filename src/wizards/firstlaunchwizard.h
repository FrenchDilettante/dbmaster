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
