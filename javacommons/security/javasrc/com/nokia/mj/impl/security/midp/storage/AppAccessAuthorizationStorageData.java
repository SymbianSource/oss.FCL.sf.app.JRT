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

package com.nokia.mj.impl.security.midp.storage;

import com.nokia.mj.impl.security.midp.common.AuthorizationRestriction;

public final class AppAccessAuthorizationStorageData
{
    public AppAccessAuthorizationStorageData(String vendorName, String domainName, String[] signersList)
    {
        this.vendorName = vendorName;
        this.domainName = domainName;
        this.signersList = signersList;
    }

    public String getVendorName()
    {
        return vendorName;
    }

    public String getDomainName()
    {
        return domainName;
    }

    public String[] getSignersList()
    {
        return signersList;
    }

    private String vendorName;
    private String domainName;
    private String[] signersList;
}

