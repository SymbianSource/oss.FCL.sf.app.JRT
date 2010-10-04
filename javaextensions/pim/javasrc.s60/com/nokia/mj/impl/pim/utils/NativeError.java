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
* Description:  Native error codes.
 *
*/


// PACKAGE
package com.nokia.mj.impl.pim.utils;

// IMPORTS
import javax.microedition.pim.FieldEmptyException;
import javax.microedition.pim.FieldFullException;
import javax.microedition.pim.PIMException;
import javax.microedition.pim.UnsupportedFieldException;
import com.nokia.mj.impl.pim.ErrorString;
import com.nokia.mj.impl.pim.GenericException;
import com.nokia.mj.impl.utils.OsErrorMessage;

// CLASS DEFINITION
/**
 * Native side error codes and common error handling operations. In general, the
 * native side error codes are interpreted to Java exceptions in failing native
 * side invocations. There are groups of functionality that share common
 * interpretation for a set of error codes; for these a common error handling
 * operation is provided.
 *
 * The class must be public so that in can be accessed from the
 * javax.microedition.pim package (classes PIM and RepeatRule).
 */
public final class NativeError
{

    // @{
    /**
     * Checks given native-originating error code for field handling operations
     * and in error throws a suitable exception for that error.
     *
     * The field handling operations are present mainly in PIMItemImpl and
     * PIMListImpl classes (data accessors, support checkers, label getters).
     *
     * The native error codes and corresponding exceptions are listed below.
     *
     * @param aNativeErrorCode
     *            The native error code to check. For error codes other than
     *            OsErrorMessage.SUCCESS an exception is thrown.
     *
     * @param aField
     *            The field to mention in the exception description where
     *            applicable.
     *
     * @param aIndex
     *            The index to mention in the exception description where
     *            applicable.
     *
     * @exception IllegalArgumentException
     *                for OsErrorMessage.KERR_ARGUMENT (invalid field) and
     *                OsErrorMessage.KERR_TOO_BIG (invalid value).
     *
     * @exception IndexOutOfBoundsException
     *                for OsErrorMessage.KERR_NOT_FOUND.
     *
     * @exception UnsupportedFieldException
     *                for OsErrorMessage.KERR_NOT_SUPPORTED.
     *
     * @exception Generic Exception with code
     *                for any other error.
     */
    public static void handleFieldHandlingError(int aNativeErrorCode, int aField,
            int aIndex)
    {
        if (aNativeErrorCode == OsErrorMessage.SUCCESS)
        {
            return; // OK
        }

        switch (aNativeErrorCode)
        {
        case OsErrorMessage.KERR_ARGUMENT:
        {
            throw new IllegalArgumentException(ErrorString.INVALID_FIELD_COLON
                                               + aField);
        }
        case OsErrorMessage.KERR_NOT_SUPPORTED:
        {
            throw new UnsupportedFieldException(ErrorString.UNSUPPORTED_FIELD_COLON
                                                + aField, aField);
        }
        case OsErrorMessage.KERR_NOT_FOUND:
        {
            throw new IndexOutOfBoundsException(ErrorString.INVALID_INDEX_COLON + aIndex);
        }
        case OsErrorMessage.KERR_TOO_BIG:
        {
            throw new IllegalArgumentException(ErrorString.INVALID_VALUE);
        }
        case OsErrorMessage.KERR_OVERFLOW:
        {
            throw new FieldFullException(ErrorString.FIELD_IS_FULL);
        }
        case OsErrorMessage.KERR_LOCKED:
        {
            throw new IllegalArgumentException(ErrorString.FIELD_IS_READ_ONLY);
        }
        case OsErrorMessage.KERR_NO_MEMORY:
        {
            throw new OutOfMemoryError();
        }
        default:
        {
            throw new GenericException(ErrorString.GENERAL_ERROR_COLON + aNativeErrorCode);
        }
        }
    }

    public static void handleFieldHandlingError(int aNativeErrorCode, int aField)
    {
        handleFieldHandlingError(aNativeErrorCode, aField, 0);
    }

    // @}

