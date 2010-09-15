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
* Description:  PIM list implementation base.
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
import java.util.Enumeration;
import java.util.Date;
import java.util.Vector;
import com.nokia.mj.impl.pim.ErrorString;
import com.nokia.mj.impl.pim.GenericException;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.rt.support.ShutdownListener;
import com.nokia.mj.impl.pim.utils.NativeError;
import com.nokia.mj.impl.utils.Logger;

// CLASS DEFINITION
/**
 * Base class for specialized PIMList classes.
 *
 * @par About item creation: New PIMItem-derived classes are created during list
 *      update. The list is updated whenever items are enumerated with one of
 *      the items() methods. During the update native-originated changes are
 *      synchronized. When items are enumerated for the first time from the
 *      list, whole content of the corresponding native database is considered
 *      as new items and are created at that time. See updateList() method.
 */
public abstract class PIMListImpl implements PIMList
{
    // Constants

    /** Label type Field. */
    private static final int LABEL_TYPE_FIELD = 1;

    /** Label type Attribute. */
    private static final int LABEL_TYPE_ATTRIBUTE = 2;

    /** Label type Array Element. */
    private static final int LABEL_TYPE_ARRAY_ELEMENT = 3;

    /** Enumeration flag: all items. See items(). */
    protected final static int ENUMERATION_ITEMS_ALL = 0;

    /** Enumeration flag: items matching given item. See items(). */
    protected final static int ENUMERATION_ITEMS_MATCHING_ITEM = 1;

    /** Enumeration flag: items matching given string. See items(). */
    protected final static int ENUMERATION_ITEMS_MATCHING_STRING = 2;

    /** Enumeration flag: items matching given category. See items(). */
    protected final static int ENUMERATION_ITEMS_MATCHING_CATEGORY = 3;

    // Member data
    private Finalizer iFinalizer;

    /** JNI native handle to the native side peer object. */
    protected int iListHandle;

    /**
     * Items.
     */
    protected ItemTable iItems;

    /**
     * Access mode, one of:
     *
     * @li PIM.READ_ONLY
     * @li PIM.WRITE_ONLY
     * @li PIM.READ_WRITE.
     */
    private int iMode = 0;

    /**
     * State flag, open or closed. New instance of PIMListImpl is always
     * initially open.
     */
    private boolean iIsOpen = true;

    // Constructors

