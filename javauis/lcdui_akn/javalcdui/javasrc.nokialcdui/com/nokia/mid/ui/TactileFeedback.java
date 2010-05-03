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
* Description:  This class implements Tactile Feedback API.
*
*/

package com.nokia.mid.ui;

import com.nokia.mj.impl.rt.legacy.ToolkitInvoker;
import com.nokia.mj.impl.rt.legacy.ToolkitObserver;
import org.eclipse.swt.widgets.*;
import javax.microedition.lcdui.*;

/**
   This class is a Java extension to support tactile feedback (audio, vibra, piezo, etc.) on touch-enabled devices.
   Tactile feedback is meant to give user more clear feeling of touch events, like tapping and dragging.
   The actual physical tactile feedback depends on the device tactile feedback implementation,
   hardware and device settings. A device may implement the feedback with different hardware methods, for example,
   vibra, audio or piezo hardware.
   <p>
   The API supports both LCDUI and eSWT UI toolkits.
   <p>
   Using this class is safe when a device does not support tactile feedback. In this case methods of this class can be
   called but this has no effect.
   <p>
   Tactile feedback can be generated in two ways:
   <ul>
    <li>By producing direct feedback from the application.</li>
    <li>By adding feedback areas to area registry, in which case the feedback will be produced by the tactile feedback system automatically
        when the defined screen area (with defined feedback) is touched.</li>
   </ul>
   <p>
   Direct feedback is given instantly with {@link #directFeedback directFeedback} specifying the feedback type.
   Direct feedback can be used when the user action is not just a tap in a certain area.
   For example if the user drags from one menu item to the other the application may give direct feedback to emphasize focus traverse.
   <p>
   In case of area registry for eSWT Control, the registry needs to be updated every time
   when the size or position of a UI component changes, or when e.g. a state of a UI component changes in
   such way that feedback type also changes. Typically the area update may be implemented in
   <code>controlMoved</code> and <code>controlResized</code> methods of <code>ControlListener</code> interface.
   <p>
   In case of area registry for LCDUI Canvas or CustomItem no further actions are required to update tactile feedback areas
   when a CustomItem is moved in a Form or when a Canvas is changed due to e.g. orientation switch.
   <p>
   There are quite tight latency requirements for tactile feedback,
   and in most cases feedback should be triggered in less than 30ms from the time when user touched the screen.
   For the area registry the feedback system is responsible of fulfilling this requirement,
   but in case of direct feedback it is the responsibility of the user of this API.
   <p>
   The tactile feedback has 2 different styles: Basic and Sensitive. Each type is defined in the device settings, not by this API.
   Each style of feedback is used in different cases. Typically Basic style is used to emphasize the tap event,
   while Sensitive style is used mostly at dragging operations, to emphasize focus traverse, text selection, etc.
   Choosing a specific style for each use case in up to developer, however it is good to follow the behavior of native applications.
   <p>
   The goal of the API is to enable user-created UI components to have tactile feedback.
   The UI component types supported by the API are limited to user-defined components only:
   <code>javax.microedition.lcdui.Canvas</code>, <code>javax.microedition.lcdui.CustomItem</code>, <code>org.eclipse.swt.widgets.Control</code>.
   Other UI components (i.e. List, TextBox, etc.) get default tactile feedback automatically from the platform.

   @version 0.001
   @since 1.2
 */

public class TactileFeedback
{

    /**
            Constant for specifying basic tactile feedback style. Actual style behaviour is set through device settings.
    */
    public static final int FEEDBACK_STYLE_BASIC = 1;

    /**
            Constant for specifying sensitive tactile feedback style. Actual style behaviour is set through device settings.
    */
    public static final int FEEDBACK_STYLE_SENSITIVE = 2;


    // handle to native toolkit
    private int iToolkitHandle;

    // handle to native MTouchFeedback
    private int iNativeTFInterface;

    // used for calling Toolkit methods
    private ToolkitInvoker iToolkitInvoker;

    // toolkit, stored as object as we don't have access to Toolkit class.
    private Object iToolkit;

    private final String tactileNotSupportedMsg  = "Tactile feedback not supported";
    private final String invalidFeedbackStyleMsg = "Invalid feedback style specified";
    private final String invalidControlTypeMsg   = "Invalid object provided for tactile feedback registration";

    private static final int TYPE_CANVAS     = 1;
    private static final int TYPE_CUSTOMITEM = 2;
    private static final int TYPE_CONTROL    = 3;

    /**
            Constructs tactile feedback engine object. The object may be used in both LCDUI and eSWT java UI toolkits.
          TactileFeedback object must be constructed before using any tactile feedback methods.
    */
    public TactileFeedback()
    {
        iToolkit = null;
        int nativeHandle = 0;

        if (_isESWT())  // check if eSWT or LCDUI midlet is running
        {
            nativeHandle =  _createMTouchFeedbackESWT(iToolkitHandle);
        }
        else
        {
            iToolkitInvoker = ToolkitInvoker.getToolkitInvoker();
            iToolkit = iToolkitInvoker.getToolkit();
            iToolkitHandle = iToolkitInvoker.toolkitGetHandle(iToolkit);

            nativeHandle =  _createMTouchFeedbackLCDUI(iToolkitHandle);
        }

        iNativeTFInterface = nativeHandle;
    }

