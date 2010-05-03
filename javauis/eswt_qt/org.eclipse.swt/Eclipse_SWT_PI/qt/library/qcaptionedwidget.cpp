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

#include <QLabel>
#include <QBoxLayout>
#include <QApplication>
#include <QStylePainter>
#include <QStyleOptionFocusRect>
#include "qcaptionedwidget.h"
#include "swtlog.h"


using namespace Java::eSWT;


QCaptionedWidget::QCaptionedWidget()
    : QFrame()
    , mFocus( false )
    {
    SWT_LOG_FUNC_CALL();
    connect( qApp, SIGNAL( focusChanged( QWidget*, QWidget* ) ),
        this, SLOT( focusChanged( QWidget*, QWidget* ) ) );
    }

void QCaptionedWidget::focusChanged( QWidget* aOldWidget, QWidget* aNewWidget )
    {
    SWT_LOG_FUNC_CALL();
    bool isOldWidgetChild = isAncestorOf( aOldWidget );
    bool isNewWidgetChild = isAncestorOf( aNewWidget );
    mFocus = isNewWidgetChild;
    if ( isOldWidgetChild != isNewWidgetChild )
        {
        update();
        }
    }

void QCaptionedWidget::paintEvent( QPaintEvent* aEvent )
    {
    SWT_LOG_FUNC_CALL();
    QFrame::paintEvent( aEvent );
    QStylePainter painter( this );
    if ( mFocus )
        {
        QStyleOptionButton optFocusHelper;
        optFocusHelper.initFrom( this );
        QStyleOptionFocusRect optFocus;
        optFocus.initFrom( this );
        optFocus.rect = painter.style()->subElementRect( QStyle::SE_PushButtonFocusRect, &optFocusHelper, this );
        painter.drawPrimitive( QStyle::PE_FrameFocusRect, optFocus );
        }
    }
