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

package com.nokia.mj.impl.security.utils;

public final class TelUtils
{
    public static NetworkCodes getNetworkCodes()
    {
        return _getNetworkCodes();
    }

    public static class NetworkCodes
    {
        public String mcc = null;
        public String mnc = null;

        public NetworkCodes(String mcc, String mnc)
        {
            this.mcc = mcc;
            this.mnc = mnc;
        }
    }

    private static native NetworkCodes _getNetworkCodes();
}