    /**
            Triggers direct tactile feedback of the specified style.
            @param style The style of the direct feedback. Use predefined values FEEDBACK_STYLE_BASIC, FEEDBACK_STYLE_SENSITIVE.
                         Actual style behavior is set through device settings.
            @throws IllegalArgumentException if the style parameter has invalid type.
    */
    public void directFeedback(int style) throws IllegalArgumentException
    {
        if (iNativeTFInterface == 0) return;
        if ((style != FEEDBACK_STYLE_BASIC) &&
                (style != FEEDBACK_STYLE_SENSITIVE)
           ) throw new IllegalArgumentException(invalidFeedbackStyleMsg);

        _doDirectFeedback(iNativeTFInterface, style);
    }

    /**
            Queries the device if it supports tactile feedback. It is safe to use this class even if the device does not support tactile feedback.
            Methods of this class can be called but this has no effect.
            @return true if the device supports tactile feedback, false otherwise.

    */
    public boolean isTouchFeedbackSupported()
    {
        if (iNativeTFInterface == 0) return false;
        return _isTouchFeedbackSupported(iNativeTFInterface);
    }

    /**
            Registers area within a UI component for tactile feedback.
            If areas overlap then the feedback is given from the last registered area.
          {@link #moveFeedbackAreaToFirstPriority moveFeedbackAreaToFirstPriority} can be used to bring a certain
          area covered by another area into action.
          If an area with specified <code>id</code> is already registered it is updated.
          When updating an existing area there is no need to unregister it before re-registering again with new coordinates and/or feedback style.
          The area specified by <code>x</code>, <code>y</code>, <code>width</code> and <code>height</code> parameters may be located on the screen just partially.

          <p>
        In case of area registry for eSWT Control, the registry needs to be updated every time
        when the size or position of a UI component changes, or when e.g. a state of a UI component changes in
        such way that feedback type also changes. Typically the area update may be implemented in
        <code>controlMoved</code> and <code>controlResized</code> methods of <code>ControlListener</code> interface.
        The update of an area may be implemented by calling registerFeedbackArea with the same area id but new coordinates and/or feedback style.
        <p>
        In case of area registry for LCDUI Canvas or CustomItem no further actions are required to update tactile feedback areas
        when a CustomItem is moved in a Form or when a Canvas is changed due to e.g. orientation switch.


            @param uiObject The UI component for tactile feedback registration. Valid object types are: <code>javax.microedition.lcdui.Canvas</code>,
                   <code>javax.microedition.lcdui.CustomItem</code>, <code>org.eclipse.swt.widgets.Control</code>.
            @param id Id of the new tactile feedback area to be registered. Id's are used to identify particular tactile feedback area within one UI component.
                   Id's do not need to be consecutive numbers.
            @param x x-coordinate of the top-left corner of tactile feedback rectangle to register.
            @param y y-coordinate of the top-left corner of tactile feedback rectangle to register.
            @param width Width of tactile feedback rectangle to register.
            @param height Height of tactile feedback rectangle to register.
            @param style The style of the feedback for specified area. Use predefined values <code>FEEDBACK_STYLE_BASIC</code>, <code>FEEDBACK_STYLE_SENSITIVE</code>.
                   Actual style behaviour is defined through device settings.
            @throws IllegalArgumentException if the uiObject parameter has invalid type.
            @throws IllegalArgumentException if the style parameter has invalid type.
    */
    public void registerFeedbackArea(Object uiObject, int id, int x, int y, int width, int height, int style) throws IllegalArgumentException
    {
        if (iNativeTFInterface == 0) return;

        if (!isValidControlType(uiObject))   throw new IllegalArgumentException(invalidControlTypeMsg);

        if ((style != FEEDBACK_STYLE_BASIC) &&
                (style != FEEDBACK_STYLE_SENSITIVE)
           ) throw new IllegalArgumentException(invalidFeedbackStyleMsg);

        int controlHandle = getControlHandle(uiObject);
        int controlType = getControlType(uiObject);

        _doRegisterFeedback(iNativeTFInterface,
                            controlHandle,
                            controlType,
                            id,
                            x, y, width, height,
                            style);
    }

    /**
            Unregisters tactile feedback area within a UI component.

            @param uiObject The UI component for tactile feedback area de-registration.
                            Valid object types are: <code>javax.microedition.lcdui.Canvas</code>,
                            <code>javax.microedition.lcdui.CustomItem</code>,
                            <code>org.eclipse.swt.widgets.Control</code>.

            @param id Id of the tactile feedback area to be unregistered.
                      Id's are used to identify particular tactile feedback area within one UI component.
                      If given id was not registered by {@link #registerFeedbackArea registerFeedbackArea}
                  then the call has no effect.
            @throws IllegalArgumentException if the uiObject parameter has invalid type.
    */
    public void unregisterFeedbackArea(Object uiObject, int id) throws IllegalArgumentException
    {
        if (iNativeTFInterface == 0) return;

        if (!isValidControlType(uiObject))   throw new IllegalArgumentException(invalidControlTypeMsg);

        int controlHandle = getControlHandle(uiObject);
        int controlType = getControlType(uiObject);

        _doRemoveFeedback(iNativeTFInterface,
                          controlHandle,
                          controlType,
                          id);

    }

