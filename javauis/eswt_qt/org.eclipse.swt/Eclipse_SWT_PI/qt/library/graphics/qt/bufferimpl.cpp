/*******************************************************************************
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial API and implementation
 *******************************************************************************/

#include "bufferimpl.h"
#include "gfxlog.h"

namespace Java { namespace GFX {

BufferImpl::BufferImpl()
: mBuffer(NULL)
{
    GFX_LOG_FUNC_CALL();
    mBuffer = new QPicture();
}

BufferImpl::~BufferImpl()
{
    GFX_LOG_FUNC_CALL();
    delete mBuffer;
}

void BufferImpl::dispose()
{
    GFX_LOG_FUNC_CALL();
    delete this;
}

QPaintDevice* BufferImpl::getBindable()
{
    GFX_LOG_FUNC_CALL();
    return static_cast<QPaintDevice*>(mBuffer);
}

void BufferImpl::getInvalidRect(int aRect[])
{
    QRect rect = mBuffer->boundingRect();
    aRect[ERectX] = rect.x();
    aRect[ERectY] = rect.y();
    aRect[ERectWidth] = rect.width();
    aRect[ERectHeight] = rect.height();
}

QPicture* BufferImpl::getPicture() 
{
    return mBuffer;
} 

} // namespace GFX
} // namespace Java
