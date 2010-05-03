/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.installer.storagehandler;

import com.nokia.mj.impl.installer.utils.ComponentId;
import com.nokia.mj.impl.installer.utils.FileRoots;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.Version;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

public class SuiteInfo
{
    // Nokia-MIDlet-On-Screen-Keypad values: "undefined", "no", "navigationkeys" and "gameactions".
    // The numerical values are compatible with old S60 implementation values.
    public static final int OSK_UNDEFINED = 0;
    public static final int OSK_NO = 0;
    public static final int OSK_GAMEACTIONS = 1;
    public static final int OSK_NAVIKEYS = 2;

    // Content info values.
    public static final int CONTENT_INFO_PLAIN = 0;
    public static final int CONTENT_INFO_DRM = 1;
    private int iContentInfo = CONTENT_INFO_PLAIN;
    private String iContentId = null;

    // States telling if this suite was installed or preinstalled.
    static final int STATE_INSTALLED = 0;
    static final int STATE_PREINSTALLED = 1;
    static final int STATE_NO_PREINSTALL = 2;
    // Default installation state.
    int iPreinstallState = STATE_INSTALLED;

    /** Separator used in global component ids. */
    private static final String GLOBAL_ID_SEPARATOR = "/";

    private Uid iUid = null;
    private ComponentId iCid = null;
    private String iName = null;
    private String iVendor = null;
    private Version iVersion = null;
    private String iJadPath = null;
    private String iJarPath = null;
    private String iJadUrl = null;
    private String iJarUrl = null;
    private String iRootDir = null;
    private int iMediaId = -1;
    private int iInitialSize = -1;
    private int iOnScreenKeypad = OSK_UNDEFINED;
    private String iAccessPoint = null;
    // Flag telling if application suite is trusted
    private boolean iTrusted = false;

    /** Filename for converted icon. This member is not saved into storage. */
    private String iConvertedIconPath = null;
    /** Application installation group. This member is not saved into storage. */
    private String iInstallationGroup = null;

    /**
     * Combined Jad and Manifest attributes. Key is attribute name,
     * value is Attribute object
     */
    private Hashtable iAttributes = null;

    /**
     * ApplicationInfos for applications in this suite.
     */
    private Vector iApplications = null;

    /**
     * Private default constructor, not to be used by clients.
     */
    private SuiteInfo()
    {
        iAttributes = new Hashtable();
        iApplications = new Vector();
    }

    /**
     * Construct a new SuiteInfo object.
     * @param aUid application uid
     */
    public SuiteInfo(Uid aUid)
    {
        this();
        iUid = aUid;
    }

    /**
     * Construct a new SuiteInfo object.
     * @param aName suite's name
     * @param aVendor suite's vendor
     */
    public SuiteInfo(String aName, String aVendor)
    {
        this();
        iName = aName;
        iVendor = aVendor;
    }

    /**
     * Returns global id for given application.
     */
    public static String getGlobalId(
        String aVendor, String aSuiteName, String aAppName)
    {
        if (aVendor == null || aSuiteName == null)
        {
            return null;
        }
        StringBuffer buf = new StringBuffer();
        buf.append(aVendor);
        buf.append(GLOBAL_ID_SEPARATOR);
        buf.append(aSuiteName);
        if (aAppName != null)
        {
            buf.append(GLOBAL_ID_SEPARATOR);
            buf.append(aAppName);
        }
        return buf.toString();
    }

    /**
     * Returns global id for this application suite.
     */
    public String getGlobalId()
    {
        return getGlobalId(getVendor(), getName(), null);
    }

    /**
     * Returns global id for the specified application in this
     * application suite.
     */
    public String getGlobalId(int aIndex)
    {
        if (getApplications() == null || getApplications().size() <= aIndex)
        {
            return null;
        }
        return getGlobalId(
                   getVendor(), getName(),
                   ((ApplicationInfo)getApplications().elementAt(aIndex)).getName());
    }

    /**
     * Get suite uid.
     */
    public Uid getUid()
    {
        return iUid;
    }

    /**
     * Get component id.
     */
    public ComponentId getComponentId()
    {
        return iCid;
    }

    /**
     * Get suite name.
     */
    public String getName()
    {
        return iName;
    }

    /**
     * Get suite vendor.
     */
    public String getVendor()
    {
        return iVendor;
    }

    /**
     * Get suite version.
     */
    public Version getVersion()
    {
        return iVersion;
    }

