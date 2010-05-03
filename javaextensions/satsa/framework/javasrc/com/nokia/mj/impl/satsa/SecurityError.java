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


package com.nokia.mj.impl.satsa;

/**
 * Native error codes which causes certain Java exception to be thrown.
 */
public final class SecurityError
{
    /**
     * Do not allow construction.
     */
    private SecurityError()
    {
    }

    // CONSTANTS

    public static final int ERR_ILLEGAL_STATE                  = -3001;
    public static final int ERR_SIGNATURE                      = -3002;
    public static final int ERR_BAD_BADDING                    = -3003;
    public static final int ERR_ILLEGAL_BLOCK_SIZE             = -3004;
    public static final int ERR_NO_SUCH_PADDING                = -3005;
    public static final int ERR_NO_SUCH_ALGORITHM              = -3006;
    public static final int ERR_INVALID_KEY                    = -3007;
    public static final int ERR_INVALID_ALGORITHM_PARAMETER    = -3008;
    public static final int ERR_SHORT_BUFFER                   = -3009;
    public static final int ERR_DIGEST                         = -3010;
    public static final int ERR_ILLEGAL_STATE_INPUT            = -3011;

    public static final int ERR_NO_MEMORY                      = -4;
    public static final int SUCCESS                            =  0;
    public static final int ERR_NOT_SUPPORTED                  = -5;
    public static final int ERR_ARGUMENT                       = -6;

}