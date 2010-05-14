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
* Description:
*
*/


package javax.microedition.midlet;

import com.nokia.mj.impl.rt.midp.MidletApplicationBase;

/**
 * A package private class in javax.microedition.midlet package for
 * accessing protected abstract methods of MIDlet class.
 * @see com.nokia.mj.impl.rt.midp.MidletApplicationBase.
 */
final class MidletApplication extends MidletApplicationBase
{

    /*** ----------------------------- PACKAGE ---------------------------- */

    /**
     * Constructor is called by the constructor of the MIDlet class.
     * @param midlet Just created MIDlet which will be forwarded to the
     *               base class that will hold a reference to the MIDlet
     *               during its life time.
     */
    MidletApplication(MIDlet midlet)
    {
        super(midlet);
    }

    /*** ---------------------------- PROTECTED --------------------------- */

    /**
     * For calling the startApp() method of the MIDlet.
     * @see javax.microedition.midlet.MIDlet#startApp()
     */
    protected void invokeStartApp()
    throws MIDletStateChangeException
    {
        getMidlet().startApp();
    }


    /**
     * For calling the destroyApp() method of the MIDlet.
     * @see javax.microedition.midlet.MIDlet#destroyApp(boolean)
     */
    protected void invokeDestroyApp(boolean unconditional)
    throws MIDletStateChangeException
    {
        getMidlet().destroyApp(unconditional);
    }

    /**
     * For calling the pauseApp() method of the MIDlet.
     * @see javax.microedition.midlet.MIDlet#pauseApp()
     */
    protected void invokePauseApp()
    {
        getMidlet().pauseApp();
    }
}
