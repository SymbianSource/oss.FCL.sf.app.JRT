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

package com.nokia.mj.impl.cenrep;

import com.nokia.mid.cenrep.CentralRepository;
import com.nokia.mid.cenrep.CentralRepositoryException;
import com.nokia.mj.impl.gcf.utils.NativeError;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.rt.support.Jvm;

/**
 * CentralRepositoryImpl implements the functionality specified in
 * com.nokia.mid.cenrep.CentralRepository class.
 */
public class CentralRepositoryImpl extends CentralRepository
{

    static
    {
        try
        {
            Jvm.loadSystemLibrary("javacentrep");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EJavaCentrep,
                        "Unable to load javacentrep dll.");
        }
    }

    private static final int INITIAL = 0;

    private static final int OPEN = 1;

    private static final int CLOSED = 2;

    /*
     * Central Repository API Function server.
     */
    private int iFunctionSourceHandle;

    /**
     * Handle to the native side peer object.
     */
    private int iCenrepHandle = 0;

    /**
     * Repository ID
     */
    private String iRepositoryId = null;

    private Finalizer iFinalizer;

    private int iState;

    /**
     *  Hidden default constructor.
     */
    private CentralRepositoryImpl()
    {
    }

    /**
     * Hidden constructor.
     *
     * @param repositoryId it is platform specific and in S60 it is
     *        Symbian Central Repository UID.
     */
    private CentralRepositoryImpl(String repositoryId)
    throws CentralRepositoryException
    {
        checkAccess();
        iState = INITIAL;
        int cenrepUid = CentralRepositoryUid.getIntValue(repositoryId);
        this.iRepositoryId = repositoryId;
        iFinalizer = registerFinalize();

        iFunctionSourceHandle = _createFunctionSource();
        iCenrepHandle = _createNativePeer(iFunctionSourceHandle, cenrepUid);
        iState = OPEN;
    }

    /**
     * See class CentralRepository for comments.
     */
    static public CentralRepository open(String repositoryId)
    throws CentralRepositoryException
    {
        return new CentralRepositoryImpl(repositoryId);
    }

    /**
     * See class CentralRepository for comments.
     */
    public void close() throws CentralRepositoryException
    {
        synchronized (this)
        {

            if (iState != CLOSED && iState == OPEN)
            {
                iState = CLOSED;
                _close(iFunctionSourceHandle, iCenrepHandle);
                _dispose(iFunctionSourceHandle, iCenrepHandle);
            }
            iRepositoryId = null;
        }
    }

    /**
     * See class CentralRepository for comments.
     */
    public String getString(String key)
    throws CentralRepositoryException
    {
        synchronized (this)
        {

            if (iState == CLOSED)
            {
                throw new CentralRepositoryException("Connection Already Closed");
            }
            long cenrepKey = CentralRepositoryKey.getLongValue(key);
            String value = _getString(iFunctionSourceHandle, iCenrepHandle, cenrepKey);
            return value;
        }
    }

    /**
     * See class CentralRepository for comments.
     */
    public int getInt(String key)
    throws CentralRepositoryException
    {
        synchronized (this)
        {

            if (iState == CLOSED)
            {
                throw new CentralRepositoryException("Connection Already Closed");
            }
            long cenrepKey = CentralRepositoryKey.getLongValue(key);
            int res = _getInt(iFunctionSourceHandle, iCenrepHandle, cenrepKey);
            return res;
        }
    }

    /**
     * See class CentralRepository for comments.
     */
    public void setString(String key, String value)
    throws CentralRepositoryException
    {
        synchronized (this)
        {

            if (iState == CLOSED)
            {
                throw new CentralRepositoryException("Connection Already Closed");
            }
            long cenrepKey = CentralRepositoryKey.getLongValue(key);
            if (value == null)
            {
                throw new CentralRepositoryException("Value is null");
            }
            _setString(iFunctionSourceHandle, iCenrepHandle, cenrepKey, value);
        }
    }

    /**
     * See class CentralRepository for comments.
     */
    public  void setInt(String key, int value)
    throws CentralRepositoryException
    {
        synchronized (this)
        {

            if (iState == CLOSED)
            {
                throw new CentralRepositoryException("Connection Already Closed");
            }
            long cenrepKey = CentralRepositoryKey.getLongValue(key);
            _setInt(iFunctionSourceHandle, iCenrepHandle, cenrepKey, value);
        }
    }

    /**
     * Registers with Finalizer to call a method when the object gets collected
     * by GC
     *
     * @return Finalizer object that will be notified when GC happens
     */
    private Finalizer registerFinalize()
    {
        return new Finalizer()
        {
            public void finalizeImpl()
            {
                registeredFinalize();
            }
        };
    }

    /**
     * Called when the object is finalized by the garbage collector.
     */
    final void registeredFinalize()
    {
        try
        {
            close();
        }
        catch (CentralRepositoryException e)
        {
            // Ignore
        }
    }

    /**
     * Creates a native side function source.
     *
     * @return handle to the native function source.
     */
    private native int _createFunctionSource();

    /**
     * Create native side peer of this Java class. It opens a repository.
     *
     * @param FunctionSource Handle handle to the native function source.
     * @param arepositoryId is platform specific and in S60 it is
     *        Symbian Central Repository UID.
     * @return handle to the native side peer.
     * @return Symbian OS error code, if openning fails.
     */
    private native int _createNativePeer(
        int aFunctionSourceHandle,
        int repositoryId);

    /**
     * Closes a repository.
     *
     * @param FunctionSourceHandle handle to the native function source.
     * @param cenrepHandle handle to the native side peer object.
     * @return Symbian OS error code, if closing fails.
     */
    private native void _close(int aFunctionSourceHandle, int cenrepHandle);

    /**
     * Dispose native side resources owned by this class.
     *
     * @param FunctionSourceHandle handle to the native function source.
     * @param cenrepHandle handle to the native side peer object.
     */
    private native void _dispose(int aFunctionSourceHandle, int cenrepHandle);

    /**
     * Returns string stored in given key.
     *
     * @param FunctionSourceHandle handle to the native function source.
     * @param cenrepHandle handle to the native side peer object.
     * @param key the key of setting to be read.
     * @param value returns the value of the setting if it is the string.
     * @return Symbian OS error code, if key is not found or
     *         stored data is not string.
     */
    private native String _getString(
        int aFunctionSourceHandle,
        int cenrepHandle,
        long key);

    /**
     * Returns integer stored in given key.
     *
     * @param FunctionSourceHandle handle to the native function source.
     * @param cenrepHandle handle to the native side peer object.
     * @param key the key of setting to be read.
     * @param value the value of the setting if it is an integer.
     * @return Symbian OS error code, if key is not found or
     *         stored data is not integer.
     */
    private native int _getInt(
        int aFunctionSourceHandle,
        int cenrepHandle,
        long key);

    /** Stores string value in key.
     *
     * @param FunctionSourceHandle handle to the native function source.
     * @param cenrepHandle handle to the native side peer object.
     * @param key the key of setting to be stored.
     * @param value the string value of the setting to be stored.
     * @return Symbian OS error code, if string value cannot be stored.
     */
    private native void _setString(
        int aFunctionSourceHandle,
        int cenrepHandle,
        long key,
        String value);

    /** Stores integer value in key.
     *
     * @param FunctionSourceHandle handle to the native function source.
     * @param cenrepHandle handle to the native side peer object.
     * @param key the key of setting to be stored.
     * @param value the integer value of the setting to be stored.
     * @return Symbian OS error code, if integer value cannot be stored.
     */
    private native void _setInt(
        int aFunctionSourceHandle,
        int cenrepHandle,
        long key,
        int value);

   

    /**
     * Checks if MIDlet is permited to access the central repository.
     * Only MIDlets in manufacturer or operator domain are allowed.
     * @throws CentralRepositoryException if MIDlet is not in manufacturer or
     *        operator domain.
     */
    private void checkAccess()
    throws SecurityException
    {

        ApplicationInfo appInfo = ApplicationInfo.getInstance();
        String protectionDomain = appInfo.getProtectionDomain();

        if (protectionDomain.equals(ApplicationInfo.MANUFACTURER_DOMAIN) == false &&
                protectionDomain.equals(ApplicationInfo.OPERATOR_DOMAIN) == false)
        {
            Logger.ELOG(Logger.EJavaCentrep, "Protection Domain: " + protectionDomain +
                        ", Access denied");
            throw new SecurityException("Access denied!");
        }
    }
}
