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


package com.nokia.mj.impl.installer.ui;

import com.nokia.mj.impl.security.midp.common.SigningCertificate;
import com.nokia.mj.impl.utils.Uid;

import java.io.InputStream;

/**
 * Installation info for JavaInstaller UI.
 */
public class InstallInfo
{
    /** Unknown drive type. */
    public static final int DRIVE_TYPE_UNKNOWN = 0;
    /** Device memory. */
    public static final int DRIVE_TYPE_PHONE_MEMORY = 1;
    /** Physically removable memory card. */
    public static final int DRIVE_TYPE_MEMORY_CARD = 2;
    /** Internal mass storage is e.g. hard disk or
        memory card that cannot be removed. */
    public static final int DRIVE_TYPE_INTERNAL_MASS_STORAGE = 3;

    /** Uid for the suite to be installed. */
    private Uid iUid = null;
    /** Name for the suite to be installed. */
    private String iName = null;
    /** Vendor for the suite to be installed. */
    private String iVendor = null;
    /** Version for the suite to be installed. */
    private String iVersion = null;
    /** Path to the icon file for the suite to be installed. */
    private String iIconPath = null;
    /** InputStream where the UI can read the suite icon. */
    private InputStream iIconInputStream = null;
    /** Suite jar size as indicated in Jad/Manifest attributes.
        If size is not specified, value is -1. */
    private long iJarSize = -1;
    /** Suite data size as indicated in Jad/Manifest attributes.
        If size is not specified, value is -1. */
    private long iDataSize = -1;

    /** Old suite information in case of an update.
        If this is not an update, this member is null. */
    private String iOldVersion = null;

    /** Applications in this suite. */
    private ApplicationInfo[] iApplications = null;

    /** Default installation drive. InstallerUi must set this
        to the value chosen by user. */
    private int iDriveId = -1;
    /** Ids for possible installation drives. */
    private int[] iDriveIds = null;
    /** Drive types for possible installation drives. */
    private int[] iDriveTypes = null;
    /** Ampunt of free disk space for possible installation drives. */
    private long[] iDriveFreeSpaces = null;

    /** Flag telling is user data should be retained in update.
        If this member is null, user should not be asked.
        If user is asked, then InstallerUi must set this
        to the value chosen by user.*/
    private Boolean iRetainData = null;

    /** Possible signing certificates for this suite.
        If no certificates are present, suite is untrusted. */
    private SigningCertificate[] iCertificates = null;

    /** Constructor. */
    private InstallInfo()
    {
    }

    /** Constructor. */
    public InstallInfo(Uid aUid, String aName,
                       String aVendor, String aVersion)
    {
        iUid = aUid;
        iName = aName;
        iVendor = aVendor;
        iVersion = aVersion;
    }

    /** Get suite uid. */
    public Uid getUid()
    {
        return iUid;
    }

    /** Get suite name. */
    public String getName()
    {
        return iName;
    }

    /** Get suite vendor. */
    public String getVendor()
    {
        return iVendor;
    }

    /** Get suite version. */
    public String getVersion()
    {
        return iVersion;
    }

    /** Get suite icon path. */
    public String getIconPath()
    {
        return iIconPath;
    }

    /** Get Jar size. */
    public long getJarSize()
    {
        return iJarSize;
    }

    /** Set Jar size. */
    public void setJarSize(long aSize)
    {
        iJarSize = aSize;
    }

    /** Get Data size. */
    public long getDataSize()
    {
        return iDataSize;
    }

    /** Set data size. */
    public void setDataSize(long aSize)
    {
        iDataSize = aSize;
    }

    /** Set suite icon path. */
    public void setIconPath(String aIconPath)
    {
        iIconPath = aIconPath;
    }

    /** Get suite icon InputStream. */
    public InputStream getIconInputStream()
    {
        return iIconInputStream;
    }

    /** Set suite icon InputStream. */
    public void setIconInputStream(InputStream aIconInputStream)
    {
        iIconInputStream = aIconInputStream;
    }

