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

package com.nokia.mj.impl.bluetooth;

import java.io.InputStream;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Vector;
import java.util.Hashtable;
import java.lang.RuntimeException;
import java.security.AccessControlException;
import javax.microedition.io.Connector;

import javax.bluetooth.BluetoothStateException;
import javax.bluetooth.DataElement;
import javax.bluetooth.DeviceClass;
import javax.bluetooth.DiscoveryAgent;
import javax.bluetooth.DiscoveryListener;
import javax.bluetooth.LocalDevice;
import javax.bluetooth.RemoteDevice;
import javax.bluetooth.ServiceRegistrationException;
import javax.bluetooth.ServiceRecord;
import javax.bluetooth.UUID;

import com.intel.bluetooth.BlueCoveImpl;
import com.intel.bluetooth.BluetoothConnectionParams;
import com.intel.bluetooth.BluetoothConnectionNotifierParams;
import com.intel.bluetooth.BluetoothConsts;
import com.intel.bluetooth.BluetoothStack;
import com.intel.bluetooth.DeviceInquiryRunnable;
import com.intel.bluetooth.DeviceInquiryThread;
import com.intel.bluetooth.RemoteDeviceHelper;
import com.intel.bluetooth.SearchServicesRunnable;
import com.intel.bluetooth.SearchServicesThread;
import com.intel.bluetooth.ServiceRecordImpl;
import com.intel.bluetooth.Utils;

import com.nokia.mj.impl.properties.bluetooth.BtDynamicPropertyHandler;
import com.nokia.mj.impl.rt.support.ShutdownListener;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.gcf.PushValidator;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.bluetooth.utils.BluetoothStateCheck;
import com.nokia.mj.impl.bluetooth.utils.DebugLog;
import com.nokia.mj.impl.bluetooth.BluetoothStackConsts;

/**
 * New native stack support should ONLY implement this interface. No other
 * classes should ideally be changed except BlueCoveImpl where the instance of
 * new class should be created.
 *
 * <p>
 * <b><u>Your application should not use this class directly.</u></b>
 */

