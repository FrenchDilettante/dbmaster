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


#include "exportwizard.h"
#include "ui_exportwizard.h"

#include "../iconmanager.h"

ExportWizard::ExportWizard(QueryToken *token, QWidget *parent)
  : QWizard(parent)
{
  setupUi(this);

  this->token = token;

  setWindowIcon(IconManager::get("filesaveas"));

  setPage(FirstPage,  new EwFirstPage(this));
  setPage(CsvPage,    new EwCsvPage(this));
  setPage(HtmlPage,   new EwHtmlPage(this));
  setPage(SqlPage,    new EwSqlPage(this));
  setPage(ExportPage, new EwExportPage(token, this));
}

/**
 * First page
 */
QString EwFirstPage::lastPath;

EwFirstPage::EwFirstPage(QWizard *parent)
    : QWizardPage(parent)
{
  setupUi(this);

  formatMap[csvRadioButton]   = ExportWizard::CsvFormat;
  formatMap[htmlRadioButton]  = ExportWizard::HtmlFormat;
  formatMap[sqlRadioButton]   = ExportWizard::SqlFormat;

  csvRadioButton->setIcon(IconManager::get("spreadsheet"));
  htmlRadioButton->setIcon(IconManager::get("html"));

  registerField("path*", pathLineEdit);
  sqlRadioButton->setVisible(false);
  connect(browseButton, SIGNAL(clicked()), this, SLOT(browse()));

  pathLineEdit->setCompleter(new QCompleter(
      new QDirModel(QStringList("*"),
                    QDir::AllDirs | QDir::NoDotAndDotDot,
                    QDir::Type, this),
      this));

  pathLineEdit->setText(lastPath);
}

void EwFirstPage::browse()
{
  QMap<ExportWizard::Format, QString> formats;
  formats[ExportWizard::CsvFormat]  = tr("CSV file (*.csv)");
  formats[ExportWizard::HtmlFormat] = tr("HTML file (*.html)");
//  formats[ExportWizard::SqlFormat]  = tr("SQL file (*.sql)");

  lastPath = QFileDialog::getSaveFileName(this,
                                          tr("Output file"),
                                          QDir::homePath(),
                                          QStringList(formats.values()).join(";;"),
                                          &formats[selectedFormat()]
                                          );

  QString extension = QFileInfo(lastPath).suffix().toLower();
  // Si le nom de fichier contient une extension, on met à jour l'option choisie
  if (extension == "csv") {
    csvRadioButton->setChecked(true);
  } else if(extension == "html") {
    htmlRadioButton->setChecked(true);
  } else if(extension == "sql") {
    sqlRadioButton->setChecked(true);
  } else {
    // L'extension n'a pas été reconnue : on va l'ajouter
    if (lastPath.endsWith(".")) {
      lastPath = lastPath.left(lastPath.length()-1);
    }

    if (csvRadioButton->isChecked()) {
      lastPath += ".csv";
    } else if (htmlRadioButton->isChecked()) {
      lastPath += ".html";
    } else if (sqlRadioButton->isChecked()) {
      lastPath += ".sql";
    }
  }

  pathLineEdit->setText(lastPath);
}

int EwFirstPage::nextId() const
{
  if(csvRadioButton->isChecked())
    return ExportWizard::CsvPage;

  if(htmlRadioButton->isChecked())
    return ExportWizard::HtmlPage;

  if(sqlRadioButton->isChecked())
    return ExportWizard::SqlPage;
}

ExportWizard::Format EwFirstPage::selectedFormat()
{
  foreach(QRadioButton *b, formatMap.keys())
    if(b->isChecked())
      return formatMap[b];

  return ExportWizard::CsvFormat;
}


/**
 * CSV page
 */
EwCsvPage::EwCsvPage( QWizard *parent )
    : QWizardPage( parent )
{
  setupUi( this );

  registerField( "csvdelimiter", delimiterLineEdit );
  registerField( "csvheader"   , headerCheckBox );
  registerField( "csvseparator", separatorLineEdit );
}

int EwCsvPage::nextId() const
{
  return ExportWizard::ExportPage;
}

/**
 * HTML page
 */
EwHtmlPage::EwHtmlPage(QWizard *parent)
  : QWizardPage(parent)
{
  setupUi(this);

  registerField("htmlexportquery",  queryGroupBox,
                "checked", SIGNAL(toggled(bool)));
  registerField("htmlsyntax",       shCheckBox);
  registerField("htmlwholeresult",  wholeResultRadioButton);
  registerField("htmllimit",        limitSpinBox);
  registerField("htmllimitto",      limitComboBox);
}

