/*
* Copyright (c) 1999-2001 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#include <mevents.h>
#include "CJavaEventServer.h"

EXPORT_C CJavaEventSourceBase::CJavaEventSourceBase()
{}

EXPORT_C CJavaEventSourceBase::~CJavaEventSourceBase()
{
    // Must never be deleted directly
    ASSERT(iRef==-1);
    if (iServer.Handle())
    {
        iServer->RemoveRef();
    }
}

EXPORT_C void CJavaEventSourceBase::ConstructL(JNIEnv& aJni,jobject aPeer,TJavaEventServer aServer)
//
// Create a weak global reference to the Java object for event dispatching
//
{
    iServer=aServer;
    iServer->AddRef();
    iPeer=aJni.NewWeakGlobalRef(aPeer);
    if (!iPeer)
        User::Leave(KErrGeneral);   // exception is thrown on return to Java
}

EXPORT_C void CJavaEventSourceBase::FinalizeSvr()
//
// Default implementation for server-side finalization
//
{
}

EXPORT_C void CJavaEventSourceBase::FinalizeJni(JNIEnv&)
//
// Default implementation for Java-side finalization
//
{
}