    /**
     * Initializes the list with native side Event Source and list peer object
     * handles and registers the object for finalization.
     *
     * Given mode applies to all security vulnerable operations.
     */
    PIMListImpl(int aListHandle, int aMode)
    {
        iListHandle = aListHandle;
        iMode = aMode;
        Logger.LOG(Logger.EPim,Logger.EInfo,"+PIMListImpl() = iListHandle = "+iListHandle);
        if (iListHandle != 0)
        {
            iItems = new ItemTable();
        }
        setShutdownListener();
        iFinalizer = registerForFinalization();
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

        if (iListHandle != 0)
        {
            _dispose(iListHandle);
            iListHandle = 0;
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
                if (iListHandle != 0)
                {
                    _dispose(iListHandle);
                    iListHandle = 0;
                }
            }
        });
    }

    // New methods

    /**
     * Gets the list type. Used for resolving permissions.
     */
    abstract int getListType();

    /**
     * Determines whether this list is still open or has it been closed.
     */
    boolean isOpen()
    {
        return iIsOpen;
    }

    /**
     * Items that are associated with this list call this method when they are
     * committed. When an item is committed it is checked whether belongs to the
     * list already and added if not. If the item has been recently created and
     * never committed, it does not yet belong to the list.
     */
    void itemCommitted(PIMItemImpl aCommittedItem)
    {
        if (!iItems.containsItem(aCommittedItem))
        {
            int committedItemHandle = aCommittedItem.jniNativeHandle();

            int error = _addCommittedItem(iListHandle, committedItemHandle);
            if (!NativeError.checkSuccess(error))
            {
                throw new GenericException(ErrorString.GENERAL_ERROR_COLON + error);
            }

            iItems.addItem(aCommittedItem);
        }
    }

    /**
     * Creates a new specialized item and associates it with the list. This is
     * the common implementation of createXyz() methods in specialized lists.
     * The polymorphic behaviour is implemented in the native side.
     */
    protected PIMItemImpl createItem()
    {
        int[] err = new int[1];
        int itemHandle = _createItem(iListHandle, err);
        if (!NativeError.checkSuccess(err[0]))
        {
            throw new GenericException(ErrorString.GENERAL_ERROR_COLON + err[0]);
        }

        PIMItemImpl item = createAssociatedItem(itemHandle);
        return item;
    }

    protected void removeItem(PIMItem aItem) throws PIMException
    {
        if (aItem == null)
        {
            throw new NullPointerException(ErrorString.ITEM_IS_NULL);
        }

        if (!(aItem instanceof PIMItemImpl))
        {
            throw new PIMException(ErrorString.REMOVAL_FAILED_COLON +
                                   ErrorString.INVALID_ITEM,
                                   PIMException.GENERAL_ERROR);
        }

        // Check that list is not closed
        if (!isOpen())
        {
            throw new PIMException(ErrorString.REMOVAL_FAILED_COLON +
                                   ErrorString.LIST_IS_CLOSED, PIMException.LIST_CLOSED);
        }

        PIMItemImpl item = (PIMItemImpl) aItem;

        if (!iItems.containsItem(item))
        {
            throw new PIMException(ErrorString.REMOVAL_FAILED_COLON +
                                   ErrorString.ITEM_IS_NOT_IN_THE_LIST,
                                   PIMException.GENERAL_ERROR);
        }
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        String action = null;
        switch (getListType())
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

        // Get localized text info for the security dialog
        PIMPermissionImpl per = new PIMPermissionImpl(action, item.getShortInfo(), getName(), PIMPermissionImpl.OPERATION_ITEM_DELETION);

        // Ensure permission from PIM API security
        appUtils.checkPermission(per);


        // First remove the native side item and only if that
        // succeeds, the java side item.

        int itemHandle = item.jniNativeHandle();
        int err = _removeItem(iListHandle, itemHandle);
        NativeError.handleRemoveItemError(err);

        PIMItemImpl removedItem = iItems.removeItem(item);

        if (removedItem != null) // shouldn't be null
        {
            removedItem.removeListAssociation();
        }
    }

    /**
     * Adds or sets new value for the REVISION field. This function is used when
     * item is imported and after an import it should countain a new REVISION
     * field
     *
     * @param aItem
     *            Item to which the field is to be set
     * @param aField
     *            A specific REVISION field
     * @param aAttributes
     *            Attributes if any
     */
    protected void updateRevisionField(PIMItem aItem, int aField,
                                       int aAttributes)
    {
        // Get the associated list of this item
        PIMList list = aItem.getPIMList();

        // If the field is supported it can be added
        if (list.isSupportedField(aField))
        {
            Date thisDate = new Date();
            try
            {
                // Check that if there is a value already
                if (aItem.countValues(aField) > 0)
                {
                    aItem.setDate(aField, 0, aAttributes, thisDate.getTime());
                }
                else
                {
                    aItem.addDate(aField, aAttributes, thisDate.getTime());
                }
            }
            catch (UnsupportedFieldException ufe)
            {
                // Ignore; the value is not supported in the list
                // this item belongs to. This exception should not
                // be thrown but we still have to catch it
            }
            catch (FieldFullException ffe)
            {
                // Ignore; no more than supported maximum number
                // of values can be added. This exception should
                // not be thrown but we still have catch it
            }
        }
    }

    // Methods from PIMList

    public synchronized String getName()
    {
        String name = _getName(iListHandle);

        if (name == null)
        {
            throw new java.lang.OutOfMemoryError();
        }

        return name;
    }

    public synchronized void close() throws PIMException
    {
        if (iIsOpen == true)
        {
            iIsOpen = false;
            if (iListHandle != 0)
            {
                int err = _close(iListHandle);
                NativeError.handlePIMListCloseError(err);
            }
            iItems = null;
        }
        else
        {
            throw new PIMException(ErrorString.LIST_IS_ALREADY_CLOSED,
                                   PIMException.LIST_CLOSED);
        }
    }

    public synchronized Enumeration items() throws PIMException
    {
        // security checks must be done here because of TCK
        ensurePimPermission(PIM.READ_ONLY);
        return doItems(ENUMERATION_ITEMS_ALL, 0, null);
    }

    /**
     * @par Notes: The given item must originate from this list, which is
     *      required in the description of this method in the PIM API spec.
     *      Ambiguously, the ContactList, EventList and ToDoList permit the item
     *      to be basically any item that implements the Contact, Event or ToDo
     *      interface, respectively. However we will live up to the tighter
     *      restrictions - for which the TCK seems to test, too.
     */
    public synchronized Enumeration items(PIMItem aMatchingItem)
    throws PIMException
    {
        // security checks must be done here because of TCK
        ensurePimPermission(PIM.READ_ONLY);

        if (aMatchingItem == null)
        {
            throw new NullPointerException(ErrorString.ITEM_IS_NULL);
        }

        if (!(aMatchingItem instanceof PIMItemImpl))
        {
            throw new PIMException(ErrorString.INVALID_ITEM,
                                   PIMException.GENERAL_ERROR);
        }

        if (aMatchingItem.getPIMList() != this)
        {
            throw new IllegalArgumentException(
                ErrorString.ITEM_DOES_NOT_ORIGINATE_FROM_THIS_LIST);
        }

        PIMItemImpl matchingItemImpl = (PIMItemImpl) aMatchingItem;
        Enumeration items = null;

        // The matching item is synchronized because, in some cases, the garbage
        // collector may collect the item before the native operation gets
        // executed in the JNI context. This synchronization prevents the GC to
        // collect
        // the object before the operation gets fully executed. Note that
        // without
        // the synchronization, aMatchingItem would be available for the GC
        // after
        // matchingItemImpl.jniNativeHandle() function call and because the
        // handle
        // is still used, it may cause that null is referenced in the native
        // side
        synchronized (matchingItemImpl)
        {
            int matchingItemHandle = matchingItemImpl.jniNativeHandle();
            items = doItems(ENUMERATION_ITEMS_MATCHING_ITEM,
                            matchingItemHandle, null);
        }

        return items;
    }

    public synchronized Enumeration items(String aMatchingValue)
    throws PIMException
    {
        // Ensure security permission
        ensurePimPermission(PIM.READ_ONLY);

        if (aMatchingValue == null)
        {
            throw new NullPointerException(ErrorString.SEARCHING_FAILED_COLON +
                                           ErrorString.STRING_VALUE_IS_NULL);
        }

        return doItems(ENUMERATION_ITEMS_MATCHING_STRING, 0, aMatchingValue);
    }

    public synchronized Enumeration itemsByCategory(String aCategory)
    throws PIMException
    {
        // aCategory may be null (PIMList.UNCATEGORIZED, which is null)
        // Ensure security permission
        ensurePimPermission(PIM.READ_ONLY);

        return doItems(ENUMERATION_ITEMS_MATCHING_CATEGORY, 0, aCategory);
    }

    public synchronized String[] getCategories() throws PIMException
    {
        int[] error = new int[1];

        String[] categories = _getCategories(iListHandle, error);

        NativeError.handleCategoryHandlingError(error[0]);
        return categories;
    }

    public synchronized boolean isCategory(String aCategory)
    throws PIMException
    {
        if (aCategory == null)
        {
            throw new NullPointerException(ErrorString.CATEGORY_MISSING);
        }

        int[] error = new int[1];

        boolean isCategory = _isCategory(iListHandle, aCategory, error);

        NativeError.handleCategoryHandlingError(error[0]);
        return isCategory;
    }

    public synchronized void addCategory(String aCategory) throws PIMException
    {
        // Ensure security permission
        ensurePimPermission(PIM.WRITE_ONLY);

        if (aCategory == null)
        {
            throw new NullPointerException(ErrorString.CATEGORY_IS_NULL);
        }

        int[] error = new int[1];

        _addCategory(iListHandle, aCategory, error);

        NativeError.handleCategoryHandlingError(error[0], aCategory);
    }

    public synchronized void deleteCategory(String aCategory,
                                            boolean aDeleteUnassignedItems) throws PIMException
    {
        checkModeMinimum(PIM.WRITE_ONLY);
        if (aCategory == null)
        {
            throw new NullPointerException(ErrorString.CATEGORY_IS_NULL);
        }
        ApplicationUtils appUtils = ApplicationUtils.getInstance();

        // Get localized text info for the security dialog
        String action = null;
        switch (getListType())
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

        PIMPermissionImpl per = new PIMPermissionImpl(action, aCategory, getName(), PIMPermissionImpl.OPERATION_CATEGORY_DELETION);

        // Ensure permission from PIM API security
        appUtils.checkPermission(per);

        int[] error = new int[1];

        // Returns items that were assigned to the deleted category
        // but after that do not belong to any category
        int[] unassignedItemHandles = _deleteCategory(iListHandle, aCategory, error);

        NativeError.handleCategoryHandlingError(error[0], aCategory);

        // If deleting unassigned items was requested, delete them
        // one by one (if any present)
        if (aDeleteUnassignedItems && (unassignedItemHandles != null))
        {
            for (int i = 0; i < unassignedItemHandles.length; i++)
            {
                int itemHandle = unassignedItemHandles[i];
                PIMItemImpl item = iItems.getItem(itemHandle);
                if (item != null)
                {
                    // pops up the deletion confirmation dialogue, too
                    removeItem(item);
                }
            }
        }
    }

    public synchronized void renameCategory(String aCurrentCategory,
                                            String aNewCategory) throws PIMException
    {
        // Ensure security permission
        ensurePimPermission(PIM.WRITE_ONLY);

        if (aCurrentCategory == null || aNewCategory == null)
        {
            throw new NullPointerException(ErrorString.CATEGORY_IS_NULL);
        }

        int error = _renameCategory(iListHandle, aCurrentCategory, aNewCategory);

        if (NativeError.checkArgumentError(error))
        {
            // We don't know which argument is invalid
            NativeError.handleCategoryHandlingError(error);
        }
        else
        {
            NativeError.handleCategoryHandlingError(error, aCurrentCategory);
        }
    }

    public synchronized int maxCategories()
    {
        return _maxCategories(iListHandle);
    }

    public synchronized boolean isSupportedField(int aField)
    {
        return _isSupportedField(iListHandle, aField);
    }

    public synchronized int[] getSupportedFields()
    {
        Logger.LOG(Logger.EPim,Logger.EInfo,"+getSupportedFields() = iListHandle = "+iListHandle);
        int[] supportedFields = _getSupportedFields(iListHandle);

        if (supportedFields == null)
        {
            throw new OutOfMemoryError();
        }

        return supportedFields;
    }

    public synchronized boolean isSupportedAttribute(int aField, int aAttribute)
    {
        return _isSupportedAttribute(iListHandle, aField, aAttribute);
    }

    public synchronized int[] getSupportedAttributes(int aField)
    {
        int[] error = new int[1];

        int[] supportedAttributes = _getSupportedAttributes(iListHandle, aField,
                                    error);

        NativeError.handleFieldHandlingError(error[0], aField);
        return supportedAttributes;
    }

    public synchronized boolean isSupportedArrayElement(int aStringArrayField,
            int aArrayElement)
    {
        return _isSupportedArrayElement(iListHandle, aStringArrayField, aArrayElement);
    }

    public synchronized int[] getSupportedArrayElements(int aStringArrayField)
    {
        int[] error = new int[1];

        int[] supportedArrayElements = _getSupportedArrayElements(iListHandle,
                                       aStringArrayField, error);

        NativeError.handleFieldHandlingError(error[0], aStringArrayField);
        return supportedArrayElements;
    }

    public synchronized int getFieldDataType(int aField)
    {
        int[] error = new int[1];

        int fieldDataType = _getFieldDataType(iListHandle, aField, error);

        NativeError.handleFieldHandlingError(error[0], aField);
        return fieldDataType;
    }

    public synchronized String getFieldLabel(int aField)
    {
        int[] labelSpec = new int[1];
        labelSpec[0] = aField;

        int[] error = new int[1];

        String label = _getLabel(iListHandle, LABEL_TYPE_FIELD, labelSpec, error);

        NativeError.handleFieldHandlingError(error[0], aField);
        return label;
    }

    public synchronized String getAttributeLabel(int aAttribute)
    {
        int[] labelSpec = new int[1];
        labelSpec[0] = aAttribute;

        int[] error = new int[1];

        String label = _getLabel(iListHandle, LABEL_TYPE_ATTRIBUTE, labelSpec, error);

        // Note that we use unusual exception descriptions here.
        NativeError.handlegetAttributeLabelError(error[0], aAttribute);

        return label;
    }

    public synchronized String getArrayElementLabel(int aStringArrayField,
            int aArrayElement)
    {
        int[] labelSpec = new int[2];
        labelSpec[0] = aStringArrayField;
        labelSpec[1] = aArrayElement;

        int[] error = new int[1];

        String label = _getLabel(

                           iListHandle, LABEL_TYPE_ARRAY_ELEMENT, labelSpec, error);

        // Note that we use unusual exception descriptions here.
        NativeError.handleArrayElementLabelError(error[0]);

        return label;
    }

    public synchronized int maxValues(int aField)
    {
        int[] error = new int[1];

        int maxValues = _maxValues(iListHandle, aField, error);

        NativeError.handleFieldHandlingError(error[0], aField);
        return maxValues;
    }

    public synchronized int stringArraySize(int aStringArrayField)
    {
        int[] error = new int[1];

        int stringArraySize = _stringArraySize(iListHandle, aStringArrayField,
                                               error);

        NativeError.handleFieldHandlingError(error[0], aStringArrayField);
        return stringArraySize;
    }

    // New methods

    /**
     * Updates list of items according to the changes in the native database.
     * Changes in items and categories are reflected appropriately.
     *
     * @par aMatchingItemHandle Handle to matching item if any
     *
     * @par NOTES: The handle to a matching item provides information that which
     *      fields must be loaded from the items if the list supports partial
     *      item loading. This is done because of the performance improvement of
     *      item handling with large databases
     *
     * @par About errors: Generally all errors are fatal during the update,
     *      because in case of out-of-memory situation the list is probably
     *      corrupt and it would be dangerous to continue execution.
     */
    protected void updateList(int aMatchingItemHandle) throws PIMException
    {
        int[] error = new int[1];

        int[] newAndRemovedItemHandles = _updateList(

                                             iListHandle, aMatchingItemHandle, error);
        NativeError.handleUpdateListError(error[0]);

        if (newAndRemovedItemHandles != null) // shouldn't be null
        {
            int removedItemsStart = 0;

            // Create and add new items and find the start of removed
            // item handles.
            for (int i = 0; i < newAndRemovedItemHandles.length; i++)
            {
                if (newAndRemovedItemHandles[i] != 0)
                {
                    int itemHandle = newAndRemovedItemHandles[i];
                    PIMItemImpl newItem = createAssociatedItem(itemHandle);
                    iItems.addItem(newItem);
                }
                else
                {
                    // boundary element; start of removed item handles
                    removedItemsStart = i + 1;
                    break;
                }
            }

            // Remove removed items
            // Loop from the removed items start to the end of the list.
            // May result to zero iterations.
            for (int i = removedItemsStart; i < newAndRemovedItemHandles.length; i++)
            {
                int itemHandle = newAndRemovedItemHandles[i];
                PIMItemImpl item = iItems.removeItem(itemHandle);

                if (item != null) // shouldn't be null
                {
                    item.removeListAssociation();
                }
            }
        }
    }

    /**
     * Common implementation of items() calls.
     *
     * This is a binary size optimization of the four different PIMList.items()
     * calls, reducing the number of necessary JNI functions from four to one.
     *
     * Optimization adds complexity to the operation signature. Enumeration type
     * and associated matching arguments must be given.
     *
     * @param aEnumerationType
     *            Enumeration type, one of: ENUMERATION_ITEMS_ALL,
     *            ENUMERATION_ITEMS_MATCHING_ITEM,
     *            ENUMERATION_ITEMS_MATCHING_STRING or
     *            ENUMERATION_ITEMS_MATCHING_CATEGORY.
     *
     * @param aMatchingItemHandle
     *            Handle to matching item. Used with
     *            ENUMERATION_ITEMS_MATCHING_ITEM; ignored otherwise.
     *
     * @param aStringArg
     *            String argument. Used with ENUMERATION_ITEMS_MATCHING_STRING
     *            and ENUMERATION_ITEMS_MATCHING_CATEGORY; ignored otherwise.
     *            May be null for category matching.
     *
     * Arguments must be pre-checked.
     */
    protected Enumeration doItems(int aEnumerationType,
                                  int aMatchingItemHandle, String aStringArg) throws PIMException
    {
        updateList(aMatchingItemHandle);
        int[] error = new int[1];

        int[] itemHandles = _items(iListHandle, aEnumerationType,
                                   aMatchingItemHandle, // used for item matching
                                   aStringArg, // used for value or category matching
                                   error);

        return processItemsResults(itemHandles, error[0]);
    }

    /**
     * Creates an enumeration of items from an array of item handles and
     * processes any errors resulting from obtaining item handles from native
     * side.
     *
     * Note: Used by EventListImpl and ToDoListImpl, too.
     */
    protected Enumeration processItemsResults(int[] aItemHandles, int aError)
    throws PIMException
    {
        if (aItemHandles == null)
        {
            throw new OutOfMemoryError();
        }
        NativeError.handleProcessItemResultsError(aError);

        Vector items = new Vector(aItemHandles.length);

        for (int i = 0; i < aItemHandles.length; i++)
        {
            int itemHandle = aItemHandles[i];
            PIMItemImpl item = iItems.getItem(itemHandle);

            if (item != null) // shouldn't be null
            {
                items.addElement(item);
            }
        }

        return items.elements();
    }

    /**
     * ensurePimPermission
     * Ensures indicated PIM API security permission for this list
     * This function is common for all list which need to verify PIM API
     * security permissions. It also check that the list has at least
     * the requested mode
     *
     * @param aMode The mode which is requested to be permitted
     */
    protected void ensurePimPermission(int aMode)
    {
        // Ensure that the list has at least the requested mode
        checkModeMinimum(aMode);

        PIMManager manager = (PIMManager)PIMManager.getInstance();

        //ensure permission
        manager.getPermission(getListType(), aMode);

    }


    /**
     * Creates new item of specialized type.
     */
    protected abstract PIMItemImpl createAssociatedItem(int aItemHandle);

    /**
     * Determines whether the list has <em>at least</em> given access mode.
     *
     * @return \c true if all modes in \a aMode are present; \c false otherwise.
     */
    boolean isModeMinimum(int aMode)
    {
        return ((iMode & aMode) == aMode);
    }

    /**
     * Checks whether the list has <em>at least</em> given access mode and
     * throws SecurityException if not.
     */
    void checkModeMinimum(int aMode)
    {
        if (!isModeMinimum(aMode))
        {
            throw new SecurityException();
        }
    }

    // Native operations

    private native void _dispose(int aPIMManagerHandle);

    /**
     * Creates a new native side specialized item that is associated with the
     * native side list but is not added to the list.
     *
     * @return Handle to the native side item or < 0 in error.
     */
    private native int _createItem(int aListHandle, int[] aError);

    /**
     * Removes given item from native database
     *
     * @return < 0 for error; KERR_NONE otherwise.
     */
    private native int _removeItem(int aListHandle, int aItemHandle);

    /**
     * Adds a new, first time committed item to the list.
     */
    private native int _addCommittedItem(int aListHandle, int aItemHandle);

    /**
     * @return List name, or null in error (taken as "out of memory").
     */
    private native String _getName(int aListHandle);

    /**
     * Closes the list.
     */
    private native int _close(int aListHandle);

    /**
     * Common JNI function for the four different items() operations. See
     * doItems().
     */
    private native int[] _items(int aListHandle, int aEnumerationType,
                                int aMatchingItemHandle, String aStringArg, int[] aError);

    private native String[] _getCategories(int aListHandle, int[] aError);

    private native boolean _isCategory(int aListHandle, String aCategory,
                                       int[] aError);

    private native void _addCategory(int aListHandle, String aCategory,
                                     int[] aError);

    /**
     * @return The native handles of items that were in the removed category and
     *         do not belong to any category any more; null if the category was
     *         not removed (i.e. it did not exist).
     */
    private native int[] _deleteCategory(int aListHandle, String aCategory,
                                         int[] aError);

    /**
     * @return Error code.
     */
    private native int _renameCategory(int aListHandle, String aCurrentCategory,
                                       String aNewCategory);

    private native int _maxCategories(int aListHandle);

    private native boolean _isSupportedField(int aListHandle, int aField);

    /**
     * @return null on error.
     */
    private native int[] _getSupportedFields(int aListHandle);

    private native boolean _isSupportedAttribute(int aListHandle, int aField,
            int aAttribute);

    private native int[] _getSupportedAttributes(int aListHandle, int aField, int[] aError);

    private native boolean _isSupportedArrayElement(int aListHandle, int aStringArrayField,
            int aArrayElement);

    private native int[] _getSupportedArrayElements(int aListHandle, int aStringArrayField,
            int[] aError);

    private native int _getFieldDataType(int aListHandle, int aField, int[] aError);

    /**
     * Common implementation for all label getting operations.
     *
     * Contents of \a aLabelSpec argument:
     *
     * @li If \a aLabelType is LABEL_TYPE_FIELD, field constant (1 element).
     *
     * @li If \a aLabelType is LABEL_TYPE_ATTRIBUTE, attribute constant (1
     *     element)
     *
     * @li If \a aLabelType is LABEL_TYPE_ARRAY_ELEMENT, string array field
     *     constant and an array element field constant (2 elements).
     *
     * @param aLabelType
     *            Label type (field, attribute or array element label).
     *
     * @param aLabelSpec
     *            Constants used as arguments for label getting functions.
     *
     * @return The label.
     */
    private native String _getLabel(int aListHandle, int aLabelType,
                                    int[] aLabelSpec, int[] aError);

    private native int _maxValues(int aListHandle, int aField, int[] aError);

    private native int _stringArraySize(int aListHandle, int aStringArrayField,
                                        int[] aError);

    /**
     * Updates the list according to native-originated external changes. Makes
     * the native side to resolve new, modified and removed items items in the
     * native database and to create new native side peer objects, updating
     * modified and removing removed items. Category changes are also reflected,
     * but this does not affect the Java side.
     *
     * @return An array containing handles to new items, zero and handles to
     *         removed items, contiguously, in that order.
     */
    private native int[] _updateList(int aListHandle, int aMatchingItemHandle,
                                     int[] aError);

}
