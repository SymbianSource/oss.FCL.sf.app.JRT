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
* Description:  Java RemCon Observable
*
*/


#include <remconcoreapitarget.h>
#include <remconcoreapitargetobserver.h>
#include <remconinterfaceselector.h>
#include <jdebug.h>

#include "javaremconobservable.h"
#include "panics.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CJavaRemConObservable::ConstructL()
// ---------------------------------------------------------------------------
//
void CJavaRemConObservable::ConstructL()
{
    DEBUG("+ CJavaRemConObservable::ConstructL()");
    iInterfaceSelector = CRemConInterfaceSelector::NewL();
    iCoreTarget = CRemConCoreApiTarget::NewL(*iInterfaceSelector, *this);
    iInterfaceSelector->OpenTargetL();
    DEBUG("- CJavaRemConObservable::ConstructL()");
}


// ---------------------------------------------------------------------------
// CJavaRemConObservable* CJavaRemConObservable::NewL()
// ---------------------------------------------------------------------------
//
CJavaRemConObservable* CJavaRemConObservable::NewL()
{
    CJavaRemConObservable* self = CJavaRemConObservable::NewLC();
    CleanupStack::Pop(self);
    return self;
}


// ---------------------------------------------------------------------------
// CJavaRemConObservable* CJavaRemConObservable::NewLC()
// ---------------------------------------------------------------------------
//
CJavaRemConObservable* CJavaRemConObservable::NewLC()
{
    CJavaRemConObservable* self = new(ELeave) CJavaRemConObservable;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}


// ---------------------------------------------------------------------------
// CJavaRemConObservable::CJavaRemConObservable()
// ---------------------------------------------------------------------------
//
CJavaRemConObservable::CJavaRemConObservable()
{
    DEBUG("+ CJavaRemConObservable::CJavaRemConObservable()");

    DEBUG("- CJavaRemConObservable::CJavaRemConObservable()");
}


// ---------------------------------------------------------------------------
// CJavaRemConObservable::~CJavaRemConObservable()
// ---------------------------------------------------------------------------
//
CJavaRemConObservable::~CJavaRemConObservable()
{
    DEBUG("+ CJavaRemConObservable::~CJavaRemConObservable()");
    /*__ASSERT_DEBUG( iObservers.Count() == 0,
                    User::Panic( KJavaRemConPanicCategory,
                                 EJavaRemConPanicObserverNotRemoved )
                  );*/
    delete iInterfaceSelector;
    iInterfaceSelector = NULL;
    iObservers.Close();
    DEBUG("- CJavaRemConObservable::~CJavaRemConObservable()");
}


// ---------------------------------------------------------------------------
// CJavaRemConObservable::AddObserver
// ---------------------------------------------------------------------------
//
void CJavaRemConObservable::AddObserverL(
    CJavaRemConObserver& aObserver)
{
    DEBUG("+ CJavaRemConObservable::SetObserverL()");
    TInt err = iObservers.Append(&aObserver);
    User::LeaveIfError(err);
    DEBUG("- CJavaRemConObservable::SetObserverL()");
}


// ---------------------------------------------------------------------------
// CJavaRemConObservable::RemoveObserver
// ---------------------------------------------------------------------------
//
void CJavaRemConObservable::RemoveObserver(
    CJavaRemConObserver &aObserver)
{
    DEBUG("+ CJavaRemConObservable::RemoveObserverL()");
    TInt index = iObservers.Find(&aObserver);
    __ASSERT_DEBUG(index != KErrNotFound,
                   User::Panic(KJavaRemConPanicCategory,
                               EJavaRemConPanicObserverNotFound)
                  );
    iObservers.Remove(index);
    DEBUG("- CJavaRemConObservable::RemoveObserver()");
}


// ---------------------------------------------------------------------------
// CJavaRemConObservable::Count
// ---------------------------------------------------------------------------
//
TInt CJavaRemConObservable::Count()
{
    return iObservers.Count();
}


// ---------------------------------------------------------------------------
// CJavaRemConObservable::MrccatoCommand
// ---------------------------------------------------------------------------
//
void CJavaRemConObservable::MrccatoCommand(TRemConCoreApiOperationId aOperationId,
        TRemConCoreApiButtonAction aButtonAct)
{
    DEBUG("+ CJavaRemConObservable::MrccatoCommand");
    for (TInt i = 0; i < iObservers.Count(); i++)
    {
        CJavaRemConObserver* observer = iObservers[i];
        observer->AddCommandMessage(aOperationId, aButtonAct);
    }
    DEBUG("- CJavaRemConObservable::MrccatoCommand");
}


