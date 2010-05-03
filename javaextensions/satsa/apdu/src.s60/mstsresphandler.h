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


#ifndef MSTSRESPHANDLER_H
#define MSTSRESPHANDLER_H

#include <jni.h>

namespace java
{
namespace satsa
{

// CLASS DECLARATION

//Base class for response handler

NONSHARABLE_CLASS(MSTSRespHandler)
{

public:

    // Setter for operation id
    virtual void SetOperationId(TInt aId) = 0;

    /**
     * Informs java side, that operation is completed.
     * @param aResponse Response bytes to be delivered to java side.
     *       Ownership transferred.
     * @param aError Error code to be delivered to java side
     */
    virtual void OperationComplete(JNIEnv* aJni, jobject aPeer,
                                   HBufC8* aResponse, TInt aError) = 0;

    virtual ~MSTSRespHandler()
    {
    }
    ;

}
;

} // namespace satsa
} // namespace java
#endif // MSTSRESPHANDLER_H
// End of File
