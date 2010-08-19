/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* This class implements general wait object for calling
* a callback function after subscribed P&S key value changes.
*
*/

#ifndef PROPERTYLISTENER_H
#define PROPERTYLISTENER_H

#include <e32base.h>
#include <e32property.h>
#include <jni.h>

namespace java
{
namespace installer
{

/**
 * This class implements wait object for calling a callback
 * function after subscribed P&S key value changes.
 */
NONSHARABLE_CLASS(CPropertyListener) : public CActive
{
public:  // Constructor and destructor

    static CPropertyListener* NewL(TUid aUid, TUint aKey);

    // Destructor.
    virtual ~CPropertyListener();

private:

    // Constructor.
    CPropertyListener(TUid aUid, TUint aKey);

    // 2nd phase constructor.
    void ConstructL();

    // from base class CActive

    void RunL();

    void DoCancel();

public: // Methods

    void ProcessEventsL(JNIEnv *aEnv, jobject aProvider);

private: // Data

    CActiveScheduler* iActiveScheduler; // Owned

    RProperty iProperty; // Property to listen
    TUid iUid; // Property UID
    TUint iKey; // Property key

    JNIEnv *iEnv; // JNI environment, not owned
    jobject iProvider; // Callback object
    jmethodID iMethod; // Callback method

    TBool iCancelled;
};

} // installer
} // java

#endif // PROPERTYLISTENER_H
