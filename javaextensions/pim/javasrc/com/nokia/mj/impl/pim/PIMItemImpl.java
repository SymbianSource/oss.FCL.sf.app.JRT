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
* Description:  PIM item implementation base.
 *
*/


// PACKAGE
package com.nokia.mj.impl.pim;

// IMPORTS
import javax.microedition.pim.FieldFullException;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMException;
import javax.microedition.pim.PIMItem;
import javax.microedition.pim.PIMList;
import javax.microedition.pim.UnsupportedFieldException;
import com.nokia.mj.impl.pim.ErrorString;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.rt.support.ShutdownListener;
import com.nokia.mj.impl.pim.utils.NativeError;

// CLASS DEFINITION
/**
 * Base class for specialized PIMItem classes.
 */
public abstract class PIMItemImpl implements PIMItem
{
    // Constants

    /**
     * String that represents a "null" array element.
     *
     * At the time of writing a bug in the Monty JNI implementation prevents us
     * from using real null array elements. When a null array element is taken
     * from an array, a a local reference is created but not passed to the
     * caller. Thus the local references to null objects cannot be released, and
     * maximum number of local references (5) is easily exceeded. (For example:
     * a valid Contact.ADDR string array field may consist of 1 string element
     * and 6 null elements.)
     *
     * (At the time of writing the bug does not appear in the KVM JNI
     * implementation.)
     *
     * The value of this String MUST be the same as KPIMNullArrayElement
     * (pimcommon.h) in the native side.
     *
     * The String is intended to be short and hard to guess.
     */
    protected final static String PIM_NULL_ARRAY_ELEMENT = "T&%3#";



    // Member data
    private Finalizer iFinalizer;

    /**
     * Handle to the native side item. The handle refers to a specialized native
     * side item, and is used as such by the derived classes.
     *
     * If we were really paranoid we would make this one private and access it
     * only through getter methods from the derived classes. Consider the
     * PIMListImpl.items( PIMItem matchingItem ) scenario.
     */
    protected int iItemHandle = -1;

    /**
     * Type of the item.
     */
    protected int iItemType = -1;

    /** Associated list, if any. */
    protected PIMListImpl iList;

    // Constructors and finalization

    /**
     * Creates new PIMItemImpl.
     *
     * @li Does not create or set the native side peer object.
     * @li Does not associate the item with a list.
     * @li Registers for finalization, but if the item handle is never set, the
     *     native side implementation is not attempted to dispose.
     */
    protected PIMItemImpl()
    {
        setShutdownListener();
        iFinalizer = registerForFinalization();
    }

    /**
     * Creates new PIMItemImpl and associates it with its native peer object.
     *
     * @li Sets the native side peer object.
     * @li Does not associate the item with a list.
     * @li Registers for finalization and makes the native side implementation
     *     to be disposed in finalization.
     */
    protected PIMItemImpl(int aItemHandle)
    {
        this();
        iItemHandle = aItemHandle;
    }

