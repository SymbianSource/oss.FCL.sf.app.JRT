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


#ifndef CJAVAM3GEVENTSOURCE_H
#define CJAVAM3GEVENTSOURCE_H

/**
@file
@internal
@released
*/


#include <mevents.h>

class CJavaM3GEventSource : public CJavaEventSourceBase
{
public:
    static TInt NewL(JNIEnv& aJni,jobject aPeer,TJavaEventServer aServer);
    ~CJavaM3GEventSource();
    static CJavaM3GEventSource* getInstance();
private:
    CJavaM3GEventSource();
    void ConstructL(JNIEnv& aJni,jobject aPeer,TJavaEventServer aServer);
};

#endif // CJAVAM3GEVENTSOURCE_H
