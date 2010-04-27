/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CSTSRESPHANDLER_H
#define CSTSRESPHANDLER_H

//  INCLUDES
#include <e32base.h>
#include "mstsresphandler.h"
#include "cstsapduevent.h"

namespace java
{
namespace satsa
{

// CLASS DECLARATION
/**
 *  Delivers response data and complete notification to java side.
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSRespHandler): public MSTSRespHandler, public CBase
{
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CSTSRespHandler* NewLC(JNIEnv& aJni,
    jobject aNotifyObject,
    jmethodID aHandleEventMethod);

    static CSTSRespHandler* NewL(JNIEnv& aJni,
                                 jobject aNotifyObject,
                                 jmethodID aHandleEventMethod);

    /**
     * Destructor.
     */
    virtual ~CSTSRespHandler();

public: // Functions from base classes

    /**
     * From MSTSRespHandler
     * @since 3.0
     */
    void SetOperationId(TInt aId);

    /**
     * From MSTSRespHandler
     * @since 3.0
     */
    void OperationComplete(JNIEnv* aJni, jobject aPeer, HBufC8* aResponse, TInt aError);

private:

    /**
     * C++ default constructor.
     */
    CSTSRespHandler();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL(JNIEnv& aJni,
                    jobject aNotifyObject,
                    jmethodID aHandleEventMethod);

private: // Data

    CSTSApduEvent* iApduEvent; //owned

};

} // namespace satsa
} // namespace java
#endif // CSTSRESPHANDLER_H
// End of File
