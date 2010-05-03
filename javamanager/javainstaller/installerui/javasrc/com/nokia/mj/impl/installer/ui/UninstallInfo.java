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
 * Uninstallation info for JavaInstaller UI.
 */
public class UninstallInfo
{
    /** Uid for the suite to be uninstalled. */
    private Uid iUid = null;
    /** Name for the suite to be uninstalled. */
    private String iName = null;
    /** Vendor for the suite to be uninstalled. */
    private String iVendor = null;
    /** Version for the suite to be uninstalled. */
    private String iVersion = null;
    /** Path to the icon file for the suite to be uninstalled. */
    private String iIconPath = null;
    /** InputStream where the UI can read the suite icon. */
    private InputStream iIconInputStream = null;

    /** Optional application specific uninstallation confirmation message.
        Can be null. */
    private String iDeleteConfirm = null;

    /** Applications in this suite. */
    private ApplicationInfo[] iApplications = null;

    /** Possible signing certificates for this suite.
        If no certificates are present, suite is untrusted. */
    private SigningCertificate[] iCertificates = null;

    /** Constructor. */
    private UninstallInfo()
    {
    }

    /** Constructor. */
    public UninstallInfo(Uid aUid, String aName,
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

    /** Get Optional application specific uninstallation
        confirmation message. */
    public String getDeleteConfirm()
    {
        return iDeleteConfirm;
    }

    /** Set Optional application specific uninstallation
        confirmation message. */
    public void setDeleteConfirm(String aDeleteConfirm)
    {
        iDeleteConfirm = aDeleteConfirm;
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

    public String toString()
    {
        StringBuffer buf = new StringBuffer("InstallInfo\n");
        buf.append("Uid: ").append(getUid()).append("\n");
        buf.append("Name: ").append(getName()).append("\n");
        buf.append("Vendor: ").append(getVendor()).append("\n");
        buf.append("Version: ").append(getVersion()).append("\n");
        buf.append("IconPath: ").append(getIconPath()).append("\n");
        if (getDeleteConfirm() != null)
        {
            buf.append("DeleteConfirm: ").append(getDeleteConfirm()).append("\n");
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
