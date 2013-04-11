#include "csvexportengine.h"
#include "csvwizardpage.h"

#include "iconmanager.h"

#include <QDebug>
#include <QVariant>

CsvExportEngine::CsvExportEngine() {
  m_wizardPage = new CsvWizardPage();
}

void CsvExportEngine::process(QFile *f) {

  QString del = wizard->field("csvdelimiter").toString();
  QString sep = wizard->field("csvseparator").toString();

  emit progress(-1);

  // columns name (= header)
  if(wizard->field("csvheader").toBool()) {
    QStringList columns;
    for( int i=0; i<model->columnCount(); i++ )
      columns << model->headerData( i, Qt::Horizontal ).toString()
                  .prepend( del ).append( del );

    f->write(columns.join(sep).append(sep).toAscii());
    f->write("\n");
  }


  /*
   * Writing data
   */
  QModelIndex idx;
  // échapement des quotes : par exemple " -> \"
  QString escape = del;
  escape.prepend("\\");
  for (int y=0; y<model->rowCount(); y++) {
    // each row

    for (int x=0; x<model->columnCount(); x++) {
      // each piece of data

      idx = model->index(y, x);
      if (idx.data().canConvert(QVariant::String)) {
        QString data = idx.data().toString().replace(del, escape);
        f->write(data.prepend(del).append(del + sep).toAscii());
      }
    }
    f->write("\n");

    emit progress(y);
  }
}

