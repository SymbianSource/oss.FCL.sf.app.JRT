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
* Description:  Java RemCon Observer
*
*/


#ifndef JAVAREMCONOBSERVABLE_H
#define JAVAREMCONOBSERVABLE_H

#include <e32base.h>
#include <remconcoreapitargetobserver.h>
#include "javaremconobserver.h"

class CRemConInterfaceSelector;
class CRemConCoreApiTarget;


/**
 * Class provides common access to CRemConInterfaceSelector for Java components
 *
 * Java RunTime needs to listen to various events like volume key press,
 * media key press and so on. For this purpose, various modules
 * in Java middleware register with CRemConCoreApiTarget to get events
 * to the observer MRemConCoreApiTargetObserver.
 * For this to happen, a target session has to be opened to RemCon,
 * which is done via CRemConInterfaceSelector::OpenTargetL(). At present,
 * whichever module wants it (say MMAPI, LCDUI etc) goes ahead and registers
 * by calling CRemConInterfaceSelector::OpenTargetL(). However when someother
 * module also calls the same, the OpenTargetL() returns KErrInUse as in
 * Java all modules are in one single process and one process can have
 * only one target session open to RemCon. This means that only one of the
 * required modules will get the events needed.
 *
 * Class provides common access to the target session to RemCon
 * which can be used by any module in Java.
 * The user of CRemConCoreApiTarget will need to use this class for the purpose
 * instead of creating themselves.
 *
 *  @lib javaremconobserver
 *  @since S60 v3.2
 */
class CJavaRemConObservable : public CBase, public MRemConCoreApiTargetObserver
{
public:

    static CJavaRemConObservable* NewL();

    static CJavaRemConObservable* NewLC();

    virtual ~CJavaRemConObservable();

public:

    /**
     * Add Observer
     *
     * @since S60 v3.2
     * @param aObserver Added observer, not owned
     */
    void AddObserverL(CJavaRemConObserver &aObserver);

    /**
     * Remove Observer
     *
     * @since S60 v3.2
     * @param aObserver Removed observer, not owned
     */
    void RemoveObserver(CJavaRemConObserver &aObserver);

    /**
     * Observers count
     *
     * @since S60 3.2
     */
    TInt Count();

public: // From MRemConCoreApiTargetObserver

    void MrccatoCommand(TRemConCoreApiOperationId aOperationId,
                        TRemConCoreApiButtonAction aButtonAct);

    void MrccatoPlay(TRemConCoreApiPlaybackSpeed aSpeed,
                     TRemConCoreApiButtonAction aButtonAct);

    void MrccatoTuneFunction(TBool aTwoPart,
                             TUint aMajorChannel,
                             TUint aMinorChannel,
                             TRemConCoreApiButtonAction aButtonAct);

    void MrccatoSelectDiskFunction(TUint aDisk,
                                   TRemConCoreApiButtonAction aButtonAct);

    void MrccatoSelectAvInputFunction(TUint8 aAvInputSignalNumber,
                                      TRemConCoreApiButtonAction aButtonAct);

    void MrccatoSelectAudioInputFunction(TUint8 aAudioInputSignalNumber,
                                         TRemConCoreApiButtonAction aButtonAct);

private:

    CJavaRemConObservable();

    void ConstructL();

private: // data

    /**
    * Observers
    */
    RPointerArray<CJavaRemConObserver> iObservers;

    /**
    * Owned.
    */
    CRemConInterfaceSelector* iInterfaceSelector;

    /**
    * Owned by iInterfaceSelector.
    */
    CRemConCoreApiTarget* iCoreTarget;
};


#endif // JAVAREMCONOBSERVABLE_H