void EwHtmlPage::initializePage()
{
  limitResultRadioButton->setChecked(
      ((ExportWizard*)wizard())->token->model()->rowCount() >= 100);
}

int EwHtmlPage::nextId() const
{
  return ExportWizard::ExportPage;
}


/**
 * SQL page
 */
EwSqlPage::EwSqlPage( QWizard *parent )
    : QWizardPage( parent )
{
  setupUi( this );
}

int EwSqlPage::nextId() const
{
  return ExportWizard::ExportPage;
}


/**
 * Export page
 */
EwExportPage::EwExportPage(QueryToken *token, QWizard *parent)
  : QWizardPage(parent)
{
  setupUi(this);

  this->token = token;
}

void EwExportPage::checkProgress()
{
  if(!finished)
    dial->show();
}

/**
 * This is a common function to all formats. The conversion must obey to some
 * rules, see the documentation for more information.
 */
void EwExportPage::initializePage()
{
  finished = false;
  dial = new QProgressDialog(this);
  dial->setWindowTitle(tr("Export running..."));
  dial->setMaximum(token->model()->rowCount()-1);
  connect(this, SIGNAL(progress(int)), dial, SLOT(setValue(int)));
  QTimer::singleShot(1000, this, SLOT(checkProgress()));
  run();
}

void EwExportPage::run()
{
  QFile f(field("path").toString());
  if( !f.open( QFile::WriteOnly ) )
  {
    QMessageBox::critical( this,
                           tr( "Openning error" ),
                           tr( "Unable to open the file %1." )
                             .arg( field( "path" ).toString() ),
                           QMessageBox::Ok );
    return;
  }

  /*
   * CSV exporting
   */
  if(wizard()->hasVisitedPage(ExportWizard::CsvPage))
  {
    QString del = field( "csvdelimiter" ).toString(); // delimiter (default ;)
    QString sep = field( "csvseparator" ).toString(); // separator (default ")

    emit progress(-1);

    // columns name (= header)
    if( field( "csvheader" ).toBool() )
    {
      QStringList columns;
      for( int i=0; i<token->model()->columnCount(); i++ )
        columns << token->model()->headerData( i, Qt::Horizontal ).toString()
                    .prepend( del ).append( del );

      f.write(columns.join(sep).append(sep).toAscii());
      f.write("\n");
    }


    /*
     * Writing data
     */
    QModelIndex idx;
    for( int y=0; y<token->model()->rowCount(); y++ )       // each row
    {
      for( int x=0; x<token->model()->columnCount(); x++ )  // each piece of data
      {
        idx = token->model()->index( y, x );
        if( idx.data().canConvert( QVariant::String ) )
          f.write( idx.data().toString()
                   .prepend( del ).append( del + sep )
                   .toAscii() );
      }
      f.write( "\n" );

      emit progress(y);
    }
  }

  /*
   * HTML exporting
   */
  if(wizard()->hasVisitedPage(ExportWizard::HtmlPage))
  {
    emit progress(-1);
    f.write("<html>\n<head>\n<title></title>\n</head>\n<body>\n");

    /*
     * Writing query
     */
    if(field("htmlexportquery").toBool())
    {
      f.write("<p>");
      f.write(token->query().replace("\n", "<br />").toAscii());
      f.write("</p>\n");
    }

    /*
     * Writing header
     */
    f.write("<table border=\"1\">\n");
    f.write("<tr>");
    for(int i=0; i<token->model()->columnCount(); i++)
      f.write(token->model()->headerData(i, Qt::Horizontal).toString()
              .prepend("<th>").append("</th>").toAscii());

    f.write("</tr>\n");

    /*
     * Writing data
     */
    QModelIndex idx;
    for(int y=0; y<token->model()->rowCount(); y++)
    {
      f.write("<tr>");
      for(int x=0; x<token->model()->columnCount(); x++)
      {
        idx = token->model()->index(y, x);
        if(idx.data().canConvert(QVariant::String))
          f.write(idx.data().toString()
                  .prepend("<td>").append("</td>")
                  .toAscii());
      }
      f.write("</tr>\n");

      emit progress(y);
    }

    f.write("</table>\n");
    f.write("</body></html>");
  }

  f.close();
  finished = true;
  emit completeChanged();
}