    // @{
    /**
     * Checks given native-originating error code for category handling
     * operations and in error throws a suitable exception for that error.
     *
     * The category handling operations are present mainly in PIMListImpl and
     * PIMItemImpl classes.
     *
     * The native error codes and corresponding exceptions are listed below.
     *
     * @param aNativeErrorCode
     *            The native error code to check. For error codes other than
     *            OsErrorMessage.SUCCESS an exception is thrown.
     *
     * @param aCategory
     *            The category to mention in the exception description where
     *            applicable.
     *
     * @exception PIMException
     *                for OsErrorMessage.KERR_SESSION_CLOSED,
     *                OsErrorMessage.KERR_ARGUMENT,
     *                OsErrorMessage.KERR_NOT_SUPPORTED,
     *                OsErrorMessage.KERR_NOT_FOUND and for any other errors by
     *                default.
     */
    public static void handleCategoryHandlingError(int aNativeErrorCode,
            String aCategory) throws PIMException
    {
        if (aNativeErrorCode == OsErrorMessage.SUCCESS)
        {
            return; // OK
        }

        switch (aNativeErrorCode)
        {
        case OsErrorMessage.KERR_SESSION_CLOSED:
        {
            throw new PIMException(ErrorString.LIST_IS_CLOSED, PIMException.LIST_CLOSED);
        }
        case OsErrorMessage.KERR_ARGUMENT:
        {
            String msg = null;
            if (aCategory != null)
            {
                msg = ErrorString.INVALID_CATEGORY_COLON + aCategory;
            }
            else
            {
                msg = ErrorString.INVALID_CATEGORY;
            }
            throw new PIMException(msg, PIMException.GENERAL_ERROR);
        }
        case OsErrorMessage.KERR_NOT_FOUND:
        {
            throw new PIMException(ErrorString.NO_SUCH_CATEGORY_COLON + aCategory,
                                   PIMException.GENERAL_ERROR);
        }
        case OsErrorMessage.KERR_NOT_SUPPORTED:
        {
            throw new PIMException(ErrorString.CATEGORIES_ARE_NOT_SUPPORTED,
                                   PIMException.FEATURE_NOT_SUPPORTED);
        }
        case OsErrorMessage.KERR_NO_MEMORY:
        {
            throw new OutOfMemoryError();
        }
        default:
        {
            throw new PIMException(ErrorString.GENERAL_ERROR,
                                   PIMException.GENERAL_ERROR);
        }
        }
    }

    public static void handleCategoryHandlingError(int aNativeErrorCode)
    throws PIMException
    {
        handleCategoryHandlingError(aNativeErrorCode, null);
    }



    public static void handleSerializationError(int aNativeErrorCode)
    throws PIMException
    {
        if (aNativeErrorCode == OsErrorMessage.SUCCESS)
        {
            return; // OK
        }

        switch (aNativeErrorCode)
        {
        case OsErrorMessage.KERR_NOT_FOUND:
        case OsErrorMessage.KERR_CORRUPT:
        case OsErrorMessage.KERR_TOO_BIG:
        case OsErrorMessage.KERR_NOT_SUPPORTED:
        case OsErrorMessage.KERR_EOF:
        {
            throw new PIMException(ErrorString.DATA_SERIALIZATION_FAILED,
                                   PIMException.GENERAL_ERROR);
        }
        case OsErrorMessage.KERR_NO_MEMORY:
        {
            throw new OutOfMemoryError();
        }
        default:
        {
            throw new GenericException(ErrorString.GENERAL_ERROR_COLON + aNativeErrorCode);
        }
        }
    }

    public static boolean checkSuccess(int aNativeErrorCode)
    {
        if (aNativeErrorCode == OsErrorMessage.SUCCESS)
        {
            return true;
        }
        return false;
    }

    public static boolean checkArgumentError(int aNativeErrorCode)
    {
        if (aNativeErrorCode == OsErrorMessage.KERR_ARGUMENT)
        {
            return true;
        }
        return false;
    }

    public static void handleRepeatRuleFieldsError(int aNativeErrorCode, int aFrequency)
    {
        switch (aNativeErrorCode)
        {
        case OsErrorMessage.SUCCESS:
        {
            return; // OK
        }
        case OsErrorMessage.KERR_ARGUMENT:
        {
            // We should never end up here because the argument is prechecked
            throw new IllegalArgumentException(ErrorString.INVALID_REPEATRULE_VALUE_COLON
                                               + aFrequency);
        }
        case OsErrorMessage.KERR_NO_MEMORY:
        {
            throw new OutOfMemoryError();
        }
        default:
        {
            throw new GenericException(ErrorString.GENERAL_ERROR_COLON + aNativeErrorCode);
        }
        }
    }

