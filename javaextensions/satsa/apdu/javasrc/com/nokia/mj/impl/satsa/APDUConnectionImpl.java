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
* Description:  Implements APDUConnection
 *
*/


package com.nokia.mj.impl.satsa;

import java.io.IOException;
import java.io.InterruptedIOException;
import javax.microedition.io.ConnectionNotFoundException;
import javax.microedition.apdu.APDUConnection;
import com.nokia.mj.impl.satsa.SecurityError;

import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.satsa.APDUError;
import com.nokia.mj.impl.satsa.APDUPermission;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.utils.Logger;

// to fetch hash of root cert,end-entity cert and protection domain
import com.nokia.mj.impl.security.midp.authentication.AuthenticationModule;
import com.nokia.mj.impl.security.midp.common.SigningInfo;
import com.nokia.mj.impl.security.midp.common.ProtectionDomain;
import com.nokia.mj.impl.security.common.Certificate;

/**
 * Refer JSR-177 Specification for details
 */
public final class APDUConnectionImpl implements APDUConnection
{
    // CONSTANTS

    // Enumeration flag for ChangePin method
    protected final static int ENUMERATION_CHANGE_PIN = 0;

    // Enumeration flag for DisablePin method
    protected final static int ENUMERATION_DISABLE_PIN = 1;

    // Enumeration flag for EnablePin method
    protected final static int ENUMERATION_ENABLE_PIN = 2;

    // Enumeration flag for EnterPin method
    protected final static int ENUMERATION_ENTER_PIN = 3;

    // Enumeration flag for SAT connection
    protected final static int ENUMERATION_SAT = 0;

    // Enumeration flag for AID connection
    protected final static int ENUMERATION_AID = 1;

    // Native handle. Set in constructor.
    private final int iHandle;

    // Response bytes will be saved to this
    private byte[] iResponseBytes;

    // Error code will be saved to this
    private int iErrorCode;

    // Dummy object for usage of PIN methods, exchangeApdu method and
    // operationComplete method locks
    protected final Object iLock;

    // Dummy object for usage of close method lock
    protected final Object iCloseLock;

    static
    {
        try
        {
            Jvm.loadSystemLibrary("javasatsa");
        }
        catch (Exception e)
        {

        }

    }

