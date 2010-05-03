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


package java.security;

import com.nokia.mj.impl.security.MessageDigestImpl;

/**
 * This is package private MessageDigest implementation.
 */
class PrivMessageDigestImpl extends java.security.MessageDigest
{
    /**
     *
     * @param aDigest
     *            passed to super class
     */
    PrivMessageDigestImpl(MessageDigestImpl aDigest)
    {
        super(aDigest);
    }
}
