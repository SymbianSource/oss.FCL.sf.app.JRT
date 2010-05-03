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

import java.util.Enumeration;

import com.nokia.mj.impl.storage.StorageAttribute;
import com.nokia.mj.impl.storage.StorageEntry;
import com.nokia.mj.impl.storage.StorageFactory;
import com.nokia.mj.impl.storage.StorageNames;
import com.nokia.mj.impl.storage.StorageSession;
import com.nokia.mj.impl.utils.Uid;

/**
 * A utility class for reading the MIDlet specfic data from storage. At the
 * moment it is known that setMidletStartArguments() is called first and
 * rigth after that setMidletAttributes() is called. Therefore the session
 * created in setMidletStartArguments() is reused in setMidletAttributes()
 * method and then closed. If this scenario changes, then the close must
 * be done also in setMidletStartArguments() method.
 *
 * @author Nokia Corporation
 * @version $Rev$
 */
final class StorageAccessor
{
    /**
     * Session to storage in order to re-use.
     */
    private static StorageSession mSession;

    /*** ----------------------------- PACKAGE ---------------------------- */

    /**
     * Sets the MidletInfo to contain a set of static data related to the
     * MIDlet.
     * @param midletInfo where to store the data.
     */
    static void setMidletStartArguments(MidletInfo midletInfo)
    {
        mSession = StorageFactory.createSession();
        mSession.open();

        // Reading the MIDlet specific attributes from APPLICATION_TABLE.
        // MIDlet UID is a key.
        StorageEntry midletEntry =
            mSession.readEntry(StorageNames.APPLICATION_TABLE,
                               midletInfo.getUid());
        // Set the main class of the MIDlet.
        midletInfo.setMainClass(
            midletEntry.getAttribute(StorageNames.MAIN_CLASS).getValue());

        // Set the name of the MIDlet.
        midletInfo.setName(
            midletEntry.getAttribute(StorageNames.NAME).getValue());

        // Set the UID of the MIDlet suite.
        midletInfo.setSuiteUid(Uid.createUid(
                                   midletEntry.getAttribute(StorageNames.PACKAGE_ID).getValue()));

        // Reading the MIDlet suite specific attributes from
        // APPLICATION_PACKAGE_TABLE. MIDlet suite UID is a key.
        StorageEntry midletSuiteEntry =
            mSession.readEntry(StorageNames.APPLICATION_PACKAGE_TABLE,
                               midletInfo.getSuiteUid());

        // Set the name of the MIDlet suite.
        midletInfo.setSuiteName(
            midletSuiteEntry.getAttribute(StorageNames.PACKAGE_NAME).
            getValue());
        // Set the vendor of the MIDlet suite.
        midletInfo.setVendor(
            midletSuiteEntry.getAttribute(StorageNames.VENDOR).getValue());

        // Set the version of the MIDlet suite.
        midletInfo.setVersion(
            midletSuiteEntry.getAttribute(StorageNames.VERSION).getValue());

        // Set the root path of the MIDlet suite.
        midletInfo.setRootPath(
            midletSuiteEntry.getAttribute(StorageNames.ROOT_PATH).getValue());

        // Set the classpath of the MIDlet suite.
        midletInfo.setClassPath(
            midletSuiteEntry.getAttribute(StorageNames.JAR_PATH).getValue());

        // Set the drm info.
        midletInfo.setDrm(
            midletSuiteEntry.getAttribute(StorageNames.CONTENT_INFO).
            getValue().equals("1"));

        StorageAttribute contentId =
            midletSuiteEntry.getAttribute(StorageNames.CONTENT_ID);
        if (contentId != null)
        {
            midletInfo.setContentId(contentId.getValue());
        }

        // Reading the MIDlet suite specific attributes from
        // MIDP_PACKAGE_TABLE. MIDlet suite UID is a key.
        StorageEntry midletSuiteSpecificEntry =
            mSession.readEntry(StorageNames.MIDP_PACKAGE_TABLE,
                               midletInfo.getSuiteUid());
        midletInfo.setProtectionDomain(
            midletSuiteSpecificEntry.getAttribute(
                StorageNames.SECURITY_DOMAIN_CATEGORY).
            getValue());
        // Set the hash of the MIDlet suite if available.
        StorageAttribute hash =
            midletSuiteSpecificEntry.getAttribute(StorageNames.HASH);
        if (hash != null)
        {
            midletInfo.setMidletHash(hash.getValue());
        }

        // Set the securoty domain name of the MIDlet suite.
        midletInfo.setProtectionDomainName(
            midletSuiteSpecificEntry.getAttribute(
                StorageNames.SECURITY_DOMAIN).getValue());

        // Set the hash of the root if available.
        StorageAttribute rootHash =
            midletSuiteSpecificEntry.getAttribute(StorageNames.CERT_HASH);
        if (rootHash != null)
        {
            midletInfo.setRootHash(rootHash.getValue());
        }

        // Session is intentionally left open in a success case since at the
        // moment setMidletAttributes() is called before starting the MIDlet,
        // so already created session can be re-used.
    }

    /**
     * Sets the MidletInfo to contain MIDlet attributes stored from manifest
     * and jad file (if exists) during installation.
     * @param midletInfo where to store the attributes.
     */
    static void setMidletAttributes(MidletInfo midletInfo)
    {
        try
        {
            // Re-use the session if available.
            if (mSession == null)
            {
                mSession = StorageFactory.createSession();
                mSession.open();
            }

            // Reading the MIDlet suite attributes from
            // APPLICATION_PACKAGE_ATTRIBUTES_TABLE. MIDlet suite UID is a key.
            StorageEntry[] attributeEntries =
                mSession.readEntries(
                    StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE,
                    midletInfo.getSuiteUid());

            if (attributeEntries != null)
            {
                int midletN = 1;

                // Loop through all the arguments and store them.
                for (int i = 0; i<attributeEntries.length; ++i)
                {
                    StorageAttribute name =
                        attributeEntries[i].getAttribute(StorageNames.NAME);
                    StorageAttribute value =
                        attributeEntries[i].getAttribute(StorageNames.VALUE);
                    midletInfo.addAttribute(name.getValue(), value.getValue());

                    // Determine what is the <n> of this MIDlet,
                    // (which attribute "Nokia-MIDlet-Localized-<n>" contains the localized
                    // name of this MIDlet)
                    if ((value.getValue().indexOf(midletInfo.getName()) > -1) &&
                            name.getValue().startsWith("MIDlet-"))
                    {
                        try
                        {
                            // Try to parse the rest of attribute name (after "MIDlet-")
                            // into an int. If this throws exception, the attribute was
                            // propably 'MIDlet-Name'
                            midletN = Integer.parseInt(name.getValue().substring(7));
                        }
                        catch (NumberFormatException ne) {};
                    }
                }

                // Set the localized name of the MIDlet.
                String localizedName = midletInfo.getAttribute("Nokia-MIDlet-Localized-" + midletN);
                if (localizedName == null)
                {
                    localizedName = midletInfo.getName();
                }

                midletInfo.setLocalizedName(localizedName);
            }
        }
        finally
        {
            // Closing the session.
            if (mSession != null)
            {
                mSession.close();
                mSession.destroySession();
                mSession = null;
            }
        }
    }
}
