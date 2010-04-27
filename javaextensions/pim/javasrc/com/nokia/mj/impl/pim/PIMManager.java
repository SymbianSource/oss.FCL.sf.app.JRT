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
* Description:  PIM singleton implementation.
 *
*/


// PACKAGE
package com.nokia.mj.impl.pim;

// IMPORTS

import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMException;
import javax.microedition.pim.PIMItem;
import javax.microedition.pim.PIMList;
import java.io.UnsupportedEncodingException;
import com.nokia.mj.impl.pim.ErrorString;
import com.nokia.mj.impl.pim.GenericException;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.rt.support.ShutdownListener;
import com.nokia.mj.impl.security.utils.SecurityPromptMessage;
import com.nokia.mj.impl.pim.utils.NativeError;


// CLASS DEFINITION
/**
 * PIM singleton implementation. PIM class acts as a starting point to the PIM
 * API. Only single instance of the class is created and that instance is
 * accessed through an accessor method. The instance is created when it is
 * requested for the first time.
 *
 * PIMLists are created when lists are being opened. The list opening operation
 * is first delegated to the native side, where the native side peer object to
 * the list is created. Then the Java side list is created and initialized with
 * a reference to its native side counterpart.
 *
 * Versit conversion operations are mostly implemented in the native side (the
 * actual parsing) but some logic needs to be implemented in the Java side
 * (cutting the input stream into such pieces that can be passed through the JNI
 * etc.)
 */
public final class PIMManager extends PIM
{

    // Constants

    // Static data

    /** The single instance of PIMManager. */
    private static PIMManager sInstance = null;

    // Member data

    /** Finalizer. */
    private Finalizer iFinalizer;

    /** Handle to PIMManager native side. */
    private int iManagerHandle;

    /** Serializer. */
    private Serializer iSerializer;

    // Methods

    /**
     * Provides the single instance of PIMManager. Creates an Event Server and
     * the PIMManager native side.
     *
     * @par Notes:
     * @li This method is coupled with the \ref
     *     javax.microedition.pim.PIM.getInstance() method, which must delegate
     *     the operation immediately to this operation.
     */
    public synchronized static PIM getInstance()
    {
        if (sInstance == null)
        {
            sInstance = new PIMManager();
        }

        return sInstance;
    }

    /**
     * Creates PIMManager.
     * Direct creation of a PIMManager is prohibited.
     */
    private PIMManager()
    {
        super();
        setShutdownListener();
        iFinalizer = registerForFinalization();
        int[] error = new int[1];
        iManagerHandle = _createPIMManager(error);
        if (!NativeError.checkSuccess(error[0]))
        {
            throw new GenericException(ErrorString.GENERAL_ERROR_COLON + error[0]);
        }

        iSerializer = new Serializer(iManagerHandle);
    }

