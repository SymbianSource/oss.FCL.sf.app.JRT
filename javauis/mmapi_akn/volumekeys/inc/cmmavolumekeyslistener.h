/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class handles the volume keys update events
*
*
*/


#ifndef CMMAVOLUMEKEYSLISTENER_H
#define CMMAVOLUMEKEYSLISTENER_H

// EXTERNAL INCLUDES
#include <e32base.h>
#include <remconcoreapitargetobserver.h>
#include <javaremconmanager.h>
#include <javaremconobserver.h>

// INTERNAL INCLUDES
#include "mmmavolumekeyslistener.h"

// CLASS DECLARATION
class CRemConInterfaceSelector;
class CRemConCoreApiTarget;

// CLASS DECLARATION
/**
 *This class handles the volume keys update events
 *
 */
NONSHARABLE_CLASS(CMMAVolumeKeysListener) : public CBase, public MRemConCoreApiTargetObserver
{
public:  // Constructors and destructor

    /**
     * Creates and returns a new instance of this class.
     * @param aListener Listener for key events
     */
    static CMMAVolumeKeysListener* NewL(MMMAVolumeKeysListener* aListener);

    // Destructor
    ~CMMAVolumeKeysListener();

private:    // Constructors

    // Default constructor
    CMMAVolumeKeysListener();

    /**
     * Second phase constructor.
     * @param aListener Listener for key events
     */
    void ConstructL(MMMAVolumeKeysListener* aListener);

public: // New functions


    void HandleTimerEvent();


    // From MRemConCoreApiTargetObserver
    void MrccatoPlay(TRemConCoreApiPlaybackSpeed aSpeed,
                     TRemConCoreApiButtonAction aButtonAct);

    void MrccatoCommand(TRemConCoreApiOperationId aOperationId,
                        TRemConCoreApiButtonAction aButtonAct);

    void MrccatoTuneFunction(TBool aTwoPart, TUint aMajorChannel,
                             TUint aMinorChannel,
                             TRemConCoreApiButtonAction aButtonAct);

    void MrccatoSelectDiskFunction(TUint aDisk,
                                   TRemConCoreApiButtonAction aButtonAct);

    void MrccatoSelectAvInputFunction(
        TUint8 aAvInputSignalNumber,
        TRemConCoreApiButtonAction aButtonAct);
    void MrccatoSelectAudioInputFunction(
        TUint8 aAudioInputSignalNumber,
        TRemConCoreApiButtonAction aButtonAct);

private:

    /**
     * TimerCallback
     * This is the callback function called from timer.
     */
    static TInt TimerCallback(TAny* aThis);

private:
    CRemConInterfaceSelector* iInterfaceSelector; //owned
    CRemConCoreApiTarget* iCoreTarget; //owned by iInterfaceSelector

    // For getting timer ticks/events, owned
    CPeriodic* iTimer;
    // Marks if volume is increased or decreased
    TBool iUp;
    // Listener listening events. Used
    MMMAVolumeKeysListener* iListener;
    CJavaRemConManager *iJavaRemConManager;
    CJavaRemConObserver *iJavaRemConObserver;
};

#endif // CMMAVOLUMEKEYSLISTENER_H

// End of File
