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


#include "tooltipframe.h"
#include "ui_tooltipframe.h"

#include "../iconmanager.h"

TooltipFrame::TooltipFrame(QWidget *parent)
  : QFrame(parent) {
  setupUi(this);

  if (tooltips.size() == 0) {
    tooltips
        << tr("Press Ctrl+Espace in the query editor to display the completer.")
        << tr("While a query is executed, you can edit an another one.")
        << tr("You can export a table or a query's content in a CSV file by right-clicking on the spreadsheet.")
        << tr("While it is connected, a database cannot be edited.")
        << tr("All queries are logged. Check them out in Tools - Logs.")
        << tr("DbMaster is a free software developped by volunteers. Join the community and help us making it better !");
  }

  srand(time(NULL));

  currentIndex = -1;
  showTooltip();

  connect(nextButton, SIGNAL(clicked()), this, SLOT(showTooltip()));

  // theme icons
  nextButton->setIcon(IconManager::get("go-next"));
  label_2->setPixmap(IconManager::get("help-faq").pixmap(32, 32));
}

void TooltipFrame::showTooltip() {
  int index;
  if(tooltips.size() > 1)
  {
    do
        index = rand() % tooltips.size();
    while(index == currentIndex);
  } else
    index = 0;

  currentIndex = index;
  label->setText(tr("Did you know ? #%1/%2")
                 .arg(currentIndex+1)
                 .arg(tooltips.size()));
  textBrowser->setText(tooltips[index]);
}
