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
* Description:
*
*/

package com.nokia.mid.ui;

import org.eclipse.swt.widgets.Internal_PackageSupport;
import com.nokia.mid.ui.internal.OS;


/**
 * This class is a Java extension to support tactile feedback (audio, vibra,
 * piezo, etc.) on touch-enabled devices. Tactile feedback is meant to give user
 * more clear feeling of touch events, like tapping and dragging. The actual
 * physical tactile feedback depends on the device tactile feedback
 * implementation, hardware and device settings. A device may implement the
 * feedback with different hardware methods, for example, vibra, audio or piezo
 * hardware.
 * <p>
 * The API supports both LCDUI and eSWT UI toolkits.
 * <p>
 * Using this class is safe when a device does not support tactile feedback. In
 * this case methods of this class can be called but this has no effect.
 * <p>
 * Tactile feedback can be generated in two ways:
 * <ul>
 * <li>By producing direct feedback from the application.</li>
 * <li>By adding feedback areas to area registry, in which case the feedback
 * will be produced by the tactile feedback system automatically when the
 * defined screen area (with defined feedback) is touched.</li>
 * </ul>
 * <p>
 * Direct feedback is given instantly with {@link #directFeedback
 * directFeedback} specifying the feedback type. Direct feedback can be used
 * when the user action is not just a tap in a certain area. For example if the
 * user drags from one menu item to the other the application may give direct
 * feedback to emphasize focus traverse.
 * <p>
 * In case of area registry for eSWT Control, the registry needs to be updated
 * every time when the size or position of a UI component changes, or when e.g.
 * a state of a UI component changes in such way that feedback type also
 * changes. Typically the area update may be implemented in
 * <code>controlMoved</code> and <code>controlResized</code> methods of
 * <code>ControlListener</code> interface.
 * <p>
 * In case of area registry for LCDUI Canvas or CustomItem no further actions
 * are required to update tactile feedback areas when a CustomItem is moved in a
 * Form or when a Canvas is changed due to e.g. orientation switch.
 * <p>
 * There are quite tight latency requirements for tactile feedback, and in most
 * cases feedback should be triggered in less than 30ms from the time when user
 * touched the screen. For the area registry the feedback system is responsible
 * of fulfilling this requirement, but in case of direct feedback it is the
 * responsibility of the user of this API.
 * <p>
 * The tactile feedback has 2 different styles: Basic and Sensitive. Each type
 * is defined in the device settings, not by this API. Each style of feedback is
 * used in different cases. Typically Basic style is used to emphasize the tap
 * event, while Sensitive style is used mostly at dragging operations, to
 * emphasize focus traverse, text selection, etc. Choosing a specific style for
 * each use case in up to developer, however it is good to follow the behavior
 * of native applications.
 * <p>
 * The goal of the API is to enable user-created UI components to have tactile
 * feedback. The UI component types supported by the API are limited to
 * user-defined components only: <code>javax.microedition.lcdui.Canvas</code>,
 * <code>javax.microedition.lcdui.CustomItem</code>,
 * <code>org.eclipse.swt.widgets.Control</code>. Other UI components (i.e. List,
 * TextBox, etc.) get default tactile feedback automatically from the platform.
 *
 * @version 0.001
 * @since 1.2
 */

public class TactileFeedback
{

    /**
     * Constant for specifying basic tactile feedback style. Actual style
     * behaviour is set through device settings.
     */
    public static final int FEEDBACK_STYLE_BASIC = 1;

    /**
     * Constant for specifying sensitive tactile feedback style. Actual style
     * behaviour is set through device settings.
     */
    public static final int FEEDBACK_STYLE_SENSITIVE = 2;

    private final String invalidFeedbackStyleMsg = "Invalid feedback style specified";
    private final String invalidControlTypeMsg = "Invalid object provided for tactile feedback registration";

    private static final int TYPE_INVALID = 0;
    private static final int TYPE_ESWT = 1;
    private static final int TYPE_LCDUI = 2;
    private static boolean feedbackEnabled;