    /**
     * Creates new PIMItemImpl and associates it with its native peer object and
     * a PIMList.
     *
     * @li Sets the native side peer object.
     * @li Associates the item with a list.
     * @li Registers for finalization and makes the native side implementation
     *     to be disposed in finalization.
     */
    protected PIMItemImpl(int aItemHandle, PIMListImpl aList)
    {
        this(aItemHandle);
        iList = aList;
    }

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
        if (iItemHandle != 0)
        {
            _dispose(iItemHandle);
            iItemHandle = 0;
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

                if (iItemHandle != 0)
                {
                    _dispose(iItemHandle);
                    iItemHandle = 0;
                }
            }

        });
    }

    // New methods

    /**
     * Gets the JNI native handle of this item.
     */
    int jniNativeHandle()
    {
        return iItemHandle;
    }

    /**
     * Gets the type of this item.
     */
    int getItemType()
    {
        return iItemType;
    }

    /**
     * Removes list association
     */
    void removeListAssociation()
    {
        iList = null;
    }

    /**
     * Copies the contents of given PIMItem-implementing object. New data is
     * appended to the existing data.
     *
     * Data is copied in a "best effort" manner. Unsupported fields are ignored.
     *
     * Any other exceptions are passed out from the method. If the given item
     * originates from PIM API, no errors should occur. If the given item does
     * not originate from PIM API, errors let the creator of the item to detect
     * errors in that item's implementation.
     *
     * Only the categories that are valid in the list are copied.
     */
    protected void baseCopyFrom(PIMItem aItem)
    {
        // Copy fields
        int[] fields = aItem.getFields();
        for (int fieldIndex = 0; fieldIndex < fields.length; fieldIndex++)
        {
            int field = fields[fieldIndex];

            int numValues = aItem.countValues(field);

            int fieldDataType = _fieldDataType(iItemHandle, field);

            for (int valueIndex = 0; valueIndex < numValues; valueIndex++)
            {

                int attributes = aItem.getAttributes(field, valueIndex);

                // About the following switch-case:
                //
                // If an exception is thrown while getting the value,
                // let it out because it probably indicates an error
                // in the given item.
                //
                // If an UnsupportedFieldException occurs when adding
                // the value, we catch and ignore it; other exceptions
                // might indicate a real error.

                try
                {
                    switch (fieldDataType)
                    {
                    case PIMItem.BOOLEAN:
                    {
                        boolean value = aItem.getBoolean(field, valueIndex);
                        addBoolean(field, attributes, value);
                        break;
                    }

                    case PIMItem.DATE:
                    {
                        long value = aItem.getDate(field, valueIndex);
                        addDate(field, attributes, value);
                        break;
                    }

                    case PIMItem.INT:
                    {
                        int value = aItem.getInt(field, valueIndex);
                        addInt(field, attributes, value);
                        break;
                    }

                    case PIMItem.BINARY:
                    {
                        byte[] value = aItem.getBinary(field, valueIndex);
                        addBinary(field, attributes, value,0,value.length);
                        break;
                    }

                    case PIMItem.STRING:
                    {
                        String value = aItem.getString(field, valueIndex);
                        addString(field, attributes, value);
                        break;
                    }

                    case PIMItem.STRING_ARRAY:
                    {
                        String[] value = aItem.getStringArray(field,
                                                              valueIndex);
                        addStringArray(field, attributes, value);
                        break;
                    }

                    default:
                    {
                        // We should not end up here
                        throw new IllegalArgumentException(
                            ErrorString.INVALID_FIELD_DATA_TYPE_COLON + fieldDataType);
                    }
                    }
                }
                catch (UnsupportedFieldException ufe)
                {
                    // Ignore; the value is not supported in the list
                    // this item belongs to. It is preferable to drop
                    // such fields.
                }
                catch (FieldFullException ffe)
                {
                    // Ignore; no more than supported maximum number
                    // of values can be added.
                }

            }
        }

        // Copy categories.
        String[] categories = aItem.getCategories();
        for (int categoryIndex = 0; categoryIndex < categories.length; categoryIndex++)
        {
            try
            {
                addToCategory(categories[categoryIndex]);
            }
            catch (PIMException pe)
            {
                // Ignore; the category is not supported in the list
                // this item belongs to. It is preferable to drop such
                // categories.
            }
        }
    }

    /**
     * Checks binary field handling operation arguments. Applies to:
     *
     * @li addBinary()
     * @li setBinary()
     */
    void checkBinaryHandlingArguments(byte[] aValue, int aOffset, int aLength)
    {
        if (aValue == null)
        {
            throw new NullPointerException(ErrorString.VALUE_IS_NULL);
        }

        if (aValue.length == 0)
        {
            throw new IllegalArgumentException(ErrorString.VALUE_IS_EMPTY);
        }

        if (aOffset < 0 || aOffset >= aValue.length)
        {
            throw new IllegalArgumentException(ErrorString.INVALID_OFFSET_COLON
                                               + aOffset);
        }

        if (aLength <= 0)
        {
            throw new IllegalArgumentException(ErrorString.INVALID_LENGTH_COLON
                                               + aLength);
        }

        // Note that if ( aOffset + aLength ) > aValue.length
        // implementation will (and by definition has to) take that
        // as "from aOffset to the end of the byte array".
    }


    /**
     * Provides the most informative description of the item, i.e.
     * contents of a string field that best identifies the item among
     * other items of same type. The info string is shown in the UI.
     *
     * For Contacts, this should be the Contact name.
     * For Events, this should be the Event summary.
     * For ToDos, this should be the ToDo summary.
     *
     * If the most descriptive field is, for some reason, not present,
     * a secondary one should be provided (e.g. Company for a Contact
     * missing the name etc.)
     *
     * @return A short, informative string identifying this item;
     *         \c null if no reasonable information is available.
     */
    abstract String getShortInfo();

    // Methods from PIMItem
    public synchronized PIMList getPIMList()
    {
        return iList;
    }

    public synchronized void commit() throws PIMException
    {
        if (iList == null)
        {
            throw new PIMException(ErrorString.ITEM_DOES_NOT_BELONG_TO_A_LIST,
                                   PIMException.GENERAL_ERROR);
        }

        if (!iList.isOpen())
        {
            throw new PIMException(ErrorString.COMMIT_OPERATION_FAILED_COLON +
                                   ErrorString.LIST_IS_CLOSED, PIMException.LIST_CLOSED);
        }

        iList.checkModeMinimum(PIM.WRITE_ONLY);
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        // Get localized text info for the security dialog
        String action = null;
        switch (iList.getListType())
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

        PIMPermissionImpl per = new PIMPermissionImpl(action, getShortInfo(), iList.getName(), PIMPermissionImpl.OPERATION_ITEM_COMMIT);

        // Ensure permission from PIM API security
        appUtils.checkPermission(per);

        int err = _commit(iItemHandle);

        NativeError.handleItemCommitError(err);

        iList.itemCommitted(this);
    }

    public synchronized boolean isModified()
    {
        return _isModified(iItemHandle);
    }

    public synchronized int[] getFields()
    {
        int[] fields = _getFields(iItemHandle);

        if (fields == null)
        {
            throw new OutOfMemoryError();
        }

        return fields;
    }

    public synchronized byte[] getBinary(int aField, int aIndex)
    {
        int[] error = new int[1];

        byte[] value = _getBinary(iItemHandle, aField, aIndex, error);

        NativeError.handleFieldHandlingError(error[0], aField, aIndex);
        return value;
    }

    public synchronized void addBinary(int aField, int aAttributes,
                                       byte[] aValue, int aOffset, int aLength)
    {
        checkBinaryHandlingArguments(aValue, aOffset, aLength);

        int error = _addBinary(iItemHandle, aField, aAttributes,
                               aValue, aOffset, aLength);

        NativeError.handleFieldHandlingError(error, aField);
    }

    public synchronized void setBinary(int aField, int aIndex, int aAttributes,
                                       byte[] aValue, int aOffset, int aLength)
    {
        checkBinaryHandlingArguments(aValue, aOffset, aLength);

        int error = _setBinary(iItemHandle, aField, aIndex, aAttributes,
                               aValue, aOffset, aLength);

        NativeError.handleFieldHandlingError(error, aField, aIndex);
    }

    public synchronized long getDate(int aField, int aIndex)
    {
        int[] error = new int[1];

        long value = _getDate(iItemHandle, aField, aIndex, error);

        NativeError.handleFieldHandlingError(error[0], aField, aIndex);
        return value;
    }

    public synchronized void addDate(int aField, int aAttributes, long aValue)
    {
        int error = _addDate(iItemHandle, aField, aAttributes, aValue);

        NativeError.handleFieldHandlingError(error, aField);
    }

    public synchronized void setDate(int aField, int aIndex, int aAttributes,
                                     long aValue)
    {
        int error = _setDate(iItemHandle, aField, aIndex, aAttributes, aValue);

        NativeError.handleFieldHandlingError(error, aField, aIndex);
    }

    public synchronized int getInt(int aField, int aIndex)
    {
        int[] error = new int[1];

        int value = _getInt(iItemHandle, aField, aIndex, error);

        NativeError.handleFieldHandlingError(error[0], aField, aIndex);
        return value;
    }

    public synchronized void addInt(int aField, int aAttributes, int aValue)
    {
        int error = _addInt(iItemHandle, aField, aAttributes, aValue);

        NativeError.handleFieldHandlingError(error, aField);
    }

    public synchronized void setInt(int aField, int aIndex, int aAttributes,
                                    int aValue)
    {
        int error = _setInt(iItemHandle, aField, aIndex, aAttributes, aValue);

        NativeError.handleFieldHandlingError(error, aField, aIndex);
    }

    public synchronized String getString(int aField, int aIndex)
    {
        int[] error = new int[1];

        String value = _getString(iItemHandle, aField, aIndex, error);

        NativeError.handleFieldHandlingError(error[0], aField, aIndex);
        return value;
    }

    public synchronized void addString(int aField, int aAttributes,
                                       String aValue)
    {
        if (aValue == null)
        {
            throw new NullPointerException(ErrorString.VALUE_IS_NULL);
        }

        int error = _addString(iItemHandle, aField, aAttributes, aValue);

        NativeError.handleFieldHandlingError(error, aField);
    }

    public synchronized void setString(int aField, int aIndex, int aAttributes,
                                       String aValue)
    {
        if (aValue == null)
        {
            throw new NullPointerException(ErrorString.VALUE_IS_NULL);
        }

        int error = _setString(iItemHandle, aField, aIndex, aAttributes, aValue);

        NativeError.handleFieldHandlingError(error, aField, aIndex);
    }

    public synchronized boolean getBoolean(int aField, int aIndex)
    {
        int[] error = new int[1];

        boolean value = _getBoolean(iItemHandle, aField, aIndex, error);

        NativeError.handleFieldHandlingError(error[0], aField, aIndex);
        return value;
    }

    public synchronized void addBoolean(int aField, int aAttributes,
                                        boolean aValue)
    {
        int error = _addBoolean(iItemHandle, aField, aAttributes, aValue);

        NativeError.handleFieldHandlingError(error, aField);
    }

    public synchronized void setBoolean(int aField, int aIndex,
                                        int aAttributes, boolean aValue)
    {
        int error = _setBoolean(iItemHandle, aField, aIndex, aAttributes,
                                aValue);

        NativeError.handleFieldHandlingError(error, aField, aIndex);
    }

    public synchronized String[] getStringArray(int aField, int aIndex)
    {
        int[] error = new int[1];

        String[] value = _getStringArray(iItemHandle, aField, aIndex, error);

        NativeError.handleFieldHandlingError(error[0], aField, aIndex);

        // JNI has problems with large number of null objects.
        // null strings have been replaced with "null tag" strings.
        // Replace the "null tags" with real null references.
        unTagNullArrayElements(value);

        return value;
    }

    public synchronized void addStringArray(int aField, int aAttributes,
                                            String[] aValue)
    {
        if (aValue == null)
        {
            throw new NullPointerException(ErrorString.VALUE_IS_NULL);
        }

        // JNI has problems with large number of null objects.
        // Replace null objects with "null tag" strings.
        // Leave original value unaltered.
        String[] taggedValue = tagNullArrayElements(aValue);

        int error = _addStringArray(iItemHandle, aField, aAttributes, taggedValue);

        NativeError.handleFieldHandlingError(error, aField);
    }

    public synchronized void setStringArray(int aField, int aIndex,
                                            int aAttributes, String[] aValue)
    {
        if (aValue == null)
        {
            throw new NullPointerException(ErrorString.VALUE_IS_NULL);
        }

        // JNI has problems with large number of null objects.
        // Replace null objects with "null tag" strings.
        // Leave original value unaltered.
        String[] taggedValue = tagNullArrayElements(aValue);

        int error = _setStringArray(iItemHandle, aField, aIndex, aAttributes, taggedValue);

        NativeError.handleFieldHandlingError(error, aField, aIndex);
    }

    public synchronized int countValues(int aField)
    {
        int numValues = _countValues(iItemHandle, aField);

        // Negative value indicates error; zero and positive values
        // are not handled.
        if (numValues < 0)
        {
            NativeError.handleFieldHandlingError(numValues, aField);
        }
        return numValues;
    }

    public synchronized void removeValue(int aField, int aIndex)
    {
        int error = _removeValue(iItemHandle, aField, aIndex);

        NativeError.handleFieldHandlingError(error, aField, aIndex);
    }

    public synchronized int getAttributes(int aField, int aIndex)
    {
        int[] error = new int[1];

        int attributes = _getAttributes(iItemHandle, aField, aIndex, error);

        NativeError.handleFieldHandlingError(error[0], aField, aIndex);
        return attributes;
    }

    public synchronized void addToCategory(String aCategory)
    throws PIMException
    {
        if (aCategory == null)
        {
            throw new NullPointerException(ErrorString.CATEGORY_IS_NULL);
        }

        int error = _addToCategory(iItemHandle, aCategory);

        NativeError.handleCategoryHandlingError(error, aCategory);
    }

    public synchronized void removeFromCategory(String aCategory)
    {
        if (aCategory == null)
        {
            throw new NullPointerException(ErrorString.CATEGORY_IS_NULL);
        }

        _removeFromCategory(iItemHandle, aCategory);
    }

    public synchronized String[] getCategories()
    {
        String[] categories = _getCategories(iItemHandle);

        if (categories == null)
        {
            throw new OutOfMemoryError();
        }

        return categories;
    }

    public synchronized int maxCategories()
    {
        return _maxCategories(iItemHandle);
    }

    /**
     * Replaces null elements in a string array with PIM_NULL_ARRAY_ELEMENT
     * strings.
     *
     * See PIM_NULL_ARRAY_ELEMENT description to find out why.
     *
     * @param aStringArray
     *            The original array, which is not altered.
     *
     * @return A copy of \a aStringArray with null elements replaced with
     *         PIM_NULL_ARRAY_ELEMENT.
     */
    protected String[] tagNullArrayElements(String[] aStringArray)
    {
        String[] taggedArray = new String[aStringArray.length];

        for (int i = 0; i < aStringArray.length; i++)
        {
            if (aStringArray[i] == null)
            {
                taggedArray[i] = PIM_NULL_ARRAY_ELEMENT;
            }
            else
            {
                taggedArray[i] = aStringArray[i];
            }
        }

        return taggedArray;
    }

    /**
     * Replaces elements tagged by PIM_NULL_ARRAY_ELEMENT with null elements.
     *
     * @param aStringArray
     *            String array from which the tags are to be removed. Removing
     *            alters the argument.
     *
     * See tagNullArrayElements() method.
     */
    protected void unTagNullArrayElements(String[] aStringArray)
    {
        for (int i = 0; i < aStringArray.length; i++)
        {
            if (PIM_NULL_ARRAY_ELEMENT.equals(aStringArray[i]))
            {
                aStringArray[i] = null;
            }
        }
    }

    /**
     * Ensures that accessing the first value of a field does not throw
     * UnsupportedFieldException or IndexOutOfBoundsException. If the item
     * belongs to a list, the list is consulted for support. Then it is ensured
     * that the field has at least one value. Note that if the item does not
     * belong to a list, it supports any valid (standard and extended) fields.
     *
     * Primarily a helper function to derived getShortInfo() methods.
     *
     * @throws IllegalArgumentException
     *             if aField is invalid.
     */
    protected boolean isFieldSupportedAndPresent(int aField)
    {
        return (((iList == null) || iList.isSupportedField(aField)) && (countValues(aField) > 0));
    }

    // Native operations

    /**
     * Disposes the native side PIM item.
     */
    private native void _dispose(int aItemHandle);

    private native int _fieldDataType(int aItemHandle, int aField);

    /**
     * @return Error code.
     */
    private native int _commit(int aItemHandle);

    private native boolean _isModified(int aItemHandle);

    /**
     * @return null on error.
     */
    private native int[] _getFields(int aItemHandle);

    /**
     * \a aField and \a aIndex are checked in the native side.
     *
     * @param aError
     *            Resulting error code.
     */
    private native byte[] _getBinary(int aItemHandle, int aField, int aIndex,
                                     int[] aError);

    /**
     * @return Error code.
     */
    private native int _addBinary(int aItemHandle, int aField, int aAttributes,
                                  byte[] aValue, int aOffset, int aLength);

    /**
     * \a aField and \a aIndex are checked in the native side.
     *
     * @return Error code.
     */
    private native int _setBinary(int aItemHandle, int aField, int aIndex,
                                  int aAttributes, byte[] aValue, int aOffset, int aLength);

    /**
     * \a aField and \a aIndex are checked in the native side.
     *
     * @param aError
     *            Resulting error code.
     */
    private native long _getDate(int aItemHandle, int aField, int aIndex,
                                 int[] aError);

    /**
     * \a aField is checked in the native side.
     *
     * @return Error code.
     */
    private native int _addDate(int aItemHandle, int aField, int aAttributes,
                                long aValue);

    /**
     * \a aField and \a aIndex are checked in the native side.
     *
     * @return Error code.
     */
    private native int _setDate(int aItemHandle, int aField, int aIndex,
                                int aAttributes, long aValue);

    /**
     * \a aField and \a aIndex are checked in the native side.
     *
     * @param aError
     *            Resulting error code.
     */
    private native int _getInt(int aItemHandle, int aField, int aIndex,
                               int[] aError);

    /**
     * \a aField is checked in the native side.
     *
     * @return Error code.
     */
    private native int _addInt(int aItemHandle, int aField, int aAttributes,
                               int aValue);

    /**
     * \a aField and \a aIndex are checked in the native side.
     *
     * @return Error code.
     */
    private native int _setInt(int aItemHandle, int aField, int aIndex,
                               int aAttributes, int aValue);

    /**
     * \a aField and \a aIndex are checked in the native side.
     *
     * @param aError
     *            Resulting error code.
     */
    private native String _getString(int aItemHandle, int aField, int aIndex,
                                     int[] aError);

    /**
     * \a aField is checked in the native side.
     *
     * @return Error code.
     */
    private native int _addString(int aItemHandle, int aField, int aAttributes,
                                  String aValue);

    /**
     * \a aField and \a aIndex are checked in the native side.
     *
     * @return Error code.
     */
    private native int _setString(int aItemHandle, int aField, int aIndex,
                                  int aAttributes, String aValue);

    /**
     * \a aField and \a aIndex are checked in the native side.
     *
     * @param aError
     *            Resulting error code.
     */
    private native boolean _getBoolean(int aItemHandle, int aField, int aIndex,
                                       int[] aError);

    /**
     * \a aField is checked in the native side.
     *
     * @return Error code.
     */
    private native int _addBoolean(int aItemHandle, int aField, int aAttributes,
                                   boolean aValue);

    /**
     * \a aField and \a aIndex are checked in the native side.
     *
     * @return Error code.
     */
    private native int _setBoolean(int aItemHandle, int aField, int aIndex,
                                   int aAttributes, boolean aValue);

    /**
     * \a aField and \a aIndex are checked in the native side.
     *
     * @param aError
     *            Resulting error code.
     */
    private native String[] _getStringArray(int aItemHandle, int aField, int aIndex,
                                            int[] aError);

    /**
     * \a aField is checked in the native side.
     *
     * @return Error code.
     */
    private native int _addStringArray(int aItemHandle, int aField, int aAttributes,
                                       String[] aValue);

    /**
     * \a aField and \a aIndex are checked in the native side.
     *
     * @return Error code.
     */
    private native int _setStringArray(int aItemHandle, int aField, int aIndex,
                                       int aAttributes, String[] aValue);

    /**
     * @return Number of values or negative value on error.
     */
    private native int _countValues(int aItemHandle, int aField);

    /**
     * @return Error code.
     */
    private native int _removeValue(int aItemHandle, int aField, int aIndex);

    private native int _getAttributes(int aItemHandle, int aField, int aIndex, int[] aError);

    /**
     * @return Error code.
     */
    private native int _addToCategory(int aItemHandle, String aCategory);

    private native void _removeFromCategory(int aItemHandle, String aCategory);

    /**
     * @return null on error.
     */
    private native String[] _getCategories(int aItemHandle);

    private native int _maxCategories(int aItemHandle);

}
