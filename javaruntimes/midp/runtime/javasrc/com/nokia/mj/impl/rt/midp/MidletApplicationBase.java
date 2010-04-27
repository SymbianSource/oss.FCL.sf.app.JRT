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


package com.nokia.mj.impl.rt.midp;

import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;

import com.nokia.mj.impl.rt.legacy.LegacySupport;

/**
 * A class for communication gate between MIDlet and the MIDP life cycle.
 * <p>
 * The MIDlet class is in such package where it is not possible to add any new
 * public classes. The issue is that some important methods are defined
 * as protected, so they can only be called from MIDlet class. The MIDlet
 * class can not contain any new public methods, so in order to delegate
 * calls to these protected methods, there must be introduced package private
 * methods that are accessed via a class which is in javax.microedition.midlet
 * package as a package private private class. This package private class will
 * be subclass of this class.
 * <p>
 * Other reason having this class is used to delegate command originated by the
 * MIDlet to the MIDP life cycle - e.g. notifyDestroyed.
 * <p>
 * During the MIDlet construction an instance of the sub class is constructed.
 * The subclass constructs this class and provides a refernce to just created
 * instance of a MIDlet subclass. This class will store the reference to the
 * MIDlet instance. During the construction of this class it will give a
 * reference to this to the MIDP life cycle, which will store the refernce in
 * order to delegate calls to MIDlet.
 *
 * @author Nokia Corporation
 * @version $Rev$
 */
public abstract class MidletApplicationBase
{

    /**
     * A reference to the MIDlet.
     */
    private MIDlet mMidlet;

    /*** ----------------------------- PUBLIC ------------------------------ */
    /**
     * This method will delegate the MIDlet wish to enter into the destroyed
     * state to the MIDP life cycle.
     */
    public final void notifyDestroyed()
    {
        MidletLifeCycle.getInstance().notifyDestroyed();
    }

    /**
     * This method will delegate the MIDlet wish to enter into the paused
     * state to the MIDP life cycle.
     */
    public final void notifyPaused()
    {
        MidletLifeCycle.getInstance().notifyPaused();
    }

    /**
     * This method will delegate the MIDlet wish to enter into the active
     * state to the MIDP life cycle.
     */
    public final void resumeRequest()
    {
        MidletLifeCycle.getInstance().resumeRequest();
    }

    /**
     * The implementation of MIDlet.getAppProperty().
     * @see javax.microedition.midlet.MIDlet#getAppProperty(java.lang.String).
     * @param key The name of the attribute.
     * @return The value of the attribute if found, otherwise null.
     */
    public final String getAppProperty(String key)
    {
        return ApplicationInfoImpl.getMidletInfo().getAttribute(key);

    }

    /*** ---------------------------- PROTECTED --------------------------- */

    /**
     * The constructor of MidletApplicationBase. Will give a reference to this
     * to be stored by the MIDP life cycle.
     * @param midlet A reference to just created MIDlet.
     */
    protected MidletApplicationBase(MIDlet midlet)
    {
        mMidlet = midlet;
        MidletLifeCycle lifeCycle = MidletLifeCycle.getInstance();
        lifeCycle.setMidletApplication(this);

        // Set the created MIDlet to be accessed by legacy runtime.
        LegacySupport.setMidlet(midlet);

    }

    /**
     * A getter for a reference to the MIDlet.
     * @return The MIDlet.
     */
    protected final MIDlet getMidlet()
    {
        return mMidlet;
    }

    /**
     * An abstract method for delegating the start cmd to the MIDlet.
     * @see javax.microedition.midlet.MIDlet#startApp().
     */
    protected abstract void invokeStartApp() throws MIDletStateChangeException;

    /**
     * An abstract method for delegating the pause cmd to the MIDlet.
     * @see javax.microedition.midlet.MIDlet#pauseApp().
     */
    protected abstract void invokePauseApp();

    /**
     * An abstract method for delegating the destroy cmd to the MIDlet.
     * The unconditional is always true and any thrown exceptions will
     * be ignored.
     * @see javax.microedition.midlet.MIDlet#destroyApp().
     * @param unconditional Always true.
     */
    protected abstract void invokeDestroyApp(boolean unconditional)
    throws MIDletStateChangeException;

    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * The default constructor is not allowed.
     */
    private MidletApplicationBase()
    {
    }
}
