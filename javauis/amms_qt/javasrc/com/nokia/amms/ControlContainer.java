/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Container for controls.
*
*/

package com.nokia.amms;

import javax.microedition.media.Control;
import javax.microedition.media.Controllable;
import com.nokia.amms.ModuleBase;
import com.nokia.amms.control.ControlImpl;
import com.nokia.mj.impl.utils.Logger;
import java.util.Hashtable;
import java.util.Enumeration;

/**
 * ControlContainer class creates java control objects according to a classname
 * which is got from native control object. Created controls are accessible
 * through Controllable interface which is implemented in this class.
 */
public class ControlContainer implements Controllable
{
    /**
     * Default control package. Used when getting control with
     * getControl method which appends default control package if package is
     * not specified.
     */
    static final String CONTROL_DEFAULT_PACKAGE =
        "javax.microedition.media.control.";

    /**
     * Package where control implementation are.
     */
    static final String PRIVATE_PACKAGE = "com.nokia";

    /**
     * Hashtable containing controls identified with a full package name.
     * All control names starts with iDefaultPackage.
     */
    private final Hashtable iControls = new Hashtable();

    /**
     * Creates new ControlContainer.
     */
    private ControlContainer()
    {
    }

    /**
     * Create new ControlContainer and populates the controls.
     *
     * @param aDefaultPackege Default control package. Used when getting control
     * with getControl method which appends default control package if package
     * is not specified.
     * @param aEventSourceHandle Handle to native event source.
     * @param aNativeHandle Handle to native control source.
     * @param aModule Module where the controls belong to.
     */
    public static final ControlContainer populateControls(
        int aEventSourceHandle,
        int aNativeHandle,
        ModuleBase aModule)
    {
        ControlContainer container = new ControlContainer();

        // Get amount of controls in native object.
        int controlCount = _getControlsCount(aEventSourceHandle,
                                             aNativeHandle);

        // Create java object for each native objects.
        for (int i = 0; i < controlCount; i++)
        {
            // Get handle to native object at index i
            int controlHandle = _getControlHandle(aEventSourceHandle,
                                                  aNativeHandle,
                                                  i);

            // Get controls class name which will be used to create java object
            String className = _getControlClassName(aEventSourceHandle,
                                                    controlHandle);

            // create java instance
            Control control = createControl(className,
                                            controlHandle,
                                            aEventSourceHandle,
                                            aModule);

            // Add package if it does not exists
            if (className.indexOf('.') < 0)
            {
                className = CONTROL_DEFAULT_PACKAGE + className;
            }
            container.iControls.put(className, control);
        }

        // population succeed, return created collection
        return container;
    }

    /**
     * Implements method defined in javax.microedition.media.Controllable.
     *
     * @see javax.microedition.media.Controllable
     * @param aControlType  the class name of the Control. The class name should
     * be given either as the fully-qualified name of the class; or if the
     * package of the class is not given, the package
     * javax.microedition.media.control is assumed.
     * @return the object that implements the control, or null.
     */
    public Control getControl(String aControlType)
    {
        if (aControlType == null)
        {
            Logger.ELOG(Logger.EJavaMMAPI,
                        "ControlContainer::getControl parameter was null");
            throw new IllegalArgumentException("argument was null");
        }

        String controlType = aControlType;

        // check if package name exists
        if (controlType.indexOf(".") == -1)
        {
            // add package name
            controlType = CONTROL_DEFAULT_PACKAGE + aControlType;
        }
        Control control = (Control)iControls.get(controlType);

        // If control does not exists with default name, check if there is
        // is a control with same type ( extends the given class name ).
        if (control == null)
        {
            try
            {
                // try to create class for control
                Class controlClass = Class.forName(controlType);

                Enumeration elements = iControls.elements();

                // search if any control is same type that requested control
                while (elements.hasMoreElements() &&
                        control == null)
                {
                    Control tmpControl = (Control)elements.nextElement();
                    if (controlClass.isInstance(tmpControl))
                    {
                        // control is found
                        control = tmpControl;
                    }
                }
            }
            catch (ClassNotFoundException cnfe)  // the class could not be found
            {
                // Exception is ignored and null is returned from this method
                Logger.ELOG(Logger.EJavaMMAPI,
                            "ControlContainer::getControl ",
                            cnfe);
            }
            catch (Error e)  // the function failed for any other reason.
            {
                // Error is ignored and null is returned from this method
                Logger.ELOG(Logger.EJavaMMAPI,
                            "ControlContainer::getControl ",
                            e);
            }
        }
        return control;
    }

    /**
     * Implements method defined in javax.microedition.media.Controllable.
     *
     * @see javax.microedition.media.Controllable
     * @return the collection of Control objects.
     */
    public Control[] getControls()
    {
        Control[] controls = new Control[ iControls.size()];
        Enumeration elements = iControls.elements();
        int i = 0;
        // Put all controls to array
        while (elements.hasMoreElements())
        {
            controls[ i ] = (Control)elements.nextElement();
            i++;
        }
        return controls;
    }

    /**
     * Invalidates the controls stored in this container.
     * After this using controls in Java side will throw
     * RuntimeException.
     */
    public void invalidateControls()
    {
        Enumeration elements = iControls.elements();

        while (elements.hasMoreElements())
        {
            // All controls are instances of ControlImpl class.
            ControlImpl control = (ControlImpl)elements.nextElement();
            control.invalidateControl();
        }
    }

    /**
     * Creates new Control instance.
     * All control classes must be in iPrivatePackage package and
     * extend the ControlImpl base class. Created control is initialized
     * with native control handle and eventsource handle.
     *
     * @param aClassName Control's class name without the package.
     * @param aControlHandle Handle to native control.
     * @param aModule Module where the control belongs to.
     * @return created control
     */
    static private final Control createControl(String aClassName,
            int aControlHandle,
            int aEventSourceHandle,
            ModuleBase aModule)
    {
        ControlImpl control = null;

        // Try to make control instance. If instantion fails, it is an internal
        // error and can only occur in development time.
        try
        {
            Class controlClass =
                Class.forName(PRIVATE_PACKAGE +
                              aClassName);
            control = (ControlImpl)controlClass.newInstance();
            control.initControl(aEventSourceHandle, aControlHandle, aModule);
        }
        catch (InstantiationException ie)
        {
            throw new OutOfMemoryError(ie.getMessage());
        }
        catch (IllegalAccessException iae)
        {
            throw new OutOfMemoryError(iae.getMessage());
        }
        catch (ClassNotFoundException cnfe)
        {
            throw new OutOfMemoryError(cnfe.getMessage());
        }
        return control;
    }


    /**
     * Return the amount of controls in native control source.
     *
     * @param aEventSourceHandle Handle to native event source.
     * @param aNativeHandle Handle to native control source object.
     */
    private static native int _getControlsCount(int aEventSourceHandle,
            int aNativeHandle);

    /**
     * Return native handle to control at specified index.
     *
     * @param aEventSourceHandle Handle to native event source.
     * @param aNativeHandle Handle to native control source object.
     * @param aControlIndex Control's index.
     */
    private static native int _getControlHandle(int aEventSourceHandle,
            int aNativeHandle,
            int aControlIndex);
    /**
     * Returns the control class name that can be used to instantiate Java
     * object.
     *
     * @param aEventSourceHandle Handle to native event source.
     * @param aControlHandle Handle to native control.
     */
    private static native String _getControlClassName(int aEventSourceHandle,
            int aControlHandle);


}