    /**
     * Constructs tactile feedback engine object. The object may be used in both
     * LCDUI and eSWT java UI toolkits. TactileFeedback object must be
     * constructed before using any tactile feedback methods.
     */
    public TactileFeedback()
    {
    }

    /**
     * Triggers direct tactile feedback of the specified style.
     *
     * @param style
     *            The style of the direct feedback. Use predefined values
     *            FEEDBACK_STYLE_BASIC, FEEDBACK_STYLE_SENSITIVE. Actual style
     *            behavior is set through device settings.
     * @throws IllegalArgumentException
     *             if the style parameter has invalid type.
     */
    public void directFeedback(int style) throws IllegalArgumentException
    {
        if ((style != FEEDBACK_STYLE_BASIC)
                && (style != FEEDBACK_STYLE_SENSITIVE))
            throw new IllegalArgumentException(invalidFeedbackStyleMsg);
        if (org.eclipse.swt.widgets.Display.getCurrent() == null)
        {
            final int fStyle = style;
            com.nokia.mj.impl.nokialcdui.LCDUIInvoker
            .eSWTUIThreadRunnerSyncExec(new Runnable()
            {
                public void run()
                {
                    OS.MTouchFeedback_InstantFeedback(fStyle);
                }
            });

        }
        else
        {
            OS.MTouchFeedback_InstantFeedback(style);
        }
    }

    /**
     * Queries the device if it supports tactile feedback. It is safe to use
     * this class even if the device does not support tactile feedback. Methods
     * of this class can be called but this has no effect.
     *
     * @return true if the device supports tactile feedback, false otherwise.
     */
    public boolean isTouchFeedbackSupported()
    {
        if (org.eclipse.swt.widgets.Display.getCurrent() == null)
        {
            com.nokia.mj.impl.nokialcdui.LCDUIInvoker
            .eSWTUIThreadRunnerSyncExec(new Runnable()
            {

                public void run()
                {
                    feedbackEnabled = OS
                                      .MTouchFeedback_TouchFeedbackSupported();
                }
            });
        }
        else
        {
            feedbackEnabled = OS.MTouchFeedback_TouchFeedbackSupported();
        }
        return feedbackEnabled;
    }

