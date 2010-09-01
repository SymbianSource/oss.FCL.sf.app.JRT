/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Java RemCon Observer
*
*/


#include <jdebug.h>

#include "javaremconmanager.h"
#include "javaremconobservable.h"
#include "panics.h"

// ======== STATIC VARIABLES ========

/**
* Singleton
*/

NONSHARABLE_CLASS(RemConnGlobals)
{
public:
    RemConnGlobals() : mObservable(0) {}

public:
    CJavaRemConObservable* mObservable;
};


#if defined(__WINSCW__)

#include <pls.h>
RemConnGlobals* getRemConnGlobals()
{
    // Access the PLS of this process
    RemConnGlobals* globals  =
        Pls<RemConnGlobals>(TUid::Uid(0x200211E1));
    return globals;
}

#else

static RemConnGlobals* sGlobals = 0;

RemConnGlobals* getRemConnGlobals()
{
    if (sGlobals == 0)
    {
        sGlobals = new RemConnGlobals();
    }
    return sGlobals;
}
#endif




// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CJavaRemConManager::ConstructL()
// ---------------------------------------------------------------------------
//
void CJavaRemConManager::ConstructL()
{
    DEBUG("+ CJavaRemConManager::ConstructL()");
    RemConnGlobals* globals = getRemConnGlobals();
    if (globals->mObservable == 0)
    {
        globals->mObservable = CJavaRemConObservable::NewL();
        RThread thread;
        iCreatorThreadId = thread.Id();
    }
    DEBUG("- CJavaRemConManager::ConstructL()");
}


// ---------------------------------------------------------------------------
// CJavaRemConManager* CJavaRemConManager::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRemConManager* CJavaRemConManager::NewL()
{
    CJavaRemConManager* self = CJavaRemConManager::NewLC();
    CleanupStack::Pop(self);
    return self;
}


// ---------------------------------------------------------------------------
// CJavaRemConManager* CJavaRemConManager::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRemConManager* CJavaRemConManager::NewLC()
{
    CJavaRemConManager* self = new(ELeave) CJavaRemConManager;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}


// ---------------------------------------------------------------------------
// CJavaRemConManager::CJavaRemConManager()
// ---------------------------------------------------------------------------
//
CJavaRemConManager::CJavaRemConManager() : iSet(EFalse)
{
    DEBUG("+ CJavaRemConManager::CJavaRemConManager()");

    DEBUG("- CJavaRemConManager::CJavaRemConManager()");
}


// ---------------------------------------------------------------------------
// CJavaRemConManager::~CJavaRemConManager()
// ---------------------------------------------------------------------------
//
CJavaRemConManager::~CJavaRemConManager()
{
    DEBUG("+ CJavaRemConManager::~CJavaRemConManager()");
    __ASSERT_DEBUG(!iSet, User::Panic(KJavaRemConPanicCategory,
                                      EJavaRemConPanicObserverNotRemoved)
                  );

    RThread thread;
    if (thread.Id() == iCreatorThreadId)
    {
        delete getRemConnGlobals()->mObservable;
        getRemConnGlobals()->mObservable = NULL;
    }
    DEBUG("- CJavaRemConManager::~CJavaRemConManager()");
}


// ---------------------------------------------------------------------------
// CJavaRemConManager::SetObserver
// ---------------------------------------------------------------------------
//
EXPORT_C void CJavaRemConManager::SetObserverL(
    CJavaRemConObserver &aObserver)
{
    DEBUG("+ CJavaRemConManager::SetObserverL()");
    if (iSet)
    {
        User::Leave(KErrInUse);
    }
    getRemConnGlobals()->mObservable->AddObserverL(aObserver);
    iSet = ETrue;
    DEBUG("- CJavaRemConManager::SetObserverL()");
}


// ---------------------------------------------------------------------------
// CJavaRemConManager::RemoveObserver
// ---------------------------------------------------------------------------
//
EXPORT_C void CJavaRemConManager::RemoveObserver(
    CJavaRemConObserver &aObserver)
{
    DEBUG("+ CJavaRemConManager::RemoveObserverL()");
    __ASSERT_DEBUG(iSet, User::Panic(KJavaRemConPanicCategory,
                                     EJavaRemConPanicObserverNotSet)
                  );

    CJavaRemConObservable* observable = getRemConnGlobals()->mObservable;
    if (observable)
    {
        observable->RemoveObserver(aObserver);
    }

    iSet = EFalse;
    DEBUG("- CJavaRemConManager::RemoveObserver()");
}


// End Of File