    /**
    * Refer JSR-177 Specification for details
    */
    public APDUConnectionImpl(String aURI, int aMode, boolean aTimeouts)
    throws ConnectionNotFoundException, SecurityException, IOException,
                IllegalArgumentException
    {
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ APDUConnectionImpl::APDUConnectionImpl()");
        iLock = new Object();
        iCloseLock = new Object();

        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ APDUConnectionImpl: Create Apps Info object");
        // Get midlet related info
        ApplicationInfo appInfo = ApplicationInfo.getInstance();

        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ APDUConnectionImpl: get suite name");
        // Get midlet suite name
        String name = appInfo.getSuiteName();

        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ APDUConnectionImpl: get vendor name");
        // Get midlet vendor name
        String vendor = appInfo.getVendor();

        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ APDUConnectionImpl: get version");
        // Get midlet version
        String version = appInfo.getVersion();

        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ APDUConnectionImpl: get domain info");
        // Get midlet domain name
        String domain = appInfo.getProtectionDomain();

        // Get the Auth & Signing objects so that we can fetch
        // root certificate & signing certificate (if midlet is signed) info

        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ APDUConnectionImpl: Create Auth module instance");
        AuthenticationModule authModule = AuthenticationModule.getInstance();

        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ APDUConnectionImpl: get signing info");
        SigningInfo signingInfo = authModule.getSigningInfo(name, version,
                                  vendor);

        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ APDUConnectionImpl: get protection domain");
        ProtectionDomain protDomain = signingInfo.getProtectionDomain();

        String signingCertHash = "";
        String rootCertHash = "";
        if (domain.equals("MFD") || domain.equals("OPD")
                || domain.equals("ITPD"))
        {
            Logger.LOG(Logger.ESATSA, Logger.EInfo,
                       "+ APDUConnectionImpl: get signing cert");
            Certificate signingCertificate = signingInfo
                                             .getSigningCertificate();

            Logger.LOG(Logger.ESATSA, Logger.EInfo,
                       "+ APDUConnectionImpl: get signing cert hash");
            signingCertHash = signingCertificate.getFingerprint();

            Logger.LOG(Logger.ESATSA, Logger.EInfo,
                       "+ APDUConnectionImpl: get root cert");
            Certificate rootCertificate = signingInfo.getRootCertificate();

            Logger.LOG(Logger.ESATSA, Logger.EInfo,
                       "+APDUConnectionImpl:get certhash for domain" + domain);
            rootCertHash = rootCertificate.getFingerprint();
        }

        // create array for receiving type of connection
        int[] connTypeArray = new int[1];

        // Create native object
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ APDUConnectionImpl: Create the connection object");
        iHandle = _create(aURI, name, vendor, version, domain, rootCertHash,
                          signingCertHash, connTypeArray);

        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ APDUConnectionImpl: Connection object created");
        // Check is handle created or is error code returned
        if (iHandle <= SecurityError.SUCCESS)
        {
            Logger.LOG(Logger.ESATSA, Logger.EInfo,
                       "+ APDUConnectionImpl:: Failed to create a handle");
            try
            {
                APDUError.check(iHandle, true, // check illegal argument error
                                true, // check security error
                                true, // check connection not found error
                                false,// do not check interrupted IO error
                                true);// throw IO exception as a default
            }
            catch (IllegalArgumentException iae)
            {
                throw new IllegalArgumentException("Creating APDU connection failed: " + iae.getMessage());
            }
            catch (SecurityException se)
            {
                throw new SecurityException("Creating APDU connection failed: " + se.getMessage());
            }
            catch (ConnectionNotFoundException cne)
            {
                throw new ConnectionNotFoundException("Creating APDU connection failed: " + cne.getMessage());
            }
            catch (IOException ie)
            {
                throw new IOException("Creating APDU connection failed: " + ie.getMessage());
            }

        }

        // check permissions
        // first element of array is returned connection type
        int connType = connTypeArray[0];
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ APDUConnectionImpl: Check Permission");
        if (connType == ENUMERATION_SAT)
        {
            Logger.LOG(Logger.ESATSA, Logger.EInfo,
                       "+ APDUConnectionImpl::Check againast SAT permission");
            // Check Permission for SAT connection
            ApplicationUtils appUtils = ApplicationUtils.getInstance();
            APDUPermission permission = new APDUPermission("apdu://*", "sat");
            appUtils.checkPermission(permission);
            Logger.LOG(Logger.ESATSA, Logger.EInfo,
                       "+ APDUConnectionImpl: SAT is allowed");
        }
        else
        {
            Logger.LOG(Logger.ESATSA, Logger.EInfo,
                       "+ APDUConnectionImpl: Check against AID permission");
            // Check Permission for AID connection
            ApplicationUtils appUtils = ApplicationUtils.getInstance();
            APDUPermission permission = new APDUPermission("apdu://*", "aid");
            appUtils.checkPermission(permission);
            Logger.LOG(Logger.ESATSA, Logger.EInfo,
                       "+ APDUConnectionImpl: AID is allowed");
        }

        // open connection
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ APDUConnectionImpl: Open the connection");
        int err = _open(iHandle);

        // check error codes
        if (err < SecurityError.SUCCESS)
        {
            Logger.LOG(Logger.ESATSA, Logger.EInfo,
                       "+ APDUConnectionImpl: Failed to open connection");
            if (err == SecurityError.ERR_NO_MEMORY)
            {
                throw new OutOfMemoryError();
            }
            try
            {
                APDUError.check(err, true, // check illegal argument error
                                true, // check security error
                                true, // check connection not found error
                                false,// do not check interrupted IO error
                                true);// throw IO exception as a default
            }
            catch (IllegalArgumentException iae)
            {
                throw new IllegalArgumentException("Opening APDU Connection failed: " + iae.getMessage());
            }
            catch (SecurityException se)
            {
                throw new SecurityException("Opening APDU Connection failed: " + se.getMessage());
            }
            catch (ConnectionNotFoundException cne)
            {
                throw new ConnectionNotFoundException("Opening APDU Connection failed: " + cne.getMessage());
            }
            catch (IOException ie)
            {
                throw new IOException("Opening APDU Connection failed: " + ie.getMessage());
            }

        }

        // else Native object is OK

    }

    /**
     * From APDUConnection.
     *
     * @see javax.microedition.apdu
     */
    public synchronized byte[] changePin(int pinID) throws IOException
    {
        byte[] response;
        try
        {
            response = doPinMethod(ENUMERATION_CHANGE_PIN, pinID);
        }
        catch (SecurityException se)
        {
            throw new SecurityException("ChangePin failed: " + se.getMessage());
        }
        catch (InterruptedIOException iie)
        {
            throw new InterruptedIOException("ChangePin failed: " + iie.getMessage());
        }
        catch (IOException ie)
        {
            throw new IOException("ChangePin failed: " + ie.getMessage());
        }
        return response;
    }

    /**
     * From APDUConnection.
     *
     * @see javax.microedition.apdu
     */
    public synchronized byte[] disablePin(int pinID) throws IOException
    {
        byte[] response;
        try
        {
            response = doPinMethod(ENUMERATION_DISABLE_PIN, pinID);
        }
        catch (SecurityException se)
        {
            throw new SecurityException("DisablePin failed: " + se.getMessage());
        }
        catch (InterruptedIOException iie)
        {
            throw new InterruptedIOException("DisablePin failed: " + iie.getMessage());
        }
        catch (IOException ie)
        {
            throw new IOException("DisablePin failed: " + ie.getMessage());
        }
        return response;
    }

