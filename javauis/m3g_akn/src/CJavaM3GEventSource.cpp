/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#include "CJavaM3GEventSource.h"
#include <jdebug.h>

#include <jni.h>
#include <mevents.h>


extern "C" TInt initM3GEventSourceL(JNIEnv& aJni,jobject aPeer,TJavaEventServer aServer)
{
    return CJavaM3GEventSource::NewL(aJni, aPeer, aServer);
}

TInt CJavaM3GEventSource::NewL(JNIEnv& aJni,jobject aPeer,TJavaEventServer aServer)
{

    CJavaM3GEventSource* self = new(ELeave) CJavaM3GEventSource();
    CleanupStack::PushL(self);
    self->ConstructL(aJni, aPeer, aServer);
    CleanupStack::Pop(self);

    return JavaMakeHandle(self);
}



CJavaM3GEventSource::CJavaM3GEventSource()
{
}


CJavaM3GEventSource::~CJavaM3GEventSource()
{
}


void CJavaM3GEventSource::ConstructL(JNIEnv& aJni,jobject aPeer,TJavaEventServer aServer)
{
    CJavaEventSourceBase::ConstructL(aJni, aPeer, aServer);
}