    /**
     * Registers area within a UI component for tactile feedback. If areas
     * overlap then the feedback is given from the last registered area.
     * {@link #moveFeedbackAreaToFirstPriority moveFeedbackAreaToFirstPriority}
     * can be used to bring a certain area covered by another area into action.
     * If an area with specified <code>id</code> is already registered it is
     * updated. When updating an existing area there is no need to unregister it
     * before re-registering again with new coordinates and/or feedback style.
     * The area specified by <code>x</code>, <code>y</code>, <code>width</code>
     * and <code>height</code> parameters may be located on the screen just
     * partially.
     *
     * <p>
     * In case of area registry for eSWT Control, the registry needs to be
     * updated every time when the size or position of a UI component changes,
     * or when e.g. a state of a UI component changes in such way that feedback
     * type also changes. Typically the area update may be implemented in
     * <code>controlMoved</code> and <code>controlResized</code> methods of
     * <code>ControlListener</code> interface. The update of an area may be
     * implemented by calling registerFeedbackArea with the same area id but new
     * coordinates and/or feedback style.
     * <p>
     * In case of area registry for LCDUI Canvas or CustomItem no further
     * actions are required to update tactile feedback areas when a CustomItem
     * is moved in a Form or when a Canvas is changed due to e.g. orientation
     * switch.
     *
     * @param uiObject
     *            The UI component for tactile feedback registration. Valid
     *            object types are: <code>javax.microedition.lcdui.Canvas</code>
     *            , <code>javax.microedition.lcdui.CustomItem</code>,
     *            <code>org.eclipse.swt.widgets.Control</code>.
     * @param id
     *            Id of the new tactile feedback area to be registered. Id's are
     *            used to identify particular tactile feedback area within one
     *            UI component. Id's do not need to be consecutive numbers.
     * @param x
     *            x-coordinate of the top-left corner of tactile feedback
     *            rectangle to register.
     * @param y
     *            y-coordinate of the top-left corner of tactile feedback
     *            rectangle to register.
     * @param width
     *            Width of tactile feedback rectangle to register.
     * @param height
     *            Height of tactile feedback rectangle to register.
     * @param style
     *            The style of the feedback for specified area. Use predefined
     *            values <code>FEEDBACK_STYLE_BASIC</code>,
     *            <code>FEEDBACK_STYLE_SENSITIVE</code>. Actual style behaviour
     *            is defined through device settings.
     * @throws IllegalArgumentException
     *             if the uiObject parameter has invalid type.
     * @throws IllegalArgumentException
     *             if the style parameter has invalid type.
     */
    public void registerFeedbackArea(Object uiObject, int id, int x, int y,
                                     int width, int height, int style) throws IllegalArgumentException
    {
        int type = controlType(uiObject);
        if (type == TYPE_INVALID)
            throw new IllegalArgumentException(invalidControlTypeMsg);

        if ((style != FEEDBACK_STYLE_BASIC)
                && (style != FEEDBACK_STYLE_SENSITIVE))
            throw new IllegalArgumentException(invalidFeedbackStyleMsg);

        int controlHandle = getControlHandle(uiObject);
        if (type == TYPE_LCDUI)
        {
            final int fControlHandle = controlHandle;
            final int fId = id;
            final int fX = x;
            final int fY = y;
            final int fWidth = width;
            final int fHeight = height;
            final int fStyle = style;
            com.nokia.mj.impl.nokialcdui.LCDUIInvoker
            .eSWTUIThreadRunnerSyncExec(new Runnable()
            {

                public void run()
                {
                    OS.MTouchFeedback_SetFeedbackArea(fControlHandle,
                                                      fId, fX, fY, fWidth, fHeight, fStyle);
                }
            });
        }
        else
        {
            OS.MTouchFeedback_SetFeedbackArea(controlHandle, id, x, y, width,
                                              height, style);
        }
    }

    /**
     * Unregisters tactile feedback area within a UI component.
     *
     * @param uiObject
     *            The UI component for tactile feedback area de-registration.
     *            Valid object types are:
     *            <code>javax.microedition.lcdui.Canvas</code>,
     *            <code>javax.microedition.lcdui.CustomItem</code>,
     *            <code>org.eclipse.swt.widgets.Control</code>.
     * @param id
     *            Id of the tactile feedback area to be unregistered. Id's are
     *            used to identify particular tactile feedback area within one
     *            UI component. If given id was not registered by
     *            {@link #registerFeedbackArea registerFeedbackArea} then the
     *            call has no effect.
     * @throws IllegalArgumentException
     *             if the uiObject parameter has invalid type.
     */
    public void unregisterFeedbackArea(Object uiObject, int id)
    throws IllegalArgumentException
    {

        int type = controlType(uiObject);
        if (type == TYPE_INVALID)
            throw new IllegalArgumentException(invalidControlTypeMsg);

        int controlHandle = getControlHandle(uiObject);
        if (type == TYPE_LCDUI)
        {
            final int fControlHandle = controlHandle;
            final int fId = id;
            com.nokia.mj.impl.nokialcdui.LCDUIInvoker
            .eSWTUIThreadRunnerSyncExec(new Runnable()
            {

                public void run()
                {
                    OS.MTouchFeedback_RemoveFeedbackArea(
                        fControlHandle, fId);
                }
            });
        }
        else
        {
            OS.MTouchFeedback_RemoveFeedbackArea(controlHandle, id);
        }

    }

