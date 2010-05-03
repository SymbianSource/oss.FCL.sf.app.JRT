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

#ifndef CMIDNOTIFY_H
#define CMIDNOTIFY_H

#include <mevents.h>
#include <jni.h>
#include "lcdui.h"

class CMIDToolkit;

class CMIDNotifyEvent : public CJavaEvent<CMIDToolkit>
        , public MMIDNotifyEvent
{
public:
    CMIDNotifyEvent(TJavaPeer aPeer);

    /**
     * From MMIDNotifyEvent
     */
    void SetResult(TInt aResult);
    void Dispose();

    /**
     * Call from Java thread to release iPeer
     * if iPeer was *not* obtained from an MMIDComponent.
     */
    void Close(JNIEnv& aEnv);

private:
    virtual void Dispatch(JNIEnv& aJni);

private:
    TInt            iResult;
    TJavaPeer       iPeer;
};

#endif // CMIDNOTIFY_H
