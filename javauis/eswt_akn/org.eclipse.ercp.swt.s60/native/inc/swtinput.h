/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTINPUT_H
#define SWTINPUT_H


#include "eswtmobileextensions.h"


/**
 * CSwtInput
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtInput)
        : public CBase
        , public MSwtInput
{
// Own methods
public:
    static CSwtInput* NewL(TSwtPeer aPeer, TInt aId);

private:
    inline CSwtInput(TSwtPeer aPeer, TInt aId);
    ~CSwtInput();

// From MSwtInput
public:
    TSwtPeer Dispose();
    TInt GetLocation();
    TInt GetType();

// Data
private:
    TSwtPeer iPeer;
    TInt iType;
    TInt iLocation;
};


#endif // SWTINPUT_H