    public static void handleItemCommitError(int aNativeErrorCode) throws PIMException
    {
        switch (aNativeErrorCode)
        {
        case OsErrorMessage.SUCCESS:
        {
            return; // OK
        }
        case OsErrorMessage.KERR_ABORT:
        {
            throw new PIMException(ErrorString.COMMIT_OPERATION_FAILED_COLON +
                                   ErrorString.ITEM_HAS_INSUFFICIENT_DATA,
                                   PIMException.GENERAL_ERROR);
        }
        case OsErrorMessage.KERR_NOT_FOUND:
        {
            throw new PIMException(ErrorString.COMMIT_OPERATION_FAILED_COLON +
                                   ErrorString.ITEM_IS_REMOVED,
                                   PIMException.GENERAL_ERROR);
        }
        case OsErrorMessage.KERR_IN_USE:
        {
            throw new PIMException(ErrorString.COMMIT_OPERATION_FAILED_COLON +
                                   ErrorString.ITEM_IN_USE,
                                   PIMException.UPDATE_ERROR);
        }
        case OsErrorMessage.KERR_DISCONNECTED:
        {
            throw new PIMException(ErrorString.COMMIT_OPERATION_FAILED_COLON +
                                   ErrorString.LIST_IS_CLOSED,
                                   PIMException.LIST_CLOSED);
        }
        case OsErrorMessage.KERR_DISMOUNTED:
        {
            throw new PIMException(ErrorString.ITEM_DOES_NOT_BELONG_TO_A_LIST,
                                   PIMException.GENERAL_ERROR);
        }
        case OsErrorMessage.KERR_NO_MEMORY:
        {
            throw new OutOfMemoryError();
        }
        default:
        {
            throw new PIMException(ErrorString.GENERAL_ERROR,
                                   PIMException.GENERAL_ERROR);
        }
        }
    }

    public static void handleRemoveItemError(int aNativeErrorCode) throws PIMException
    {
        switch (aNativeErrorCode)
        {
        case OsErrorMessage.SUCCESS:
        {
            return; // OK
        }
        case OsErrorMessage.KERR_SESSION_CLOSED:
        {
            throw new PIMException(ErrorString.REMOVAL_FAILED_COLON +
                                   ErrorString.LIST_IS_CLOSED,
                                   PIMException.LIST_CLOSED);
        }
        case OsErrorMessage.KERR_ARGUMENT:
        {
            // It is an interesting situation if we get here,
            // because the item _was_ found in the Java side list.
            throw new PIMException(ErrorString.REMOVAL_FAILED_COLON +
                                   ErrorString.ITEM_IS_NOT_IN_THE_LIST,
                                   PIMException.GENERAL_ERROR);
        }
        case OsErrorMessage.KERR_NOT_FOUND:
        {
            throw new PIMException(ErrorString.REMOVAL_FAILED_COLON +
                                   ErrorString.ITEM_EXISTS_NO_MORE,
                                   PIMException.GENERAL_ERROR);
        }
        case OsErrorMessage.KERR_IN_USE:
        {
            throw new PIMException(ErrorString.ITEM_IN_USE,
                                   PIMException.GENERAL_ERROR);
        }
        case OsErrorMessage.KERR_NO_MEMORY:
        {
            throw new OutOfMemoryError();
        }
        default:
        {
            throw new GenericException(ErrorString.GENERAL_ERROR_COLON + aNativeErrorCode);
        }
        }
    }

    public static void handlePIMListCloseError(int aNativeErrorCode)
    throws PIMException
    {
        switch (aNativeErrorCode)
        {
        case OsErrorMessage.SUCCESS:
        {
            // OK
            return;
        }
        case OsErrorMessage.KERR_SESSION_CLOSED:
        {
            throw new PIMException(ErrorString.LIST_IS_ALREADY_CLOSED,
                                   PIMException.LIST_CLOSED);
        }
        default:
        {
            throw new PIMException(ErrorString.GENERAL_ERROR,
                                   PIMException.GENERAL_ERROR);
        }
        }
    }

    public static void handleArrayElementLabelError(int aNativeErrorCode)
    {
        switch (aNativeErrorCode)
        {
        case OsErrorMessage.SUCCESS:
        {
            // OK
            return;
        }
        case OsErrorMessage.KERR_ARGUMENT:
        {
            throw new IllegalArgumentException(ErrorString.CANNOT_OBTAIN_LABEL_COLON +
                                               ErrorString.INVALID_FIELD_OR_ARRAY_ELEMENT);
        }
        case OsErrorMessage.KERR_NOT_SUPPORTED:
        {
            throw new UnsupportedFieldException(ErrorString.CANNOT_OBTAIN_LABEL_COLON +
                                                ErrorString.UNSUPPORTED_FIELD_OR_ARRAY_ELEMENT);
        }
        case OsErrorMessage.KERR_NO_MEMORY:
        {
            throw new OutOfMemoryError();
        }
        default:
        {
            throw new GenericException(ErrorString.GENERAL_ERROR_COLON + aNativeErrorCode);
        }
        }
    }

