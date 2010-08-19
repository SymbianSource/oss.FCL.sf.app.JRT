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

#include "propertylistener.h"
#include "logger.h"

using namespace java::installer;

// ============================ MEMBER FUNCTIONS ===============================

CPropertyListener* CPropertyListener::NewL(TUid aUid, TUint aKey)
{
    CPropertyListener* self = new(ELeave) CPropertyListener(aUid, aKey);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CPropertyListener::CPropertyListener(TUid aUid, TUint aKey)
    : CActive(CActive::EPriorityStandard), iUid(aUid), iKey(aKey)
{
}

void CPropertyListener::ConstructL()
{
    User::LeaveIfError(iProperty.Attach(iUid, iKey));
    // PropertyListener is always created from a thread which does
    // not have ActiveScheduler, so create ActiveScheduler now.
    iActiveScheduler = new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(iActiveScheduler);
    // Add this object to the ActiveScheduler.
    CActiveScheduler::Add(this);
}

CPropertyListener::~CPropertyListener()
{
    Cancel();
    iProperty.Close();
    if (iActiveScheduler)
    {
        delete iActiveScheduler;
        iActiveScheduler = 0;
    }
}

void CPropertyListener::ProcessEventsL(JNIEnv *aEnv, jobject aProvider)
{
    // Get the java side callback method.
    jclass clazz = aEnv->GetObjectClass(aProvider);
    jmethodID method = aEnv->GetMethodID(clazz, "valueChanged", "(I)V");

    iEnv = aEnv;
    iProvider = aProvider;
    iMethod = method;

    // Run propertyListener once so that the current property value
    // gets delivered and property value changes are subscribed.
    RunL();

    // Start ActiveScheduler.
    CActiveScheduler::Start();
}

void CPropertyListener::RunL()
{
    if (iCancelled)
    {
        CActiveScheduler::Stop();
        return;
    }

    // Resubscribe before handling the new value to prevent missing updates.
    iProperty.Subscribe(iStatus);
    SetActive();

    TInt value;
    TInt err = iProperty.Get(value);
    if (KErrNone == err)
    {
        //ILOG3(EJavaInstaller,
        //      "CPropertyListener::RunL: property value changed"
        //      ", uid: 0x%x, key: 0x%x, value: %d", iUid.iUid, iKey, value);
        // Use callback method to deliver the changed property value to
        // Java side object.
        iEnv->CallVoidMethod(iProvider, iMethod, value);
    }
    else if (KErrNotFound == err)
    {
        // Property deleted, no actions needed.
    }
    else
    {
        // Error while reading property value.
        ELOG3(EJavaInstaller,
              "CPropertyListener::RunL: getting property value failed"
              ", uid: 0x%x, key: 0x%x, err=%d", iUid.iUid, iKey, err);
    }
}

void CPropertyListener::DoCancel()
{
    iCancelled = ETrue;
    iProperty.Cancel();
}
