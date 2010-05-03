/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


#ifndef CMIDREMCONOBSERVER_H
#define CMIDREMCONOBSERVER_H

#include <e32base.h>
// MRemConCoreApiTargetObserver interface for recieving keys events
#include <remconcoreapitargetobserver.h>
// MRemConErrorObserver interface - an error notification interface
#include <remconerrorobserver.h>

// for Media key listener array (iListeners)
#include "MMIDMediaKeysListener.h"
#include "javaremconmanager.h"
#include "javaremconobserver.h"

class CRemConInterfaceSelector;
class CRemConCoreApiTarget;


/**
 * This class handles media keys events that are received via MRemConCoreApiTargetObserver interface.
 * The received media key events are forawrded to listeners object that registered to this class.
 * Listeners implement MMIDMediaKeysListener interface and in practice they are CMIDCanvas or
 * CMIDCustomItem objects. There is only one CMIDRemConObserver per MIDlet (or none if Media key support
 * is not enbaled by the jad attribute).
 *
 * @since S60 3.2
 */
NONSHARABLE_CLASS(CMIDRemConObserver) : public CBase, public MRemConCoreApiTargetObserver, MRemConErrorObserver
{
public:

    /**
     * Creates and returns a new instance of this class.
     */
    static CMIDRemConObserver* NewL();

    /**
     * Destructor.
     */
    ~CMIDRemConObserver();

    /**
     * Adds a new listener to iListeners array.
     *
     * @param aListener     The listener to be added.
     * @since S60 3.2
     */
    void AddMediaKeysListenerL(MMIDMediaKeysListener* aListener);

    /**
     * Removes a listener from iListeners array.
     *
     * @param aListener     The listener to be removed.
     * @since S60 3.2
     */
    void RemoveMediaKeysListener(MMIDMediaKeysListener* aListener);


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
    CMIDRemConObserver();

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
    void PostEventToListeners(MMIDMediaKeysListener::TMediaKey aKeyCode, TMIDKeyEvent::TEvent aEventType);

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
    RArray<MMIDMediaKeysListener*> iListeners;

    /**
     * The key for which keyRepeated event are being generated with iRepeatTimer.
     */
    MMIDMediaKeysListener::TMediaKey iKeyRepeating;
    CJavaRemConManager *iJavaRemConManager;
    CJavaRemConObserver *iJavaRemConObserver;
};

#endif // CMIDREMCONOBSERVER_H

// End of File
