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


#ifndef CMIDEVENT_H
#define CMIDEVENT_H

#include <mevents.h>
#include "CMIDToolkit.h"

class CMIDEvent : public CJavaEvent<CMIDToolkit>
{
public:
    CMIDEvent(jmethodID aMethod, jobject aSource, TEventType aType, TInt aParam0=0, TInt aParam1=0,TInt aParam2=0);

private:
    void Dispatch(JNIEnv& aJni);

private:
    jmethodID   iMethod;
    jobject     iSource;
    TEventType  iType;
    TInt        iParam0;
    TInt        iParam1;
    TInt        iParam2;
};


#endif // CMIDEVENT_H
