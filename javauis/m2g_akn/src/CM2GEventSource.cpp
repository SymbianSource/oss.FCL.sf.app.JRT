/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Event server framework's event source implementation
*
*/

// INCLUDE FILES
#include "CM2GEventSource.h"

M2G_NS_START

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// -----------------------------------------------------------------------------
// CM2GEventSource::NewL
// -----------------------------------------------------------------------------
TInt CM2GEventSource::NewL(
    JNIEnv& aJni, jobject aPeer, TJavaEventServer aServer)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GEventSource::NewLC()");
    CM2GEventSource* self = new(ELeave) CM2GEventSource;
    // Cleanup the event source object if a leave occurs during construction:
    TConstructor selfCleanup(self, aJni);
    self->ConstructL(aJni, aPeer, aServer);
    // Pop the cleanup of the object and create a handle:
    return selfCleanup.GetHandle();
}

// -----------------------------------------------------------------------------
// CM2GEventSource::~CM2GEventSource
// -----------------------------------------------------------------------------
CM2GEventSource::~CM2GEventSource()
{
}

// -----------------------------------------------------------------------------
// CM2GEventSource::CM2GEventSource
// -----------------------------------------------------------------------------
CM2GEventSource::CM2GEventSource()
{
}

// -----------------------------------------------------------------------------
// CM2GEventSource::CM2GEventSource
// -----------------------------------------------------------------------------
void CM2GEventSource::ConstructL(
    JNIEnv& aJni, jobject aPeer, TJavaEventServer aServer)
{
    CJavaEventSourceBase::ConstructL(aJni, aPeer, aServer);
}

M2G_NS_END

