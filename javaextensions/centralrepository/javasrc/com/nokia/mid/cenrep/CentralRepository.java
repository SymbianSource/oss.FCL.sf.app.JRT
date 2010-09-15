/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mid.cenrep;

import com.nokia.mj.impl.cenrep.CentralRepositoryImpl;

/**
 * The <code>CentralRepository</code> is class to manage application
 * settings and share data with other runtimes. In S60 this API can be used
 * to set and get values in Symbian Central Repositories.<P>
 *
 * Example of usage:<P>
 *
 * <code style="white-space: pre">
 * class CentralRepositoryExample {
 *     private static final String EXAMPLE_REPOSITORY= "0x20000000";
 *     private static final String EXAMPLE_KEY1 = "0x00000001";
 *     private static final String EXAMPLE_KEY2 = "0x00000002";
 *
 *     public modifySetting() throws CentralRepositoryException {
 *         CentralRepository cenrep = CentralRepository.open(EXAMPLE_REPOSITORY);
 *         try {
 *             String value1 = cenrep.getString(EXAMPLE_KEY1);
 *             int value2 = cenrep.getInt(EXAMPLE_KEY2);

 *             cenrep.setString(EXAMPLE_KEY1, value3);
 *             cenrep.setInt(EXAMPLE_KEY2, value4);
 *         }
 *         catch (CentralRepositoryException cre) {
 *             System.out.println(cre);
 *         }
 *         finally {
 *             cenrep.close();
 *         }
 *     }
 * }</code>
 */
public abstract class CentralRepository
{

    /**
     *  Hidden default constructor.
     */
    protected CentralRepository()
    {
    }

    /**
     * Opens central repository.
     *
     * @param repositoryId it is platform specific and in S60 it is
     *        Symbian Central Repository UID.
     * @return An instance of CentralRepository class
     *         for accessing a repository.
     * @throws CentralRepositoryException if openning fails.
     */
    static public CentralRepository open(String repositoryId)
    throws CentralRepositoryException
    {
        return CentralRepositoryImpl.open(repositoryId);
    }

    /**
     * Closes central repository. If get or set methods are used after
     * close operation, exception will be thrown.
     */
    public abstract void close()
    throws CentralRepositoryException;

    /**
     * Returns string stored in given key.
     *
     * @param key the key of setting to be read.
     * @return the value of the setting if it is the string.
     * @throws CentralRepositoryException if key is not found or
     *         stored data is not string.
     */
    public abstract String getString(String key)
    throws CentralRepositoryException;

    /**
     * Returns integer stored in given key.
     *
     * @param key the key of setting to be read.
     * @return the value of the setting if it is an integer.
     * @throws CentralRepositoryException if key is not found or
     *         stored data is not integer.
     */
    public abstract int getInt(String key)
    throws CentralRepositoryException;

    /** Stores string value in key.
     *
     * @param key the key of setting to be stored.
     * @param value the string value of the setting to be stored.
     * @throws CentralRepositoryException if string value cannot be stored.
     */
    public abstract void setString(String key, String value)
    throws CentralRepositoryException;

    /** Stores integer value in key.
     *
     * @param key the key of setting to be stored.
     * @param value the integer value of the setting to be stored.
     * @throws CentralRepositoryException if integer value cannot be stored.
     */
    public abstract void setInt(String key, int value)
    throws CentralRepositoryException;
}