    /**
     * From APDUConnection.
     *
     * @see javax.microedition.apdu
     */
    public synchronized byte[] enablePin(int pinID) throws IOException
    {
        byte[] response;
        try
        {
            response = doPinMethod(ENUMERATION_ENABLE_PIN, pinID);
        }
        catch (SecurityException se)
        {
            throw new SecurityException("EnablePin failed: " + se.getMessage());
        }
        catch (InterruptedIOException iie)
        {
            throw new InterruptedIOException("EnablePin failed: " + iie.getMessage());
        }
        catch (IOException ie)
        {
            throw new IOException("EnablePin failed: " + ie.getMessage());
        }
        return response;
    }

    /**
     * From APDUConnection.
     *
     * @see javax.microedition.apdu
     */
    public synchronized byte[] enterPin(int pinID) throws IOException
    {
        byte[] response;
        try
        {
            response = doPinMethod(ENUMERATION_ENTER_PIN, pinID);
        }
        catch (SecurityException se)
        {
            throw new SecurityException("EnterPin failed: " + se.getMessage());
        }
        catch (InterruptedIOException iie)
        {
            throw new InterruptedIOException("EnterPin failed: " + iie.getMessage());
        }
        catch (IOException ie)
        {
            throw new IOException("EnterPin failed: " + ie.getMessage());
        }
        return response;

    }

    /**
     * From APDUConnection.
     *
     * @see javax.microedition.apdu
     */
    public synchronized byte[] exchangeAPDU(byte[] commandAPDU)
    throws IOException
    {
        synchronized (iLock)
        {
            if (commandAPDU == null)
            {
                // Command APDU cannot be null.
                throw new IllegalArgumentException("ExchangeAPDU failed: " + APDUError.TXT_IA_PARAM_NULL);
            }

            int err = _exchangeAPDU(iHandle, commandAPDU);
            byte[] response;
            try
            {
                response = finalizeMethod(err, true, // check illegal argument error
                                          false);// do not check cancel error
            }
            catch (IllegalArgumentException iae)
            {
                throw new IllegalArgumentException("ExchangeAPDU failed: " + iae.getMessage());
            }
            catch (SecurityException se)
            {
                throw new SecurityException("ExchangeAPDU failed: " + se.getMessage());
            }
            catch (InterruptedIOException iie)
            {
                throw new InterruptedIOException("ExchangeAPDU failed: " + iie.getMessage());
            }
            catch (IOException ie)
            {
                throw new IOException("ExchangeAPDU failed: " + ie.getMessage());
            }
            return response;
        }
    }

    /**
     * From APDUConnection.
     *
     * @see javax.microedition.apdu
     */
    public synchronized byte[] getATR()
    {
        // error array contains only one value, which indicates the error code
        int[] errArray = new int[1];
        byte[] nativeResponse = _getATR(iHandle, errArray);

        // first element of error array is returned error code
        int err = errArray[0];

        // check error codes
        if (err < SecurityError.SUCCESS)
        {
            if (err == SecurityError.ERR_NO_MEMORY)
            {
                throw new OutOfMemoryError();
            }
            // we will return null if some other error code arrives
            nativeResponse = null;
        }

        // null if the card has been removed or the connection has been closed.
        return copyToJava(nativeResponse);
    }

    /**
     * From APDUConnection.
     *
     * @see javax.microedition.apdu
     */
    public synchronized byte[] unblockPin(int blockedPinID, int unblockingPinID)
    throws IOException
    {
        synchronized (iLock)
        {
            int err = _unblockPin(iHandle, blockedPinID, unblockingPinID);

            byte[] response;
            try
            {
                response = finalizeMethod(err, false, // do not check illegal argument error
                                          true);                     // check cancel error
            }
            catch (SecurityException se)
            {
                throw new SecurityException("UnblockPin failed: " + se.getMessage());
            }
            catch (InterruptedIOException iie)
            {
                throw new InterruptedIOException("UnblockPin failed: " + iie.getMessage());
            }
            catch (IOException ie)
            {
                throw new IOException("UnblockPin failed: " + ie.getMessage());
            }
            return response;
        }
    }

