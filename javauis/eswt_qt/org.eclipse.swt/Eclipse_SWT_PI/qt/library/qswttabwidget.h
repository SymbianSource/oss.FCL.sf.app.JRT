/*******************************************************************************
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

#ifndef QSWTTABWIDGET_H
#define QSWTTABWIDGET_H

#include <QTabWidget>

namespace Java { namespace eSWT {


/**
 * QSwtTabWidget is needed for getting the correct size
 * for QStackedWidget when widget is invisible.
 */
class QSwtTabWidget : public QTabWidget
    {
    Q_OBJECT

public:
    QSwtTabWidget(QWidget* parent = 0);
    
   /**
    * Returns the size for QStackedWidget according to the
    * current size of the QTabWidget.
    */
    QSize stackedWidgetSize() const;
    };

} // namespace eSWT

} // namespace Java

#endif // QSWTTABWIDGET_H

