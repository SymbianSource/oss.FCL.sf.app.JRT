/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class implements general wait object for calling a desired
*                callback function after asynchronous notification
*
*/

#ifndef ASYNCWAITCALLBACK_H
#define ASYNCWAITCALLBACK_H

#include <e32base.h>

namespace java
{
namespace installer
{

/**
 * This class implements wait object for calling a desired
 * callback function after rendezvous returns specified
 * reason code from the process that this object is listening to.
 */
NONSHARABLE_CLASS(CAsyncWaitCallBack) : public CActive
{
public:  // Constructors and destructor

    static CAsyncWaitCallBack* NewL(TCallBack aCallBack);

    // Destructor.
    virtual ~CAsyncWaitCallBack();

public:  // From base classes

    // from base class CActive

    void RunL();

    void DoCancel();

public:

    void Wait(RProcess aProcessToListen, TInt aActivatingReasonCode);

private:

    // Constructor.
    CAsyncWaitCallBack();

    // 2nd phase constructor.
    void ConstructL(TCallBack aCallBack);

private: //  Data

    // Callback member
    TCallBack   iCallBack;

    // When request completes with this status code,
    // callback is called.
    TInt iActivatingReasonCode;

    RProcess iProcessToListen;
};

} // installer
} // java

#endif // ASYNCWAITCALLBACK_H
