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
import java.util.Enumeration;
import java.util.Vector;
import com.nokia.mj.impl.pim.ErrorString;
import com.nokia.mj.impl.pim.GenericException;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.rt.support.ShutdownListener;
import com.nokia.mj.impl.security.utils.SecurityPromptMessage;
import com.nokia.mj.impl.pim.utils.NativeError;
import com.nokia.mj.impl.utils.Tokenizer;
import com.nokia.mj.impl.pim.Calendar;

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
    private Vector iCalInfo;

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
    public PIMManager()
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
        iCalInfo = new Vector();
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
        return doOpenPIMList(aPimListType, aMode, null, null);
    }


    /**
     * this method is used to open existsing calendar, create new calendar and delete existsing calendar by passing string  as name.
     * function will parse the string and do the operation.
     * user as to pass the string in given format only
     */
    public synchronized PIMList openPIMList(int aPimListType, int aMode,
                                            String aName) throws PIMException
    {
        if (aName == null)
        {
            throw new NullPointerException(ErrorString.OPENING_LISTS_FAILED_COLON +
                                           ErrorString.LIST_NAME_IS_NULL);
        }
        //Check if the aName is as per the MultipleCalendar Parameter definition
        //aName = [calendarname "/"] listname ["?operation=" ["create" | "delete"]]
        if (isMultiCalendarParam(aName))
        {

            String calendarName = null;
            String operation = null;
            String listName = null;

            listName = getListName(aName);
            operation = getOperation(aName);
            calendarName = aName.substring(0, aName.indexOf("/"));
            if (isListNameValid(listName))
            {
                if (operation == null)
                {
                    if (!calendarName.trim().equals(""))
                    {
                        //This is the case of opening an existing calendar
                        return doOpenPIMList(aPimListType, aMode, listName, "C:" + calendarName);
                    }
                    else
                    {
                        //This is the case with PIMException
                        throw new PIMException("Can not" + operation + "Default Calendar", PIMException.GENERAL_ERROR);                        
                    }
                }
                else if (operation.equals("create"))
                {
                    createCalendar(calendarName);
                    int[] error = new int[1];
                    int listHandle = _openPIMList(iManagerHandle, aPimListType, listName, calendarName, error);
                    PIMListImpl pimList = new EventListImpl(listHandle, aMode);
                    return pimList;
                }
                else if (operation.equals("delete"))
                {
                    deleteCalendar(calendarName);
                    int listHandle = 0;
                    PIMListImpl pimList = new EventListImpl(listHandle, aMode);
                    // Close the dummy list
                    pimList.close();
                    return pimList;
                }
                else
                {
                    throw new IllegalArgumentException("Invalid operation");
                }
            } // if isListNameValid(listName) block ends here
            else
            {
                throw new IllegalArgumentException("Invalid List name");
            }
        } // if isMultiCalendarParam(aName) block ends here
        else
        {
            //This is not the case of Multiple Calendar, so follow the default calendar path
            return doOpenPIMList(aPimListType, aMode, aName);
        }
    }

    private boolean isMultiCalendarParam(String aListTypeName)
    {
        //If there is "?" and/or "=" is present in aListTypeName
        //return true
        boolean ret = false;
        if ((aListTypeName.indexOf("/") != -1) || (aListTypeName.indexOf("?") != -1) || (aListTypeName.indexOf("=") != -1))
        {
            ret = true;
        }
        return ret;
    }

    private String getListName(String aName)
    {
        // Get Operation
        String operation = "?operation=";
        int operationIndex = aName.indexOf(operation);

        if (operationIndex == -1)
        {
            operationIndex = aName.length();
        }

        return aName.substring(aName.indexOf("/") + 1, operationIndex);
    }

    private boolean isListNameValid(String aListName)
    {
        String lists[] = listPIMLists(PIM.EVENT_LIST);
        boolean listExist = false;

        for (int i = 0; i < lists.length; i++)
        {
            if (aListName.trim().equalsIgnoreCase(lists[i]))
            {
                listExist = true;
                break;
            }
        }

        return listExist;
    }

    private String getOperation(String aName)
    {
        String aOperation = null;
        String operation = "?operation=";
        int operationIndex = aName.indexOf(operation);

        if (operationIndex != -1)
        {
            aOperation = aName.substring(operationIndex + operation.length());
        }


        return aOperation;

    }

    public synchronized String[] listPIMLists(int aPimListType)
    {

        if (aPimListType != PIM.CONTACT_LIST && aPimListType != PIM.EVENT_LIST && aPimListType != PIM.TODO_LIST)
        {
            throw new java.lang.IllegalArgumentException(ErrorString.LISTING_FAILED_DOT + ErrorString.INVALID_LIST_TYPE_COLON + aPimListType);
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
        * Enumerates the calendars currently present in the device.
        *
        * @return A list of Calendar names
        * @throws java.lang.SecurityException
        *             if the application is not given permission to read PIM lists
        */
    public synchronized Calendar[] listCalendars()
    {
        // security check
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        PIMPermissionImpl per = new PIMPermissionImpl("pim://*", PIMPermissionImpl.ACTION_READ_EVENTS + "," + PIMPermissionImpl.ACTION_READ_TODOS);
        appUtils.checkPermission(per);
        int[] error = new int[1];

        String[] calendarFileLists = _listCalendars(iManagerHandle, error);
        String[] calendarNameLists = _listCalendarNames(iManagerHandle, error);
        if (!NativeError.checkSuccess(error[0]))
        {
            throw new GenericException(ErrorString.GENERAL_ERROR_COLON + error[0]);
        }
        Vector tokens = new Vector();
        Vector tokenNames = new Vector();
        int length = calendarFileLists.length;
        for (int i = 0; i < length; i++)
        {
            String str[] = Tokenizer.split(calendarFileLists[i], ":");
            String strname[] = Tokenizer.split(calendarNameLists[i], ":");


            if (str[0].equals("C"))
            {
                tokens.addElement(str[1]);

            }
            if (strname[0].equals("C"))
            {

                tokenNames.addElement(strname[1]);
            }
            else
            {

                tokenNames.addElement(strname[0]);
            }
        }
        String[] calendarLists = new String[tokens.size()];
        String[] calendarNames = new String[tokenNames.size()];
        tokens.copyInto(calendarLists);
        tokenNames.copyInto(calendarNames);
        Calendar[] calendarobjlist = new Calendar[calendarLists.length];
        for (int i = 0; i < calendarLists.length; i++)
        {
            Calendar cal = new Calendar(calendarLists[i], calendarNames[i]);
            calendarobjlist[i] = cal;
        }
        //return calendarLists;
        return calendarobjlist;
    }


    private synchronized void createCalendar(String aCalName) throws PIMException
    {

        String displayName = aCalName;
        // security check
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        PIMPermissionImpl per = new PIMPermissionImpl("pim://*", PIMPermissionImpl.ACTION_WRITE_EVENTS + "," +  PIMPermissionImpl.ACTION_WRITE_TODOS);
        appUtils.checkPermission(per);
        String fileName = "C:" + aCalName;
        int error = _createCalendar(iManagerHandle, fileName,displayName);
        NativeError.handleCreateCalendarError(error, aCalName);
    }

    private synchronized void deleteCalendar(String aCalName) throws PIMException
    {
        // security check
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        PIMPermissionImpl per = new PIMPermissionImpl(PIMPermissionImpl.ACTION_WRITE_EVENTS + "," + PIMPermissionImpl.ACTION_WRITE_TODOS, aCalName, null, -1);
        appUtils.checkPermission(per);

        boolean isCalPresent = false;
        String fileName = "C:" + aCalName;
        int index;
        for (index = 0; index < iCalInfo.size(); index++)
        {
            CalendarListInfo calList = (CalendarListInfo) iCalInfo.elementAt(index);
            String calName = calList.iCalName;
            if (calName.equals(fileName))
            {
                Enumeration e = calList.iList.elements();
                while (e.hasMoreElements())
                {
                    PIMList list = (PIMList) e.nextElement();
                    try
                    {
                        list.close();
                    }
                    catch (PIMException ex)
                    {
                        //throw new GenericException("Delete entry failed. " + ex.toString());
                    }
                }
                calList.iList.removeAllElements();
                isCalPresent = true;
                break;
            }
        }
        if (isCalPresent)
        {
            int error = _deleteCalendar(iManagerHandle, fileName);
            NativeError.handleDeleteCalendarError(error, aCalName);        	
            iCalInfo.removeElementAt(index);
        }
        else
        {
        	throw new PIMException("Can't delete specified calendar as calendar does not exist ", PIMException.LIST_NOT_ACCESSIBLE);
        }
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

    private PIMList doOpenPIMList(int aPimListType, int aMode, String aName)
    throws PIMException
    {

        if (aPimListType != PIM.CONTACT_LIST && aPimListType != PIM.EVENT_LIST && aPimListType != PIM.TODO_LIST)
        {
            throw new java.lang.IllegalArgumentException(
                ErrorString.INVALID_LIST_TYPE_COLON + aPimListType);
        }

        if (aMode != PIM.READ_ONLY && aMode != PIM.WRITE_ONLY && aMode != PIM.READ_WRITE)
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
                             iManagerHandle, aPimListType, aName, null, error); // if null, open default
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
///////////////////////
    // New private methods

    /**
     * Common implementation of the list opening. Arguments and permissions are
     * pre-checked.
     *
     * @param aName
     *            If null, default list is opened.
     */
    private PIMList doOpenPIMList(int aPimListType, int aMode, String aName, String aCalName)
    throws PIMException
    {

        if (aPimListType != PIM.CONTACT_LIST && aPimListType != PIM.EVENT_LIST && aPimListType != PIM.TODO_LIST)
        {
            throw new java.lang.IllegalArgumentException(
                ErrorString.INVALID_LIST_TYPE_COLON + aPimListType);
        }

        if (aMode != PIM.READ_ONLY && aMode != PIM.WRITE_ONLY && aMode != PIM.READ_WRITE)
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
                             iManagerHandle, aPimListType, aName, aCalName, error);
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
                                    String aPimListName, String aCalName,
                                    int[] aError);

    private native String[] _listPIMLists(int aManagerHandle, int aPimListType,
                                          int[] aError);

    private native String[] _listCalendars(int aManagerHandle, int[] aError);

    private native String[] _listCalendarNames(int aManagerHandle, int[] aError);

    private native int _createCalendar(int aManagerHandle, String aCalName, String aDisplayName);

    private native int _deleteCalendar(int aManagerHandle, String aCalName);

    class CalendarListInfo
    {

        String iCalName;
        Vector iList;

        CalendarListInfo(String aCalName, PIMList aPIMList)
        {
            iCalName = aCalName;
            iList = new Vector();
            iList.addElement(aPIMList);
        }
    }
}

// End of file
