/*
** -- Qt Includes --
*/

#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>

/*
** -- Local Includes --
*/

#include "bgraphicsscene.h"

/*
** -- bgraphicsscene() --
*/

bgraphicsscene::bgraphicsscene(QObject *parent):QGraphicsScene(parent)
{
}

/*
** -- ~bgraphicsscene() --
*/

bgraphicsscene::~bgraphicsscene()
{
}

/*
** -- mouseDoubleClickEvent() --
*/

void bgraphicsscene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsScene::mouseDoubleClickEvent(event);

#if QT_VERSION < 0x050000
  if(event && itemAt(event->scenePos()))
#else
  if(event && itemAt(event->scenePos(), QTransform()))
#endif
    emit itemDoubleClicked();
}

/*
** -- keyPressEvent() --
*/

void bgraphicsscene::keyPressEvent(QKeyEvent *event)
{
  QGraphicsScene::keyPressEvent(event);

  if(event)
    if(event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
      emit deleteKeyPressed();
}