    /**
     * Disposes the PIMManager native peer and Event Source, if the handles are
     * valid. Invalid (negative) handles indicate that their creation failed in
     * the first place.
     */
    public Finalizer registerForFinalization()
    {
        return new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };
    }

    void doFinalize()
    {
        if (iFinalizer == null)
        {
            return;
        }
        iFinalizer = null;

        if (iManagerHandle != 0)
        {
            _dispose(iManagerHandle);
            iManagerHandle = 0;
        }
    }

    /**
     * Registers for shutdown listener
     */
    private void setShutdownListener()
    {
        // Get the insatnce of ApplicationUtils.
        ApplicationUtils appUtils = ApplicationUtils.getInstance();

        // Get the name of the application.
        appUtils.addShutdownListener(new ShutdownListener()
        {
            public void shuttingDown()
            {

                if (iManagerHandle != 0)
                {
                    _dispose(iManagerHandle);
                    iManagerHandle = 0;
                }
            }

        });
    }

    /**
     * Provides the handle to the native side PIM Manager. Needed by some other
     * implementation classes in this package.
     */
    int managerHandle()
    {
        return iManagerHandle;
    }

    // Methods from PIM

    public synchronized PIMList openPIMList(int aPimListType, int aMode)
    throws PIMException
    {
        return doOpenPIMList(aPimListType, aMode, null);
    }

    public synchronized PIMList openPIMList(int aPimListType, int aMode,
                                            String aName) throws PIMException
    {
        if (aName == null)
        {
            throw new NullPointerException(ErrorString.OPENING_LISTS_FAILED_COLON +
                                           ErrorString.LIST_NAME_IS_NULL);
        }

        return doOpenPIMList(aPimListType, aMode, aName);
    }

    public synchronized String[] listPIMLists(int aPimListType)
    {
        if (aPimListType != PIM.CONTACT_LIST && aPimListType != PIM.EVENT_LIST
                && aPimListType != PIM.TODO_LIST)
        {
            throw new java.lang.IllegalArgumentException(ErrorString.LISTING_FAILED_DOT +
                    ErrorString.INVALID_LIST_TYPE_COLON + aPimListType);
        }
        // Ensure permission
        getPermission(aPimListType, PIM.READ_ONLY);

        int[] error = new int[1];

        String[] lists = _listPIMLists(iManagerHandle, aPimListType, error);
        NativeError.handleListPIMListError(error[0]);

        return lists;
    }

    public synchronized PIMItem[] fromSerialFormat(java.io.InputStream aIs,
            String aEnc) throws PIMException, UnsupportedEncodingException
    {
        return iSerializer.fromSerialFormat(aIs, aEnc);
    }

    public synchronized void toSerialFormat(PIMItem aItem,
                                            java.io.OutputStream aOs, String aEnc, String aDataFormat)
    throws PIMException, UnsupportedEncodingException
    {
        iSerializer.toSerialFormat(aItem, aOs, aEnc, aDataFormat);
    }

    public synchronized String[] supportedSerialFormats(int aPimListType)
    {
        return iSerializer.supportedSerialFormats(aPimListType);
    }


    /**
     * getPermission
     * Ensures permissions for PIM operations.
     * The possible operations in this case are reading from
     * the list or writing to the list. Note that read and write permission
     * must be handled separately since there is no dialog for READ_WRITE
     * permission mode.
     *
     * @param aListType The accessed PIM API list type. PIM.CONTACT_LIST,
     *                  PIM.EVENT_LIST or PIM.TODO_LIST
     * @param aMode     The used mode. PIM.READ_ONLY or PIM.WRITE_ONLY. This
     *                  function throws IllegalArgumentException of some other
     *                  mode is passed as a parameter
     */
    public void  getPermission(int aListType, int aMode)
    {
        // This function supports PIM.READ_ONLY or PIM.WRITE_ONLY modes
        // since PIM.READ_WRITE cannot be used to check which type of
        // an operation is requested
        if (aMode != PIM.READ_ONLY &&
                aMode != PIM.WRITE_ONLY)
        {
            throw new IllegalArgumentException();
        }

        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        String action = null;
        if (aMode == PIM.WRITE_ONLY)
        {
            switch (aListType)
            {
            case PIM.CONTACT_LIST:
                action = PIMPermissionImpl.ACTION_WRITE_CONTACTS;
                break;
            case PIM.EVENT_LIST:
                action = PIMPermissionImpl.ACTION_WRITE_EVENTS;
                break;
            case PIM.TODO_LIST:
                action = PIMPermissionImpl.ACTION_WRITE_TODOS;
                break;
            }
        }
        else
        {
            switch (aListType)
            {
            case PIM.CONTACT_LIST:
                action = PIMPermissionImpl.ACTION_READ_CONTACTS;
                break;
            case PIM.EVENT_LIST:
                action = PIMPermissionImpl.ACTION_READ_EVENTS;
                break;
            case PIM.TODO_LIST:
                action = PIMPermissionImpl.ACTION_READ_TODOS;
                break;
            }
        }
        PIMPermissionImpl per = new PIMPermissionImpl("pim://*", action);
        // Ensure permission from PIM API security
        appUtils.checkPermission(per);
    }


    // New private methods
    /**
     * Common implementation of the list opening. Arguments and permissions are
     * pre-checked.
     *
     * @param aName
     *            If null, default list is opened.
     */
    private PIMList doOpenPIMList(int aPimListType, int aMode, String aName)
    throws PIMException
    {
        if (aPimListType != PIM.CONTACT_LIST && aPimListType != PIM.EVENT_LIST
                && aPimListType != PIM.TODO_LIST)
        {
            throw new java.lang.IllegalArgumentException(
                ErrorString.INVALID_LIST_TYPE_COLON + aPimListType);
        }

        if (aMode != PIM.READ_ONLY && aMode != PIM.WRITE_ONLY
                && aMode != PIM.READ_WRITE)
        {
            throw new java.lang.IllegalArgumentException(
                ErrorString.INVALID_MODE_COLON + aMode);
        }

        // Both permissions must be checked separately if aMode is
        // PIM.READ_WRITE
        if (aMode == PIM.READ_WRITE)
        {
            // First ensure read access permission
            // Get localized text info for the security dialog
            getPermission(aPimListType, PIM.READ_ONLY);
            getPermission(aPimListType, PIM.WRITE_ONLY);

        }
        else
        {
            getPermission(aPimListType, aMode);
        }
        int[] error = new int[1];
        int listHandle = _openPIMList(

                             iManagerHandle, aPimListType, aName, error); // if null, open default
        // list
        NativeError.handleOpenPIMListError(error[0], aPimListType, aName);


        // Create new pim list of right type
        PIMListImpl pimList = null;

        switch (aPimListType)
        {
        case PIM.CONTACT_LIST:
        {
            pimList = new ContactListImpl(listHandle, aMode);
            break;
        }

        case PIM.EVENT_LIST:
        {
            pimList = new EventListImpl(listHandle, aMode);
            break;
        }

        case PIM.TODO_LIST:
        {
            pimList = new ToDoListImpl(listHandle, aMode);
            break;
        }

        default:
        {
            // We should never end up here
            throw new PIMException(ErrorString.GENERAL_ERROR,
                                   PIMException.GENERAL_ERROR);
        }
        }

        return pimList;
    }


    // Native operations
    /**
     * Creates PIMManager native side.
     *
     * @return Handle to the native side PIMManager.
     */
    private native int _createPIMManager(int[] aError);

    private native void _dispose(int aPIMManagerHandle);

    /**
     * @param aPimlistName
     *            List name. If null, default list is opened.
     *
     * @return Handle to new native side PIM list of given type or negative
     *         value on error.
     */
    private native int _openPIMList(int aManagerHandle, int aPimListType,
                                    String aPimListName, int[] aError);

    private native String[] _listPIMLists(int aManagerHandle, int aPimListType,
                                          int[] aError);

}

// End of file
