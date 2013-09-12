#include "tooltipframe.h"
#include "ui_tooltipframe.h"

#include "../iconmanager.h"

#include <time.h>

TooltipFrame::TooltipFrame(QWidget *parent)
  : QFrame(parent) {
  setupUi(this);

  if (tooltips.size() == 0) {
    tooltips
        << tr("Press Ctrl+Espace in the query editor to display the completer.")
        << tr("While a query is executed, you can edit an another one.")
        << tr("You can export a table or a query's content in a CSV file by right-clicking on the spreadsheet.")
        << tr("All queries are logged. Check them out in the Console tab.")
        << tr("DbMaster is a free software developped by volunteers. Join the community and help us making it better !")
        << tr("Move from a tab to another by pressing Alt+Left/Right.")
        << tr("Name your connections with an alias to avoid blanks.")
        << tr("You like having a good-looking code ? In the Edit menu, case can be modified.")
        << tr("'want to prove to your teacher that your query works ? Right-clic on the result, and export it in HTML or CSV.");
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
  if (tooltips.size() > 1) {
    do {
        index = rand() % tooltips.size();
    } while (index == currentIndex);
  } else {
    index = 0;
  }

  currentIndex = index;
  textBrowser->setText(tooltips[index]);
}
