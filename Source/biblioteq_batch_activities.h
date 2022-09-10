#ifndef _BIBLIOTEQ_BATCH_ACTIVITIES_H_
#define _BIBLIOTEQ_BATCH_ACTIVITIES_H_

#include "ui_biblioteq_batch_activities_browser.h"

class biblioteq;

class biblioteq_batch_activities: public QMainWindow
{
  Q_OBJECT

 public:
  biblioteq_batch_activities(biblioteq *parent);
  void show(QMainWindow *parent);

 private:
  enum BorrowTableColumns
    {
      CATEGORY_COLUMN = 0,
      COPY_IDENTIFIER_COLUMN = 2,
      IDENTIFIER_COLUMN = 1,
      RESULTS_COLUMN = 3
    };

  enum Pages
    {
      Borrow = 0
    };

  Ui_batchActivitiesBrowser m_ui;
  biblioteq *m_qmain;
  void borrow(void);
  void changeEvent(QEvent *event);

 private slots:
  void slotAddBorrowingRow(void);
  void slotClose(void);
  void slotDeleteBorrowingRow(void);
  void slotGo(void);
  void slotReset(void);
  void slotSetGlobalFonts(const QFont &font);
};

#endif