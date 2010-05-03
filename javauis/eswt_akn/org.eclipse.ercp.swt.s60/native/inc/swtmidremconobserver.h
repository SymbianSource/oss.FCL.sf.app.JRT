/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/

#ifndef SWTMIDREMCONOBSERVER_H
#define SWTMIDREMCONOBSERVER_H

#include <e32base.h>
#include <remconcoreapitargetobserver.h>
#include <remconerrorobserver.h>

#include "swtmidmediakeyslistener.h"

class CRemConInterfaceSelector;
class CRemConCoreApiTarget;
class CJavaRemConManager;
class CJavaRemConObserver;


/**
 * This class handles media keys events that are received via MRemConCoreApiTargetObserver interface.
 * The received media key events are forwarded to listeners object that registered to this class.
 * Listeners implement MMIDMediaKeysListener interface. There is only one CMIDRemConObserver per
 * MIDlet (or none if Media key support is not enbaled by the jad attribute).
 *
 * @since S60 3.2
 */
NONSHARABLE_CLASS(CSwtMIDRemConObserver) : public CBase, public MRemConCoreApiTargetObserver, MRemConErrorObserver
{
public:

    /**
     * Creates and returns a new instance of this class.
     */
    IMPORT_C static CSwtMIDRemConObserver* NewL();

    /**
     * Destructor.
     */
    IMPORT_C ~CSwtMIDRemConObserver();

    /**
     * Adds a new listener to iListeners array.
     *
     * @param aListener     The listener to be added.
     * @since S60 3.2
     */
    IMPORT_C void AddMediaKeysListenerL(MSwtMediaKeysListener* aListener);

    /**
     * Removes a listener from iListeners array.
     *
     * @param aListener     The listener to be removed.
     * @since S60 3.2
     */
    IMPORT_C void RemoveMediaKeysListener(MSwtMediaKeysListener* aListener);


// from MRemConCoreApiTargetObserver
    /**
     * Called by RemCon when a command (i.e. media key event in this case) has been received.
     *
     * @param aOperationId  The operation ID of the command.
     * @param aButtonAct    The button action associated with the command.
     * @since S60 3.2
     */
    void MrccatoCommand(TRemConCoreApiOperationId, TRemConCoreApiButtonAction aButtonAct);

    /**
     * Called by RemCon when a 'play' command has been received.
     *
     * @param aSpeed        The playback speed.
     * @param aButtonAct    The button action associated with the command.
     * @since S60 3.2
     */
    void MrccatoPlay(TRemConCoreApiPlaybackSpeed aSpeed, TRemConCoreApiButtonAction aButtonAct);

// from MRemConErrorObserver
    /**
     * Called by RemCon when an error occurs.
     *
     * @param aErrorId  The error that has occurred.
     * @since S60 3.2
     */
    void MrceoError(TInt aError);

private:
    /**
     * Default constructor.
     */
    CSwtMIDRemConObserver();

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Posts media key event to all listeners.
     *
     * @param aKeyCode      The key code of the media key.
     * @param aEventType    The type of the event, pressed, repeated or released.
     * @since S60 3.2
     */
    void PostRawEventToListeners(MSwtMediaKeysListener::TMediaKey aKeyCode, TInt aEventType);

    /**
     * Posts media key event to all listeners.
     *
     * @param aKeyCode      The key code of the media key.
     * @param aEventType    The type of the event, pressed, repeated or released.
     * @since S60 3.2
     */
    void PostClickEventToListeners(MSwtMediaKeysListener::TMediaKey aKeyCode);

    /**
     * Static callback function called by the repeat timer.
     *
     * @param aThis     Pointer to an instance of this class.
     * @return TInt     Returns zero always.
     * @since S60 3.2
     */
    static TInt RepeatTimerCallback(TAny* aThis);

    /**
     * Generates keyRepeated events when repeat timer expires.
     *
     * @since S60 3.2
     */
    void HandleRepeatTimerEvent();

private:
    /**
     * Interface selector, owned.
     */
    CRemConInterfaceSelector* iInterfaceSelector;

    /**
     * Owned by iInterfaceSelector.
     */
    CRemConCoreApiTarget* iCoreTarget;

    /**
     * Timer for generating keyRepeated events, owned.
     */
    CPeriodic* iRepeatTimer;

    /**
     * Media key listener array.
     */
    RArray<MSwtMediaKeysListener*> iListeners;

    /**
     * The key for which keyRepeated event are being generated with iRepeatTimer.
     */
    MSwtMediaKeysListener::TMediaKey iKeyRepeating;

    /**
     * Provides common access to CRemConInterfaceSelector for Java components
     */
    CJavaRemConManager *iJavaRemConManager;

    /**
     * Used for receiving RemCon API events
     */
    CJavaRemConObserver *iJavaRemConObserver;
};

#endif // SWTMIDREMCONOBSERVER_H

// End of File
