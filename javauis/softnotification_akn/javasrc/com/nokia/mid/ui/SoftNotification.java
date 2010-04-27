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
* Description:  Public interface of the SoftNotification API.
 *
*/


// PACKAGE
package com.nokia.mid.ui;

import com.nokia.mj.impl.dynamicsoftnotification.SoftNotificationImpl;

/**
 * <p>The <code>SoftNotification</code> is a class to manage soft
 * notifications. Soft notifications are reminders that inform the user of events
 * that have occurred in the user's absence, or while the user was busy with some
 * application. Text, and also graphics, can be used to communicate the message
 * to the user. Soft notifications are displayed as pop-ups in device home screen
 * (idle view).
 * Soft notifications are persistent and restarting the device does not delete
 * created soft notifications from the home screen.</p>
 *
 * <p>The user can respond to the soft notification by using the softkeys. The
 * softkey 1 is used for activating a function, for example opening a message
 * that has arrived. Application that created the soft notification will be activated
 * when the soft notification is accepted by the user. The softkey 2 is used to
 * discard the notification without taking any further action.</p>
 *
 * <p>The application that launched a soft notification can control it and also
 * discard it. It is possible to use the Applications key during a soft
 * notification; in that case, the soft notification disappears, but reappears
 * when the user returns to the device home screen, unless the application
 * responsible for the notification has discarded it.</p>
 *
 * <h3>Grouping soft notifications</h3>
 *
 * <p>Soft notifications can be displayed for the user in two different appearances:
 * <li> <b>Ungrouped</b> soft notification (left figure): These notifications
 * contain one piece of information each. In ungrouped soft notification there can
 * be one or two lines of text and an image visible.
 * Ungrouped appearance is used when there's
 * only one active soft notification or the topmost soft notification does not
 * support grouping.
 * <li> <b>Grouped</b> soft notification (right figure): If there are several
 * soft notifications on the screen and they support grouping, the notifications
 * are grouped and shown as a list of notifications.
 * Only one line of text is shown per a notification and there are no images visible.
 * The user can pick up one notification at a time and react
 * to it. If user discards the notification group, then all the shown notifications
 * are discarded and are not to be shown again.</p>
 *
 * <p><img src="images/ungrouped.jpg" alt="Image of ungrouped soft notification.">
 * <img src="images/grouped.jpg" alt="Image of grouped soft notification."></p>
 *
 * <p>Soft notifications created through this API may be grouped with other
 * platform notifications if all of them supports grouping. A soft notification
 * supports grouping if <code>groupText</code> is given in
 * {@link #setText(String text, String groupText)}</p>
 *
 * <h3>Persistent notifications</h3>
 * <p>Soft notifications are persistent meaning that when an application
 * has created a notification and set it to the home screen, the notification
 * exists in the home screen until:
 * <li>User accepts the notification,
 * <li>User dismisses the notification, or
 * <li>Application removes the notification</p>
 *
 * <p>A soft notification stays on the device home screen even if the application
 * is closed or the device is restarted.</p>
 *
 * <p>Accepting (selecting) a soft notification created by an application causes
 * this application to be activated in the following ways:
 * <li>If the application was running on the background, it will be set to
 * the foreground.
 * <li>If the application was running and had registered a listener
 * for the notification,
 * {@link SoftNotificationListener#notificationSelected(SoftNotification)}
 * is called.
 * <li>If the application is not running, it will be started and set to the
 * foreground.</p>
 *
 * <p><b>Notice</b> that accepting a notification belonging to a closed application
 * causes only the application to be started but does not
 * generate any listener events for the application. This is because there cannot
 * be any listeners registered by the closed application.
 * Thus, when an application is started by a notification, the application has
 * to decide which view is shown for the user without having the information about
 * the selected notification.</p>
 *
 * <p>Dismissing a soft notification does not bring the application to
 * the foreground or start the application neither. However, if a
 * listener has been set,
 * {@link SoftNotificationListener#notificationDismissed(SoftNotification)}
 * is called.</p>
 *
 * <p>Because soft notifications are persistent, they will remain on the home screen
 * even if the application is closed. It is possible to get access to these
 * notifications later when the application is restarted.
 * For accessing notifications later, the application has to store the IDs of
 * the notifications to a permanent memory (for example, RecordStore).
 * When the application is
 * restarted, the application may recreate those notifications by using the
 * stored IDs.</p>
 *
 * <p><b>Notice</b> that since it is possible that the user has dismissed notifications
 * when the application was closed, it is not possible to know whether the
 * recreated notification still exist on the device home screen.
 * In many cases it may be worthile to first remove all those notifications
 * from the screen and then add only the needed ones back to the screen.</p>
 *
 *
 * <h3>Example of usage</h3>
 *
 * <code style="white-space: pre">
 *class SoftNoteExample implements SoftNotificationListener
 *    {
 *    private SoftNotification iSoftNotification;
 *
 *    public SoftNoteExample()
 *        {
 *        iSoftNotification = SoftNotification.newInstance();
 *        iSoftNotification.setListener( this );
 *        }
 *
 *    public void Show1NewMail() throws SoftNotificationException
 *        {
 *        // Read image e.g. from the MIDlet JAR package or filesystem
 *        byte[] image = readImage( "mail.png" );
 *
 *        // Supports grouping since groupText is given here.
 *        iSoftNotification.setText( "You have 1 new mail", "1 new mail" );
 *        iSoftNotification.setSoftkeyLabels( "Show", "Exit" );
 *        iSoftNotification.setImage( image );
 *        iSoftNotification.post();
 *        }
 *
 *    public void notificationSelected(SoftNotification notification)
 *        {
 *        // called when user selects the soft notification
 *        }
 *
 *    public void notificationDismissed(SoftNotification notification)
 *        {
 *        // called when user dismisses the soft notification
 *        }
 *
 *    private byte[] readImage( String aImageName )
 *        {
 *        // Read image e.g. from the MIDlet JAR package or filesystem.
 *        byte[] imageData = ...
 *        //...
 *        return imageData;
 *        }
 *    }</code>
 */
