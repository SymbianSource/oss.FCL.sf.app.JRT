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

import java.util.Hashtable;
import java.util.Enumeration;

import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.utils.Logger;

/**
 * A class for caching important MIDlet specific data. This data
 * is used mostly to serve the ApplicationInfo APIs, but to some extent
 * this data is used by the life cycle.
 */
final class MidletInfo
{

    /**
     * The protection domain category of the MIDlet.
     */
    private String  mMidletProtectionDomain = null;

    /**
     * The UID of the MIDlet.
     */
    private Uid     mMidletUid              = null;

    /**
     * The UID of the MIDlet suite.
     */
    private Uid     mMidletSuiteUid         = null;

    /**
     * The name of the MIDlet.
     */
    private String  mMidletName             = null;

    /**
     * The localized name of the MIDlet.
     */
    private String  mMidletLocalizedName    = null;

    /**
     * The name of the MIDlet suite.
     */
    private String  mMidletSuiteName        = null;

    /**
     * The vendor of the MIDlet suite.
     */
    private String  mMidletVendor           = null;

    /**
     * The version of the MIDlet suite.
     */
    private String  mMidletVersion          = null;

    /**
     * The root path of the MIDlet suite. This is the location where the jar
     * (jad) file exists and where the different JSRs may store data.
     */
    private String  mMidletRootPath         = null;

    /**
     * The main class of the MIDlet.
     */
    private String  mMidletMainClass        = null;

    /**
     * The class path of the MIDlet.
     */
    private String  mMidletClassPath        = null;

    /**
     * The calculated hash for the jar file of the MIDlet suite.
     */
    private String  mMidletHash             = null;

    /**
     * The Hash of the root certificate used to authenticate the MIDlet.
     */
    private String  mRootHash               = null;

    /**
     * The protection domain name.
     */
    private String  mProtectionDomainName   = null;

    /**
     * The info if MIDlet is DRM protected.
     */
    private boolean mDrm                    = false;

    /**
     * The unique DRM content id.
     */
    private String  mContentId   = null;


    /**
     * A collection of attributes of the MIDlet suite. This will contain all
     * attributes from mainfest and jad file (if exists).
     */
    private Hashtable  mMidletAttributes    = null;


    /*** ----------------------------- PACKAGE ---------------------------- */

    /**
     * Constructor of MidletInfo.
     */
    MidletInfo()
    {
    }

    /**
     * A setter for protection domain of the MIDlet suite.
     * @param protectionDomain The protection domain to be set.
     */
    void setProtectionDomain(String protectionDomain)
    {
        mMidletProtectionDomain = protectionDomain;
    }

    /**
     * A getter for protection domain.
     * @return The protection domain of the MIdlet suite.
     */
    String getProtectionDomain()
    {
        return mMidletProtectionDomain;
    }

    /**
     * A setter for the UID of the MIDlet.
     * @param midletUid The uid of the MIDlet to be set.
     */
    void setUid(Uid midletUid)
    {
        mMidletUid = midletUid;
    }

    /**
     * A getter for the UID of the MIDlet.
     * @return The uid of the MIDlet.
     */
    Uid getUid()
    {
        return mMidletUid;
    }

    /**
     * A setter for the UID of the MIDlet suite.
     * @param midletSuiteUid The uid of the MIDlet suite to be set.
     */
    void setSuiteUid(Uid midletSuiteUid)
    {
        mMidletSuiteUid = midletSuiteUid;
    }

    /**
     * A getter for the UID of the MIDlet suite.
     * @return The uid of the MIDlet suite.
     */
    Uid getSuiteUid()
    {
        return mMidletSuiteUid;
    }

    /**
     * A setter for the name of the MIDlet suite.
     * @param suiteName The name of the MIDlet suite to be set.
     */
    void setSuiteName(String suiteName)
    {
        mMidletSuiteName = suiteName;
    }

    /**
     * A getter for the name of the MIDlet suite.
     * @return The name of the MIDlet suite.
     */
    String getSuiteName()
    {
        return mMidletSuiteName;
    }

    /**
     * A setter for the name of the MIDlet.
     * @param name The name of the MIDlet to be set.
     */
    void setName(String name)
    {
        mMidletName = name;
    }

    /**
     * A getter for the name of the MIDlet.
     * @return The name of the MIDlet.
     */
    String getName()
    {
        return mMidletName;
    }

    /**
     * A setter for the localized name of the MIDlet.
     * @param name The localized name of the MIDlet to be set.
     */
    void setLocalizedName(String name)
    {
        mMidletLocalizedName = name;
    }

    /**
     * A getter for the localized name of the MIDlet.
     * @return The localized name of the MIDlet.
     */
    String getLocalizedName()
    {
        return mMidletLocalizedName;
    }

    /**
     * A setter for the vendor of the MIDlet suite.
     * @param vendor The vendor of the MIDlet suite to be set.
     */
    void setVendor(String vendor)
    {
        mMidletVendor = vendor;
    }

    /**
     * A getter for the vendor of the MIDlet suite.
     * @return The vendor of the MIDlet suite.
     */
    String getVendor()
    {
        return mMidletVendor;
    }

    /**
     * A setter for the version of the MIDlet suite.
     * @param version The version of the MIDlet suite to be set.
     */
    void setVersion(String version)
    {
        mMidletVersion = version;
    }

    /**
     * A getter for the version of the MIDlet suite.
     * @return The version of the MIDlet suite.
     */
    String getVersion()
    {
        return mMidletVersion;
    }

    /**
     * A setter for the rootPath of the MIDlet suite.
     * @param rootPath The rootPath of the MIDlet suite to be set.
     */
    void setRootPath(String rootPath)
    {
        mMidletRootPath = rootPath;
    }

