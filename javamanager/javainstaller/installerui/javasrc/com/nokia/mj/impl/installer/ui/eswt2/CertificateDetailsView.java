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


package com.nokia.mj.impl.installer.ui.eswt2;

import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.security.midp.common.SigningCertificate;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;

/**
 * CertificateDetailsView displays certificate details to the user.
 */
public class CertificateDetailsView extends ConfirmationViewBase
{
    private SigningCertificate[] iCerts = null;
    private int iCertIndex = 0;

    /** Constructor */
    protected CertificateDetailsView()
    {
        super();
    }

    /** Constructor */
    protected CertificateDetailsView(
        InstallerUiEswt aInstaller, Composite aParent, String aTitle,
        SigningCertificate[] aCerts, int aCertIndex)
    {
        super(aInstaller, aParent, 1, true);
        iCerts = aCerts;
        iCertIndex = aCertIndex;
        setTitle(aTitle);
        setCommands(null, InstallerUiTexts.get(InstallerUiTexts.CLOSE));
    }

    /**
     * This method is called once before view is opened.
     * Inheriting class must implement this method.
     */
    protected void createView()
    {
        if (iCertIndex == -1)
        {
            createUncertifiedView();
        }
        else
        {
            createCertificateDetailsView();
        }
    }

    /**
     * Creates certificate details view for uncertified appliction.
     */
    private void createUncertifiedView()
    {
        int horizontalSpan = getColumns();
        int labelStyle = SWT.WRAP;

        // Add title label.
        Label detailsLabel = createLabel(
                                 InstallerUiTexts.get(InstallerUiTexts.NOT_CERTIFIED_TITLE),
                                 horizontalSpan, labelStyle);
        setCssId(detailsLabel, "heading");

        // Add domain category label.
        createLabel("", horizontalSpan, labelStyle);
        Label domainLabel = createLabel(
            InstallerUiTexts.get(
                InstallerUiTexts.DOMAIN,
                new String[] {
                    InstallerUiTexts.get(InstallerUiTexts.DOMAIN_UTP)
                }),
            horizontalSpan, labelStyle);
        createLabel("", horizontalSpan, labelStyle);

        // Add warning label.
        String appName = "";
        if (iInstallerUi.getInstallInfo() != null)
        {
            appName = iInstallerUi.getInstallInfo().getName();
        }
        Label warningLabel = createLabel(
                                 InstallerUiTexts.get(InstallerUiTexts.NOT_CERTIFIED_INFO,
                                                      new String[] { appName }),
                                 horizontalSpan, labelStyle);

        setCssId(warningLabel, "detailsWarningLabel");
    }

    /**
     * Creates certificate details view.
     */
    private void createCertificateDetailsView()
    {
        int horizontalSpan = getColumns();
        int labelStyle = SWT.WRAP;

        // Add title label.
        Label detailsLabel = createLabel
                             (InstallerUiTexts.get
                              (InstallerUiTexts.CERTIFICATE_TITLE),
                              horizontalSpan, labelStyle);
        setCssId(detailsLabel, "heading");

        SigningCertificate certificate = iCerts[iCertIndex];

        Label domainLabel = createLabel
                            (InstallerUiTexts.get
                             (InstallerUiTexts.DOMAIN,
                              new String[] { getDomainCategoryString(certificate) }),
                             horizontalSpan, labelStyle);

        Label issuerLabel = createLabel
                            (InstallerUiTexts.get
                             (InstallerUiTexts.ISSUER,
                              new String[] { certificate.getFormattedIssuer() }),
                             horizontalSpan, labelStyle);

        Label subjectLabel = createLabel
                             (InstallerUiTexts.get
                              (InstallerUiTexts.SUBJECT,
                               new String[] { certificate.getFormattedSubject() }),
                              horizontalSpan, labelStyle);

        Label organizationLabel = createLabel
                             (InstallerUiTexts.get
                              (InstallerUiTexts.ORGANIZATION,
                               new String[] { certificate.getOrganization() }),
                              horizontalSpan, labelStyle);

        Label validFromLabel = createLabel
                               (InstallerUiTexts.get
                                (InstallerUiTexts.VALID_FROM,
                                 new Object[] { certificate.getValidFrom() }),
                                horizontalSpan, labelStyle);

        Label validUntilLabel = createLabel
                                (InstallerUiTexts.get
                                 (InstallerUiTexts.VALID_UNTIL,
                                  new Object[] { certificate.getValidUntil() }),
                                 horizontalSpan, labelStyle);

        Label serialNumberLabel = createLabel
                                  (InstallerUiTexts.get
                                   (InstallerUiTexts.SERIAL_NUMBER,
                                    new String[] { certificate.getSerialNumber() }),
                                   horizontalSpan, labelStyle);

        Label fingerprintLabel = createLabel
                                 (InstallerUiTexts.get
                                  (InstallerUiTexts.FINGERPRINT,
                                   new String[] { certificate.getFormattedFingerprint() }),
                                  horizontalSpan, labelStyle);
    }

    /**
     * This method is called after user has answered
     * to confirmation.
     * Inheriting class must implement this method.
     */
    protected void getDataFromView()
    {
        // nop
    }

    /**
     * Returns SWT style for this view.
     */
    protected int getStyle()
    {
        return SWT.V_SCROLL;
    }

    /**
     * Returns localised domain category text for the given certificate.
     */
    private String getDomainCategoryString(SigningCertificate aCertificate)
    {
        String result = "";
        String domainCategory = aCertificate.getProtectionDomain().getCategory();
        if (domainCategory.equals(ApplicationInfo.MANUFACTURER_DOMAIN))
        {
            result = InstallerUiTexts.get(InstallerUiTexts.DOMAIN_MANU);
        }
        else if (domainCategory.equals(ApplicationInfo.OPERATOR_DOMAIN))
        {
            result = InstallerUiTexts.get(InstallerUiTexts.DOMAIN_OPER);
        }
        else if (domainCategory.equals(ApplicationInfo.IDENTIFIED_THIRD_PARTY_DOMAIN))
        {
            result = InstallerUiTexts.get(InstallerUiTexts.DOMAIN_ITP);
        }
        else if (domainCategory.equals(ApplicationInfo.UNIDENTIFIED_THIRD_PARTY_DOMAIN))
        {
            result = InstallerUiTexts.get(InstallerUiTexts.DOMAIN_UTP);
        }
        return result;
    }
}
