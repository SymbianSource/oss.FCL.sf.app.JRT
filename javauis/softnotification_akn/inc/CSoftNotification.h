/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Controls the native soft notifications.
*
*/


#ifndef CSOFTNOTIFICATION_H
#define CSOFTNOTIFICATION_H

// INCLUDES
#include <e32base.h>
#include <AknDynamicSoftNotifier.h>
#include <jutils.h>

// FORWARD DECLARATION
class TAknDynamicSoftNotificationParams;
class CSoftNotificationEventSource;
class CSoftNotificationEvent;


// CLASS DECLARATION
/**
 *  Controls the native soft notifications.
 *  This class delegates method calls to AvKon soft notification.
 *
 *  @since 3.2
 */
NONSHARABLE_CLASS(CSoftNotification) : public CBase,
        public MAknDynamicSoftNoteObserver
{
public:  // Constructors and destructor

    /**
     * Static constructor
     */
    static CSoftNotification* NewL(TInt aAppId,
    TInt aNotificationId,
    CSoftNotificationEventSource& aEventSource);

    /**
     * Destructor.
     */
    virtual ~CSoftNotification();

public: // From MAknDynamicSoftNotifierObserver

    /**
     * Dynamic soft notification was accepted by user.
     *
     * @param aNoteId Identification of notification.
     */
    void NotificationAccepted(TInt aIdentifier);

    /**
     * Dynamic soft notification was canceled by user.
     *
     * @param aNoteId Identification of notification.
     */
    void NotificationCanceled(TInt aIdentifier);

public: // New functions

    /**
     * Add a custom soft notification. If a custom soft notification with
     * exactly the same parameters already exists,
     * its count is increased by aCount.
     *
     * @param aNotificationId identifier for this notification
     * @param aCount addition count
     */
    void ShowSoftNotificationL();

    /**
     * Cancels custom soft notification.
     *
     * @param aNotificationId identifier for this notification
     */
    void RemoveSoftNotificationL();

    /**
     * Set a text for a soft notification.
     *
     * @param aSingularText singular text for soft notification
     * @param aPluralText plural text for soft notification
     */
    void SetTextL(const TDesC& aText, const TDesC& aGroupText);

    /**
     * Set labels for soft notification's softkeys.
     *
     * @param aSoftkey1Label Label for the softkey 1
     * @param aSoftkey2Label Label for the softkey 2
     */
    void SetSoftkeyLabelsL(const TDesC& aSoftkey1Label,
                           const TDesC& aSoftkey2Label);

    /**
     * Set an image for a soft notification.
     *
     * @param aImage image for soft notification
     */
    void SetImageL(const TDesC8& aImage);

    /**
     * Returns the notification id.
     *
     * @return notification id
     */
    TInt Id();

    /**
     * Set Java peer observer instance for event callbacks.
     * Note: the observer has to be a global weak reference.
     *
     * @param aPeer Java peer observer as a weak reference.
     * @param aMethodId Callback method identifier.
     */
    void SetPeerObserver(jobject aPeer, jmethodID aMethodId);

    /**
     * Removes the Java peer observer. Deletes the stored
     * global weak reference.
     *
     * @param aJniEnv JNI environment used for deleting the
     *        stored weak reference.
     */
    void RemovePeerObserver(JNIEnv& aJniEnv);

private:  // Constructors
    /**
     * C++ constructor.
     *
     * @param aAppId View activation application id.
     * @param aNotificationId Notification id.
     * @param aEventSource Event source used for posting events from
     *        native to Java side.
     */
    CSoftNotification(TInt aAppId,
                      TInt aNotificationId,
                      CSoftNotificationEventSource& aEventSource);

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

private:  // New methods

    /**
     * Posts an event to the Java side.
     *
     * @param aEventId Id of the event to be posted.
     */
    void PostEventL(TInt aEventId);

    /**
     * Set assigned member data to custom notification parameters
     *
     * @param aParam custom soft notification params to fill
     */
    void FillNotificationParams(TAknDynamicSoftNotificationParams& aParam);

private:  // Data
    /// Own. AvKon custom soft notifier
    CAknDynamicSoftNotifier* iNotifier;
    /// View activation application id
    TUid iAppId;
    /// Notification Id
    TInt iNotificationId;
    /// Own. Softkey 1 label
    HBufC* iSoftkey1;
    /// Own. Softkey 2 label
    HBufC* iSoftkey2;
    /// Own. Note label when single dialog is shown
    HBufC* iLabel;
    /// Own. Note label when notifications are groupped
    HBufC* iGroupLabel;
    /// Own. Image data byte array
    HBufC8* iImageData;

    /// Observing Java peer object.
    jobject iPeer;
    /// Java peer callback method.
    jmethodID iMethodId;
    /// Event source instance.
    CSoftNotificationEventSource& iEventSource;
};

#endif // CSOFTNOTIFICATION_H