    /**
     * Get jad path.
     */
    public String getJadPath()
    {
        return iJadPath;
    }

    /**
     * Get jar path.
     */
    public String getJarPath()
    {
        return iJarPath;
    }

    /**
     * Get jad url.
     */
    public String getJadUrl()
    {
        return iJadUrl;
    }

    /**
     * Get jar url.
     */
    public String getJarUrl()
    {
        return iJarUrl;
    }

    /**
     * Get suite root directory.
     */
    public String getRootDir()
    {
        return iRootDir;
    }

    /**
     * Get id for installation media.
     */
    public int getMediaId()
    {
        return iMediaId;
    }

    /**
     * Get application initial size.
     */
    public int getInitialSize()
    {
        return iInitialSize;
    }

    /**
     * Get OnScreenKeypad value.
     */
    public int getOnScreenKeypad()
    {
        return iOnScreenKeypad;
    }

    /**
     * Get content info.
     */
    public int getContentInfo()
    {
        return iContentInfo;
    }

    /**
     * Get content id.
     */
    public String getContentId()
    {
        return iContentId;
    }

    /**
     * Get access point for this suite.
     */
    public String getAccessPoint()
    {
        return iAccessPoint;
    }

    /**
     * Get suite attributes.
     *
     * @return hashtable where key is attribute name and
     * value is com.nokia.mj.impl.utils.Attribute object.
     */
    public Hashtable getAttributes()
    {
        return iAttributes;
    }

    /**
     * Get attribute.
     *
     * @param aName attribute name
     * @return com.nokia.mj.impl.utils.Attribute object or null
     * if attribute is not found
     */
    public Attribute getAttribute(String aName)
    {
        return (Attribute)iAttributes.get(aName);
    }

    /**
     * Get attribute value.
     *
     * @param aName attribute name
     * @return attribute value or null if attribute is not found
     */
    public String getAttributeValue(String aName)
    {
        Attribute attr = (Attribute)iAttributes.get(aName);
        if (attr != null)
        {
            return attr.getValue();
        }
        return null;
    }

    /**
     * Get suite applications.
     *
     * @return Vector of ApplicationInfo objects
     */
    public Vector getApplications()
    {
        return iApplications;
    }

    /**
     * Get icon path for the first application that has an icon.
     */
    public String getFirstIconPath()
    {
        if (iApplications == null)
        {
            return null;
        }
        String result = null;
        for (int i = 0; i < iApplications.size(); i++)
        {
            ApplicationInfo appInfo =
                (ApplicationInfo)iApplications.elementAt(i);
            String iconPath = appInfo.getIconPath();
            if (iconPath != null && iconPath.length() > 0)
            {
                result = iconPath;
                break;
            }
        }
        return result;
    }

    /**
     * Get application uids.
     *
     * @return array of application uids or null if suite contains no applications
     */
    public Uid[] getApplicationUids()
    {
        if (iApplications == null)
        {
            return null;
        }
        Uid[] uids = new Uid[iApplications.size()];
        for (int i = 0; i < iApplications.size(); i++)
        {
            uids[i] = ((ApplicationInfo)iApplications.elementAt(i)).getUid();
        }
        return uids;
    }

    /**
     * Get icon path for converted icon for speccified
     * application in the suite.
     */
    public String getRegisteredIconPath(int aAppIndex)
    {
        if (getApplications() == null || getApplications().size() <= aAppIndex)
        {
            return null;
        }
        String convertedIconPath =
            ((ApplicationInfo)getApplications().elementAt(aAppIndex))
            .getConvertedIconPath();
        if (convertedIconPath == null)
        {
            return null;
        }
        return FileRoots.getRegisteredIconDir(
                   FileUtils.getDrive(getRootDir())) +
               getUid().toString() + FileUtils.pathSeparator() +
               FileUtils.getName(convertedIconPath);
    }

    /**
     * Tells if application suite is trusted.
     *
     * @return true if suite is trusted, false otherwise
     */
    public boolean isTrusted()
    {
        return iTrusted;
    }

    /**
     * Get icon path for converted icon.
     */
    public String getConvertedIconPath()
    {
        return iConvertedIconPath;
    }

    /**
     * Set icon path for converted icon.
     */
    public void setConvertedIconPath(String aConvertedIconPath)
    {
        iConvertedIconPath = aConvertedIconPath;
    }

