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
* Description:
*
*/

package com.nokia.mj.impl.security.midp.authorization;

import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettings;
import com.nokia.mj.impl.security.midp.storage.SecurityStorage;

/**
 * Manages (stores, retrieves) the MIDlet suite's security settings
 * (allowed and current settings) related to prompting/user interaction.
 */
public final class UserSecuritySettingsModule
{
    /**
     * Creates an instance of the UserSecuritySettingsModule
     *
     * @return An instance of UserSecuritySettingsModule
     */
    public static UserSecuritySettingsModule createInstance()
    {
        if (self == null)
        {
            self = new UserSecuritySettingsModule();
        }
        return self;
    }

    /**
     * Retrieves some particular security settings identified by name,
     * settings associated to a particular MIDlet suite
     *
     * @param msUID = the UID of the MIDlet suite whose user setings are
     *                retrieved
     * @param settingsName = The name of the settings to be retrieved
     * @return The security settings
     */
    public UserSecuritySettings getUserSecuritySettings(
        Uid msUID,
        String settingsName)
    {
        return storage.readUserSecuritySettings(msUID, settingsName);
    }

    /**
     * Retrieves all the security settings associated to a particular
     * MIDlet suite
     *
     * @param msUID = the UID of the MIDlet suite whose user setings are
     *                retrieved
     * @return A list of all the security settings associated to a particular
     * MIDlet suite
     */
    public UserSecuritySettings[] getUserSecuritySettings(Uid msUID)
    {
        return storage.readUserSecuritySettings(msUID);
    }

    /**
     * Associates some security settings with a particular MIDlet suite.
     *
     * @param msUID = the UID of the MIDlet suite whose user setings are
     *                being set
     * @param settings = The security settings to be associated to a particular
     *                   MIDlet suite. Actually only the currentInteractionMode
     *                   of a particular setting can be changed, but for
     *                   clarity the UserSecuritySettings has been preferred
     *                   instead of a type corresponding to the
     *                   currentInteractionMode only
     */
    public void setUserSecuritySettings(
        Uid msUID,
        UserSecuritySettings settings)
    {
        storage.writeUserSecuritySettings(msUID,
                                          settings.getName(), settings.getCurrentInteractionMode());
    }

    private UserSecuritySettingsModule()
    {
        storage = new SecurityStorage();
    }

    // self
    private static UserSecuritySettingsModule self;

    // storage
    private SecurityStorage storage;
}
