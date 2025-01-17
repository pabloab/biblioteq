/*
** Copyright (c) 2006 - present, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from BiblioteQ without specific prior written permission.
**
** BIBLIOTEQ IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** BIBLIOTEQ, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "biblioteq.h"
#include "biblioteq_otheroptions.h"

biblioteq_otheroptions::biblioteq_otheroptions(biblioteq *parent):QMainWindow()
{
  m_ui.setupUi(this);
  biblioteq_misc_functions::sortCombinationBox(m_ui.books_accession_number);
  qmain = parent;
  connect(m_ui.close,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotClose(void)));
  connect(m_ui.custom_query_reset,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotResetCustomQueryColors(void)));
  connect(m_ui.item_mandatory_field_color,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectColor(void)));
  connect(m_ui.item_query_result_color,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectColor(void)));
  connect(m_ui.main_window_canvas_background_color,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectMainwindowCanvasBackgroundColor(void)));
  connect(m_ui.save,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSave(void)));

  if(qmain)
    connect(qmain,
	    SIGNAL(fontChanged(const QFont &)),
	    this,
	    SLOT(setGlobalFonts(const QFont &)));

  m_keywordsItemDelegate = new biblioteq_otheroptions_item_delegate
    (biblioteq_otheroptions_item_delegate::ParentTypes::Keywords, this);
  m_shortcutsItemDelegate = new biblioteq_otheroptions_item_delegate
    (biblioteq_otheroptions_item_delegate::ParentTypes::Shortcuts, this);
  connect(m_shortcutsItemDelegate,
	  SIGNAL(changed(void)),
	  this,
	  SLOT(slotMainWindowShortcutChanged(void)));
  m_ui.custom_query->setItemDelegateForColumn(1, m_keywordsItemDelegate);
  m_ui.publication_date->verticalHeader()->setSectionResizeMode
    (QHeaderView::Fixed);
  m_ui.shortcuts->setItemDelegateForColumn(1, m_shortcutsItemDelegate);
  prepareSQLKeywords();
  prepareSettings();
  prepareShortcuts();
}

biblioteq_otheroptions::~biblioteq_otheroptions()
{
}

QColor biblioteq_otheroptions::availabilityColor(const QString &it) const
{
  QSettings settings;
  QString value("white");
  auto itemType(QString(it).remove(" ").toLower().trimmed());

  if(itemType == "book" || itemType == "books")
    value = settings.value
      ("otheroptions/book_availability_color").toString();
  else if(itemType == "cd" || itemType == "musiccds")
    value = settings.value
      ("otheroptions/cd_availability_color").toString();
  else if(itemType == "dvd" || itemType == "dvds")
    value = settings.value
      ("otheroptions/dvd_availability_color").toString();
  else if(itemType == "greyliterature")
    value = settings.value
      ("otheroptions/grey_literature_availability_color").toString();
  else if(itemType == "journal" || itemType == "journals")
    value = settings.value
      ("otheroptions/journal_availability_color").toString();
  else if(itemType == "magazine" || itemType == "magazines")
    value = settings.value
      ("otheroptions/magazine_availability_color").toString();
  else if(itemType == "videogame" || itemType == "videogames")
    value = settings.value
      ("otheroptions/videogame_availability_color").toString();

  auto color(QColor(value.remove('&')));

  if(!color.isValid())
    color = QColor(Qt::green);

  return color;
}

QColor biblioteq_otheroptions::itemMandatoryFieldColor(void) const
{
  QSettings settings;

  auto color
    (QColor(settings.value("otheroptions/item_mandatory_field_color").
	    toString().remove('&').trimmed()));

  if(!color.isValid())
    color = QColor(255, 248, 220);

  return color;
}

QColor biblioteq_otheroptions::itemQueryResultColor(void) const
{
  QSettings settings;

  auto color(QColor(settings.value("otheroptions/item_query_result_color").
		    toString().remove('&').trimmed()));

  if(!color.isValid())
    color = QColor(162, 205, 90);

  return color;
}

QMap<QString, QColor> biblioteq_otheroptions::customQueryColors(void) const
{
  QMap<QString, QColor> map;

  for(int i = 0; i < m_ui.custom_query->rowCount(); i++)
    {
      auto item1 = m_ui.custom_query->item(i, 0);
      auto item2 = m_ui.custom_query->item(i, 1);

      if(item1 && item2)
	map[item1->text()] = QColor(item2->text());
    }

  return map;
}

QString biblioteq_otheroptions::publicationDateFormat(const QString &it) const
{
  QSettings settings;
  QString format("");
  auto itemType(it.toLower().trimmed());

  if(itemType == "books")
    format = settings.value
      ("otheroptions/book_publication_date_format").toString();
  else if(itemType == "dvds")
    format = settings.value
      ("otheroptions/dvd_publication_date_format").toString();
  else if(itemType == "greyliterature")
    format = settings.value
      ("otheroptions/grey_literature_date_format").toString();
  else if(itemType == "journals")
    format = settings.value
      ("otheroptions/journal_publication_date_format").toString();
  else if(itemType == "magazines")
    format = settings.value
      ("otheroptions/magazine_publication_date_format").toString();
  else if(itemType == "musiccds")
    format = settings.value
      ("otheroptions/cd_publication_date_format").toString();
  else if(itemType == "photographcollections")
    format = settings.value
      ("otheroptions/photograph_publication_date_format").toString();
  else if(itemType == "videogames")
    format = settings.value
      ("otheroptions/videogame_publication_date_format").toString();

  format = format.trimmed();

  if(format.isEmpty())
    return biblioteq::s_databaseDateFormat;
  else
    return format;
}

bool biblioteq_otheroptions::isMembersColumnVisible(const QString &text) const
{
  for(int i = 0; i < m_ui.members_visible_columns->count(); i++)
    {
      auto item = m_ui.members_visible_columns->item(i);

      if(item && item->text() == text)
	return item->checkState() == Qt::Checked;
    }

  return false;
}

bool biblioteq_otheroptions::showBookReadStatus(void) const
{
  QSettings settings;

  return settings.value("otheroptions/book_read_status", false).toBool();
}

bool biblioteq_otheroptions::showMainTableImages(void) const
{
  QSettings settings;

  return settings.value("show_maintable_images", true).toBool();
}

bool biblioteq_otheroptions::showMainTableProgressDialogs(void) const
{
  QSettings settings;

  return settings.value("show_maintable_progress_dialogs", true).toBool();
}

int biblioteq_otheroptions::booksAccessionNumberIndex(void) const
{
  QSettings settings;

  return qBound
    (0,
     settings.value("otheroptions/books_accession_number_index").toInt(),
     m_ui.books_accession_number->count() - 1);
}

int biblioteq_otheroptions::iconsViewColumnCount(void) const
{
  QSettings settings;

  return qBound
    (m_ui.icons_view_column_count->minimum(),
     settings.value("otheroptions/icons_view_column_count").toInt(),
     m_ui.icons_view_column_count->maximum());
}

void biblioteq_otheroptions::changeEvent(QEvent *event)
{
  if(event)
    switch(event->type())
      {
      case QEvent::LanguageChange:
	{
	  m_ui.retranslateUi(this);
	  biblioteq_misc_functions::sortCombinationBox
	    (m_ui.books_accession_number);
	  break;
	}
      default:
	break;
      }

  QMainWindow::changeEvent(event);
}

void biblioteq_otheroptions::keyPressEvent(QKeyEvent *event)
{
  if(event && event->key() == Qt::Key_Escape)
#ifdef Q_OS_ANDROID
    hide();
#else
    close();
#endif

  QMainWindow::keyPressEvent(event);
}

void biblioteq_otheroptions::prepareAvailability(void)
{
  QSettings settings;
  QStringList list1;
  QStringList list2;
  QStringList list3;

  list1 << tr("Books")
	<< tr("DVDs")
	<< tr("Grey Literature")
	<< tr("Journals")
	<< tr("Magazines")
	<< tr("Music CDs")
	<< tr("Video Games");
  list2 << settings.value("otheroptions/book_availability_color").toString()
	<< settings.value("otheroptions/dvd_availability_color").toString()
	<< settings.value("otheroptions/grey_literature_availability_color").
           toString()
	<< settings.value("otheroptions/journal_availability_color").toString()
	<< settings.value("otheroptions/magazine_availability_color").toString()
	<< settings.value("otheroptions/cd_availability_color").toString()
	<< settings.value("otheroptions/videogame_availability_color").
           toString();
  list3 << "books"
	<< "dvds"
	<< "greyliterature"
	<< "journals"
	<< "magazines"
	<< "musiccds"
	<< "videogames";
  m_ui.availability_color->setRowCount(list1.size());
  m_ui.availability_color->setSortingEnabled(false);
  m_ui.availability_colors->setChecked
    (settings.value("otheroptions/availability_colors", false).toBool());

  for(int i = 0; i < list1.size(); i++)
    {
      auto item = new QTableWidgetItem(list1.at(i));
      auto layout = new QHBoxLayout();
      auto pushButton = new QPushButton();
      auto spacer1 = new QSpacerItem
	(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
      auto spacer2 = new QSpacerItem
	(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
      auto widget = new QWidget();

      connect(pushButton,
	      SIGNAL(clicked(void)),
	      this,
	      SLOT(slotSelectAvailabilityColor(void)));
      widget->setLayout(layout);
      layout->addSpacerItem(spacer1);
      layout->addWidget(pushButton);
      layout->addSpacerItem(spacer2);
      layout->setContentsMargins(0, 0, 0, 0);
      item->setData(Qt::UserRole, list3.at(i));
      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.availability_color->setCellWidget(i, AVAILABILITY_COLOR, widget);
      m_ui.availability_color->setItem(i, ITEM_TYPE, item);
      pushButton->setText(list2.at(i));
      pushButton->setStyleSheet
	(QString("background-color: %1").arg(QString(list2.at(i)).remove('&')));
    }

  m_ui.availability_color->resizeColumnToContents(ITEM_TYPE);
  m_ui.availability_color->resizeRowsToContents();
  m_ui.availability_color->setSortingEnabled(true);
  m_ui.availability_color->sortByColumn(0, Qt::AscendingOrder);
}

void biblioteq_otheroptions::prepareMembersVisibleColumns(QTableWidget *table)
{
  if(!table)
    return;

  m_ui.members_visible_columns->clear();

  QMap<QString, bool> map;
  QSettings settings;

  foreach(const auto &string,
	  settings.value("otheroptions/members_visible_columns", "").
	  toString().split(','))
    {
      auto list(string.split('='));

      if(list.size() == 2)
	map[list.at(0).mid(0, 128).trimmed()] =
	  QVariant(list.at(1).mid(0, 5).trimmed()).toBool();
    }

  for(int i = 0; i < table->columnCount(); i++)
    if(table->horizontalHeaderItem(i))
      {
	auto item = new QListWidgetItem(table->horizontalHeaderItem(i)->text());

	if(map.contains(item->text()))
	  item->setCheckState
	    (map.value(item->text()) ? Qt::Checked : Qt::Unchecked);
	else
	  item->setCheckState(Qt::Checked);

	item->setFlags
	  (Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
	m_ui.members_visible_columns->addItem(item);
      }
}

void biblioteq_otheroptions::prepareSQLKeywords(void)
{
  QStringList list;

  list << "ALL"
       << "AND"
       << "ANY"
       << "AS"
       << "ASC"
       << "BETWEEN"
       << "CASE"
       << "DEFAULT"
       << "DELETE"
       << "DESC"
       << "DISTINCT"
       << "EXISTS"
       << "FROM"
       << "FULL OUTER JOIN"
       << "GROUP BY"
       << "HAVING"
       << "IN"
       << "INNER JOIN"
       << "IS NULL"
       << "IS NOT NULL"
       << "JOIN"
       << "LEFT JOIN"
       << "LIKE"
       << "LIMIT"
       << "MATCH"
       << "NOT"
       << "NOT NULL"
       << "OFFSET"
       << "OR"
       << "ORDER BY"
       << "OUTER JOIN"
       << "RIGHT JOIN"
       << "ROWNUM"
       << "SELECT"
       << "SELECT DISTINCT"
       << "SELECT TOP"
       << "SET"
       << "TOP"
       << "UNION"
       << "UNION ALL"
       << "UNIQUE"
       << "UPDATE"
       << "VALUES"
       << "WHERE";
  m_ui.custom_query->setRowCount(list.size());
  m_ui.custom_query->setSortingEnabled(false);

  for(int i = 0; i < list.size(); i++)
    {
      auto item = new QTableWidgetItem(list.at(i));

      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.custom_query->setItem(i, 0, item);
      item = new QTableWidgetItem(QColor(Qt::black).name());
      item->setData(Qt::DecorationRole, QColor(Qt::black));
      item->setFlags
	(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.custom_query->setItem(i, 1, item);
    }

  m_ui.custom_query->setSortingEnabled(true);
  m_ui.custom_query->sortByColumn(0, Qt::AscendingOrder);
}

void biblioteq_otheroptions::prepareSettings(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  prepareAvailability();
  prepareShortcuts();

  QSettings settings;
  QStringList list1;
  QStringList list2;
  QStringList list3;

  {
    QMap<QString, QColor> map;

    foreach(const auto &string,
	    settings.value("otheroptions/custom_query_colors", "").
	    toString().split(','))
      {
	auto list(string.split('='));

	if(list.size() == 2)
	  map[list.at(0).mid(0, 64).toUpper().trimmed()] =
	    QColor(list.at(1).mid(0, 64).trimmed());
      }

    for(int i = 0; i < m_ui.custom_query->rowCount(); i++)
      {
	auto item1 = m_ui.custom_query->item(i, 0);
	auto item2 = m_ui.custom_query->item(i, 1);

	if(item1 && item2)
	  {
	    item2->setData(Qt::DecorationRole, map.value(item1->text()));
	    item2->setText(map.value(item1->text()).name());
	  }
      }
  }

  {
    QMap<QString, bool> map;

    foreach(const auto &string,
	    settings.value("otheroptions/members_visible_columns", "").
	    toString().split(','))
      {
	auto list(string.split('='));

	if(list.size() == 2)
	  map[list.at(0).mid(0, 128).trimmed()] =
	    QVariant(list.at(1).mid(0, 5).trimmed()).toBool();
      }

    for(int i = 0; i < m_ui.members_visible_columns->count(); i++)
      {
	auto item = m_ui.members_visible_columns->item(i);

	if(item)
	  {
	    if(map.contains(item->text()))
	      item->setCheckState
		(map.value(item->text()) ? Qt::Checked : Qt::Unchecked);
	    else
	      item->setCheckState(Qt::Checked);
	  }
      }
  }

  list1 << tr("Books")
	<< tr("DVDs")
	<< tr("Grey Literature")
	<< tr("Journals")
	<< tr("Magazines")
	<< tr("Music CDs")
	<< tr("Photograph Collections")
	<< tr("Video Games");
  list2 << settings.value("otheroptions/book_publication_date_format").
           toString()
	<< settings.value("otheroptions/dvd_publication_date_format").toString()
	<< settings.value("otheroptions/grey_literature_date_format").toString()
	<< settings.value("otheroptions/journal_publication_date_format").
           toString()
	<< settings.value("otheroptions/magazine_publication_date_format").
           toString()
    	<< settings.value("otheroptions/cd_publication_date_format").toString()
	<< settings.value("otheroptions/photograph_publication_date_format").
           toString()
	<< settings.value("otheroptions/videogame_publication_date_format").
           toString();
  list3 << "books"
	<< "dvds"
	<< "greyliterature"
	<< "journals"
	<< "magazines"
	<< "musiccds"
	<< "photographcollections"
	<< "videogames";
  m_ui.book_read_status->setChecked
    (settings.value("otheroptions/book_read_status", false).toBool());
  m_ui.books_accession_number->setCurrentIndex
    (qBound(0,
	    settings.value("otheroptions/books_accession_number_index").toInt(),
	    m_ui.books_accession_number->count() - 1));
  m_ui.generated_letter->setPlainText
    (QString::
     fromUtf8(QByteArray::
	      fromBase64(settings.value("otheroptions/generated_letter").
			 toByteArray()).constData()));
  m_ui.icons_view_column_count->setValue
    (settings.value("otheroptions/icons_view_column_count").toInt());
  m_ui.isbn10_display_format->setCurrentIndex
    (qBound(0,
	    settings.value("otheroptions/isbn10_display_format_index").toInt(),
	    m_ui.isbn10_display_format->count() - 1));
  m_ui.isbn13_display_format->setCurrentIndex
    (qBound(0,
	    settings.value("otheroptions/isbn13_display_format_index").toInt(),
	    m_ui.isbn13_display_format->count() - 1));

  QColor color
    (settings.value("otheroptions/item_mandatory_field_color").
     toString().remove('&').trimmed());

  if(!color.isValid())
    color = QColor(255, 248, 220);

  m_ui.item_mandatory_field_color->setStyleSheet
    (QString("background-color: %1").arg(color.name()));
  m_ui.item_mandatory_field_color->setText(color.name());
  color = QColor
    (settings.value("otheroptions/item_query_result_color").
     toString().remove('&').trimmed());

  if(!color.isValid())
    color = QColor(162, 205, 90);

  m_ui.item_query_result_color->setStyleSheet
    (QString("background-color: %1").arg(color.name()));
  m_ui.item_query_result_color->setText(color.name());
  m_ui.publication_date->setRowCount(list1.size());
  m_ui.publication_date->setSortingEnabled(false);

  for(int i = 0; i < list1.size(); i++)
    {
      auto str(list2.at(i).trimmed());

      if(str.isEmpty())
	str = biblioteq::s_databaseDateFormat;

      auto item = new QTableWidgetItem(list1.at(i));

      item->setData(Qt::UserRole, list3.at(i));
      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.publication_date->setItem(i, ITEM_TYPE, item);
      item = new QTableWidgetItem(str);
      item->setData(Qt::UserRole, list3.at(i));
      item->setFlags
	(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.publication_date->setItem(i, PUBLICATION_DATE_FORMAT, item);
    }

  m_ui.publication_date->resizeColumnToContents(ITEM_TYPE);
  m_ui.publication_date->resizeRowsToContents();
  m_ui.publication_date->setSortingEnabled(true);
  m_ui.publication_date->sortByColumn(0, Qt::AscendingOrder);
  color = QColor
    (settings.value("mainwindow_canvas_background_color").
     toString().remove('&').trimmed());

  if(!color.isValid())
    color = Qt::white;

  m_ui.main_window_canvas_background_color->setStyleSheet
    (QString("background-color: %1").arg(color.name()));
  m_ui.main_window_canvas_background_color->setText(color.name());
  m_ui.show_maintable_images->setChecked
    (settings.value("show_maintable_images", true).toBool());
  m_ui.show_maintable_progress_dialogs->setChecked
    (settings.value("show_maintable_progress_dialogs", true).toBool());
  m_ui.show_maintable_tooltips->setChecked
    (settings.value("show_maintable_tooltips", false).toBool());
  QApplication::restoreOverrideCursor();
}

void biblioteq_otheroptions::prepareShortcuts(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_ui.shortcuts->setRowCount(0);

  if(!qmain)
    return;

  m_ui.shortcuts->setRowCount(qmain->shortcuts().size());
  m_ui.shortcuts->setSortingEnabled(false);

  QSettings settings;
  QString shortcut
    (settings.value("custom_query_favorite_shortcut").toString().trimmed());
  auto map(qmain->shortcuts());

  map[tr("Custom Query Favorite")] = shortcut;

  QMapIterator<QString, QKeySequence> it(map);
  int i = -1;

  while(it.hasNext())
    {
      i += 1;
      it.next();

      auto item = new QTableWidgetItem(it.key());

      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.shortcuts->setItem(i, 0, item);
      item = new QTableWidgetItem(it.value().toString());

      if(it.key() != tr("Custom Query Favorite"))
	item->setFlags(Qt::ItemIsSelectable);
      else
	item->setFlags
	  (Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

      m_ui.shortcuts->setItem(i, 1, item);
    }

  m_ui.shortcuts->resizeColumnsToContents();
  m_ui.shortcuts->setSortingEnabled(true);
  m_ui.shortcuts->sortByColumn(0, Qt::AscendingOrder);
  slotMainWindowShortcutChanged();
  QApplication::restoreOverrideCursor();
}

void biblioteq_otheroptions::setGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(auto widget, findChildren<QWidget *> ())
    {
      auto f(font);

      f.setBold(widget->font().bold());
      widget->setFont(f);
      widget->update();
    }

  m_ui.publication_date->resizeRowsToContents();
  update();
}

void biblioteq_otheroptions::showNormal(void)
{
  prepareSettings();
  QMainWindow::showNormal();
}

void biblioteq_otheroptions::slotClose(void)
{
#ifdef Q_OS_ANDROID
  hide();
#else
  close();
#endif
}

void biblioteq_otheroptions::slotMainWindowShortcutChanged(void)
{
  if(!m_ui.shortcuts->item(0, 0))
    return;

  QMap<QString, int> map;
  static auto color(m_ui.shortcuts->item(0, 0)->background());

  for(int i = 0; i < m_ui.shortcuts->rowCount(); i++)
    {
      auto item = m_ui.shortcuts->item(i, 1);

      if(!item)
	continue;

      if(map.contains(item->text()))
	{
	  item->setBackground(QColor(255, 114, 118));
	  m_ui.shortcuts->item
	    (map.value(item->text()), 1)->setBackground(item->background());
	}
      else
	item->setBackground(color);

      map[item->text()] = i;
    }
}

void biblioteq_otheroptions::slotPreviewCanvasBackgroundColor
(const QColor &color)
{
  m_ui.main_window_canvas_background_color->setStyleSheet
    (QString("background-color: %1").arg(color.name()));
  m_ui.main_window_canvas_background_color->setText(color.name());
}

void biblioteq_otheroptions::slotResetCustomQueryColors(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_ui.custom_query->setSortingEnabled(false);

  for(int i = 0; i < m_ui.custom_query->rowCount(); i++)
    {
      auto item = m_ui.custom_query->item(i, 1);

      if(item)
	{
	  item->setData(Qt::DecorationRole, QColor(Qt::black));
	  item->setText(QColor(Qt::black).name());
	}
    }

  m_ui.custom_query->setSortingEnabled(true);
  QApplication::restoreOverrideCursor();
}

void biblioteq_otheroptions::slotSave(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  QSettings settings;
  QString string("");

  settings.setValue
    ("otheroptions/availability_colors", m_ui.availability_colors->isChecked());

  for(int i = 0; i < m_ui.custom_query->rowCount(); i++)
    {
      auto item1 = m_ui.custom_query->item(i, 0);
      auto item2 = m_ui.custom_query->item(i, 1);

      if(item1 && item2)
	{
	  string.append(item1->text());
	  string.append("=");
	  string.append(item2->text().remove('&'));
	  string.append(",");
	}
    }

  if(string.isEmpty())
    settings.setValue("otheroptions/custom_query_colors", "");
  else
    settings.setValue
      ("otheroptions/custom_query_colors", string.mid(0, string.length() - 1));

  string.clear();

  for(int i = 0; i < m_ui.members_visible_columns->count(); i++)
    {
      auto item = m_ui.members_visible_columns->item(i);

      if(item)
	{
	  string.append(item->text());
	  string.append("=");
	  string.append(item->checkState() == Qt::Checked ? "true" : "false");
	  string.append(",");
	}
    }

  if(string.isEmpty())
    settings.setValue("otheroptions/members_visible_columns", "");
  else
    settings.setValue
      ("otheroptions/members_visible_columns",
       string.mid(0, string.length() - 1));

  QStringList list;

  list << "otheroptions/book_availability_color"
       << "otheroptions/dvd_availability_color"
       << "otheroptions/grey_literature_availability_color"
       << "otheroptions/journal_availability_color"
       << "otheroptions/magazine_availability_color"
       << "otheroptions/cd_availability_color"
       << "otheroptions/videogame_availability_color";

  for(int i = 0; i < list.size(); i++)
    {
      QString value("");
      auto widget = m_ui.availability_color->cellWidget(i, AVAILABILITY_COLOR);
      const auto &key(list.at(i));

      if(widget)
	{
	  auto pushButton = widget->findChild<QPushButton *> ();

	  if(pushButton)
	    value = pushButton->text().trimmed().toLatin1();
	}

      settings.setValue(key, value);
    }

  list.clear();
  list << "otheroptions/book_publication_date_format"
       << "otheroptions/dvd_publication_date_format"
       << "otheroptions/grey_literature_date_format"
       << "otheroptions/journal_publication_date_format"
       << "otheroptions/magazine_publication_date_format"
       << "otheroptions/cd_publication_date_format"
       << "otheroptions/photograph_publication_date_format"
       << "otheroptions/videogame_publication_date_format";

  for(int i = 0; i < list.size(); i++)
    {
      auto item = m_ui.publication_date->item(i, PUBLICATION_DATE_FORMAT);
      auto value(biblioteq::s_databaseDateFormat);
      const auto &key(list.at(i));

      if(item)
	value = item->text().trimmed();

      settings.setValue(key, value);
    }

  for(int i = 0; i < m_ui.shortcuts->rowCount(); i++)
    if(m_ui.shortcuts->item(i, 0) &&
       m_ui.shortcuts->item(i, 0)->text() == tr("Custom Query Favorite") &&
       m_ui.shortcuts->item(i, 1))
      {
	settings.setValue
	  ("custom_query_favorite_shortcut",
	   m_ui.shortcuts->item(i, 1)->text().trimmed());
	break;
      }

  settings.setValue
    ("mainwindow_canvas_background_color",
     m_ui.main_window_canvas_background_color->text().remove('&').toLatin1());
  settings.setValue
    ("otheroptions/book_read_status", m_ui.book_read_status->isChecked());
  settings.setValue
    ("otheroptions/books_accession_number_index",
     m_ui.books_accession_number->currentIndex());
  settings.setValue
    ("otheroptions/generated_letter",
     m_ui.generated_letter->toPlainText().toUtf8().toBase64());
  settings.setValue
    ("otheroptions/icons_view_column_count",
     m_ui.icons_view_column_count->value());
  settings.setValue
    ("otheroptions/isbn10_display_format_index",
     m_ui.isbn10_display_format->currentIndex());
  settings.setValue
    ("otheroptions/isbn13_display_format_index",
     m_ui.isbn13_display_format->currentIndex());
  settings.setValue
    ("otheroptions/item_mandatory_field_color",
     m_ui.item_mandatory_field_color->text().remove('&').toLatin1());
  settings.setValue
    ("otheroptions/item_query_result_color",
     m_ui.item_query_result_color->text().remove('&').toLatin1());
  settings.setValue
    ("show_maintable_images", m_ui.show_maintable_images->isChecked());
  settings.setValue
    ("show_maintable_progress_dialogs",
     m_ui.show_maintable_progress_dialogs->isChecked());
  settings.setValue
    ("show_maintable_tooltips", m_ui.show_maintable_tooltips->isChecked());
  settings.sync();
  m_isbn10Format = m_ui.isbn10_display_format->currentText();
  m_isbn13Format = m_ui.isbn13_display_format->currentText();
  emit mainWindowCanvasBackgroundColorChanged
    (QColor(m_ui.main_window_canvas_background_color->text().remove('&')));
  emit saved();
  QApplication::restoreOverrideCursor();
}

void biblioteq_otheroptions::slotSelectAvailabilityColor(void)
{
  auto pushButton = qobject_cast<QPushButton *> (sender());

  if(!pushButton)
    return;

  QColorDialog dialog(this);

  dialog.setCurrentColor(QColor(pushButton->text().remove('&')));

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      pushButton->setStyleSheet
	(QString("background-color: %1").arg(dialog.selectedColor().name()));
      pushButton->setText(dialog.selectedColor().name());
    }
  else
    QApplication::processEvents();
}

void biblioteq_otheroptions::slotSelectColor(void)
{
  auto pushButton = qobject_cast<QPushButton *> (sender());

  if(!pushButton)
    return;

  QColor color(pushButton->text().remove('&').trimmed());
  QColorDialog dialog(this);

  dialog.setCurrentColor(color);

  if(dialog.exec() == QDialog::Accepted)
    {
      pushButton->setStyleSheet
	(QString("background-color: %1").arg(dialog.selectedColor().name()));
      pushButton->setText(dialog.selectedColor().name());
      QApplication::processEvents();
    }
  else
    QApplication::processEvents();
}

void biblioteq_otheroptions::slotSelectMainwindowCanvasBackgroundColor(void)
{
  QColor color(m_ui.main_window_canvas_background_color->text().remove('&'));
  QColorDialog dialog(this);

  dialog.setCurrentColor(color);
  connect(&dialog,
	  SIGNAL(currentColorChanged(const QColor &)),
	  this,
	  SIGNAL(mainWindowCanvasBackgroundColorPreview(const QColor &)));
  connect(&dialog,
	  SIGNAL(currentColorChanged(const QColor &)),
	  this,
	  SLOT(slotPreviewCanvasBackgroundColor(const QColor &)));

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      m_ui.main_window_canvas_background_color->setStyleSheet
	(QString("background-color: %1").arg(dialog.selectedColor().name()));
      m_ui.main_window_canvas_background_color->setText
	(dialog.selectedColor().name());
    }
  else
    {
      QApplication::processEvents();
      emit mainWindowCanvasBackgroundColorChanged(QColor());
      m_ui.main_window_canvas_background_color->setStyleSheet
	(QString("background-color: %1").arg(color.name()));
      m_ui.main_window_canvas_background_color->setText(color.name());
    }
}
