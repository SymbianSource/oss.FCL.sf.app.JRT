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

#include "qtasktipbar.h"


using namespace Java::eSWT;


QTaskTipBar::QTaskTipBar(QWidget* parent) 
    : QProgressBar(parent) 
    {
    }

QSize QTaskTipBar::sizeHint() const
    {
    // To make the progressbar not appear on a SWT.NONE style 
    // TaskTip, this bar returns (0,0) as it's preferred size.
    return QSize(0,0);
    }



