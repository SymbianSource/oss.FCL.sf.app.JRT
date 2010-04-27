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

/**
 * A utility class for consuming DTM rights.
 *
 * @author Nokia Corporation
 * @version $Rev$
 */
final class DrmUtil
{

    /**
     * A pointer to native peer.
     */
    private static int mNativeHandle = 0;


    /*** ----------------------------- PACKAGE ---------------------------- */

    /**
     * Signals the DRM engine that the MIDlet is about to start. If there are
     * no rights to start MIDlet any more, then a error dialog is prompted to
     * the user and an exception will be thrown.
     * The signalling and checking is done only to DRM protected MIDlets.
     * @throws StartupException if not allowed to start.
     */
    static void consumeRightsStart()
    {
        if (Log.mOn) Log.logI("Verifying MIDlet Suite DRM rights.");

        MidletInfo midletInfo = ApplicationInfoImpl.getMidletInfo();
        if (midletInfo.getDrm())
        {
            // This causes the VM to open application's Jar
            // file during the startup phase. CAF panics for some reason
            // without this. Probably the reson is that the file is opened
            // from different thread than where it is closed.
            Object clazz = null;
            try
            {
                clazz = Class.forName("com.nokia.mj.foofoofoo.foofoofoo")
                        .newInstance();
            }
            catch (Exception e)
            {
            }

            Object result = _consumeRights(midletInfo.getClassPath(),
                                           midletInfo.getContentId(), true, 0);

            if (result instanceof String)
            {
                RuntimeErrorDialog.getDrmExpiredDialog().showDialog();
                throw new StartupException("Rights object check failed. '" +
                                           result+"'", false);
            }
            else
            {
                mNativeHandle = ((Integer)result).intValue();
            }
        }
    }

    /**
     * Signals the DRM engine that the MIDlet is about to close.
     * The signalling and checking is done only to DRM protected MIDlets.
     */
    static void consumeRightsStop()
    {
        MidletInfo midletInfo = ApplicationInfoImpl.getMidletInfo();
        if (mNativeHandle != 0 && midletInfo != null && midletInfo.getDrm())
        {
            if (Log.mOn) Log.logI("Ending DRM right consuming.");

            _consumeRights(midletInfo.getClassPath(),
                           midletInfo.getContentId(),
                           false,
                           mNativeHandle);

            mNativeHandle = 0;
        }
    }

    /*** ----------------------------- PRIVATE ---------------------------- */
    /**
     * Constructor of DrmUtil not allowed.
     */
    DrmUtil()
    {
    }

    /*** ----------------------------- NATIVE ----------------------------- */
    /**
    * Will decrement applicable stateful rights such as play counts where
    * necessary.
    * @param uri Uri for the file.
    * @param drmContentId  A unique DRM content id.
    * @param startPhase true if starting the MIDlet, false on closing.
    * @return Status string of the operation. Will be empty on success
    *         cases, but will contain some understandable error text
    *         in failure cases.
    */
    private static native Object _consumeRights(String uri,
            String drmContentId,
            boolean startPhase,
            int handle);
}