    /**
     * From APDUConnection.
     *
     * @see javax.microedition.apdu
     */
    public void close() throws IOException
    {
        // close can be called safely if some other method is active
        synchronized (iCloseLock)
        {
            int err = _close(iHandle);
            // check error codes
            if (err < SecurityError.SUCCESS)
            {
                if (err == SecurityError.ERR_NO_MEMORY)
                {
                    throw new OutOfMemoryError();
                }
                // throw IOException in all other cases
                try
                {
                    APDUError.check(err, false, // do not check illegal argument error
                                    false, // do not check security error
                                    false, // do not check connection not found error
                                    false, // do not check interrupted IO error
                                    true);// throw IO exception as a default
                }
                catch (IOException iex)
                {
                    throw new IOException("Closing connection failed: " + iex.getMessage());
                }


            }
        }
    }

    /**
     * Sets responsebytes and possible error code and notifies wait loop that
     * operation is ready
     *
     * @param aOperationID
     *            Id of the returned operation
     * @param aErrorCode
     *            Error code on error situations
     * @param aResponseBytes
     *            Response data
     */
    public void operationComplete(int aOperationID, int aErrorCode,
                                  byte[] aResponseBytes)
    {
        // set error code
        iErrorCode = aErrorCode;

        // set response data
        iResponseBytes = copyToJava(aResponseBytes);
        // notify that response is ready

        synchronized (iLock)
        {
            iLock.notifyAll();
        }

    }

    // PRIVATE METHODS
    /**
     * Handels PIN method calls, error checking and array copy.
     */
    private byte[] doPinMethod(int pinMethodType, int pinID) throws IOException
    {
        synchronized (iLock)
        {
            int err = _doPinMethod(iHandle, pinMethodType, pinID);

            return finalizeMethod(err, false, // do not check illegal argument
                                  // error
                                  true);// check cancel error

        }
    }

    /**
     * Checks error codes depending on paremeter, starts wait and checks error
     * code when wait is released. Throws correct exception or returns response
     * bytes or null depending on situation.
     */
    private byte[] finalizeMethod(int aErr, boolean aCheckIllegalArg,
                                  boolean aCheckCancelErr) throws IOException
    {
        // check error codes (if leave occured in native side before
        // asynchronous call was done
        APDUError.check(aErr, aCheckIllegalArg, true, // check security error
                        false,// do not check connection not found error
                        true, // check interrupted IO error
                        true);// throw IO exception as a default

        // waits until operationComplete method notifies this lock object
        try
        {
            iLock.wait();
        }
        catch (InterruptedException e)
        {
            throw new IOException(e.getMessage());
        }
        // error code is set on operationComplete method
        if (iErrorCode < SecurityError.SUCCESS)
        {
            if (iErrorCode == SecurityError.ERR_NO_MEMORY)
            {
                throw new OutOfMemoryError();
            }
            if ((aCheckCancelErr) && (iErrorCode == APDUError.ERR_CANCEL))
            {
                return null;
            }
            // throw IO exception in all other cases
            APDUError.check(iErrorCode, false, // do not check illegal argument
                            // error
                            false, // do not check security error
                            false, // do not check connection not found error
                            false, // do not check interrupted IO error
                            true);// throw IO exception as a default
        }
        return iResponseBytes;
    }

    /**
     * Copies byte array to java.
     *
     * @param aSource
     *            Data which will be copied
     * @return Created byte array
     */
    private byte[] copyToJava(byte[] aSource)
    {
        // will return null, if source is null
        byte[] destination = null;
        if (aSource != null)
        {
            // Copy response to new buffer,it is not a reference to a native
            // buffer.
            destination = new byte[aSource.length];

            if (destination != null)
            {
                // src, src_position, dst, dst_position, length
                System.arraycopy(aSource, 0, // from beginning
                                 destination, 0, // from beginning
                                 destination.length);
            }
            // will return null, if creation failed and destination is null
        }
        return destination;
    }

    // NATIVE METHODS

    /**
     * Creates native instance or returns error if construction fails. Calls
     * also native instance open method for opening connection.
     */
    private native int _create(String aURI, String aName, String aVendor,
                               String aVersion, String aDomain, String aRootCertHash,
                               String aSigningCertHash, int[] aType);

    /**
     * Open the connection.
     *
     * @param aHandle
     *            Handle to corresponding native object.
     * @return Error code
     */
    static private native int _open(int aHandle);

    /**
     * Common implementation of PIN method calls.
     *
     */
    static private native int _doPinMethod(int aHandle, int aPinMethodType,
                                           int aPinID);

    /**
     * Exchanges an APDU command with a smart card application.
     */
    static private native int _exchangeAPDU(int aHandle, byte[] aCommandAPDU);

    /**
     * Exchanges an APDU command with a smart card application.
     */
    static private native byte[] _getATR(int aHandle, int[] aError);

    /**
     *
     */
    static private native int _unblockPin(int aHandle, int aBlockedPinID,
                                          int aUnblockingPinID);

    /**
     * Close the connection.
     */
    static private native int _close(int aHandle);
}
