/**************************************************************************\
 *
 *  This file is part of the SIM Quarter extension library for Coin.
 *  Copyright (C) 1998-2005 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License ("GPL") version 2
 *  as published by the Free Software Foundation.  See the file COPYING
 *  at the root directory of this source distribution for additional
 *  information about the GNU GPL.
 *
 *  For using SIM Quarter with software that can not be combined with
 *  the GNU GPL, and for taking advantage of the additional benefits of
 *  our support services, please contact Systems in Motion about acquiring
 *  a Coin Professional Edition License.
 *
 *  See <URL:http://www.coin3d.org/> for more information.
 *
 *  Systems in Motion AS, Bygd�y all� 5, N-0257 Oslo, NORWAY. (www.sim.no)
 *
\**************************************************************************/

#include <Quarter/devices/DragDropHandler.h>

#include <QUrl>
#include <QFileInfo>
#include <QDragEnterEvent>
#include <QDropEvent>

#include <Inventor/SoInput.h>
#include <Inventor/nodes/SoSeparator.h>

#include <Quarter/CoinWidget.h>
#include <Quarter/devices/DeviceManager.h>
#include <stdlib.h>

DragDropHandler::DragDropHandler(void)
{
  this->suffixes << "iv" << "wrl";
}

DragDropHandler::~DragDropHandler()
{
  
}

const SoEvent * 
DragDropHandler::translateEvent(QEvent * event)
{
  switch (event->type()) {
  case QEvent::DragEnter:
    this->dragEnterEvent((QDragEnterEvent *) event);
    break;
  case QEvent::Drop:
    this->dropEvent((QDropEvent *) event);
    break;
  default:
    break;
  }
  return NULL;
}
  
void 
DragDropHandler::dragEnterEvent(QDragEnterEvent * event)
{
  const QMimeData * mimedata = event->mimeData();
  if (!mimedata->hasUrls() & !mimedata->hasText()) return;

  if (mimedata->hasUrls()) { 
    QFileInfo fileinfo(mimedata->urls().takeFirst().path());
    QString suffix = fileinfo.suffix().toLower();
    if (!this->suffixes.contains(suffix)) { return; }
  }
  
  event->acceptProposedAction();
}

void 
DragDropHandler::dropEvent(QDropEvent * event)
{
  const QMimeData * mimedata = event->mimeData();
  
  SoSeparator * root;
  SoInput in;
  
  if (mimedata->hasUrls()) { 
    QString path = mimedata->urls().takeFirst().path();
    // attempt to open file
    if (!in.openFile(path.toLatin1().constData())) return;

  } else if (mimedata->hasText()) {
    QByteArray bytes = mimedata->text().toUtf8();
    in.setBuffer((void *) bytes.constData(), bytes.size());
    if (!in.isValidBuffer()) return;
  }

  // attempt to import it
  root = SoDB::readAll(&in);
  if (root == NULL) return;
  
  // get CoinWidget and set new scenegraph
  CoinWidget * coinwidget = (CoinWidget *) this->manager->getWidget();
  coinwidget->setSceneGraph(root);
  coinwidget->updateGL();
}
