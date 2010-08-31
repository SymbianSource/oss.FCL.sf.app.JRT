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
#ifndef BUFFERIMPL_H
#define BUFFERIMPL_H

#include <QPicture>
#include "graphics.h"

namespace Java { namespace GFX {

class BufferImpl: public Buffer
{
public:
    /**
     * Ctor
     */
    BufferImpl();

    /**
     * Dtor
     */
    virtual ~BufferImpl();

    /**
     * From Buffer
     * @see Buffer
     */
    virtual void dispose();    
    virtual QPaintDevice* getBindable();
    virtual void getInvalidRect(int aRect[]);
    virtual QPicture* getPicture(); 

private:
    QPicture* mBuffer;        
};

} // namespace GFX
} // namespace Java

#endif //BUFFERIMPL_H