    /**
            Removes all tactile feedback for a UI component.
            @param uiObject The UI component for tactile feedback area de-registration.
                            Valid object types are: <code>javax.microedition.lcdui.Canvas</code>,
                            <code>javax.microedition.lcdui.CustomItem</code>,
                            <code>org.eclipse.swt.widgets.Control</code>.

            @throws IllegalArgumentException if the uiObject parameter has invalid type.
    */
    public void removeFeedbackForComponent(Object uiObject)
    {
        if (iNativeTFInterface == 0) return;

        if (!isValidControlType(uiObject))   throw new IllegalArgumentException(invalidControlTypeMsg);

        int controlHandle = getControlHandle(uiObject);

        int controlType = getControlType(uiObject);

        _doRemoveFeedbackForControl(iNativeTFInterface,
                                    controlHandle,
                                    controlType);
    }

    /**
            Moves the specified tactile feedback area to first priority.
            Priority is significant in case of overlapping tactile feedback areas.
            If the tactile feedback styles of registered areas are different,
            the feedback from first-priority area will be given.

            @param uiObject The UI component for tactile feedback area de-registration.
                            Valid object types are: <code>javax.microedition.lcdui.Canvas</code>,
                            <code>javax.microedition.lcdui.CustomItem</code>,
                            <code>org.eclipse.swt.widgets.Control</code>.
            @param id Id of the tactile feedback area to be unregistered.
                      Id's are used to identify particular tactile feedback area within one UI component.
                      If given id was not registered by {@link #registerFeedbackArea registerFeedbackArea}
                  then the call has no effect.
            @throws IllegalArgumentException if the uiObject parameter has invalid type.
    */
    public void moveFeedbackAreaToFirstPriority(Object uiObject, int id)
    {
        if (iNativeTFInterface == 0) return;

        if (!isValidControlType(uiObject))   throw new IllegalArgumentException(invalidControlTypeMsg);

        int controlHandle = getControlHandle(uiObject);
        int controlType = getControlType(uiObject);

        _doMoveFeedbackAreaToFirstPriority(iNativeTFInterface,
                                           controlHandle,
                                           controlType,
                                           id);
    }


    private boolean isValidControlType(Object obj)
    {
        if (!((obj instanceof javax.microedition.lcdui.Canvas)     ||
                (obj instanceof javax.microedition.lcdui.CustomItem) ||
                (obj instanceof org.eclipse.swt.widgets.Control)
             )
           ) return false;

        return true;
    }

    private int getControlHandle(Object uiObject)
    {
        int controlHandle = 0;

        if (uiObject instanceof javax.microedition.lcdui.Canvas)
        {
            controlHandle = iToolkitInvoker.getDisplayableHandle((javax.microedition.lcdui.Displayable)uiObject);
        }
        else if (uiObject instanceof javax.microedition.lcdui.CustomItem)
        {
            controlHandle = iToolkitInvoker.getItemHandle(uiObject);
        }
        else if (uiObject instanceof org.eclipse.swt.widgets.Control)
        {
            org.eclipse.swt.widgets.Control ctrl = (org.eclipse.swt.widgets.Control)uiObject;
            controlHandle = ctrl.handle;
        }

        return controlHandle;
    }

    private int getControlType(Object uiObject)
    {
        if (uiObject instanceof javax.microedition.lcdui.Canvas)
        {
            return TYPE_CANVAS;
        }
        else if (uiObject instanceof javax.microedition.lcdui.CustomItem)
        {
            return TYPE_CUSTOMITEM;
        }
        else if (uiObject instanceof org.eclipse.swt.widgets.Control)
        {
            return TYPE_CONTROL;
        }

        return 0;
    }

    private native boolean _isESWT();

    private native int _createMTouchFeedbackLCDUI(int aToolkitHandle);
    private native int _createMTouchFeedbackESWT(int aToolkitHandle);

    private native boolean _isTouchFeedbackSupported(int aNativeTFInterface);

    private native void _doDirectFeedback(int aNativeTFInterface,
                                          int aStyle);

    private native void _doRegisterFeedback(int iNativeTFInterface,
                                            int controlHandle,
                                            int controlType,
                                            int id,
                                            int x, int y, int width, int height,
                                            int style);



    private native void _doRemoveFeedback(int MTouchFeedbackHandle,
                                          int controlHandle,
                                          int controlType,
                                          int id);

    private native void _doRemoveFeedbackForControl(int MTouchFeedbackHandle,
            int controlHandle,
            int controlType);

    private native void _doMoveFeedbackAreaToFirstPriority(int MTouchFeedbackHandle,
            int controlHandle,
            int controlType,
            int id);
}