    /**
     * Get application installation group.
     */
    public String getInstallationGroup()
    {
        return iInstallationGroup;
    }

    /**
     * Set application installation group.
     *
     * @param aInstallationGroup application installation group
     */
    public void setInstallationGroup(String aInstallationGroup)
    {
        iInstallationGroup = aInstallationGroup;
    }

    /**
     * Set suite uid.
     *
     * @param aUid suite uid
     */
    public void setUid(Uid aUid)
    {
        iUid = aUid;
    }

    /**
     * Set component id.
     */
    public void setComponentId(ComponentId aCid)
    {
        iCid = aCid;
    }

    /**
     * Set suite name.
     *
     * @param aName suite name
     */
    public void setName(String aName)
    {
        iName = aName;
    }

    /**
     * Set suite vendor.
     *
     * @param aVendor suite vendor
     */
    public void setVendor(String aVendor)
    {
        iVendor = aVendor;
    }

    /**
     * Set suite version.
     *
     * @param aVersion suite version
     */
    public void setVersion(Version aVersion)
    {
        iVersion = aVersion;
    }

    /**
     * Set jad path.
     *
     * @param aJadPath jad path
     */
    public void setJadPath(String aJadPath)
    {
        iJadPath = aJadPath;
    }

    /**
     * Set jar path.
     *
     * @param aJarPath jar path
     */
    public void setJarPath(String aJarPath)
    {
        iJarPath = aJarPath;
    }

    /**
     * Set jad url.
     *
     * @param aJadUrl jad url
     */
    public void setJadUrl(String aJadUrl)
    {
        iJadUrl = aJadUrl;
    }

    /**
     * Set jar url.
     *
     * @param aJarUrl jar url
     */
    public void setJarUrl(String aJarUrl)
    {
        iJarUrl = aJarUrl;
    }

    /**
     * Set suite root directory.
     *
     * @param aRootDir suite root directory
     */
    public void setRootDir(String aRootDir)
    {
        iRootDir = aRootDir;
    }

    /**
     * Set id for installation media.
     *
     * @param aMediaId id for installation media
     */
    public void setMediaId(int aMediaId)
    {
        iMediaId = aMediaId;
    }

    /**
     * Set application initial size.
     */
    public void setInitialSize(int aInitialSize)
    {
        iInitialSize = aInitialSize;
    }

    /**
     * Set OnScreenKeypad value.
     */
    public void setOnScreenKeypad(int aOnScreenKeypad)
    {
        iOnScreenKeypad = aOnScreenKeypad;
    }

    /**
     * Set content info.
     */
    public void setContentInfo(int aContentInfo)
    {
        iContentInfo = aContentInfo;
    }

    /**
     * Set content id.
     */
    public void setContentId(String aContentId)
    {
        iContentId = aContentId;
    }

    /**
     * Set access point for this suite.
     */
    public void setAccessPoint(String aAccessPoint)
    {
        iAccessPoint = aAccessPoint;
    }

    /**
     * Set suite attributes.
     *
     * @param Attributes suite attributes
     */
    public void setAttributes(Hashtable aAttributes)
    {
        iAttributes = aAttributes;
    }

    /**
     * Set attribute.
     *
     * @param aName attribute name
     * @param aAttribute attribute to be set
     */
    public void setAttribute(String aName, Attribute aAttribute)
    {
        iAttributes.put(aName, aAttribute);
    }

    /**
     * Set attribute value.
     *
     * @param aName attribute name
     * @param aValue attribute value
     */
    public void setAttribute(String aName, String aValue)
    {
        Attribute attr = (Attribute)iAttributes.get(aName);
        if (attr == null)
        {
            attr = new Attribute(aName, aValue);
        }
        else
        {
            attr = new Attribute(attr.getName(), aValue, attr.isTrusted());
        }
        iAttributes.put(aName, attr);
    }

    /**
     * Set suite applications.
     *
     * @param aApplications suite applications
     */
    public void setApplications(Vector aApplications)
    {
        iApplications = aApplications;
    }

    /**
     * Add suite application.
     *
     * @param aApplication application to be added
     */
    public void addApplication(ApplicationInfo aApplication)
    {
        iApplications.addElement(aApplication);
    }

    /**
     * Set flag telling if application suite is trusted.
     *
     * @param aTrusted true if suite is trusted, false otherwise
     */
    public void setTrusted(boolean aTrusted)
    {
        iTrusted = aTrusted;
    }

