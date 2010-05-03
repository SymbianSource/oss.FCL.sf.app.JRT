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


#ifndef CSTSAPDUEVENT_H
#define CSTSAPDUEVENT_H

//  INCLUDES

#include <jni.h>

namespace java
{
namespace satsa
{

//  CLASS DECLARATION

// This class is used to post events to the java.


NONSHARABLE_CLASS(CSTSApduEvent)
{
public:
    CSTSApduEvent(JNIEnv& aJni,
                  jobject aNotifyObject,
                  jmethodID aHandleEventMethod);

    virtual ~CSTSApduEvent();

public:

    /**
     * Setter for operation id
     * @param aId Id of java side operation
     *
     */
    void SetId(TInt aId);

    /**
     * Setter for error code
     * @param aError Error code to be delivered to java side
     */
    void SetErrorCode(TInt aError);

    /**
     * Setter for response data
     * @param aResponseData Response bytes to be delivered to java side
     */
    void SetData(HBufC8* aResponseData);

    // To call the java mathod
    void Dispatch(JNIEnv* aJni, jobject aPeer);

private: // data

    // To call java method
    JNIEnv& iJni;
    // java side object which method will be called
    jobject iListenerObject;
    // method of that java object, which will be called
    jmethodID iHandleEventMethod;
    TInt iId;
    TInt iError;
    HBufC8* iResponseData; //not owned

}
;

} // namespace satsa
} // namespace java
#endif // CSTSAPDUEVENT_H
