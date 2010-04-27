/*******************************************************************************
 * Copyright (c) 2008, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/

#ifndef SWTJAVABUFFERDRAWER_H
#define SWTJAVABUFFERDRAWER_H

#include <e32base.h>
#include <w32std.h>

class MSwtDisplay;
class MSwtGc;

/**
 * CSwtJavaBufferDrawer
 *
 * This class implements a command buffer flushing facility for a graphics context
 * implementing the MSwtGc inteface.
 *
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtJavaBufferDrawer) : public CBase
{
public:
    /**
     * Constructor
     */
    CSwtJavaBufferDrawer(MSwtDisplay& aDisplay);

    /**
     * Destructor
     */
    virtual ~CSwtJavaBufferDrawer();

    /**
     * Flushes the given drawing operations to the given graphics context
     * @param aGc        The graphics context to flush to
     * @param aIntParams An Integer array with all the parameters and info about the buffered
                         drawing operations except the string parameters
     * @param aIntCount  The length of the integer array
     * @param aStrParams All the string parameters of the buffered drawing operations
     */
    void Draw(MSwtGc* aGc, TInt* aIntParams, TInt aIntCount, HBufC* aStrParams, RWindow* aWindow, const TRect& aDrawRect);

private:
    // The Display that owns this object
    MSwtDisplay& iDisplay;
};

#endif // SWTJAVABUFFERDRAWER_H
