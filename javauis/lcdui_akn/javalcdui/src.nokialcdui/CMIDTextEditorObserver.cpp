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
* Description: Delivers editor input actions to the Java side.
*
*/


// INTERNAL INCLUDES
#include "CMIDTextEditorObserver.h"

// EXTERNAL INCLUDES
#include <j2me/jdebug.h>

// ---------------------------------------------------------------------------
// CMIDTextEditorEvent::CMIDTextEditorEvent
// ---------------------------------------------------------------------------
//
CMIDTextEditorObserver::CMIDTextEditorEvent::CMIDTextEditorEvent(
    jobject aPeer,
    jmethodID aMethod,
    TUint aParam) :
        CJavaEvent(CJavaEventBase::EDisposableEvent),
        iPeer(aPeer), iMethod(aMethod), iParam(aParam)
{
}

// ---------------------------------------------------------------------------
// CMIDTextEditorObserver::~CMIDTextEditorEvent
// ---------------------------------------------------------------------------
//
CMIDTextEditorObserver::CMIDTextEditorEvent::~CMIDTextEditorEvent()
{
    // Nothing to do.
}

// ---------------------------------------------------------------------------
// CMIDTextEditorObserver::~CMIDTextEditorEvent
// ---------------------------------------------------------------------------
//
void CMIDTextEditorObserver::CMIDTextEditorEvent::Dispatch(JNIEnv& aJniEnv)
{
    DEBUG("CMIDTextEditorObserver::Dispatch +");

    // Make a direct call to the Java side.
    aJniEnv.CallVoidMethod(iPeer, iMethod, iParam);

    // Exception is cleared after this call returns to base class.

    DEBUG("CMIDTextEditorObserver::Dispatch -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorObserver::NewL
// ---------------------------------------------------------------------------
//
CMIDTextEditorObserver* CMIDTextEditorObserver::NewL(
    CMIDToolkit& aToolkit,
    JNIEnv& aJniEnv,
    jobject aPeer,
    jmethodID aMethod)
{
    CMIDTextEditorObserver* self =  new(ELeave) CMIDTextEditorObserver(
        aToolkit, aJniEnv, aPeer, aMethod);

    return self;
}

// ---------------------------------------------------------------------------
// CMIDTextEditorObserver::~CMIDTextEditorObserver
// ---------------------------------------------------------------------------
//
CMIDTextEditorObserver::~CMIDTextEditorObserver()
{
}

// ---------------------------------------------------------------------------
// CMIDTextEditorObserver::Dispose
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDTextEditorObserver::Dispose()
{
    DEBUG("CMIDTextEditorObserver::Dispose +");

    delete this;

    DEBUG("CMIDTextEditorObserver::Dispose -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorObserver::InputAction
// ---------------------------------------------------------------------------
//
void CMIDTextEditorObserver::NotifyInputAction(TUint aActions)
{
    DEBUG("CMIDTextEditorObserver::NotifyInputAction +");

    TRAP_IGNORE(
    {
        // Cannot currently use reusable since events may occur
        // really fast causing some events to be missed or overwritten.
        CMIDTextEditorEvent* event =
        new CMIDTextEditorEvent(iPeer, iMethod, aActions);
        // Post event
        iToolkit.PostEvent(event, CJavaEventBase::EEventPriority);
    });

    DEBUG("CMIDTextEditorObserver::NotifyInputAction -");
}

// ---------------------------------------------------------------------------
// CMIDTextEditorObserver::CMIDTextEditorObserver
// ---------------------------------------------------------------------------
//
CMIDTextEditorObserver::CMIDTextEditorObserver(
    CMIDToolkit& aToolkit,
    JNIEnv& aJniEnv,
    jobject aPeer,
    jmethodID aMethod) :
        iToolkit(aToolkit),
        iJniEnv(aJniEnv),
        iPeer(aPeer),
        iMethod(aMethod)
{
}

// End of file