    public static void handlegetAttributeLabelError(int aNativeErrorCode,
            int aAttribute)
    {
        switch (aNativeErrorCode)
        {
        case OsErrorMessage.SUCCESS:
        {
            // OK
            return;
        }
        case OsErrorMessage.KERR_ARGUMENT:
        {
            throw new IllegalArgumentException(ErrorString.CANNOT_OBTAIN_LABEL_COLON +
                                               ErrorString.INVALID_ATTRIBUTE_COLON
                                               + aAttribute);
        }
        case OsErrorMessage.KERR_NOT_SUPPORTED:
        {
            throw new UnsupportedFieldException(ErrorString.CANNOT_OBTAIN_LABEL_COLON +
                                                ErrorString.UNSUPPORTED_ATTRIBUTE_COLON
                                                + aAttribute, aAttribute);
        }
        case OsErrorMessage.KERR_NO_MEMORY:
        {
            throw new OutOfMemoryError();
        }
        default:
        {
            throw new GenericException(ErrorString.GENERAL_ERROR_COLON + aNativeErrorCode);
        }
        }
    }

    public static void handleUpdateListError(int aNativeErrorCode) throws PIMException
    {
        switch (aNativeErrorCode)
        {
        case OsErrorMessage.SUCCESS:
        {
            // OK
            return;
        }
        case OsErrorMessage.KERR_SESSION_CLOSED:
        {
            throw new PIMException(ErrorString.UPDATING_FAILED +
                                   ErrorString.LIST_IS_CLOSED,
                                   PIMException.LIST_CLOSED);
        }
        case OsErrorMessage.KERR_NO_MEMORY:
            // fall to default
        case OsErrorMessage.KERR_CORRUPT:
            // fall to default
        default:
        {
            throw new GenericException(ErrorString.GENERAL_ERROR_COLON + aNativeErrorCode);
        }
        }
    }

    public static void handleProcessItemResultsError(int aNativeErrorCode)
    throws PIMException
    {
        switch (aNativeErrorCode)
        {
        case OsErrorMessage.SUCCESS:
        {
            // OK
            break;
        }
        case OsErrorMessage.KERR_SESSION_CLOSED:
        {
            throw new PIMException(ErrorString.LIST_IS_CLOSED,
                                   PIMException.LIST_CLOSED);
        }
        case OsErrorMessage.KERR_ARGUMENT:
        {
            throw new IllegalArgumentException(ErrorString.INVALID_ARGUMENT);
        }
        case OsErrorMessage.KERR_NO_MEMORY:
        {
            throw new OutOfMemoryError();
        }
        default:
        {
            throw new PIMException(ErrorString.GENERAL_ERROR,
                                   PIMException.GENERAL_ERROR);
        }
        }
    }

    public static void handleListPIMListError(int aNativeErrorCode)
    {
        switch (aNativeErrorCode)
        {
        case OsErrorMessage.SUCCESS:
        {
            // OK
            return;
        }
        case OsErrorMessage.KERR_ARGUMENT:
        {
            // Shouldn't happen because arguments are prechecked
            throw new java.lang.IllegalArgumentException(ErrorString.INVALID_ARGUMENT);
        }
        case OsErrorMessage.KERR_NO_MEMORY:
        {
            throw new OutOfMemoryError();
        }
        default:
        {
            throw new GenericException(ErrorString.GENERAL_ERROR_COLON + aNativeErrorCode);
        }
        }
    }

    public static void handleOpenPIMListError(int aNativeErrorCode, int aPimListType,
            String aName) throws PIMException
    {
        switch (aNativeErrorCode)
        {
            // error code
        case OsErrorMessage.SUCCESS:
        {
            // OK
            return;
        }
        case OsErrorMessage.KERR_ARGUMENT:
        {
            // This should never happen, because the arguments
            // have been pre-checked before delegation to native side
            throw new java.lang.IllegalArgumentException(ErrorString.INVALID_ARGUMENT);
        }
        case OsErrorMessage.KERR_NOT_SUPPORTED:
        {
            throw new PIMException("List type " + aPimListType
                                   + " is not supported.", PIMException.FEATURE_NOT_SUPPORTED);
        }
        case OsErrorMessage.KERR_NOT_FOUND:
        {
            throw new PIMException(ErrorString.INVALID_LIST_NAME_COLON + aName,
                                   PIMException.GENERAL_ERROR);
        }
        case OsErrorMessage.KERR_ALREADY_EXISTS:
        {
            throw new PIMException(ErrorString.LIST_IS_ALREADY_OPEN,
                                   PIMException.GENERAL_ERROR);
        }
        case OsErrorMessage.KERR_NO_MEMORY:
        {
            throw new OutOfMemoryError();
        }
        default:
        {
            throw new PIMException(ErrorString.GENERAL_ERROR,
                                   PIMException.GENERAL_ERROR);
        }
        }
    }

