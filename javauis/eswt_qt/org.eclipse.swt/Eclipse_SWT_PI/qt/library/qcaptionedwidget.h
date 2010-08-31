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

#ifndef QCAPTIONEDWIDGET_H
#define QCAPTIONEDWIDGET_H


#include <QFrame>


class QLabel;


namespace Java { namespace eSWT {


class QCaptionedWidget
    : public QFrame
    {
    Q_OBJECT

public:
    QCaptionedWidget();

public slots:
    void focusChanged( QWidget* aOldWidget, QWidget* aNewWidget );

// From QWidget
protected:
    void paintEvent( QPaintEvent *aEvent );

private:
    bool mFocus;
};

} // namespace eSWT

} // namespace Java

#endif // QCAPTIONEDWIDGET_H
