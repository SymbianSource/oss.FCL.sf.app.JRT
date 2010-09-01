/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "coreuieventsource.h"

#include "logger.h"

CoreUiEventSource::CoreUiEventSource()
{
    JELOG2(EJavaUI);
}

void CoreUiEventSource::ConstructL(JNIEnv& jni,
                                   jobject peer,
                                   TJavaEventServer server)
{
    JELOG2(EJavaUI);

    CJavaEventSourceBase::ConstructL(jni, peer, server);
}

CoreUiEventSource::~CoreUiEventSource()
{
    JELOG2(EJavaUI);
}