    /**
     * Removes all tactile feedback for a UI component.
     *
     * @param uiObject
     *            The UI component for tactile feedback area de-registration.
     *            Valid object types are:
     *            <code>javax.microedition.lcdui.Canvas</code>,
     *            <code>javax.microedition.lcdui.CustomItem</code>,
     *            <code>org.eclipse.swt.widgets.Control</code>.
     * @throws IllegalArgumentException
     *             if the uiObject parameter has invalid type.
     */
    public void removeFeedbackForComponent(Object uiObject)
    {
        if (isTouchFeedbackSupported() == false)
            return;

        int type = controlType(uiObject);
        if (type == TYPE_INVALID)
            throw new IllegalArgumentException(invalidControlTypeMsg);

        int controlHandle = getControlHandle(uiObject);

        if (type == TYPE_LCDUI)
        {
            final int fControlHandle = controlHandle;
            com.nokia.mj.impl.nokialcdui.LCDUIInvoker
            .eSWTUIThreadRunnerSyncExec(new Runnable()
            {

                public void run()
                {
                    OS.MTouchFeedback_RemoveFeedbackForControl(fControlHandle);
                }
            });
        }
        else
        {
            OS.MTouchFeedback_RemoveFeedbackForControl(controlHandle);
        }

    }

    /**
     * Moves the specified tactile feedback area to first priority. Priority is
     * significant in case of overlapping tactile feedback areas. If the tactile
     * feedback styles of registered areas are different, the feedback from
     * first-priority area will be given.
     *
     * @param uiObject
     *            The UI component for tactile feedback area de-registration.
     *            Valid object types are:
     *            <code>javax.microedition.lcdui.Canvas</code>,
     *            <code>javax.microedition.lcdui.CustomItem</code>,
     *            <code>org.eclipse.swt.widgets.Control</code>.
     * @param id
     *            Id of the tactile feedback area to be unregistered. Id's are
     *            used to identify particular tactile feedback area within one
     *            UI component. If given id was not registered by
     *            {@link #registerFeedbackArea registerFeedbackArea} then the
     *            call has no effect.
     * @throws IllegalArgumentException
     *             if the uiObject parameter has invalid type.
     */
    public void moveFeedbackAreaToFirstPriority(Object uiObject, int id)
    {
        if (isTouchFeedbackSupported() == false)
            return;

        int type = controlType(uiObject);
        if (type == TYPE_INVALID)
            throw new IllegalArgumentException(invalidControlTypeMsg);

        int controlHandle = getControlHandle(uiObject);
        if (type == TYPE_LCDUI)
        {
            final int fControlHandle = controlHandle;
            final int fId = id;
            com.nokia.mj.impl.nokialcdui.LCDUIInvoker
            .eSWTUIThreadRunnerSyncExec(new Runnable()
            {

                public void run()
                {
                    OS.MTouchFeedback_MoveFeedbackAreaToFirstPriority(
                        fControlHandle, fId);
                }
            });
        }
        else
        {
            OS.MTouchFeedback_MoveFeedbackAreaToFirstPriority(
                controlHandle, id);
        }

    }


    private int controlType(Object obj)
    {
        if ((obj instanceof javax.microedition.lcdui.Canvas)
                || (obj instanceof javax.microedition.lcdui.CustomItem))
        {
            return TYPE_LCDUI;
        }
        else if ((obj instanceof org.eclipse.swt.widgets.Control))
        {
            return TYPE_ESWT;
        }
        return TYPE_INVALID;
    }

    private int getControlHandle(Object uiObject)
    {
        int controlHandle = 0;
        org.eclipse.swt.widgets.Control eSwtControl = null;
        if (uiObject instanceof javax.microedition.lcdui.Canvas
                || uiObject instanceof javax.microedition.lcdui.CustomItem)
        {
            eSwtControl = com.nokia.mj.impl.nokialcdui.LCDUIInvoker
                          .getEswtControl(uiObject);
        }
        else if (uiObject instanceof org.eclipse.swt.widgets.Control)
        {
            eSwtControl = (org.eclipse.swt.widgets.Control) uiObject;
        }

        if (eSwtControl != null)
        {
            controlHandle = Internal_PackageSupport.topHandle(eSwtControl);
        }
        return controlHandle;
    }

}
