/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

#ifndef SWTBUFFERFLUSHEVENT_H_
#define SWTBUFFERFLUSHEVENT_H_

#include <QEvent>
#include <org_eclipse_swt_internal_qt_OS.h>

namespace Java { namespace eSWT {

class SwtBufferFlushEvent: public QEvent
{
public:
    SwtBufferFlushEvent() : QEvent((Type)org_eclipse_swt_internal_qt_OS_QSWTEVENT_BUFFERFLUSH) {}
    virtual ~SwtBufferFlushEvent() {}
};

}
}

#endif // SWTBUFFERFLUSHEVENT_H_
