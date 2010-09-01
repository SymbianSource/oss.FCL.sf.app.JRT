/*******************************************************************************
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTMASKHANDLER_H_
#define SWTMASKHANDLER_H_

#include "eswtgraphics.h"

/**
 * Class manages masks for Trees, because CAknTree ignores images without
 * masks
 * @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtMaskHandler)
{
// Own api
public:
    CSwtMaskHandler(TDisplayMode aDisplayMode);
    virtual ~CSwtMaskHandler();
    CFbsBitmap* GetMask(TSize aSize);

private:
    CFbsBitmap* NewMaskL(TSize aSize);

// Data
private:
    // Pointers to masks
    RPointerArray<CFbsBitmap> iMasks;

    // Display mode
    TDisplayMode iDisplayMode;
};

#endif // SWTMASKHANDLER_H_

