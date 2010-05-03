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

#ifndef QTASKTIPBAR_H
#define QTASKTIPBAR_H


#include <QSize>
#include <QProgressBar>

namespace Java { namespace eSWT {


class QTaskTipBar 
    : public QProgressBar
    {
    Q_OBJECT    

public:
    QTaskTipBar(QWidget* parent = 0);

    virtual QSize sizeHint() const;
    };

} // namespace eSWT

} // namespace Java

#endif // QTASKTIPBAR_H

