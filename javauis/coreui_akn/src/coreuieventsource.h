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


#ifndef COREUIEVENTSOURCE_H
#define COREUIEVENTSOURCE_H

//  INCLUDES
#include "mevents.h"


NONSHARABLE_CLASS(CoreUiEventSource):
        public CJavaEventSource< CoreUiEventSource >
{
public:
    /**
     * Default constructor.
     */
    CoreUiEventSource();

    /**
     * Destructor.
     */
    virtual ~CoreUiEventSource();

    /**
     * 2nd phase constructor.
     *
     * @param jni JNI environment
     * @param peer Java side object for the event source
     * @param server Event server
     */
    void ConstructL(JNIEnv& jni,
                    jobject peer,
                    TJavaEventServer server);

};

#endif // COREUIEVENTSOURCE_H
