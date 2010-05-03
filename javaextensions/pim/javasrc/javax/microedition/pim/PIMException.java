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
* Description:  PIM exception.
 *
*/


// PACKAGE
package javax.microedition.pim;

// CLASS DEFINITION
/**
 * PIM API 1.0 compliant PIMException.
 */
public class PIMException extends java.lang.Exception
{
    public static final int FEATURE_NOT_SUPPORTED = 0;

    public static final int GENERAL_ERROR = 1;

    public static final int LIST_CLOSED = 2;

    public static final int LIST_NOT_ACCESSIBLE = 3;

    public static final int MAX_CATEGORIES_EXCEEDED = 4;

    public static final int UNSUPPORTED_VERSION = 5;

    public static final int UPDATE_ERROR = 6;

    private int iExceptionReason;

    public PIMException()
    {
        super();
        iExceptionReason = GENERAL_ERROR;
    }

    public PIMException(String aDetailMessage)
    {
        super(aDetailMessage);
        iExceptionReason = GENERAL_ERROR;
    }

    public PIMException(String aDetailMessage, int aReason)
    {
        super(aDetailMessage);
        iExceptionReason = aReason;
    }

    public int getReason()
    {
        return iExceptionReason;
    }
}