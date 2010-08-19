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


#include <hal.h>
#include "swtinput.h"
#include "swtmobiledevice.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtInput::NewL
// ---------------------------------------------------------------------------
//
CSwtInput* CSwtInput::NewL(TSwtPeer aPeer, TInt aId)
{
    CSwtInput* self = new(ELeave) CSwtInput(aPeer);
    CleanupStack::PushL(self);
    self->ConstructL(aId);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtInput::CSwtInput
// ---------------------------------------------------------------------------
//
CSwtInput::CSwtInput(TSwtPeer aPeer)
        : iPeer(aPeer)
        , iType(KErrNotFound)
        , iLocation(KErrNotFound)
{
}

void CSwtInput::ConstructL(TInt aId)
{
    RArray<CSwtMobileDevice::TSwtHwInput> inputs;
    CleanupClosePushL(inputs);
    CSwtMobileDevice::GetHwInputsL(inputs);
    if (aId < inputs.Count())
    {
        iType = inputs[aId].iType;
        iLocation = inputs[aId].iLocation;
    }
    CleanupStack::PopAndDestroy(&inputs);
}


// ---------------------------------------------------------------------------
// CSwtInput::~CSwtInput
// ---------------------------------------------------------------------------
//
CSwtInput::~CSwtInput()
{
}

// ---------------------------------------------------------------------------
// CSwtInput::Dispose
// From MSwtInput
// ---------------------------------------------------------------------------
//
TSwtPeer CSwtInput::Dispose()
{
    TSwtPeer peer(iPeer);
    delete this;
    return peer;
}

// ---------------------------------------------------------------------------
// CSwtInput::GetLocation
// From MSwtInput
// ---------------------------------------------------------------------------
//
TInt CSwtInput::GetLocation()
{
    return iLocation;
}

// ---------------------------------------------------------------------------
// CSwtInput::GetType
// From MSwtInput
// ---------------------------------------------------------------------------
//
TInt CSwtInput::GetType()
{
    return iType;
}
