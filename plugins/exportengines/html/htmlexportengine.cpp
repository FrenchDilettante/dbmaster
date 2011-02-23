#include "htmlexportengine.h"
#include "htmlwizardpage.h"

HtmlExportEngine::HtmlExportEngine() {
  m_wizardPage = new HtmlWizardPage(model, NULL);
}

void HtmlExportEngine::process(QFile *f) {

  emit progress(-1);
  f->write("<html>\n<head>\n<title></title>\n</head>\n<body>\n");

  /*
   * Writing query
   */
//  if(wizard->field("htmlexportquery").toBool())
//  {
//    f->write("<p>");
//    f->write(token->query().replace("\n", "<br />").toAscii());
//    f->write("</p>\n");
//  }

  /*
   * Writing header
   */
  f->write("<table border=\"1\">\n");
  f->write("<tr>");
  for(int i=0; i<model->columnCount(); i++) {
    f->write(model->headerData(i, Qt::Horizontal).toString()
            .prepend("<th>").append("</th>").toAscii());
  }

  f->write("</tr>\n");

  /*
   * Writing data
   */
  QModelIndex idx;
  for (int y=0; y<model->rowCount(); y++) {
    f->write("<tr>");
    for (int x=0; x<model->columnCount(); x++) {
      idx = model->index(y, x);
      if(idx.data().canConvert(QVariant::String))
        f->write(idx.data().toString()
                .prepend("<td>").append("</td>")
                .toAscii());
    }
    f->write("</tr>\n");

    emit progress(y);
  }

  f->write("</table>\n");
  f->write("</body></html>");
}

void HtmlExportEngine::setModel(QAbstractItemModel *m) {
  model = m;
  ((HtmlWizardPage*) m_wizardPage)->setModel(m);
}

Q_EXPORT_PLUGIN2(dbm_html_exportengine, HtmlExportEngine)