public abstract class SoftNotification
{

    /**
     * Hidden default constructor.
     *
     * @see #newInstance()
     * @see #newInstance(int notificationId)
     */
    protected SoftNotification()
    {
    }

    /**
     * Returns a new instance of SoftNotification class with a reference to
     * previous soft notification instance.
     *
     * <p>Identification is used to access a soft notification created by another
     * instance of SoftNotification. If the provided identification is valid and
     * such soft notification exists, it can be updated and removed through
     * this instance. If the identification is not valid or does not exist,
     * it will be ignored and a new soft notification is created as defined in
     * {@link #newInstance()}.</p>
     *
     * @param notificationId Identification of previous soft notification.
     * @return An instance of SoftNotification class.
     * @see #getId()
     */
    public static SoftNotification newInstance(int notificationId)
    {
        return new SoftNotificationImpl(notificationId);
    }

    /**
     * <p>Returns a new instance of SoftNotification class. This instance's
     * identification will be a nonvalid negative value until
     * {@link #post()} is called.</p>
     *
     * @return an instance of SoftNotification class.
     */
    public static SoftNotification newInstance()
    {
        return new SoftNotificationImpl();
    }

    /**
     * <p>Get soft notification identification of this instance. Soft notifications are
     * persistent, so client should store the identification, if it wants to access the
     * soft notification from another instance e.g. after device is restarted.</p>
     *
     * <p><b>Notice</b> that identification might change during the lifetime of the
     * instance if {@link #post()} is called.</p>
     *
     * @return Soft notification identification.
     * @see #newInstance(int notificationId)
     */
    public abstract int getId();

    /**
     * <p>Shows a new soft notification or updates the content of previously posted
     * notification. If the application did not specify a valid identification in
     * the notification construction,
     * then new identification will be generated on first call to post(). If
     * previously posted soft notification does not exist, a new one is created
     * and a new identification will be generated. Client needs to set the attributes
     * shown in the soft notification before calling post or the default values
     * will be used. This call does not reset assigned text, image or softkeys.</p>
     *
     * <p><b>Notice</b> that identification of the soft notification may change every
     * time the function is called. So when needed, it is recommended to ask
     * the latest id from the notification by using {@link #getId()} function.</p>
     *
     * @throws SoftNotificationException Thrown if posting operation fails.
     */
    public abstract void post() throws SoftNotificationException;