    public static void handleToSerialFormatError(int aNativeErrorCode)
    throws PIMException
    {
        if (aNativeErrorCode == OsErrorMessage.KERR_NO_MEMORY)
        {
            throw new OutOfMemoryError();
        }
        else if (aNativeErrorCode != OsErrorMessage.SUCCESS)
        {
            throw new PIMException(ErrorString.DATA_SERIALIZATION_FAILED_COLON +
                                   ErrorString.ITEM_HAS_INVALID_DATA,
                                   PIMException.GENERAL_ERROR);
        }
    }

    /**
     * Common native-originating error handling for RepeatRule field handling
     * operations. Note that error handling for RepeatRule field handling
     * operations differ from those in PIMItem and PIMList.
     */
    public static void handleRRFieldHandlingError(int aNativeErrorCode,
            int aField)
    {
        if (aNativeErrorCode == OsErrorMessage.SUCCESS)
        {
            return; // OK
        }

        switch (aNativeErrorCode)
        {
        case OsErrorMessage.KERR_ARGUMENT:
        {
            throw new IllegalArgumentException(ErrorString.INVALID_FIELD_COLON
                                               + aField);
        }
        case OsErrorMessage.KERR_NOT_FOUND:
        {
            throw new FieldEmptyException(ErrorString.FIELD_NOT_FOUND_COLON + aField);
        }
        case OsErrorMessage.KERR_TOO_BIG:
        {
            throw new IllegalArgumentException(ErrorString.INVALID_VALUE);
        }
        case OsErrorMessage.KERR_NO_MEMORY:
        {
            throw new OutOfMemoryError();
        }
        default:
        {
            throw new GenericException(ErrorString.GENERAL_ERROR_COLON + aNativeErrorCode);
        }
        }
    }
    public static void handleCreateCalendarError(int aNativeErrorCode, String aCalName) 
    throws PIMException
    {
        if (aNativeErrorCode == OsErrorMessage.SUCCESS)
        {
            return; // OK
        }

        switch (aNativeErrorCode)
        {
        case OsErrorMessage.KERR_ARGUMENT:
        {
            throw new IllegalArgumentException(
                "Creation of calendar failed: calendar name is invalid"
                + aCalName);
        }
        case OsErrorMessage.KERR_BAD_NAME:
        {
            throw new IllegalArgumentException(
                "Creation of calendar failed: calendar name is not valid "
                + aCalName);
        }
        case OsErrorMessage.KERR_ALREADY_EXISTS:
        {
             throw new PIMException(ErrorString.CALENDAR_ALREADY_EXISTS, aNativeErrorCode);
        }
        default:
        {
            throw new GenericException(ErrorString.GENERAL_ERROR_COLON + aNativeErrorCode);
        }
        }
    }

    public static void handleDeleteCalendarError(int aNativeErrorCode, String aCalName) throws PIMException
    {


        if (aNativeErrorCode == OsErrorMessage.SUCCESS)
        {
            return; // OK
        }

        switch (aNativeErrorCode)
        {
        case OsErrorMessage.KERR_ACCESS_DENIED:
        {
            throw new PIMException("The default calendar cannot be deleted",  aNativeErrorCode);
        }
        case OsErrorMessage.KERR_ARGUMENT:
        {
            // we should never end up here since the calendar name is
            // verified to be not empty before making   the native call
            throw new IllegalArgumentException(
                "Deletion of calendar failed: calendar name is null");
        }
        case OsErrorMessage.KERR_NOT_FOUND:
        {
            throw new IllegalArgumentException(
                "Deletion of calendar failed: calendar by name "
                + aCalName + " cannot be found");
        }
        case OsErrorMessage.KERR_BAD_NAME:
        {
            throw new IllegalArgumentException(
                "Deletion of calendar failed: calendar name "
                + aCalName + " includes a path explicitly");
        }
        default:
        {
            throw new GenericException(ErrorString.GENERAL_ERROR_COLON + aNativeErrorCode);
        }
        }
    }
    /**
     * Construction prohibited.
     */
    private NativeError()
    {
    }

}
