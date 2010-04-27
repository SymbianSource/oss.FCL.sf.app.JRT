/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#include "CMIDToolkit.h"
#include "javax_microedition_lcdui_CommandPeer.h"

LOCAL_C void CreateCommandL
(
    CMIDToolkit* aToolkit,
    TInt* aHandle,
    const TDesC* aShortLabel,
    const TDesC* aLongLabel,
    jint aType,
    jint aPriority,
    jint aCommandID
)
{
    MMIDCommand::TCommandType type = (MMIDCommand::TCommandType)aType;

    MMIDCommand* command = aToolkit->ComponentFactory()->CreateCommandL(*aShortLabel,*aLongLabel, type, aPriority, aCommandID);
    CleanupDisposePushL(command);
    *aHandle = aToolkit->RegisterComponentL(command, NULL);
    CleanupPopComponent(command);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_CommandPeer__1create
(
    JNIEnv* aJni,
    jobject,
    jint aToolkit,
    jstring aShortLabel,
    jstring aLongLabel,
    jint aCommandType,
    jint aPriority,
    jint aCommandID
)
{
    RJString shortLabel(*aJni,aShortLabel);
    RJString longLabel(*aJni,aLongLabel);

    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    const TDesC* pShortLabel = &shortLabel;
    const TDesC* pLongLabel = &longLabel;
    TInt handle = 0;

    TInt err = toolkit->ExecuteTrap(&CreateCommandL, toolkit, &handle, pShortLabel, pLongLabel, aCommandType, aPriority, aCommandID);
    if (err != KErrNone)
    {
        handle = err;
    }

    return handle;
}

