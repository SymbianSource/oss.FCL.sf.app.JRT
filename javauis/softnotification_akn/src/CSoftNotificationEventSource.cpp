/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class provides CJavaEventSource services
 *
*/


//  INCLUDE FILES
#include <jdebug.h> // DEBUG
#include "CSoftNotificationEventSource.h"


// -----------------------------------------------------------------------------
// CSoftNotificationEventSource::CSoftNotificationEventSource
// -----------------------------------------------------------------------------
//
CSoftNotificationEventSource::CSoftNotificationEventSource()
{
}

// -----------------------------------------------------------------------------
// CSoftNotificationEventSource::ConstructL
// -----------------------------------------------------------------------------
//
void CSoftNotificationEventSource::ConstructL(
    JNIEnv& aJni,
    jobject aPeer,
    TJavaEventServer aServer)
{
    DEBUG("CSoftNotificationEventSource::ConstructL");

    CJavaEventSourceBase::ConstructL(aJni, aPeer, aServer);
}

// -----------------------------------------------------------------------------
// CSoftNotificationEventSource::~CSoftNotificationEventSource
// -----------------------------------------------------------------------------
//
CSoftNotificationEventSource::~CSoftNotificationEventSource()
{
    DEBUG("CSoftNotificationEventSource::~CSoftNotificationEventSource");
}

// -----------------------------------------------------------------------------
// CSoftNotificationEventSource::FinalizeSvr
// From CJavaEventSource.
// -----------------------------------------------------------------------------
//
void CSoftNotificationEventSource::FinalizeSvr()
{
    DEBUG("CSoftNotificationEventSource::FinalizeSvr");
}

//  END OF FILE
