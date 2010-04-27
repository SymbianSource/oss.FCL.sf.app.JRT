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

/**
 * Localized security resources module
 * This is a container of localized resources:
 *  - the names of the settings/function groups (this one is needed in the
 *    settings UI view). Example: "NetAccess"
 *  - the names of the interaction modes ("oneshot", "session, "blanket")
 *  - the texts of the security runtime prompts for each of the
 *    settings/function groups.
 *    Example: "Do you allow this MIDlet to access network?"
 *  - security error messages (at least those which should be shown to the
 *    user, like mutually exclusive settings).
 *    Example:"This and this settings can not be changed simultaneously to
 *    this value due to ..."
 * The resources are identified by a resourceID (= the text in engineering
 * english; e.g. netaccess). This container comes with an initial set of
 * elements: the localized names and texts of the security runtime prompts
 * for the predefined function groups plus the localized texts for the
 * interaction modes.
 * SecurityPolicyModule can register new names and texts for other than the
 * predefined function groups (these are provided by the extensions).
 */
public final class LocalizedSecurityResourcesModule
{
    /**
     * Creates an instance of the LocalizedSecurityResourcesModule
     *
     * @return An instance of LocalizedSecurityResourcesModule
     */
    public static LocalizedSecurityResourcesModule createInstance()
    {
        if (self == null)
        {
            self = new LocalizedSecurityResourcesModule();
        }
        return self;
    }

    /**
     * Retrieves the localized info of a certain security settings
     *
     * @param settingsName The name of the settings whose localized info
     *        is retrieved
     * @return The localized info of the queried settings
     */
    public LocalizedSecuritySettingsInfo getLocalizedSecuritySettings(
        String settingsName)
    {
        // Add implementation.
        return null;
    }

    /**
     * Registers the localized info of a certain security settings
     *
     * @param settingsName The name of the settings whose localized info
     *        is registered
     * @param localizedInfo The localized info
     */
    public void registerLocalizedSecuritySettings(
        String settingsName,
        LocalizedSecuritySettingsInfo localizedInfo)
    {
        // Add implementation.
    }

    /**
     * Retrieves the localized info of a certain security resource
     * (e.g. localized string for "oneshot")
     *
     * @param resourceID The identifier of the resource whose localized
     *        info is retrieved
     * @return The localized info of the queried resource
     */
    public String getLocalizedSecurityResource(String resourceID)
    {
        // Add implementation.
        return null;
    }

    // self
    private static LocalizedSecurityResourcesModule self;
}
