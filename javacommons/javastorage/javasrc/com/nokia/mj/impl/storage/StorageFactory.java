/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.storage;

/**
 * StorageFactory is used to create new session to Java Storage. Each session
 * can handle one connection to storage.
 */
public final class StorageFactory
{
    /**
     * Create storage session. Each session can handle one connection
     * to storage.
     *
     * @return session to JavaStorage API.
     * @throws StorageException if session creation fails.
     */
    public static StorageSession createSession() throws StorageException
    {
        return (StorageSession) new StorageSessionImpl();
    }
}
