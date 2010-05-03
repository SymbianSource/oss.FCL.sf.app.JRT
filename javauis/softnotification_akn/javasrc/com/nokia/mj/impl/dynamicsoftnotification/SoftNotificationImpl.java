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
* Description:  Implementation class of SoftNotification API.
 *
*/

package com.nokia.mj.impl.dynamicsoftnotification;

// IMPORTS
import com.nokia.mid.ui.SoftNotificationException;
import com.nokia.mid.ui.SoftNotificationListener;
import com.nokia.mj.impl.rt.legacy.MIDEventServer;
import com.nokia.mj.impl.rt.legacy.LegacyRtPort;
import com.nokia.mj.impl.rt.legacy.NativeError;

import com.nokia.mj.impl.rt.support.Finalizer;


/**
 * SoftNotificationImpl implements the functionality specified in
 * com.nokia.mid.ui.SoftNotification class.
 */
public class SoftNotificationImpl extends com.nokia.mid.ui.SoftNotification
{
    // Native CSoftNotificationEventSource handle
    private int iEventSourceHandle;

    // Native CSoftNotification handle
    private int iSoftNotificationHandle = NativeError.KErrNotFound;

    // The name of the Notification event server
    private final static String NOTIF_EVENT_SERVER_NAME = "java-softnotification";

    // Notification event server
    private final static MIDEventServer sEventServer;

    // Observer of this class set by the API user. An array is used to allow
    // listener synchronization.
    private SoftNotificationListener[] iListener;

    // Listener event when user accepted notification
    private final static int EVENT_ACCEPT = 1;

    // Listener event when user dismissed notification
    private final static int EVENT_DISMISS = 2;

    // Error message prefix for exceptions.
    private final static String ERROR_MESSAGE_PREFIX = "Failed to ";


    // The class will have native resources so register it for
    // finalization.
    private Finalizer mFinalizer = new Finalizer()
    {
        public void finalizeImpl()
        {
            doFinalize();
        }
    };

    // Static block of the class.
    static
    {
        com.nokia.mj.impl.rt.support.Jvm.loadSystemLibrary("javasoftnotification");
        sEventServer = new MIDEventServer(NOTIF_EVENT_SERVER_NAME);
    }


    /**
     * Constructor. New instance with old identifier.
     *
     * @param aNotificationId Identification of previous soft notification.
     */
    public SoftNotificationImpl(int aNotificationId)
    {
        initialize(aNotificationId);
    }

    /**
     * Constructor. New instance.
     */
    public SoftNotificationImpl()
    {
        initialize(NativeError.KErrNotFound);
    }

    /**
     * Initializes the instance.
     *
     * @param aNotificationId Id of the soft notification.
     */
    protected void initialize(int aNotificationId)
    {
        // Create array for the listener. Array is used to allow listener
        // synchronization.
        iListener = new SoftNotificationListener[ 1 ];

        // Create native event source.
        iEventSourceHandle = _createEventSource(sEventServer.getHandle());

        if (iEventSourceHandle <= NativeError.KErrNone)
        {
            throw new OutOfMemoryError();
        }

        // Create the native peer for this class.
        iSoftNotificationHandle = _createNativePeer(
                                      iEventSourceHandle, LegacyRtPort.getMidletUid(), aNotificationId);

        if (iSoftNotificationHandle <= NativeError.KErrNone)
        {
            // Native construction failed.
            throw new OutOfMemoryError();
        }
    }

    /**
     * Called when the object is finalized by the garbage collector.
     */
    private void doFinalize()
    {
        if (mFinalizer != null)
        {
            _destroy(iEventSourceHandle, iSoftNotificationHandle);
            mFinalizer = null;
        }
    }

    /**
     * Notification callback, called from the native side.
     *
     * @param aEventArg Occurred event.
     */
    private void notificationCallback(int aEventArg)
    {
        // Synchronize the listener usage since the user may want to set it
        // to null during execution of this function.
        synchronized (iListener)
        {
            SoftNotificationListener listener = iListener[ 0 ];

            if (listener != null)
            {
                if (aEventArg == EVENT_ACCEPT)
                {
                    listener.notificationSelected(this);
                }
                else if (aEventArg == EVENT_DISMISS)
                {
                    listener.notificationDismissed(this);
                }
            }
        }
    }

    /**
     * Checks the given error value. Throws SoftNotificationException
     * if the given error value is other than NativeError.KErrNone.
     *
     * @param aError Error value to be checked.
     * @param aErrorMessage Message to be included in the exception.
     */
    private final void checkError(int aError, String aErrorMessage)
    throws SoftNotificationException
    {
        if (aError != NativeError.KErrNone)
        {
            throw new SoftNotificationException(
                ERROR_MESSAGE_PREFIX + aErrorMessage, aError);
        }
    }

    // Functions from the base class.

    /**
     * See class SoftNotification for comments
     */
    public int getId()
    {
        return _getId(iEventSourceHandle, iSoftNotificationHandle);
    }

    /**
     * See class SoftNotification for comments
     */
    public void post() throws SoftNotificationException
    {
        int err = _showSoftNotification(
                      iEventSourceHandle,
                      iSoftNotificationHandle);

        checkError(err, "add soft notification");
    }

