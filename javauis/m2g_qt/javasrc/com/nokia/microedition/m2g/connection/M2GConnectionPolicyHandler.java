/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.microedition.m2g.connection;

/**
 * Interface for connection policy handler
 */
public interface M2GConnectionPolicyHandler
{
    /**
     * Get connection policy
     * @return Policy
     */
    public M2GConnectionPolicy getConnectionPolicy();

    /**
     * Set connection policy
     * @param aConnectionPolicy Policy
     */
    public void setConnectionPolicy(M2GConnectionPolicy aConnectionPolicy);
}