// ---------------------------------------------------------------------------
// CJavaRemConObservable::MrccatoPlay
// ---------------------------------------------------------------------------
//
void CJavaRemConObservable::MrccatoPlay(TRemConCoreApiPlaybackSpeed aSpeed,
                                        TRemConCoreApiButtonAction aButtonAct)
{
    DEBUG("+ CJavaRemConObservable::MrccatoPlay");
    for (TInt i = 0; i < iObservers.Count(); i++)
    {
        CJavaRemConObserver* observer = iObservers[i];
        observer->AddPlayMessage(aSpeed, aButtonAct);
    }
    DEBUG("- CJavaRemConObservable::MrccatoPlay");
}


// ---------------------------------------------------------------------------
// CJavaRemConObservable::MrccatoTuneFunction
// ---------------------------------------------------------------------------
//
void CJavaRemConObservable::MrccatoTuneFunction(TBool /*aTwoPart*/,
        TUint /*aMajorChannel*/,
        TUint /*aMinorChannel*/,
        TRemConCoreApiButtonAction /*aButtonAct*/)
{
    DEBUG("+ CJavaRemConObservable::MrccatoTuneFunction");
    /*for ( TInt i = 0; i < iObservers.Count(); i++)
        {
        MRemConCoreApiTargetObserver* observer = iObservers[i];
        observer->MrccatoTuneFunction( aTwoPart, aMajorChannel,
                                       aMinorChannel, aButtonAct );
        } */
    DEBUG("- CJavaRemConObservable::MrccatoTuneFunction");
}


// ---------------------------------------------------------------------------
// CJavaRemConObservable::MrccatoSelectDiskFunction
// ---------------------------------------------------------------------------
//
void CJavaRemConObservable::MrccatoSelectDiskFunction(TUint /*aDisk*/,
        TRemConCoreApiButtonAction /*aButtonAct*/)
{
    DEBUG("+ CJavaRemConObservable::MrccatoSelectDiskFunction");
    /*for ( TInt i = 0; i < iObservers.Count(); i++)
        {
        MRemConCoreApiTargetObserver* observer = iObservers[i];
        observer->MrccatoSelectDiskFunction( aDisk, aButtonAct );
        } */
    DEBUG("- CJavaRemConObservable::MrccatoSelectDiskFunction");
}


// ---------------------------------------------------------------------------
// CJavaRemConObservable::MrccatoSelectAvInputFunction
// ---------------------------------------------------------------------------
//
void CJavaRemConObservable::MrccatoSelectAvInputFunction(TUint8 /*aAvInputSignalNumber*/,
        TRemConCoreApiButtonAction /*aButtonAct*/)
{
    DEBUG("+ CJavaRemConObservable::MrccatoSelectAvInputFunction");
    /*for ( TInt i = 0; i < iObservers.Count(); i++)
        {
        MRemConCoreApiTargetObserver* observer = iObservers[i];
        observer->MrccatoSelectAvInputFunction( aAvInputSignalNumber,
                                                aButtonAct);
        }*/
    DEBUG("- CJavaRemConObservable::MrccatoSelectAvInputFunction");
}


// ---------------------------------------------------------------------------
// CJavaRemConObservable::MrccatoSelectAudioInputFunction
// ---------------------------------------------------------------------------
//
void CJavaRemConObservable::MrccatoSelectAudioInputFunction(TUint8 /*aAudioInputSignalNumber*/,
        TRemConCoreApiButtonAction /*aButtonAct*/)
{
    DEBUG("+ CJavaRemConObservable::MrccatoSelectAudioInputFunction");
    /*for ( TInt i = 0; i < iObservers.Count(); i++)
       {
       MRemConCoreApiTargetObserver* observer = iObservers[i];
       observer->MrccatoSelectAudioInputFunction( aAudioInputSignalNumber,
                                                  aButtonAct );
       }*/
    DEBUG("- CJavaRemConObservable::MrccatoSelectAudioInputFunction");
}

// End Of File
