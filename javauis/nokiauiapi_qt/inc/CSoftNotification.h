/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

#ifndef CSOFTNOTIFICATION_H
#define CSOFTNOTIFICATION_H

#include <e32base.h>
#include <jni.h>
#include <HbIndicatorsymbian.h>
#include <HbDeviceNotificationDialogsymbian.h>

NONSHARABLE_CLASS(CSoftNotification) : public CBase,
        MHbIndicatorSymbianObserver, MHbDeviceNotificationDialogObserver
    {
public:
  
    /**
     * Static constructor
     */
    static CSoftNotification* NewL(TInt aAppId, TInt aNotificationId);

    /**
     * Destructor.
     */
    virtual ~CSoftNotification();

public:
    // From MHbIndicatorSymbianObserver

    /**
     * Dynamic soft notification was activated by user.
     */
    void IndicatorUserActivated(const TDesC &aType,
            CHbSymbianVariantMap &aData);
    
    /**
     * Notification dialog is touched by the user.
     *
     * @param aDialog Notification dialog
     */
    void NotificationDialogActivated(const CHbDeviceNotificationDialogSymbian* aDialog);
    
    /**
     * Notification dialog is closed.
     *
     * @param aDialog Notification dialog
     * @param aCompletionCode Notification dialog completion code
     */
    void NotificationDialogClosed(const CHbDeviceNotificationDialogSymbian* aDialog, TInt aCompletionCode);

public:
    // New functions

    /**
     * Add a custom soft notification. 
     */
    bool ShowSoftNotificationL();

    /**
     * Cancels custom soft notification.
     */
    void RemoveSoftNotificationL();

    /**
     * Set a text for a soft notification.
     *
     * @param aSingularText singular text for soft notification
     * @param aPluralText plural text for soft notification
     */
    void SetTextL(const TDesC& aPrimaryText, const TDesC& aSecondaryText);
    
    /**
     * Set an image path for a soft notification.
     *
     * @param aImagePath image path for soft notification
     */
    void SetImagePathL(const TDesC& aImagePath);

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
     * @param aPeer Java peer observer
     * @param aMethodId Callback method identifier.
     * @param aJniEnv JNI environment
     */
    void SetPeerObserver(jobject aPeer, jmethodID aMethodId, JNIEnv* aJniEnv);

private:
    // Constructors
    /**
     * C++ constructor.
     *
     * @param aAppId View activation application id.
     * @param aNotificationId Notification id.
     */
    CSoftNotification(TInt aAppId, TInt aNotificationId);

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

private:
    // New methods

    /**
     * Set assigned member data to custom notification parameters
     *
     * @return aParam custom soft notification params to fill
     */
    CHbSymbianVariant* NotificationParamsL();
    
    /**
     * Storing softnotification to database
     */
    void AddorUpdateSoftNotificationToStorage();

private:
    // Data

    CHbIndicatorSymbian* iIndicator;
    CHbDeviceNotificationDialogSymbian* iNotificationDialog; 
    TUid iMidletId;
    TInt iNotificationId;
    HBufC* iPrimaryText;
    HBufC* iSecondaryText;
    HBufC* iImagePath;
    jobject iPeer;
    jmethodID iMethodId;
    JNIEnv* iJniEnv;
    bool iIsNoteExist;
    };

#endif // CSOFTNOTIFICATION_H

