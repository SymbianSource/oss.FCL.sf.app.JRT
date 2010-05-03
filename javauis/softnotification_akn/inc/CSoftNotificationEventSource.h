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


#ifndef CSOFTNOTIFICATIONEVENTSOURCE_H
#define CSOFTNOTIFICATIONEVENTSOURCE_H

//  INCLUDES
#include <mevents.h>


//  CLASS DECLARATION
/**
 *   This class provides CJavaEventSource services.
 */
NONSHARABLE_CLASS(CSoftNotificationEventSource):
        public CJavaEventSource< CSoftNotificationEventSource >
{
public:
    /**
     * C++ default constructor.
     */
    CSoftNotificationEventSource();

    /**
     * 2nd phase constructor.
     *
     * @param aJni JNI environment
     * @param aPeer Java side object for the event source
     * @param aServer Event server
     */
    void ConstructL(JNIEnv& aJni,
                    jobject aPeer,
                    TJavaEventServer aServer);

    /**
     * Destructor.
     */
    virtual ~CSoftNotificationEventSource();

private: // from CJavaEventSource
    /**
     * Called when finalization occurs.
     */
    void FinalizeSvr();

};

#endif // CSOFTNOTIFICATIONEVENTSOURCE_H