    /**
     * <p>Removes the posted soft notification from the device home screen. If the
     * soft notification has not been posted or it has already been removed,
     * this call does nothing. Remove is automatically called when
     * the user accepts or dismisses the soft notification.</p>
     *
     * @throws SoftNotificationException Thrown if removing the note fails.
     */
    public abstract void remove() throws SoftNotificationException;

    /**
     * <p>Sets listener for the soft notification. Listener is notified when the
     * user accepts or dismisses the soft notification. Setting a new listener will
     * overwrite the previous listener. Listener can be removed by setting it to
     * <code>null</code>.</p>
     *
     * @param listener Listener for the soft notification.
     */
    public abstract void setListener(SoftNotificationListener listener);

    /**
     * <p>Sets the text field to be displayed in the soft notification. The
     * platform may truncate the text to a suitable size to fit into the soft
     * notification. If the text is not set before calling {@link #post()}, the
     * default value "" is used. An ungrouped soft notification has multiple lines
     * for the text, but a grouped notification has only one line.
     * Therefore the text assigned for a grouped notification should be
     * shorter. Multiple lines can be created by adding a line break
     * (character <code>'\n'</code>) inside the string.</p>
     *
     * <p>A notification supports grouping if <code>groupText</code> is
     * other than <code>null</code> or an empty string (<code>""</code>).</p>
     *
     * <p><b>Notice</b> that the modified soft notification data is not visible for
     * the user until {@link #post()} is called.</p>
     *
     * @param text A text to be displayed in the soft notification when ungrouped
     *        soft notification is shown.
     * @param groupText A text be to displayed in the soft notification when there are
     *        also other notifications visible (grouped form). The notification
     *        will not support grouping if <code>null</code> or an empty string
     *        (<code>""</code>) is used.
     *
     * @throws SoftNotificationException Thrown if setting text fails.
     */
    public abstract void setText(String text, String groupText)
    throws SoftNotificationException;

    /**
     * <p>Sets the textual labels for the softkeys. The platform may truncate the
     * text to a suitable size to fit to the softkey labels, so a relatively
     * short texts are preferred for the labels. If labels are not provided,
     * then the default softkeys will be used (Show & Exit).</p>
     *
     * <p>The functionalities of these softkeys remain always the same even if
     * their texts are changed. Softkey 1 (Show) opens
     * up the application and dismisses the soft notification. Softkey 2 (Exit)
     * dismisses the soft notification.</p>
     *
     * <p><b>Notice</b> that the modified soft notification data is not visible for
     * the user until {@link #post()} is called.</p>
     *
     * @param softkey1Label Text for the softkey 1.
     * @param softkey2Label Text for the softkey 2.
     *
     * @throws SoftNotificationException Thrown if setting softkeys fails.
     */
    public abstract void setSoftkeyLabels(String softkey1Label, String softkey2Label)
    throws SoftNotificationException;

    /**
     * <p>Sets an image to be displayed in the soft notification. The image can be
     * in any format that the platform's image decoder or SVG engine is able to
     * decode (for example, JPG, GIF, PNG, and SVG (even SVGS and MBM) images may
     * be supported in the platform). Image mask is used when available. </p>
     *
     * <p>The image is given as a byte array, for example the data may be read directly
     * from the file system or MIDlet JAR package.</p>
     *
     * <p>Images will be scaled to fit in the soft notification pop-up as the image
     * area of the notification is relatively small. For saving resources in scaling,
     * the size of the image should be kept small (preferably no larger than
     * 300x300 px).</p>
     *
     * <p>The function can also be used for removing an already set image from
     * the soft notification. This can be done by setting <code>null</code> instead
     * of any actual image.</p>
     *
     * <p><b>Notice</b> that the modified soft notification data is not visible for
     * the user until {@link #post()} is called.</p>
     *
     * @param image image to be displayed in the soft notification or
     *        <code>null</code> if any existing image should be removed
     *        from the notification.
     *
     * @throws SoftNotificationException Thrown if setting image fails.
     */
    public abstract void setImage(byte[] image) throws SoftNotificationException;
}