    /**
     * A getter for the rootPath of the MIDlet suite.
     * @return The rootPath of the MIDlet suite.
     */
    String getRootPath()
    {
        return mMidletRootPath;
    }

    /**
     * A setter for the main class of the MIDlet.
     * @param mainClass The main class of the MIDlet to be set.
     */
    void setMainClass(String mainClass)
    {
        mMidletMainClass = mainClass;
    }

    /**
     * A getter for the main class of the MIDlet.
     * @return The main class of the MIDlet.
     */
    String getMainClass()
    {
        return mMidletMainClass;
    }

    /**
     * A setter for the classPath of the MIDlet suite.
     * @param classPath The classPath of the MIDlet suite to be set.
     */
    void setClassPath(String classPath)
    {
        mMidletClassPath = classPath;
    }

    /**
     * A getter for the classPath of the MIDlet suite.
     * @return The classPath of the MIDlet suite.
     */
    String getClassPath()
    {
        return mMidletClassPath;
    }

    /**
     * A setter for the hash of the jar file of MIDlet suite.
     * @param midletHash The hash of the MIDlet suite to be set.
     */
    void setMidletHash(String midletHash)
    {
        mMidletHash = midletHash;
    }

    /**
     * A getter for the hash of the jar file of MIDlet suite.
     * @return The hash of the MIDlet suite.
     */
    String getMidletHash()
    {
        return mMidletHash;
    }

    /**
     * A setter for the hash of the root certificate.
     * @param rootHash The hash of the root certificate.
     */
    void setRootHash(String rootHash)
    {
        mRootHash = rootHash;
    }

    /**
     * A getter for the hash of the root certificate.
     * @return The hash of the root certificate.
     */
    String getRootHash()
    {
        return mRootHash;
    }

    /**
     * A setter for the name of the security domain.
     * @param protectionDomainName The name of the security domain.
     */
    void setProtectionDomainName(String protectionDomainName)
    {
        mProtectionDomainName = protectionDomainName;
    }

    /**
     * A getter for the name of the security domain.
     * @return The name of the security domain.
     */
    String getProtectionDomainName()
    {
        return mProtectionDomainName;
    }

    /**
     * A setter for the info if MIDlet is DRM protected.
     * @param drm true if drm protected.
     */
    void setDrm(boolean drm)
    {
        mDrm = drm;
    }

    /**
     * A getter for the info if MIDlet is DRM protected.
     * @return true if drm protected.
     */
    boolean getDrm()
    {
        return mDrm;
    }

    /**
     * A setter for the content id of DRM protectet MIDlet.
     * @param contentId DRM content id.
     */
    void setContentId(String contentId)
    {
        mContentId = contentId;
    }

    /**
     * A getter for the content id of DRM protectet MIDlet.
     * @return DRM content id.
     */
    String getContentId()
    {
        return mContentId;
    }

    /**
     * Adds a new attribute into hash table. Creates a hash table if it
     * doesn't exist.
     * @param key The key of the attribute.
     * @value key The value of the attribute.
     */
    void addAttribute(String key, String value)
    {
        if (mMidletAttributes == null)
        {
            mMidletAttributes = new Hashtable();
        }
        mMidletAttributes.put(key, value);
    }

    /**
     * Searches the attribute using the provided key. If the attributes are not
     * avalable, those are fetched from the Java Storage.
     * @param key The search key.
     * @return the value of the attribute if found, otherwise null.
     */
    String getAttribute(String key)
    {
        if (mMidletAttributes == null)
        {
            try
            {
                // Attributes not yet fetched from the storage. Fetching now.
                StorageAccessor.setMidletAttributes(this);
            }
            catch (Throwable th)
            {
                mMidletAttributes = null;
                Logger.ELOG(Logger.EJavaRuntime,
                            "Error in MidletInfo.getAttribute: "+th,
                            th);
            }
        }
        if (mMidletAttributes != null)
        {
            return (String)mMidletAttributes.get(key);
        }
        return null;
    }

    /**
     * A getter for MIDlet suite attributes.
     * @return the MIDlet suite attributes.
     */
    Hashtable getMidletAttributes()
    {
        return mMidletAttributes;
    }

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Returns a string representation of the MidletInfo.
     * @return a string representation of the MidletInfo.
     */
    public String toString()
    {
        String s = "\nMIDletDomain     = " + getProtectionDomain() +
                   "\nMIDletUid        = " + getUid() +
                   "\nMIDletSuiteUid   = " + getSuiteUid() +
                   "\nMIDletSuiteName  = " + getSuiteName() +
                   "\nMIDletName       = " + getName() +
                   "\nMIDletVendor     = " + getVendor() +
                   "\nMIDletVersion    = " + getVersion() +
                   "\nMIDletRootPath   = " + getRootPath() +
                   "\nMIDletMainClass  = " + getMainClass() +
                   "\nMidletClassPath  = " + getClassPath() +
                   "\nMidletHash       = " + getMidletHash() +
                   "\nRootHash         = " + getRootHash() +
                   "\nMidletDomainName = " + getProtectionDomainName() +
                   "\nMidletDrm        = " + getDrm() +
                   "\nMidletContentId  = " + getContentId() +
                   "\nmMidletLocalizedName = " + getLocalizedName() +
                   "\nArgs:";

        if (mMidletAttributes != null)
        {
            Enumeration e = mMidletAttributes.keys();
            while (e != null && e.hasMoreElements())
            {
                Object key = e.nextElement();
                Object value = mMidletAttributes.get(key);
                s += "\n  key: " + key + ", value:" + value;
            }
        }
        return s;
    }
}