    /**
     * See class SoftNotification for comments
     */
    public void remove() throws SoftNotificationException
    {
        int err = _removeSoftNotification(
                      iEventSourceHandle,
                      iSoftNotificationHandle);

        checkError(err, "remove notification");
    }

    /**
     * See class SoftNotification for comments
     */
    public void setListener(SoftNotificationListener aListener)
    {
        // Synchronize the listener setting since the user may set it
        // to null during it is used elsewhere in this class.
        synchronized (iListener)
        {
            iListener[ 0 ] = aListener;
        }
    }

    /**
     * See class SoftNotification for comments
     */
    public void setText(String aText, String aGroupText)
    throws SoftNotificationException
    {
        int err = _setText(
                      iEventSourceHandle,
                      iSoftNotificationHandle,
                      aText,
                      aGroupText);

        checkError(err, "set note text");
    }

    /**
     * See class SoftNotification for comments
     */
    public void setSoftkeyLabels(String aSoftkey1Label, String aSoftkey2Label)
    throws SoftNotificationException
    {
        int err = _setSoftkeyLabels(
                      iEventSourceHandle,
                      iSoftNotificationHandle,
                      aSoftkey1Label,
                      aSoftkey2Label);

        checkError(err, "set softkeys");
    }

    /**
     * See class SoftNotification for comments
     */
    public void setImage(byte[] aImageData) throws SoftNotificationException
    {
        int err = _setImage(
                      iEventSourceHandle,
                      iSoftNotificationHandle,
                      aImageData);

        checkError(err, "set image");
    }


    // Native methods.

    /**
     * Creates a native side event source.
     *
     * @param aEventServerHandle Handle to the native event server.
     * @return Handle to the native event source.
     */
    private native int _createEventSource(int aEventServerHandle);

    /**
     * Creates native side peer of this Java class.
     *
     * @param aEventSourceHandle Handle to the native event source.
     * @param aMIDletID Id of the midlet.
     * @param aNotificationId Id of the notification.
     * @return Handle to the native side peer; negative value in the case of
     *         and error.
     */
    private native int _createNativePeer(
        int aEventSourceHandle,
        int aMIDletID,
        int aNotificationId);

    /**
     * Destroys native side resources owned by this class.
     *
     * @param aEventSourceHandle Handle to the native event source.
     * @param aNotificationHandle Handle to the native side peer object.
     */
    private native void _destroy(
        int aEventSourceHandle,
        int aNotificationHandle);

    /**
     * Shows the soft notification for the user.
     *
     * @param aEventSourceHandle Handle to the native event source.
     * @param aSoftNotificationHandle Handle to the native side peer object.
     * @return NativeError.KErrNone if no errors occurred during the operation;
     *         otherwise some error value is returned.
     */
    private native int _showSoftNotification(
        int aEventSourceHandle,
        int aSoftNotificationHandle);

    /**
     * Removes the soft notification.
     *
     * @param aEventSourceHandle Handle to the native event source.
     * @param aSoftNotificationHandle Handle to the native side peer object.
     * @return NativeError.KErrNone if no errors occurred during the operation;
     *         otherwise some error value is returned.
     */
    private native int _removeSoftNotification(
        int aEventSourceHandle,
        int aSoftNotificationHandle);

    /**
     * Sets the text field to be displayed in the soft notification pop-up.
     *
     * @param aEventSourceHandle Handle to the native event source.
     * @param aSoftNotificationHandle Handle to the native side peer object.
     * @param aText A text to be displayed in soft notification when ungrouped
     *        soft notification is shown.
     * @param aGroupText A text be to displayed in soft notification when there
     *        are also other notifications visible (grouped form).
     * @return NativeError.KErrNone if no errors occurred during the operation;
     *         otherwise some error value is returned.
     */
    private native int _setText(
        int aEventSourceHandle,
        int aSoftNotificationHandle,
        String aText,
        String aGroupText);

    /**
     * Sets the textual labels for the softkeys.
     *
     * @param aEventSourceHandle Handle to the native event source.
     * @param aSoftNotificationHandle Handle to the native side peer object.
     * @param aSoftKey1Label Text for the softkey 1.
     * @param aSoftKey2Label Text for the softkey 2.
     * @return NativeError.KErrNone if no errors occurred during the operation;
     *         otherwise some error value is returned.
     */
    private native int _setSoftkeyLabels(
        int aEventSourceHandle,
        int aSoftNotificationHandle,
        String aSoftkey1Label,
        String aSoftkey2Label);

    /**
     * Sets the image to be displayed in soft notification.
     *
     * @param aEventSourceHandle Handle to the native event source.
     * @param aSoftNotificationHandle Handle to the native side peer object.
     * @param aImageData Data of the image to be set.
     * @return NativeError.KErrNone if no errors occurred during the operation;
     *         otherwise some error value is returned.
     */
    private native int _setImage(
        int aEventSourceHandle,
        int aSoftNotificationHandle,
        byte[] aImageData);

    /**
     * Returns the id of the soft notification.
     *
     * @param aEventSourceHandle Handle to the native event source.
     * @param aSoftNotificationHandle Handle to the native side peer object.
     * @return Id of the soft notification.
     */
    private native int _getId(
        int aEventSourceHandle,
        int aSoftNotificationHandle);
}
