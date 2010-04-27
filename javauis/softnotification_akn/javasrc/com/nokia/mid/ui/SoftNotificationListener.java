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
* Description:  Listener interface of the SoftNotification API.
 *
*/


// PACKAGE
package com.nokia.mid.ui;

/**
 * <code>SoftNotificationListener</code> interface is used by applications which
 * need callbacks from soft notifications. Listener will get a notification when the
 * soft notification is either selected or dismissed by the user. Listener
 * interface can be registered with
 * {@link SoftNotification#setListener(SoftNotificationListener)} method.
 *
 * @see SoftNotification
 */
public interface SoftNotificationListener
{

    /**
     * Indicates that a soft notification was selected by the user. The
     * platform calls {@link SoftNotification#remove()} before
     * notificationSelected() is called. Application that posted the soft
     * notification is activated before the listener will get the notification.
     *
     * @param notification A notification object identifying the soft
     *          notification being responded to by the user. The MIDlet must
     *          implement the functionality to be performed when the
     *          notification is selected.
     */
    public abstract void notificationSelected(SoftNotification notification);

    /**
     * Indicates that a soft notification was dismissed by the user. The
     * platform calls {@link SoftNotification#remove()} before
     * notificationDismissed() is called.
     *
     * @param notification A notification object identifying the soft
     *          notification being responded to by the user. The application must
     *          implement the functionality to be performed when the soft
     *          notification is dismissed, it is not required to do anything.
     */
    public abstract void notificationDismissed(SoftNotification notification);

}