    /**
     * Set flag telling if application suite is preinstalled.
     *
     * @param aTrusted true if suite is preinstalled, false otherwise
     */
    public void setPreinstallation(boolean aPreinstallation)
    {
        if (aPreinstallation)
        {
            iPreinstallState = STATE_PREINSTALLED;
        }
        else
        {
            iPreinstallState = STATE_INSTALLED;
        }
    }

    /**
     * Calculates initial size from MIDlet-Data-Size and
     * MIDlet-Jar-Size attributes, or if they are not present
     * from jar file size.
     */
    public int calculateInitialSize()
    {
        int size = 0;
        String dataSize = getAttributeValue("MIDlet-Data-Size");
        String jarSize = getAttributeValue("MIDlet-Jar-Size");
        try
        {
            if (dataSize != null)
            {
                size += Integer.parseInt(dataSize);
            }
            if (jarSize != null)
            {
                size += Integer.parseInt(jarSize);
            }
        }
        catch (NumberFormatException nfe)
        {
            // ignore
        }
        if (size == 0 && getJarPath() != null)
        {
            // Attributes not present, get the jar file size.
            size = (int)FileUtils.getSize(getJarPath());
        }
        return size;
    }

    /**
     * Returns short string representation of this object.
     */
    public String toShortString()
    {
        StringBuffer buf = new StringBuffer("Suite:\n");
        buf.append("  Uid: ").append(getUid()).append("\n");
        buf.append("  Name: ").append(getName()).append("\n");
        buf.append("  Vendor: ").append(getVendor()).append("\n");
        buf.append("  Version: ").append(getVersion()).append("\n");
        buf.append("  Trusted: ").append(isTrusted()).append("\n");
        buf.append("  RootDir: ").append(getRootDir()).append("\n");
        if (iApplications != null)
        {
            buf.append("Applications:\n");
            for (int i = 0; i < iApplications.size(); i++)
            {
                ApplicationInfo appInfo = (ApplicationInfo)iApplications.elementAt(i);
                buf.append("  ").append(appInfo).append("\n");
            }
        }
        return buf.toString();
    }

    /**
     * Returns string representation of this object.
     */
    public String toString()
    {
        StringBuffer buf = new StringBuffer("Suite:\n");
        buf.append("  Uid: ").append(getUid()).append("\n");
        if (getComponentId() != null)
        {
            buf.append("  ComponentId: ").append(getComponentId()).append(", ");
        }
        buf.append("  Name: ").append(getName()).append("\n");
        buf.append("  Vendor: ").append(getVendor()).append("\n");
        buf.append("  Version: ").append(getVersion()).append("\n");
        buf.append("  Trusted: ").append(isTrusted()).append("\n");
        buf.append("  ContentInfo: ").append(getContentInfo()).append("\n");
        buf.append("  ContentId: ").append(getContentId()).append("\n");
        buf.append("  RootDir: ").append(getRootDir()).append("\n");
        buf.append("  JadPath: ").append(getJadPath()).append("\n");
        buf.append("  JarPath: ").append(getJarPath()).append("\n");
        buf.append("  JadUrl: ").append(getJadUrl()).append("\n");
        buf.append("  JarUrl: ").append(getJarUrl()).append("\n");
        buf.append("  AccessPoint: ").append(getAccessPoint()).append("\n");
        buf.append("  MediaId: ").append(getMediaId()).append("\n");
        buf.append("  InitialSize: ").append(getInitialSize()).append("\n");
        buf.append("  PreinstallState: ").append(iPreinstallState).append("\n");
        buf.append("  OnScreenKeypad: ").append(getOnScreenKeypad()).append("\n");
        buf.append("  ConvertedIcon: ").append(getConvertedIconPath()).append("\n");
        buf.append("  Group: ").append(getInstallationGroup()).append("\n");
        if (iApplications != null)
        {
            buf.append("Applications:\n");
            for (int i = 0; i < iApplications.size(); i++)
            {
                ApplicationInfo appInfo = (ApplicationInfo)iApplications.elementAt(i);
                buf.append("  ").append(appInfo).append("\n");
            }
        }
        if (iAttributes != null)
        {
            buf.append("Attributes:\n");
            Enumeration e = iAttributes.elements();
            while (e.hasMoreElements())
            {
                Attribute attr = (Attribute)e.nextElement();
                buf.append("  ").append(attr).append("\n");
            }
        }
        return buf.toString();
    }
}
