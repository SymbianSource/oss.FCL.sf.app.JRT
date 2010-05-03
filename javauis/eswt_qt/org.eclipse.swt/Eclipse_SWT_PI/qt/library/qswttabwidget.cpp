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
#include <QStyleOptionTabWidgetFrame>
#include "qswttabwidget.h"

using namespace Java::eSWT;

//
// QSwtTabWidget
//
QSwtTabWidget::QSwtTabWidget(QWidget* parent) 
    : QTabWidget(parent) 
    {
    }

QSize QSwtTabWidget::stackedWidgetSize() const
    {
    QStyleOptionTabWidgetFrame option;
    initStyleOption(&option);
    QRect contentsRect = style()->subElementRect(QStyle::SE_TabWidgetTabContents, &option, this);
    return contentsRect.size();
    }


