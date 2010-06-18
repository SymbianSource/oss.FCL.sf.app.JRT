/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class setup MMA plugins.
*
*/


package com.nokia.microedition.media;

import com.nokia.mj.impl.utils.Logger;

/**
 * Class used to setup external components.
 */
public class Setup
{
    // Class names used to invoke external components.
    private static final String[] MMA_PLUGINS =
    {
        "com.nokia.amms.MMAInvokeListenerImpl"
    };

    /**
     * Private constructor not to allow construction.
     */
    private Setup()
    {
    }

    /**
     * Setup external components.
     * @param aEventSourceHandle Native handle MMA event source.
     */
    public static void setup(int aEventSourceHandle)
    {
        // Go through all plugins
        for (int i = 0; i < MMA_PLUGINS.length; i++)
        {
            invokeComponent(aEventSourceHandle, MMA_PLUGINS[ i ]);
        }
    }

    /**
     * Invokes other component. This method returns without creating the
     * components if the class is not found.
     *
     * @param aEventSourceHandle Handle to native mma event source.
     * @param aClassName Full class name to create. Class must have public
     * default constructor and implement MMAInvokeListener interface.
     * @throws OutOfMemoryError if component cannot be created.
     */
    private static void invokeComponent(int aEventSourceHandle, String aClassName)
    {
        try
        {
            Class invokerClass =
                Class.forName(aClassName);
            MMAInvokeListener invoker =
                (MMAInvokeListener)invokerClass.newInstance();
            invoker.notifyInvoke(aEventSourceHandle);
        }
        catch (InstantiationException ie)
        {
            throw new OutOfMemoryError("Instantiation failed, " + ie.toString());
        }
        catch (IllegalAccessException iae)
        {
            throw new OutOfMemoryError("Illegal access, " + iae.toString());
        }
        catch (ClassNotFoundException cnfe)
        {
            // If invoker class cannot be found, plugin is not added to J2ME
            // build and MMA is used without it.
            Logger.ELOG(Logger.EJavaMMAPI, "MMA::Setup:: ", cnfe);
        }
    }
}
// End of File