public class BluetoothStackS60 implements BluetoothStack,
        DeviceInquiryRunnable, SearchServicesRunnable
{
    /*
     * All these version information are realted to Bluecove. Nothing related to
     * JSR-82 or native BT Stack.
     */
    private static final int VERSION_MAJOR_1 = 2;

    private static final int VERSION_MAJOR_2 = 1;

    private static final int VERSION_MINOR = 0;

    private static final int VERSION_BUILD = 51;

    /*
     *
     */
    private static final int DATA_EOF = -1;

    private static final int RECEIVE_MTU_MAX = 672;

    /*
     * Members specific to operations of JSR 82
     */
    private long iFunctionServer;

    /*
     * Discovery Agent Specific
     */
    private static final int SDP_IDLE = 0;

    private static final int SDP_DEVICE_INQUIRY = 1;

    private static final int SDP_SERVICE_SEARCH = 2;

    private static final int SDP_POPULATE_RECORD = 3;

    private int iServiceDiscoveryStatus;

    private int iDeviceDiscoveryStatus;

    private long iDiscoveryAgentHandle;

    /*
     * Device Discovery Specific
     */
    private DiscoveryListener iDiscoveryListener;

    private DeviceInquiryThread iDeviceInquiryThread;

    /*
     * Service Search Specific
     */
    private static final int UUID_SIZE_IN_BYTES = 16;

    private SearchServicesThread iServiceSearchThread;

    private Hashtable iInputStreamHash;

    private boolean iServiceClassBitsModified;

    public BluetoothStackS60()
    {
        DebugLog.debug("+ BluetoothStackS60: Constructor");
        iServiceClassBitsModified = false;
        iDiscoveryAgentHandle = 0;
        iDeviceDiscoveryStatus = SDP_IDLE;
        iServiceDiscoveryStatus = SDP_IDLE;
        DebugLog.debug("- BluetoothStackS60: Constructor");
    }

    /**
     * Checks for power on and discoverability of the device. If Bluetooth is
     * not switched ON currently, it will prompt for user permission to switch
     * ON bluetooth. And if user denies that, then this function will throw
     * BluetoothStateException; otherwise, it will turn ON the bluetooth.
     *
     * If Bluetooth device is discoverable by all, it will prompt for user
     * permission to change the visibility of the device. And if user denies
     * that, then this function will throw BluetoothStateException; otherwise,
     * it will change the visibility such that this device is discoverable by
     * all.
     *
     * @param turnOnVisibility
     *            true to check bluetooth visibility of the device.
     *
     *            NOTE: The prompt is not displayed in case the application is
     *            manufacturer domain.
     *
     * @throws BluetoothStateException
     *             in case bluetooth could not be switched on
     */
    private synchronized void checkBluetoothRadio(boolean turnOnVisibility)
    throws BluetoothStateException
    {
        // Check Power on and Discoverability mode.
        try
        {
            // Check for power on now.
            BluetoothStateCheck stateChecker = (BluetoothStateCheck) new BTPlatformControl();

            String appName = ApplicationInfo.getInstance().getName();

            boolean ready = true;
            if (!stateChecker.checkDevicePowerOn(appName, false))
            {
                ready = false;
            }

            if (turnOnVisibility)
            {
                if (!stateChecker.checkDeviceDiscoverability(appName, false))
                {
                    ready = false;
                }
            }

            if (!ready)
            {
                throw new BluetoothStateException();
            }
        }
        catch (Exception ex)
        {
            throw new BluetoothStateException("Unable to switch bluetooth on");
        }
    }

    // Helper functions specific to Bluetooth Stack (Cleanup)

    private void dispose()
    {
        DebugLog.debug("+ BluetoothStackS60:dispose");

        if (SDP_DEVICE_INQUIRY == iDeviceDiscoveryStatus)
        {
            cancelInquiry(iDiscoveryListener);
        }
        if (SDP_SERVICE_SEARCH == iServiceDiscoveryStatus)
        {
            cancelServiceSearch(iServiceSearchThread.getTransID());
        }
        else if (SDP_POPULATE_RECORD == iServiceDiscoveryStatus)
        {
            cancelPopulateServicesRecordAttributeValues();
        }

        try
        {
            if (iServiceClassBitsModified)
            {
                setLocalDeviceServiceClasses(0);
            }
            iServiceClassBitsModified = false;
        }
        catch (Exception e)
        {
            // No need to handle anything here...
        }

        DebugLog.debug("- BluetoothStackS60:dispose");
    }

    // Native JNI function specific to Bluetooth Stack

    /**
     * Creates the function server.
     *
     * @return handle to the function server.
     */
    private native long _createFunctionServer();


    // ---------------------- Library initialization

    /**
     * Used by library initialization to detect if shared library already
     * loaded. The caller with catch UnsatisfiedLinkError and will load
     * libraries returned by requireNativeLibraries().
     */
    public boolean isNativeCodeLoaded()
    {
        return true;
    }

    /**
     * List the native libraries that need to be loaded.
     *
     * @see java.lang.System#loadLibrary(java.lang.String)
     * @return array of library names used by implementation.
     */
    public LibraryInformation[] requireNativeLibraries()
    {
        return null;
    }

    /**
     * Used to verify native library version. versionMajor1 * 1000000 +
     * versionMajor2 * 10000 + versionMinor * 100 + versionBuild
     *
     * @return Version number in decimal presentation. e.g. 2030407 for version
     *         2.3.4 build 7
     */
    public int getLibraryVersion() throws BluetoothStateException
    {
        return (VERSION_MAJOR_1 * 1000000 + VERSION_MAJOR_2 * 10000
                + VERSION_MINOR * 100 + VERSION_BUILD);
    }

    /**
     * Used if OS Supports multiple Bluetooth stacks 0x01 winsock; 0x02 widcomm;
     * 0x04 bluesoleil; 0x08 BlueZ; 0x10 OS X stack; 0x80 S60 Symbian stack;
     *
     * @return stackID
     */
    public int detectBluetoothStack()
    {
        return BlueCoveImpl.BLUECOVE_STACK_DETECT_S60;
    }

    /**
     *
     * @param aNativeDebugCallback
     *            DebugLog.class
     * @param aOn
     */
    public void enableNativeDebug(Class aNativeDebugCallback, boolean aOn)
    {
        // Debug is enabled at compiletime. So, nothing done here
    }

    /**
     * Call is made when we want to use this stack.
     */
    public void initialize() throws BluetoothStateException
    {
        iFunctionServer = _createFunctionServer();
        iInputStreamHash = new Hashtable();

        // Get the instance of ApplicationUtils
        ApplicationUtils appUtils = ApplicationUtils.getInstance();

        // Get the name of the application.
        appUtils.addShutdownListener(new ShutdownListener()
        {
            public void shuttingDown()
            {
                try
                {
                    dispose();// Do cleaning...
                }
                catch (Exception ex)
                {
                    // nothing to do
                }
            }
        });
    }

    public void destroy()
    {
        // Nothing to do here at this time :)
    }

    public String getStackID()
    {
        return BlueCoveImpl.STACK_S60;
    }

    /**
     * Called from long running native code to see if thread interrupted. If yes
     * InterruptedIOException would be thrown.
     *
     * @return true if interrupted
     */
    public boolean isCurrentThreadInterruptedCallback()
    {
        return false;
    }

    /**
     * @return implemented features, see FEATURE_* constants
     */
    public int getFeatureSet()
    {
        return FEATURE_L2CAP | FEATURE_SERVICE_ATTRIBUTES
               | FEATURE_SET_DEVICE_SERVICE_CLASSES;
    }

    // ---------------------- LocalDevice

    /**
     * Retrieves the Bluetooth address of the local device.
     *
     * @see javax.bluetooth.LocalDevice#getBluetoothAddress()
     */
    public String getLocalDeviceBluetoothAddress()
    throws BluetoothStateException
    {
        DebugLog.debug("+ BluetoothStackS60:getLocalDeviceBluetoothAddress");

        return com.nokia.mj.impl.bluetooth.BTPlatformControl
               ._getBluetoothAddress();
    }

    /**
     * Retrieves the name of the local device.
     *
     * @see javax.bluetooth.LocalDevice#getFriendlyName()
     */
    public String getLocalDeviceName()
    {
        DebugLog.debug("+ BluetoothStackS60:getLocalDeviceName");
        return com.nokia.mj.impl.bluetooth.BTPlatformControl
               ._getBluetoothName();
    }

    /**
     * Retrieves the class of the local device.
     *
     * @see javax.bluetooth.LocalDevice#getDeviceClass()
     */
    public DeviceClass getLocalDeviceClass()
    {
        int result = 0;

        DebugLog.debug("+ BluetoothStackS60:getLocalDeviceClass");

        result = com.nokia.mj.impl.bluetooth.BTPlatformControl
                 ._getBluetoothDeviceClass();
        return new DeviceClass(result);
    }

    /**
     * Implementation for local device service class.
     *
     * @see javax.bluetooth.ServiceRecord#setDeviceServiceClasses(int) and
     * @see javax.bluetooth.LocalDevice#updateRecord(javax.bluetooth.ServiceRecord)
     * @param aClassOfDevice
     */
    public void setLocalDeviceServiceClasses(int aServiceClassBits)
    {
        DebugLog.debug("+ BluetoothStackS60:setLocalDeviceServiceClasses "
                       + Integer.toHexString(aServiceClassBits));

        int ret = com.nokia.mj.impl.bluetooth.BTPlatformControl
                  ._setServiceClasses(aServiceClassBits);

        if (0 != ret)
        {
            throw new RuntimeException(
                "Unable to set device service class bits");
        }
        iServiceClassBitsModified = true;

        DebugLog.debug("- BluetoothStackS60:setLocalDeviceServiceClasses "
                       + ret);
    }

    /**
     * @see javax.bluetooth.LocalDevice#setDiscoverable(int)
     */
    public boolean setLocalDeviceDiscoverable(int aMode)
    throws BluetoothStateException
    {
        int mode = 0;

        DebugLog.debug("+ BluetoothStackS60:setLocalDeviceDiscoverable");

        return com.nokia.mj.impl.bluetooth.BTPlatformControl
               ._setBluetoothDiscoverable(aMode);

    }

    /**
     * @see javax.bluetooth.LocalDevice#getDiscoverable()
     */
    public int getLocalDeviceDiscoverable()
    {
        DebugLog.debug("+ BluetoothStackS60:getLocalDeviceDiscoverable");
        return com.nokia.mj.impl.bluetooth.BTPlatformControl
               ._getBluetoothDiscoverable();
    }

    /**
     * @see javax.bluetooth.LocalDevice#isPowerOn()
     */
    public boolean isLocalDevicePowerOn()
    {
        DebugLog.debug("+ BluetoothStackS60:isLocalDevicePowerOn");
        return BTPlatformControl._isPowerOn();
    }

    /**
     * @see javax.bluetooth.LocalDevice#getProperty(String)
     */
    public String getLocalDeviceProperty(String aProperty)
    {
        BtDynamicPropertyHandler btProperty = new BtDynamicPropertyHandler();

        DebugLog.debug("+ BluetoothStackS60:getLocalDeviceProperty");
        String propertyValue = btProperty.getProperty(aProperty);

        DebugLog.debug("- BluetoothStackS60: getLocalDeviceProperty "
                       + aProperty + ":" + propertyValue);

        return propertyValue;
    }

    // ---------------------- Remote Device authentication

    /**
     * Attempts to authenticate RemoteDevice. Return <code>false</code> if the
     * stack does not support authentication.
     *
     * @see javax.bluetooth.RemoteDevice#authenticate()
     */
    public boolean authenticateRemoteDevice(long aAddress) throws IOException
    {
        DebugLog.debug("+ BluetoothStackS60: authenticateRemoteDevice "
                       + aAddress);
        return _isAuthenticated(aAddress);
    }

    /**
     * @see javax.bluetooth.RemoteDevice#isAuthenticated()
     * @return null if not implemented
     */
    public Boolean isRemoteDeviceAuthenticated(long aAddress)
    {
        if (true == _isAuthenticated(aAddress))
            return Boolean.TRUE;

        return Boolean.FALSE;
    }

    private native boolean _isAuthenticated(long aAddress);

    /**
     * Sends an authentication request to a remote Bluetooth device. Non JSR-82,
     *
     * @param aAddress
     *            Remote Device address
     * @param aPasskey
     *            A Personal Identification Number (PIN) to be used for device
     *            authentication.
     * @return <code>true</code> if authentication is successful; otherwise
     *         <code>false</code>
     * @throws IOException
     *             if there are error during authentication.
     */
    public boolean authenticateRemoteDevice(long aAddress, String aPasskey)
    throws IOException
    {
        return false;
    }

    /**
     * Removes authentication between local and remote bluetooth devices. Non
     * JSR-82,
     *
     * @param aAddress
     *            Remote Device address authentication.
     * @throws IOException
     *             if there are error during authentication.
     */

    public void removeAuthenticationWithRemoteDevice(long aAddress)
    throws IOException
    {
        // Nothing to do. Non-JSR82 method
    }

    /**
     * @see javax.bluetooth.RemoteDevice#isTrustedDevice()
     * @return null if not implemented
     */
    public Boolean isRemoteDeviceTrusted(long aAddress)
    {
        // CreateRemoteDevice();
        if (true == _isTrusted(aAddress))
            return Boolean.TRUE;

        return Boolean.FALSE;
    }

    private native boolean _isTrusted(long aAddress);

    // Helper function specific to Discovery Agent.

    /**
     * Creates the native peer discovery agent, if it is not yet created.
     */
    private void CreateDiscoveryAgent()
    {
        if (iDiscoveryAgentHandle == 0)
        {
            iDiscoveryAgentHandle = _createNativeDiscoveryAgent(iFunctionServer);
        }
    }


    // Native JNI functions specific to Discovery Agent.

    /**
     * Creates the native peer discovery agent.
     *
     * @param aFunctionServer
     *            function server
     * @return handle to the native peer discovery agent
     */
    private native long _createNativeDiscoveryAgent(long aFunctionServer);


    // ---------------------- Device Inquiry

    /**
     * called by JSR-82 code Device Inquiry
     *
     * @see javax.bluetooth.DiscoveryAgent#startInquiry(int,
     *      javax.bluetooth.DiscoveryListener)
     */
    public boolean startInquiry(int aAccessCode, DiscoveryListener aListener)
    throws BluetoothStateException
    {
        DebugLog.debug("+ BluetoothStackS60:startInquiry AccessCode:"
                       + aAccessCode);

        if (SDP_IDLE != iDeviceDiscoveryStatus)
        {
            throw new BluetoothStateException("Bluetooth device is busy");
        }

        // Check to see if power is on.
        checkBluetoothRadio(false);

        // WE can safely store listener as member as there cannot be multiple
        // requests at a given point of time.
        iDiscoveryListener = aListener;

        CreateDiscoveryAgent();
        return DeviceInquiryThread.startInquiry(this, this, aAccessCode,
                                                aListener);
    }

    /**
     * called by JSR-82 code Device Inquiry
     *
     * @see javax.bluetooth.DiscoveryAgent#cancelInquiry(javax.bluetooth.DiscoveryListener)
     */
    public boolean cancelInquiry(DiscoveryListener aListener)
    {
        boolean retVal = false;

        if (aListener != iDiscoveryListener)
        {
            // TCK: DiscoveryAgent0034
            return false;
        }

        DebugLog.debug("+ BluetoothStackS60:cancelInquiry");
        if (SDP_DEVICE_INQUIRY == iDeviceDiscoveryStatus)
        {
            retVal = _cancelInquiry(iDiscoveryAgentHandle);
            iDeviceDiscoveryStatus = SDP_IDLE;
        }
        DebugLog.debug("- BluetoothStackS60:cancelInquiry");
        return retVal;
    }

    /**
     * called by implementation when device name is unknown or
     * <code>alwaysAsk</code> is <code>true</code>
     *
     * @see javax.bluetooth.RemoteDevice#getFriendlyName(boolean)
     */
    public String getRemoteDeviceFriendlyName(long aAddress) throws IOException
    {
        DebugLog.debug("+ BluetoothStackS60:getRemoteDeviceFriendlyName"
                       + " Device:" + aAddress);
        // Will be called only when we need to go to the native side to fetch
        // the device friendly name.
        CreateDiscoveryAgent();
        return _getRemoteDeviceFriendlyName(iDiscoveryAgentHandle, aAddress);
    }

    /**
     * @see javax.bluetooth.DiscoveryAgent#retrieveDevices(int)
     * @return null if not implemented
     */
    public RemoteDevice[] retrieveDevices(int aOption)
    {
        return null;
    }

    // ---------------------- DeviceInquiryRunnable

    /**
     * Common synchronous method called by DeviceInquiryThread. Should throw
     * BluetoothStateException only if it can't start Inquiry
     */
    public int runDeviceInquiry(DeviceInquiryThread aStartedNotify,
                                int aAccessCode, DiscoveryListener aListener)
    throws BluetoothStateException
    {
        // Should not return until inquiry has been completed
        DebugLog.debug("+ BluetootStackS60:runDeviceInquiry AccessCode:"
                       + aAccessCode);

        iDeviceInquiryThread = aStartedNotify;
        iDeviceDiscoveryStatus = SDP_DEVICE_INQUIRY;

        // Access code will be GIAC, LIAC or some valid number between 0x9E8B00
        // to 0x9E8B3F
        // NOTE: This is a synchronous call. Returns after Inquiry completion.
        int status = _runDeviceInquiry(aAccessCode, iDiscoveryAgentHandle);

        iDeviceDiscoveryStatus = SDP_IDLE;

        DebugLog.debug("- BluetootStackS60:runDeviceInquiry status:" + status);
        return status;
    }

    /**
     * Convenience method called from native code
     */
    public void deviceDiscoveredCallback(DiscoveryListener aListener,
                                         long aDeviceAddr, int aDeviceClass, String aDeviceName,
                                         boolean aPaired)
    {
    }

    // Helper functions specific to Device Inquiry

    /**
     * Device inquiry callback which then calls started Notify
     */
    public void deviceInquiryStartedCallback()
    {
        DebugLog.debug("+ BluetoothStackS60:deviceInquiryStartedCallback");
        RemoteDeviceHelper.clearDevicesCashed(this);
        iDeviceInquiryThread.deviceInquiryStartedCallback();
    }

    /**
     * Convenience method called from native code, whenever device found.
     *
     * @param deviceAddr
     *            Bluetooth address of the found device
     * @param deviceName
     *            Bluetooth device name of the found device
     * @param deviceClass
     *            Bluetooth device class of the found device
     */
    public void deviceDiscoveredCallBack(String aDeviceAddr,
                                         String aDeviceName, int aDeviceClass)
    {
        DebugLog.debug("+ BluetoothStackS60:deviceDiscoveredCallBack");
        long devAddr = Utils.hexStringToLong(aDeviceAddr);
        boolean paired = false;

        RemoteDevice remoteDevice = RemoteDeviceHelper.createRemoteDevice(this,
                                    devAddr, aDeviceName, paired);

        if (SDP_IDLE == iDeviceDiscoveryStatus || null == iDiscoveryListener)
            return;

        DeviceClass devClass = new DeviceClass(aDeviceClass);

        DebugLog.debug("  BluetoothStackS60:deviceDiscoveredCallback"
                       + " Device Name:" + aDeviceName);
        DebugLog.debug("  BluetoothStackS60:deviceDiscoveredCallback"
                       + " Device Addr:" + aDeviceAddr);
        DebugLog.debug("  BluetoothStackS60:deviceDiscoveredCallback"
                       + " deviceClass:" + devClass);
        iDiscoveryListener.deviceDiscovered(remoteDevice, devClass);
        DebugLog.debug("- BluetoothStackS60:deviceDiscoveredCallback");
    }

    // Native JNI functions specific to Device Discovery

    /**
     * Starts the device inquiry. If device found, then call made to
     * deviceDiscoveredCallback.
     *
     * @param aAccessCode
     *            inquiry access code (GIAC/LIAC)
     * @param aDiscAgentHandle
     *            handle to native discovery agent
     * @return device inquiry completion status :
     *         <p>
     *         <i><b> DiscoveryListener.INQUIRY_COMPLETED </b></i> on
     *         successfull completion
     *         </p>
     *         <p>
     *         <i><b> DiscoveryListener.INQUIRY_TERMINATED </b></i> on
     *         termination
     *         </p>
     *         <p>
     *         <i><b> DiscoveryListener.INQUIRY_ERROR </b></i> if any error
     *         occured
     *         </p>
     */
    private native int _runDeviceInquiry(int aAccessCode, long aDiscAgentHandle)
    throws BluetoothStateException;

    /**
     * Cancel the device inquiry, if it is running.
     *
     * @param aDiscoveryAgentHandle
     *            handle to native discovery agent.
     * @return true on success.
     */
    private native boolean _cancelInquiry(long aDiscoveryAgentHandle);

    /**
     * Retrieves the bluetooth device name of the remote device.
     *
     * @param aDiscoveryAgentHandle
     *            handle to native discovery agent.
     * @param aDeviceAddress
     *            Remote bluetooth device address.
     * @return string contains the name of the remote bluetooth device.
     */
    private native String _getRemoteDeviceFriendlyName(
        long aDiscoveryAgentHandle, long aDeviceAddress);

    // ---------------------- Service search

    /**
     * called by JSR-82 code Service search
     *
     * @see javax.bluetooth.DiscoveryAgent#searchServices(int[],UUID[],
     *      javax.bluetooth.RemoteDevice,javax.bluetooth.DiscoveryListener)
     */
    public int searchServices(int[] aAttrSet, UUID[] aUuidSet,
                              RemoteDevice aDevice, DiscoveryListener aListener)
    throws BluetoothStateException
    {
        DebugLog.debug("+ BluetoothStackS60:searchServices()");

        // Checking Listeners
        DebugLog.debug("  searchServices(): Checking Listeners");
        if (aListener == null || aDevice == null)
        {
            DebugLog.debug("  searchServices(): No Listeners!!");
            throw new NullPointerException("DiscoveryListener is null");
        }

        if (SDP_IDLE != iServiceDiscoveryStatus)
        {
            throw new BluetoothStateException("Bluetooth device is busy");
        }

        if (aAttrSet == null)
        {
            aAttrSet = new int[] { 0x0000, 0x0001, 0x0004 };
        }

        // Checking UUID Set
        DebugLog.debug("  searchServices(): Checking UUID Set");
        final int uuidLen = aUuidSet.length;

        if (uuidLen == 0)
        {
            DebugLog.debug("  searchServices():"
                           + " UUID set length can not be zero");
            throw new IllegalArgumentException(
                "UUID set length can not be zero");
        }

        // Making sure that there is no duplication of UUID in the UUID set
        for (int index = 0; index < uuidLen; index++)
        {
            final UUID uuid = aUuidSet[index];
            for (int index2 = index + 1; index2 < uuidLen; index2++)
            {
                if (uuid.equals(aUuidSet[index2]))
                {
                    DebugLog.debug("  searchServices():"
                                   + " Duplication of one or more UUIDs!!");
                    throw new IllegalArgumentException(
                        "One or more UUIDs are duplicated");
                }
            }
        }

        // Check to see if power is on.
        checkBluetoothRadio(false);

        // creating native discovery agent if it is not yet created....
        CreateDiscoveryAgent();

        DebugLog.debug("  searchServices():"
                       + " Calling SearchServicesThread.startSearchServices");
        return SearchServicesThread.startSearchServices(this, this, aAttrSet,
                aUuidSet, aDevice, aListener);
    }

    /**
     * called by JSR-82 code Service search
     *
     * @see javax.bluetooth.DiscoveryAgent#cancelServiceSearch(int)
     */
    public boolean cancelServiceSearch(int aTransID)
    {
        boolean retVal = false;
        if (SDP_SERVICE_SEARCH == iServiceDiscoveryStatus)
        {
            retVal = _cancelServiceSearch(iDiscoveryAgentHandle);
            iServiceDiscoveryStatus = SDP_IDLE;
        }
        return retVal;
    }

    /**
     * Called by ServiceRecord.populateRecord(int[] attrIDs) during Service
     * search.
     *
     * @see javax.bluetooth.ServiceRecord#populateRecord(int[])
     */
    public boolean populateServicesRecordAttributeValues(
        ServiceRecordImpl aServiceRecord, int[] aAttrIDs)
    throws IOException
    {
        if (SDP_IDLE != iServiceDiscoveryStatus)
        {
            throw new BluetoothStateException("Bluetooth device is busy");
        }

        // creating native discovery agent if it is not yet created....
        CreateDiscoveryAgent();

        iServiceDiscoveryStatus = SDP_POPULATE_RECORD;
        long remoteDeviceAddress = RemoteDeviceHelper.getAddress(aServiceRecord
                                   .getHostDevice());
        int retVal = _populateServiceRecordAttributeValues(remoteDeviceAddress,
                     aServiceRecord.getHandle(), aAttrIDs, aServiceRecord,
                     iDiscoveryAgentHandle);
        iServiceDiscoveryStatus = SDP_IDLE;
        if (retVal == BluetoothStackConsts.SERVICE_SEARCH_ERROR
                || retVal == BluetoothStackConsts.SERVICE_SEARCH_TERMINATED
                || retVal == BluetoothStackConsts.SERVICE_SEARCH_NO_RECORDS
                || retVal == BluetoothStackConsts.SERVICE_SEARCH_DEVICE_NOT_REACHABLE)
        {
            return false;
        }
        return true;
    }

    // ---------------------- SearchServicesRunnable

    /**
     * Common synchronous method called by SearchServicesThread. Should throw
     * BluetoothStateException only if it can't start Search
     *
     * Note : implementation limitation : only one searchSearvices at a time
     */
    public int runSearchServices(SearchServicesThread aSst, int[] aAttrSet,
                                 UUID[] aUuidSet, RemoteDevice aRemoteDevice,
                                 DiscoveryListener aListener) throws BluetoothStateException
    {
        DebugLog.debug("+ BluetoothStackS60:runSearchServices()");
        int status = 0;

        iServiceSearchThread = aSst;
        iServiceDiscoveryStatus = SDP_SERVICE_SEARCH;

        String remoteDevAddrStr = aRemoteDevice.getBluetoothAddress();
        long remoteDeviceAddress = Long.parseLong(remoteDevAddrStr, 16);
        byte[] uuidBytes = UUIDArrayToBytesArray(aUuidSet);

        // Blocked at native
        status = _runSearchServices(remoteDeviceAddress, uuidBytes, aSst
                                    .getAttrSet(), iDiscoveryAgentHandle);
        DebugLog.debug("  runSearchServices():After _runSearchServices:"
                       + " status: " + status);

        if (SDP_IDLE == iServiceDiscoveryStatus)
        {
            DebugLog.debug("  runSearchServices(): status = "
                           + "DiscoveryListener.SERVICE_SEARCH_TERMINATED");
            status = DiscoveryListener.SERVICE_SEARCH_TERMINATED;
        }
        else if (status == DiscoveryListener.SERVICE_SEARCH_COMPLETED)
        {
            Vector records = iServiceSearchThread.getServicesRecords();
            if (records.size() != 0)
            {
                DebugLog.debug("  runSearchServices(): status = "
                               + "DiscoveryListener.SERVICE_SEARCH_COMPLETED");
                DebugLog.debug("  runSearchServices(): Records Count:"
                               + records.size());
                ServiceRecord servRecordArray[] = new ServiceRecord[records
                        .size()];
                records.copyInto(servRecordArray);

                iServiceSearchThread.getListener().servicesDiscovered(
                    iServiceSearchThread.getTransID(), servRecordArray);
                status = DiscoveryListener.SERVICE_SEARCH_COMPLETED;
            }
            else
            {
                DebugLog.debug("  runSearchServices(): status = "
                               + "DiscoveryListener.SERVICE_SEARCH_NO_RECORDS");
                status = DiscoveryListener.SERVICE_SEARCH_NO_RECORDS;
            }
        }
        iServiceDiscoveryStatus = SDP_IDLE;
        return status;
    }

    /**
     * Called by native, whenever service found.
     *
     * @param handle
     * @return Returns a new Service record, in which native fills service
     *         details.
     */
    public ServiceRecordImpl createServiceRecord(long aHandle)
    {
        DebugLog.debug("+ BluetoothStackS60:createServiceRecord()");
        return new ServiceRecordImpl(this, iServiceSearchThread.getDevice(),
                                     aHandle);
    }

    /**
     * Called by native. This will add the service record to the list of service
     * records maintained by bluecove.
     *
     * @param serviceRecord
     *            Properly populated service record.
     */
    public void serviceDiscoveredCallback(ServiceRecordImpl aServiceRecord)
    {
        DebugLog.debug("+ BluetoothStackS60:serviceDiscoveredCallback()");
        iServiceSearchThread.addServicesRecords(aServiceRecord);
        return;
    }

    /**
     * Called by native, to inform Bluecove that service search successfully
     * started.
     *
     */
    public void serviceSearchStartedCallback()
    {
        DebugLog.debug("+ BluetoothStackS60:serviceSearchStartedCallback()");
        iServiceSearchThread.searchServicesStartedCallback();
    }

    /**
     * Internal function called whenever midlet closes while populate service
     * record is executing.
     *
     */
    private void cancelPopulateServicesRecordAttributeValues()
    {
        if (SDP_POPULATE_RECORD == iServiceDiscoveryStatus)
        {
            _cancelPopulateServicesRecordAttributeValues(iDiscoveryAgentHandle);
        }
        return;
    }

    // Helper functions specific to Sevice Search

    /**
     * Converts array of UUIDs to a Byte array.
     *
     * @param aUuidSet
     *            Array of UUIDs.
     * @return byte array contains all UUIDs provided by aUuidSet
     */
    private static byte[] UUIDArrayToBytesArray(UUID[] aUuidSet)
    {
        DebugLog.debug("+ BluetoothStackS60:convertUUIDArrayToBytesArray()");
        final int numUUIDs = aUuidSet.length;

        final int byteArrayLength = numUUIDs * UUID_SIZE_IN_BYTES;
        final byte[] uuidBytes = new byte[byteArrayLength];
        int bytesSoFar = 0; // Index in uuidBytes of where to add next UUID

        for (int index = 0; index < numUUIDs; index++)
        {
            final UUID uuid = aUuidSet[index];
            System.arraycopy(Utils.UUIDToByteArray(uuid), 0, uuidBytes,
                             bytesSoFar, UUID_SIZE_IN_BYTES);
            bytesSoFar += UUID_SIZE_IN_BYTES;
        }
        DebugLog.debug("- convertUUIDArrayToBytesArray()::uuids:", uuidBytes);
        return uuidBytes;
    }

    // Native JNI functions specific to Sevice Search

    /**
     * Cancels the service search, if it is running.
     *
     * @param aDiscoveryAgentHandle
     *            handle to native discovery agent.
     * @return True if success; otherwise False.
     */
    private native boolean _cancelServiceSearch(long aDiscoveryAgentHandle);

    /**
     * Retrieves the values by contacting the remote Bluetooth device for a set
     * of service attribute IDs of a service that is available on a Bluetooth
     * device.
     *
     * @param aRemoteDevice
     *            Address of remote bluetooth device.
     * @param aHandle
     *            Handle to service record.
     * @param aAttrIDs
     *            Integer Array contains set of attribute IDs.
     * @param aServiceRecordImpl
     *            current service record implementation.
     * @param aDiscoveryAgentHandle
     *            handle to native discovery agent.
     * @return Status of the populate record.
     * @throws IOException
     *             If the remote device cannot be reached; or if this
     *             ServiceRecord was deleted from the SDDB of the remote device
     */
    private native int _populateServiceRecordAttributeValues(
        long aRemoteDevice, long aHandle, int[] aAttrIDs,
        ServiceRecordImpl aServiceRecordImpl, long aDiscoveryAgentHandle)
    throws IOException;

    /**
     * Searches for services on a remote Bluetooth device that have all the
     * UUIDs specified in aUuidSet.
     *
     * @param aRemoteDevice
     *            Address of remote bluetooth device.
     * @param aUuidSet
     *            Byte Array contains set of UUIDs.
     * @param aAttrIDs
     *            Integer Array contains set of attribute IDs.
     * @param aDiscoveryAgentHandle
     *            handle to native discovery agent.
     * @return Status of service search.
     */
    private native int _runSearchServices(long aRemoteDevice, byte[] aUuidSet,
                                          int[] aAttrIDs, long aDiscoveryAgentHandle);

    /**
     * Cancels populate service record operation, if it is running.
     *
     * @param aDiscoveryAgentHandle
     *            handle to native discovery agent.
     */
    private native void _cancelPopulateServicesRecordAttributeValues(
        long aDiscoveryAgentHandle);

    /**
     * Used to create handle for
     * {@link com.intel.bluetooth.BluetoothRFCommClientConnection}
     *
     * @see javax.microedition.io.Connector#open(String, int, boolean)
     */
    public long connectionRfOpenClientConnection(
        BluetoothConnectionParams aParams) throws IOException
    {

        DebugLog.debug("BluetoothStackS60: connectionRfOpenClientConnection");
        DebugLog.debug("BluetoothStackS60: connectionRfOpenClientConnection:"
                       + " Address: " + aParams.address);
        DebugLog.debug("BluetoothStackS60: connectionRfOpenClientConnection:"
                       + " Channel: " + aParams.channel);
        DebugLog.debug("BluetoothStackS60: connectionRfOpenClientConnection:"
                       + " Authenticate: " + aParams.authenticate);
        DebugLog.debug("BluetoothStackS60: connectionRfOpenClientConnection:"
                       + " Encrypt: " + aParams.encrypt);
        ApplicationUtils appUtils = ApplicationUtils.getInstance();

        if (aParams.isBtGOEP)
        {
            BTOBEXPermission permission = new BTOBEXPermission("*", "client");
            appUtils.checkPermission(permission);
        }
        else
        {
            BTPermission permission = new BTPermission("*", "client");
            appUtils.checkPermission(permission);
        }

        checkBluetoothRadio(false);

        long nativeHandle = _rfClientOpen(aParams.address, aParams.channel,
                                          aParams.authenticate, aParams.encrypt, iFunctionServer);

        // We now open an Inputsream immediately so that we can start prefetch.
        BluetoothStreamer streamer = new BluetoothStreamer(this, nativeHandle);

        // This starts prefetch immediately
        InputStream stream = streamer.openInputStream();

        // Put it into hash table to that it can be retrieved based on the
        // native handle
        iInputStreamHash.put(new Long(nativeHandle),
                             new BluetoothPrefetchSource(streamer, stream));

        // Give bluecove the handle it needs
        return nativeHandle;
    }

    private native long _rfClientOpen(long aBtAddres, int aChannel,
                                      boolean aAuthenticate, boolean aEncrypt, long aServer);

    /**
     * @param handle
     * @param expected
     *            Value specified when connection was open
     *            ServiceRecord.xxAUTHENTICATE_xxENCRYPT
     * @return expected if not implemented by stack
     * @throws IOException
     *
     * @see javax.bluetooth.RemoteDevice#isAuthenticated()
     * @see javax.bluetooth.RemoteDevice#isEncrypted()
     */
    public int rfGetSecurityOpt(long aHandle, int aExpected) throws IOException
    {
        DebugLog.debug("BluetoothStackS60: rfGetSecurityOpt");
        return aExpected;
    }

    /**
     * @see com.intel.bluetooth.BluetoothRFCommClientConnection
     * @see com.intel.bluetooth.BluetoothRFCommConnection#close()
     * @see com.intel.bluetooth.BluetoothRFCommConnection#closeConnectionHandle(long)
     */
    public void connectionRfCloseClientConnection(long aHandle)
    throws IOException
    {
        DebugLog.debug("BluetoothStackS60:connectionRfCloseClientConnection():"
                       + " Closing Client");

        BluetoothPrefetchSource dataSource = (BluetoothPrefetchSource) iInputStreamHash
                                             .get(new Long(aHandle));
        InputStream source = dataSource.getInputStream();
        DebugLog.debug("BluetoothStackS60: connectionRfCloseClientConnection:"
                       + " After closing Streamer inputstream");
        try
        {
            source.close();
            source = null;
        }
        catch (Exception e)
        {
            DebugLog
            .debug("BluetoothStackS60: connectionRfCloseClientConnection:"
                   + " Exception thrown during close");
        }
        DebugLog.debug("BluetoothStackS60: connectionRfCloseClientConnection:"
                       + " After closing client connection ");
        BluetoothStreamer streamer = dataSource.getStreamer();
        DebugLog.debug("BluetoothStackS60: connectionRfCloseClientConnection:"
                       + " Closing Streamer");
        streamer.close();
        streamer = null;
        DebugLog.debug("BluetoothStackS60: connectionRfCloseClientConnection:"
                       + " Close complete");
        iInputStreamHash.remove(new Long(aHandle));
    }

    /**
     * @see com.intel.bluetooth.BluetoothRFCommServerConnection
     * @see #connectionRfCloseClientConnection(long)
     * @see javax.microedition.io.Connection#close()
     */
    public void connectionRfCloseServerConnection(long aHandle)
    throws IOException
    {
        DebugLog.debug("+ BluetoothStackS60: "
                       + "connectionRfCloseServerConnection()");
        clientClose(aHandle);
    }

    /**
     * Used to create handle for
     * {@link com.intel.bluetooth.BluetoothRFCommConnectionNotifier}
     *
     * @see javax.microedition.io.Connector#open(String, int, boolean)
     */
    public long rfServerOpen(BluetoothConnectionNotifierParams aParams,
                             ServiceRecordImpl aServiceRecord) throws IOException
    {
        DebugLog.debug("+ BluetoothStackS60:rfServerOpen()");
        Uid suiteUid = ApplicationInfo.getInstance().getSuiteUid();

        ApplicationUtils appUtils = ApplicationUtils.getInstance();

        if (aParams.obex)
        {
            String baseUri = "btgoep://localhost:" + aParams.uuid;
            if (PushValidator.isRegisteredPushUriStartingWith(baseUri,
                    suiteUid, PushValidator.RUNTIME_COMMS))
            {
                throw new IOException("URI is reserved by other application");
            }
            BTOBEXPermission permission = new BTOBEXPermission("*", "server");
            appUtils.checkPermission(permission);
        }
        else
        {
            String baseUri = "btspp://localhost:" + aParams.uuid;
            if (PushValidator.isRegisteredPushUriStartingWith(baseUri,
                    suiteUid, PushValidator.RUNTIME_COMMS))
            {
                throw new IOException("URI is reserved by other application");
            }
            BTPermission permission = new BTPermission("*", "server");
            appUtils.checkPermission(permission);
        }

        // Check for visibility and power on.
        checkBluetoothRadio(true);

        long handle = _rfServerOpen(aParams.iUrl, aParams.authorize,
                                    aParams.authenticate, aParams.encrypt,aParams.obex);

        boolean success = false;

        try
        {
            int channel = _rfGetChannel(handle);
            aServiceRecord.populateRFCOMMAttributes(0, channel, aParams.uuid,
                                                    aParams.name, aParams.obex);

            if (!_isRfServerAlreadyRunning(handle))
            {
                DebugLog.debug("  BluetoothStackS60:rfServerOpen(): "
                               + "Register SDP record with: ");
                DebugLog.debug("  BluetoothStackS60:rfServerOpen():     "
                               + "Channel: " + channel);
                DebugLog.debug("  BluetoothStackS60:rfServerOpen():     "
                               + "UUID: " + Utils.UUIDToByteArray(aParams.uuid));
                DebugLog.debug("  BluetoothStackS60:rfServerOpen():     "
                               + "Name : " + aParams.name);

                _rfcommRegisterSDPRecord(handle, channel, Utils
                                         .UUIDToByteArray(aParams.uuid), aParams.name,
                                         aParams.obex);
            }

            _restoreRfServiceRecord(handle, aServiceRecord);

            success = true;

            DebugLog.debug("- BluetoothStackS60:rfServerOpen()");
            return handle;
        }
        finally
        {
            if (success != true)
            {
                _rfServerClose(handle);
            }
        }
    }

    private native boolean _isRfServerAlreadyRunning(long aHandle);

    private native void _restoreRfServiceRecord(long aHandle,
            ServiceRecordImpl aServiceRecord);

    private native long _rfServerOpen(String aName, boolean aAuthorize,
                                      boolean aAuthenticate, boolean aEncrypt,boolean aIsObex)
    throws IOException;

    public native void _rfcommRegisterSDPRecord(long aHandle, int aChannel,
            byte[] aUuid, String aName, boolean aIsObex) throws IOException;

    public native int _rfGetChannel(long aHandle);

    public native int _rfServerGetServiceRecord(long aHandle);

    /**
     * @see javax.bluetooth.LocalDevice#updateRecord(javax.bluetooth.ServiceRecord)
     */
    public void rfServerUpdateServiceRecord(long aHandle,
                                            ServiceRecordImpl aServiceRecord, boolean aAcceptAndOpen)
    throws ServiceRegistrationException
    {
        if (true == aAcceptAndOpen)
        {
            int srvRecHandle = _rfServerGetServiceRecord(aHandle);
            aServiceRecord.populateServiceRecordHandle(srvRecHandle);
        }
        else
        {
            sdpUpdateServiceRecord("btspp", aHandle, aServiceRecord);
        }
    }

    /**
     * Used to create handle for
     * {@link com.intel.bluetooth.BluetoothRFCommServerConnection}
     *
     * @see com.intel.bluetooth.BluetoothRFCommConnectionNotifier#acceptAndOpen()
     * @see javax.microedition.io.StreamConnectionNotifier#acceptAndOpen()
     */
    public long rfServerAcceptAndOpenRfServerConnection(long aHandle)
    throws IOException
    {
        DebugLog.debug("+ BluetoothStackS60: "
                       + "rfServerAcceptAndOpenRfServerConnection()");
        long nativeHandle = _rfServerAcceptAndOpenServerConnection(aHandle);

        // After accepting client, we again go to read immediately.
        BluetoothStreamer streamer = new BluetoothStreamer(this, nativeHandle);

        // This starts prefetch immediately
        InputStream stream = streamer.openInputStream();

        // Put it into hash table to that it can be retrieved based on the
        // native handle
        iInputStreamHash.put(new Long(nativeHandle),
                             new BluetoothPrefetchSource(streamer, stream));

        return nativeHandle;
    }

    public native long _rfServerAcceptAndOpenServerConnection(long aHandle)
    throws IOException;

    /**
     * @see com.intel.bluetooth.BluetoothConnectionNotifierBase#close()
     * @see javax.microedition.io.Connection#close()
     */
    public void rfServerClose(long aHandle, ServiceRecordImpl aServiceRecord)
    throws IOException
    {
        _rfServerClose(aHandle);
    }

    public native int _rfServerClose(long aHandle) throws IOException;

    /**
     * @see javax.bluetooth.RemoteDevice#getRemoteDevice(javax.microedition.io.Connection)
     */
    public long getConnectionRfRemoteAddress(long aHandle) throws IOException
    {
        return getRemoteAddress(aHandle);
    }

    /**
     * @see javax.bluetooth.RemoteDevice#encrypt(javax.microedition.io.Connection,
     *      boolean)
     */
    public boolean rfEncrypt(long aAddress, long aHandle, boolean aOn)
    throws IOException
    {
        return false;
    }

    /**
     * @see java.io.InputStream#available()
     * @see com.intel.bluetooth.BluetoothRFCommInputStream#available()
     */
    public int connectionRfReadAvailable(long aHandle) throws IOException
    {
        DebugLog.debug("+ BluetoothStackS60::connectionRfReadAvailable");
        BluetoothPrefetchSource dataSource = (BluetoothPrefetchSource) iInputStreamHash
                                             .get(new Long(aHandle));
        InputStream source = dataSource.getInputStream();
        int ret = source.available();
        DebugLog.debug("- BluetoothStackS60:: " + "connectionRfReadAvailable: "
                       + ret);
        return ret;
    }

    public int connectionRfRead(long aHandle) throws IOException
    {
        DebugLog.debug("+ BluetoothStackS60::connectionRfRead() ");
        BluetoothPrefetchSource dataSource = (BluetoothPrefetchSource) iInputStreamHash
                                             .get(new Long(aHandle));
        InputStream source = dataSource.getInputStream();
        int ret = source.read();
        DebugLog.debug("- BluetoothStackS60::connectionRfRead() ");
        return ret;
    }

    public int connectionRfRead(long aHandle, byte[] aBytes, int aOffset,
                                int aLength) throws IOException
    {
        DebugLog.debug("+ BluetoothStackS60::connectionRfRead Handle:"
                       + Long.toString(aHandle, 16) + " Offset:" + aOffset
                       + " Length:" + aLength);
        BluetoothPrefetchSource dataSource = (BluetoothPrefetchSource) iInputStreamHash
                                             .get(new Long(aHandle));
        InputStream source = dataSource.getInputStream();
        return source.read(aBytes, aOffset, aLength);
    }

    public void connectionRfWrite(long aHandle, int aByte) throws IOException
    {
        // We will go to native with a single write.
        byte[] singleByte = new byte[1];
        singleByte[0] = (byte)(aByte & 0xFF);
        connectionRfWrite(aHandle, singleByte, 0, 1);
    }

    public void connectionRfWrite(long aHandle, byte[] aBytes, int aOffset,
                                  int aLength) throws IOException
    {
        // We will turn it into writing a byte array.
        int bytesToWrite = aLength;
        byte[] toWrite = new byte[aLength];
        System.arraycopy(aBytes, aOffset, toWrite, 0, aLength);
        clientSend(aHandle, toWrite);
    }

    /**
     * @see com.intel.bluetooth.BluetoothRFCommOutputStream#flush()
     */
    public void connectionRfFlush(long aHandle) throws IOException
    {
        // If we decide to maintain internal buffer, we need to flush.
        // As of now, since we write to socket directly, we need not worry.
    }

    // ---------------------- Client and Server L2CAP connections

    /**
     * Used to create handle for
     * {@link com.intel.bluetooth.BluetoothL2CAPClientConnection}
     */
    public long l2OpenClientConnection(BluetoothConnectionParams aParams,
                                       int aReceiveMTU, int aTransmitMTU) throws IOException
    {
        DebugLog.debug("BluetoothStackS60: l2OpenClientConnection");
        DebugLog.debug("BluetoothStackS60: l2OpenClientConnection:"
                       + " Address: " + aParams.address);
        DebugLog.debug("BluetoothStackS60: l2OpenClientConnection:"
                       + " Channel: " + aParams.channel);
        DebugLog.debug("BluetoothStackS60: l2OpenClientConnection:"
                       + " Authenticate: " + aParams.authenticate);
        DebugLog.debug("BluetoothStackS60: l2OpenClientConnection:"
                       + " Encrypt: " + aParams.encrypt);
        DebugLog.debug("BluetoothStackS60: l2OpenClientConnection:" + " RMTU: "
                       + aReceiveMTU);
        DebugLog.debug("BluetoothStackS60: l2OpenClientConnection:" + " TMTU: "
                       + aTransmitMTU);

        if (aReceiveMTU > RECEIVE_MTU_MAX)
        {
            throw new IllegalArgumentException(
                "Invalid ReceiveMTU value: " + aReceiveMTU);
        }

        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        BTPermission permission = new BTPermission("*", "client");

        appUtils.checkPermission(permission);

        // Check to see if power is on.
        checkBluetoothRadio(false);

        // Should not return until the connection has succeeded.
        long nativeHandle = _l2OpenClientConnection(aParams.address,
                            aParams.channel, aParams.authenticate, aParams.encrypt,
                            aReceiveMTU, aTransmitMTU, iFunctionServer);

        // After accepting client, we again go to read immediately.
        BluetoothL2CAPSource streamer = new BluetoothL2CAPSource(this,
                nativeHandle);

        BluetoothPrefetchSource source = new BluetoothPrefetchSource(streamer);
        // Put it into hash table to that it can be retrieved based on the
        // native handle
        iInputStreamHash.put(new Long(nativeHandle), source);

        return nativeHandle;
    }

    private native long _l2OpenClientConnection(long aBtAddress, int aChannel,
            boolean aAuthenticate, boolean aEncrypt, int aReceiveMTU,
            int aTransmitMTU, long aFserver);

    /**
     * Closing {@link com.intel.bluetooth.BluetoothL2CAPClientConnection}
     *
     * @see javax.microedition.io.Connection#close()
     */
    public void l2CloseClientConnection(long aHandle) throws IOException
    {
        DebugLog.debug("BluetoothStackS60:l2CloseClientConnection():"
                       + " Closing Client");
        DebugLog.debug("BluetoothStackS60: l2CloseClientConnection:"
                       + " After closing client connection ");

        clientClose(aHandle);
        DebugLog.debug("BluetoothStackS60: l2CloseClientConnection:"
                       + " Close complete");
        iInputStreamHash.remove(new Long(aHandle));
    }

    /**
     * Used to create handle for
     * {@link com.intel.bluetooth.BluetoothL2CAPConnectionNotifier}
     *
     * @see javax.microedition.io.Connector#open(String, int, boolean)
     */
    public long l2ServerOpen(BluetoothConnectionNotifierParams aParams,
                             int aReceiveMTU, int aTransmitMTU, ServiceRecordImpl aServiceRecord)
    throws IOException
    {
        DebugLog.debug("+ BluetoothStackS60:l2ServerOpen()");

        if (aReceiveMTU > RECEIVE_MTU_MAX)
        {
            throw new IllegalArgumentException(
                "Invalid receive MTU param value " + aReceiveMTU);
        }
        Uid suiteUid = ApplicationInfo.getInstance().getSuiteUid();
        String baseUri = "btl2cap://localhost:" + aParams.uuid;

        DebugLog.debug(baseUri);
        if (PushValidator.isRegisteredPushUriStartingWith(baseUri, suiteUid,
                PushValidator.RUNTIME_COMMS))
        {
            throw new IOException("URI is reserved by other application");
        }
        ApplicationUtils appUtils = ApplicationUtils.getInstance();

        BTPermission permission = new BTPermission("*", "server");

        appUtils.checkPermission(permission);

        // CSB: change with parameter in url.

        DebugLog.debug("  BluetoothStackS60: "
                       + "l2ServerOpen Checking Bluetooth state");
        // Check for visibility and power on.
        checkBluetoothRadio(true);

        DebugLog.debug("  BluetoothStackS60: "
                       + "l2ServerOpen Calling _l2ServerOpen");
        long handle = _l2ServerOpen(aParams.iUrl, aParams.authorize,
                                    aParams.authenticate, aParams.encrypt, aParams.master,
                                    aReceiveMTU, aTransmitMTU);

        boolean success = false;

        try
        {
            int channel = _l2ServerGetPSM(handle);
            aServiceRecord.populateL2CAPAttributes(0, channel, aParams.uuid,
                                                   aParams.name);

            if (!_isL2ServerAlreadyRunning(handle))
            {
                // If the server is not already running in push, then we
                // advertise
                _l2capRegisterSDPRecord(handle, channel, Utils
                                        .UUIDToByteArray(aParams.uuid), aParams.name);
            }

            _restoreL2ServiceRecord(handle, aServiceRecord);
            success = true;
            DebugLog.debug("(PSM)channel:" + channel);

            DebugLog.debug("- BluetoothStackS60:l2ServerOpen() Server Handle:"
                           + Long.toString(handle, 16));
            return handle;
        }
        finally
        {
            if (success != true)
            {
                _l2ServerClose(handle);
            }
        }
    }

    private native boolean _isL2ServerAlreadyRunning(long aHandle);

    private native void _restoreL2ServiceRecord(long aHandle,
            ServiceRecordImpl aServiceRecord);

    private native long _l2ServerOpen(String aUrl, boolean aAuthorize,
                                      boolean aAuthenticate, boolean aEncrypt, boolean aMaster,
                                      int aReceiveMTU, int aTransmitMTU) throws IOException;

    public native int _l2ServerGetPSM(long aHandle) throws IOException;

    public native int _l2ServerGetServiceRecord(long aHandle);

    public native void _l2capRegisterSDPRecord(long aHandle, int aChannel,
            byte[] aUuid, String aName) throws IOException;

    /**
     * @see javax.bluetooth.LocalDevice#updateRecord(javax.bluetooth.ServiceRecord)
     */
    public void l2ServerUpdateServiceRecord(long aHandle,
                                            ServiceRecordImpl aServiceRecord, boolean aAcceptAndOpen)
    throws ServiceRegistrationException
    {
        DebugLog.debug("+ BluetoothStackS60:l2ServerUpdateServiceRecord()");
        if (true == aAcceptAndOpen)
        {
            int srvRecHandle = _l2ServerGetServiceRecord(aHandle);
            aServiceRecord.populateServiceRecordHandle(srvRecHandle);
        }
        else
        {
            sdpUpdateServiceRecord("btl2cap", aHandle, aServiceRecord);
        }
        DebugLog.debug("- BluetoothStackS60:l2ServerUpdateServiceRecord()");
    }

    /**
     * Used to create handle for
     * {@link com.intel.bluetooth.BluetoothL2CAPServerConnection}
     *
     * @see com.intel.bluetooth.BluetoothL2CAPConnectionNotifier#acceptAndOpen()
     * @see javax.bluetooth.L2CAPConnectionNotifier#acceptAndOpen()
     */
    public long l2ServerAcceptAndOpenServerConnection(long aHandle)
    throws IOException
    {
        long nativeHandle;
        int[] receiveMtu = new int[1];
        DebugLog.debug("+ BluetoothStackS60:"
                       + "l2ServerAcceptAndOpenServerConnection()");
        nativeHandle = _l2ServerAcceptAndOpenServerConnection(aHandle,
                       receiveMtu);
        DebugLog.debug("  BluetoothStackS60:"
                       + "l2ServerAcceptAndOpenServerConnection()"
                       + " Connection Handle:" + Long.toString(nativeHandle, 16)
                       + " Receive MTU:" + receiveMtu);

        // After accepting client, we again go to read immediately.
        BluetoothL2CAPSource streamer = new BluetoothL2CAPSource(this,
                nativeHandle, receiveMtu[0]);
        BluetoothPrefetchSource source = new BluetoothPrefetchSource(streamer);
        // Put it into hash table to that it can be retrieved based on the
        // native handle
        iInputStreamHash.put(new Long(nativeHandle), source);

        return nativeHandle;
    }

    public native long _l2ServerAcceptAndOpenServerConnection(long aHandle,
            int[] receiveMtu) throws IOException;

    /**
     * Closing {@link com.intel.bluetooth.BluetoothL2CAPServerConnection}
     *
     * @see #l2CloseClientConnection(long)
     */
    public void l2CloseServerConnection(long aHandle) throws IOException
    {
        DebugLog.debug("+ BluetoothStackS60:l2CloseServerConnection "
                       + "Handler:" + Long.toString(aHandle, 16));
        clientClose(aHandle);
        return;
    }

    /**
     * @see com.intel.bluetooth.BluetoothConnectionNotifierBase#close()
     * @see javax.microedition.io.Connection#close()
     */
    public void l2ServerClose(long aHandle, ServiceRecordImpl aServiceRecord)
    throws IOException
    {
        DebugLog.debug("+ BluetoothStackS60:l2ServerClose" + " Handler:"
                       + Long.toString(aHandle, 16));
        DebugLog.debug("+ BluetoothStackS60:l2ServerClose()");
        _l2ServerClose(aHandle);
        DebugLog.debug("- BluetoothStackS60:l2ServerClose()");
    }

    public native int _l2ServerClose(long aHandle) throws IOException;

    /**
     * @see #rfGetSecurityOpt(long, int)
     */
    // Expected Can be one of the following.
    // ServiceRecord.NOAUTHENTICATE_NOENCRYPT;
    // ServiceRecord.AUTHENTICATE_ENCRYPT;
    // ServiceRecord.AUTHENTICATE_NOENCRYPT;
    public int l2GetSecurityOpt(long aHandle, int aExpected) throws IOException
    {
        DebugLog.debug("BluetoothStackS60: l2GetSecurityOpt");
        return aExpected;
    }

    /**
     * Get Transmit MTU value
     */
    public int l2GetTransmitMTU(long aHandle) throws IOException
    {
        DebugLog.debug("+ BluetoothStackS60: l2GetTransmitMTU");
        int result = _l2GetTransmitMTU(aHandle);
        if (result < 0)
        {
            throw new IOException("Retrieving TransmitMTU failed");
        }
        DebugLog.debug("- BluetoothStackS60: l2GetTransmitMTU: " + result);
        return result;
    }

    private native int _l2GetTransmitMTU(long aHandle);

    /**
     * Retrieve Receive MTU value
     */
    public int l2GetReceiveMTU(long aHandle) throws IOException
    {
        DebugLog.debug("+ BluetoothStackS60: l2GetReceiveMTU");
        int result = _l2GetReceiveMTU(aHandle);
        if (result < 0)
        {
            throw new IOException("Retrieving ReceiveMTU failed");
        }
        DebugLog.debug("- BluetoothStackS60: l2GetReceiveMTU: " + result);
        return result;

    }

    public native int _l2GetReceiveMTU(long aHandle);

    /**
     * @see javax.bluetooth.L2CAPConnection#ready()
     */
    public boolean l2Ready(long aHandle) throws IOException
    {
        DebugLog.debug("BluetoothStackS60: l2Ready");
        if (available(aHandle) > 0)
        {
            DebugLog.debug("BluetoothStackS60: l2Ready: Return True");
            return true;
        }
        else
        {
            DebugLog.debug("BluetoothStackS60: l2Ready: Return False");
            return false;
        }
    }

    public int l2Receive(long aHandle, byte[] aInBuf) throws IOException
    {
        // After accepting client, we again go to read immediately.
        BluetoothPrefetchSource source = (BluetoothPrefetchSource) iInputStreamHash
                                         .get(new Long(aHandle));
        BluetoothL2CAPSource streamer = source.getL2Source();
        return streamer.receive(aHandle, aInBuf);
    }

    public void l2Send(long aHandle, byte[] aData) throws IOException
    {
        int tMtu = _l2GetTransmitMTU(aHandle);
        int newBufLen = (tMtu < aData.length) ? tMtu : aData.length;

        byte[] toSend = new byte[newBufLen];
        System.arraycopy(aData, 0, toSend, 0, newBufLen);
        clientSend(aHandle, toSend);
    }

    /**
     * @see javax.bluetooth.RemoteDevice#getRemoteDevice(javax.microedition.io.Connection)
     */
    public long l2RemoteAddress(long aHandle) throws IOException
    {
        return getRemoteAddress(aHandle);
    }

    /**
     * @see javax.bluetooth.RemoteDevice#encrypt(javax.microedition.io.Connection,
     *      boolean)
     */
    public boolean l2Encrypt(long aAddress, long aHandle, boolean aOn)
    throws IOException
    {
        DebugLog.debug("BluetoothStackS60: l2Encrypt");
        return false;
    }

    // ------------------------------------------------------------------------------
    // Common Client Operations. For both RFCOMM and L2CAP
    // ------------------------------------------------------------------------------
    private int available(long aHandle) throws IOException
    {
        DebugLog.debug("BluetoothStackS60: available");

        int ret = _available(aHandle);
        if (ret < 0)
        {
            throw new IOException(
                "available() method failed: connection is closed");
        }
        DebugLog.debug("BluetoothStackS60: available: Returning: " + ret);
        return ret;
    }

    private native int _available(long aHandle);

    private long getRemoteAddress(long aHandle) throws IOException
    {
        long retVal = _getRemoteAddress(aHandle);
        if (retVal <= 0)
        {
            throw new IOException("Retrieving remote device address failed");
        }
        return retVal;
    }

    private native long _getRemoteAddress(long aHandle);

    int clientSend(long handle, byte[] data) throws IOException
    {
        DebugLog.debug("BluetoothStackS60: clientSend" + data.length + ":"
                       + data);

        int retVal = 0;
        retVal = _clientSend(handle, data, data.length);
        if (retVal < 0)
        {
            throw new IOException("Send failed. Symbian OS error: " + retVal);
        }

        // It will be the number of bytes written
        return retVal;
    }

    private native int _clientSend(long aHandle, byte[] aData, long aLength);

    private native void _registerForCallback(long aHandle);

    // Both stream and datagram types translate to reading a single block of
    // data
    // This is common implementation.
    int clientReceive(long aHandle, byte[] aInBuf) throws IOException
    {

        DebugLog.debug("BluetoothStackS60: clientReceive");
        int retVal = 0;
        BluetoothPrefetchSource dataSource = (BluetoothPrefetchSource) iInputStreamHash
                                             .get(new Long(aHandle));
        Object iReadLock = dataSource.getLock();

        synchronized (iReadLock)
        {
            DebugLog.debug("BluetoothStackS60: clientReceive Requested Length:"
                           + aInBuf.length);
            DebugLog.debug("BluetoothStackS60: clientReceive:"
                           + " Registering for callback");
            if (!dataSource.isClosed())
            {
                _registerForCallback(aHandle);
            }
            else
            {
                return 0;
            }
            if ((available(aHandle) <= 0))
            {

                DebugLog
                .debug("BluetoothStackS60: clientReceive: Entering lock");
                try
                {
                    DebugLog.debug("BluetoothStackS60: clientReceive:"
                                   + " Waiting for read to complete");
                    iReadLock.wait();
                }
                catch (Exception e)
                {
                    // nothing to do
                }
            }
            else
            {
                DebugLog.debug("BluetoothStackS60: clientReceive Data present."
                               + " GOing to read");
            }

            dataSource = (BluetoothPrefetchSource) iInputStreamHash
                         .get(new Long(aHandle));

            if (!dataSource.isClosed())
            {
                DebugLog.debug("BluetoothStackS60: clientReceive:"
                               + " Calling native receive");
                retVal = _clientReceive(aHandle, aInBuf);
            }
            else
            {
                DebugLog.debug("BluetoothStackS60: clientReceive:"
                               + " source already closed");
                retVal = 0;
            }
        }

        if (retVal < 0)
        {
            throw new IOException("Read failed. Symbian OS error: " + retVal);
        }

        if (retVal == 0)
            retVal = DATA_EOF;

        DebugLog.debug("BluetoothStackS60: clientReceive: return: " + retVal);

        return retVal;
    }

    private native int _clientReceive(long aHandle, byte[] aData);

    void clientClose(long aHandle)
    {
        try
        {
            BluetoothPrefetchSource dataSource = (BluetoothPrefetchSource) iInputStreamHash
                                                 .get(new Long(aHandle));
            Object iReadLock = dataSource.getLock();
            synchronized (iReadLock)
            {
                DebugLog.debug("BluetoothStackS60: clientClose");
                dataSource.setClosed();
                iReadLock.notify();
            }
            _clientClose(aHandle);
            DebugLog
            .debug("BluetoothStackS60: clientClose: notifying read lock");
        }
        catch (Exception e)
        {
            DebugLog.debug("Exception in ClientClose");
        }
    }

    private native void _clientClose(long aHandle);

    public void receiveCompleteCallback(long aHandle)
    {
        DebugLog.debug("BluetoothStackS60: receiveCompleteCallback");
        try
        {
            BluetoothPrefetchSource dataSource = (BluetoothPrefetchSource) iInputStreamHash
                                                 .get(new Long(aHandle));
            Object iReadLock = dataSource.getLock();
            synchronized (iReadLock)
            {
                DebugLog.debug("BluetoothStackS60: receiveCompleteCallback."
                               + " Notifying read lock");
                iReadLock.notify();
            }
        }
        catch (Exception e)
        {
            // Nothing to do
        }
    }

    // ------------------------------------------------------------------------------
    // END Common Client Operations. For both RFCOMM and L2CAP
    // ------------------------------------------------------------------------------

    // -------------Update Record--------------------

    /**
     * Updates natvie SDDB's service record attributes.
     *
     * @param aProtocol
     *            In case of L2cap, "btl2cap" ; in case of RFcomm, "btspp";
     *            otherwise "btgoep"
     * @param aHandle
     *            Native handle to connection
     * @param aServiceRecord
     *            Modified service record
     * @throws ServiceRegistrationException
     *             if the local SDDB could not be updated successfully due to
     *             insufficient disk space, database locks, etc.
     */
    private void sdpUpdateServiceRecord(String aProtocol, long aHandle,
                                        ServiceRecordImpl aServiceRecord)
    throws ServiceRegistrationException
    {
        DebugLog.debug("+ BluetoothStackS60:sdpUpdateServiceRecord");
        int[] ids = aServiceRecord.getAttributeIDs();

        if ((ids == null) || (ids.length == 0))
        {
            return;
        }

        _initializeUpdateRecord(aProtocol, aHandle,
                                aServiceRecord.deviceServiceClasses);

        try
        {
            for (int i = ids.length - 1; i >= 0; --i)
            {
                int attrID = ids[i];
                switch (attrID)
                {
                case BluetoothConsts.ServiceRecordHandle:
                case BluetoothConsts.ProtocolDescriptorList:
                    /* Nothing to do. As these two Attribute IDs are fixed */
                    continue;
                }


                sdpAddAttribute(aProtocol, aHandle, attrID, aServiceRecord
                                .getAttributeValue(attrID));
            }
        }
        finally
        {
            _completesUpdateRecord(aProtocol, aHandle);

        }
        DebugLog.debug("- BluetoothStackS60:sdpUpdateServiceRecord");
    }

    /**
     * Helper function to sdpUpdateServiceRecord.
     *
     * @param aProtocol
     *            In case of L2cap, "btl2cap" ; in case of RFcomm, "btspp";
     *            otherwise "btgoep"
     * @param aHandle
     *            Native handle to connection
     * @param aAttrID
     *            Integer represents service attribute id
     * @param aElement
     *            DataElement represents service attribute value
     * @throws ServiceRegistrationException
     *             if the local SDDB could not be updated successfully due to
     *             insufficient disk space, database locks, etc.
     */
    private void sdpAddAttribute(String aProtocol, long aHandle, int aAttrID,
                                 DataElement aElement) throws ServiceRegistrationException
    {

        int type = aElement.getDataType();

        DebugLog.debug("+ BluetoothStackS60:sdpAddAttribute():" + aProtocol
                       + ":" + type);

        switch (type)
        {
        case DataElement.NULL:
            _sdpAddAttribute(aProtocol, aHandle, aAttrID, type, 0, null);
            break;
        case DataElement.BOOL:
            _sdpAddAttribute(aProtocol, aHandle, aAttrID, type, aElement
                             .getBoolean() ? 1 : 0, null);
            break;
        case DataElement.U_INT_1:
        case DataElement.INT_1:
        case DataElement.U_INT_2:
        case DataElement.INT_2:
        case DataElement.U_INT_4:
        case DataElement.INT_4:
        case DataElement.INT_8:
            _sdpAddAttribute(aProtocol, aHandle, aAttrID, type, aElement
                             .getLong(), null);
            break;
        case DataElement.U_INT_8:
        case DataElement.U_INT_16:
        case DataElement.INT_16:
            _sdpAddAttribute(aProtocol, aHandle, aAttrID, type, 0,
                             (byte[]) aElement.getValue());
            break;
        case DataElement.UUID:
            _sdpAddAttribute(aProtocol, aHandle, aAttrID, type, 0, Utils
                             .UUIDToByteArray((UUID) aElement.getValue()));
            break;
        case DataElement.STRING:
            byte[] bstr = ((String) aElement.getValue()).getBytes();
            _sdpAddAttribute(aProtocol, aHandle, aAttrID, type, 0, bstr);
            break;
        case DataElement.URL:
            byte[] burl = ((String) aElement.getValue()).getBytes();
            _sdpAddAttribute(aProtocol, aHandle, aAttrID, type, 0, burl);
            break;
        case DataElement.DATSEQ:
        case DataElement.DATALT:
            _sdpAttributeListStart(aProtocol, aHandle, aAttrID, type);
            for (Enumeration e = (Enumeration) aElement.getValue(); e
                    .hasMoreElements();)
            {
                DataElement child = (DataElement) e.nextElement();
                sdpAddAttribute(aProtocol, aHandle, -1, child);
            }
            _sdpAttributeListEnd(aProtocol, aHandle, aAttrID);
            break;
        default:
            throw new IllegalArgumentException("Invalid data type");
        }
        DebugLog.debug("- BluetoothStackS60:sdpAddAttribute()");
    }

    /**
     * Creates/Opens the persistent file, whenever push connection exist. And
     * also stores the aDeviceServiceClasses at the beginning of the opened file
     *
     * @param protocol
     *            In case of L2cap, "btl2cap" ; in case of RFcomm, "btspp";
     *            otherwise "btgoep"
     * @param handle
     *            Native handle to connection
     * @param aDeviceServiceClasses
     *            The DeviceServiceClasses value related to this service
     */
    private native void _initializeUpdateRecord(String aProtocol, long aHandle,
            int aDeviceServiceClasses);

    /**
     * Closes the persistent file.
     *
     * @param protocol
     *            In case of L2cap, "btl2cap" ; in case of RFcomm, "btspp";
     *            otherwise "btgoep"
     * @param handle
     *            Native handle to connection
     */
    private native void _completesUpdateRecord(String aProtocol, long aHandle);

    /**
     * Updates the SDDB service attributes
     *
     * @param protocol
     *            In case of L2cap, "btl2cap" ; in case of RFcomm, "btspp";
     *            otherwise "btgoep"
     * @param handle
     *            Native handle to connection
     * @param attrID
     *            Integer represents service attribute id
     * @param attrType
     *            Integer represents data type
     * @param numValue
     *            value represents as a long number
     * @param bytesValue
     *            value represents as a byte[]
     * @throws ServiceRegistrationException
     *             if the local SDDB could not be updated successfully due to
     *             insufficient disk space, database locks, etc.
     */
    private native void _sdpAddAttribute(String aProtocol, long aHandle,
                                         int aAttrID, int aAttrType, long aNumValue, byte[] aBytesValue)
    throws ServiceRegistrationException;

    /**
     * Indicates the start of the List, either SEQ or ALT
     *
     * @param protocol
     *            In case of L2cap, "btl2cap" ; in case of RFcomm, "btspp";
     *            otherwise "btgoep"
     * @param handle
     *            Native handle to connection
     * @param attrID
     *            Integer represents service attribute id
     * @param attrType
     *            Integer represents type of the list, DATSEQ/DATALT
     * @throws ServiceRegistrationException
     *             if the local SDDB could not be updated successfully due to
     *             insufficient disk space, database locks, etc.
     */
    private native void _sdpAttributeListStart(String aProtocol, long aHandle,
            int aAttrID, int aAttrType) throws ServiceRegistrationException;

    /**
     * Indicates the end of the List, either SEQ or ALT
     *
     * @param protocol
     *            In case of L2cap, "btl2cap" ; in case of RFcomm, "btspp";
     *            otherwise "btgoep"
     * @param handle
     *            Native handle to connection
     * @param attrID
     *            Integer represents service attribute id
     * @throws ServiceRegistrationException
     *             if the local SDDB could not be updated successfully due to
     *             insufficient disk space, database locks, etc.
     */
    private native void _sdpAttributeListEnd(String aProtocol, long aHandle,
            int aAttrID) throws ServiceRegistrationException;
}