    /** Get old suite version in case of an update. */
    public String getOldVersion()
    {
        return iOldVersion;
    }

    /** Set old suite version in case of an update. */
    public void setOldVersion(String aOldVersion)
    {
        iOldVersion = aOldVersion;
    }

    /** Get applications. */
    public ApplicationInfo[] getApplications()
    {
        return iApplications;
    }

    /** Set applications. */
    public void setApplications(ApplicationInfo[] aApplications)
    {
        iApplications = aApplications;
    }

    /** Get possible certificates. */
    public SigningCertificate[] getCertificates()
    {
        return iCertificates;
    }

    /** Set possible certificates. */
    public void setCertificates(SigningCertificate[] aCertificates)
    {
        iCertificates = aCertificates;
    }

    /** Get installation drive id. */
    public int getDriveId()
    {
        return iDriveId;
    }

    /** Set installation drive id. */
    public void setDriveId(int aDriveId)
    {
        iDriveId = aDriveId;
    }

    /** Get possible installation drive ids. */
    public int[] getDriveIds()
    {
        return iDriveIds;
    }

    /** Get possible installation drive types. */
    public int[] getDriveTypes()
    {
        return iDriveTypes;
    }

    /** Get amount of free space on installation drives. */
    public long[] getDriveFreeSpaces()
    {
        return iDriveFreeSpaces;
    }

    /** Set possible installation drive ids and types. */
    public void setDriveIdsTypes(int[] aIds, int[] aTypes)
    {
        iDriveIds = aIds;
        iDriveTypes = aTypes;
    }

    /** Set amount of free spaces in installation drives. */
    public void setDriveFreeSpaces(long[] aFreeSpaces)
    {
        iDriveFreeSpaces = aFreeSpaces;
    }

    /** Get flag telling is user data should be retained in upgrade.
        If null, then user should not be asked.*/
    public Boolean getRetainData()
    {
        return iRetainData;
    }

    /** Set flag telling is user data should be retained in upgrade.
        If null, then user should not be asked.*/
    public void setRetainData(boolean aRetainData)
    {
        iRetainData = new Boolean(aRetainData);
    }

    public String toString()
    {
        StringBuffer buf = new StringBuffer("InstallInfo\n");
        buf.append("Uid: ").append(getUid()).append("\n");
        buf.append("Name: ").append(getName()).append("\n");
        buf.append("Vendor: ").append(getVendor()).append("\n");
        buf.append("Version: ").append(getVersion()).append("\n");
        if (getOldVersion() != null)
        {
            buf.append("OldVersion: ").append(getOldVersion()).append("\n");
        }
        buf.append("IconPath: ").append(getIconPath()).append("\n");
        buf.append("JarSize: ").append(getJarSize()).append("\n");
        buf.append("DataSize: ").append(getDataSize()).append("\n");
        if (getRetainData() != null)
        {
            buf.append("RetainData: ").append(getRetainData()).append("\n");
        }
        buf.append("DriveId: ").append(iDriveId).append("\n");
        for (int i = 0; iDriveIds != null && i < iDriveIds.length; i++)
        {
            buf.append("Drives[").append(i).append("]: ")
            .append(iDriveIds[i]).append(", DriveType: ")
            .append(iDriveTypes[i]);
            if (iDriveFreeSpaces != null && i < iDriveFreeSpaces.length)
            {
                buf.append(", FreeSpace: ").append(iDriveFreeSpaces[i]);
            }
            buf.append("\n");
        }
        for (int i = 0; iApplications != null && i < iApplications.length; i++)
        {
            buf.append("Applications[").append(i).append("]: ")
            .append(iApplications[i]).append("\n");
        }
        for (int i = 0; iCertificates != null && i < iCertificates.length; i++)
        {
            buf.append("Certificates[").append(i).append("]: ")
            .append(iCertificates[i]).append("\n");
        }
        return buf.toString();
    }
}
