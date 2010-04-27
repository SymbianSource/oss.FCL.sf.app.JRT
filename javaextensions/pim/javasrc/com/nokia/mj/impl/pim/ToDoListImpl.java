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
* Description:  ToDo list implementation.
 *
*/


// PACKAGE
package com.nokia.mj.impl.pim;

// IMPORTS

import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMException;
import javax.microedition.pim.PIMItem;
import javax.microedition.pim.ToDo;
import javax.microedition.pim.ToDoList;
import java.util.Enumeration;
import com.nokia.mj.impl.pim.ErrorString;

// CLASS DEFINITION
/**
 * ToDo list implementation.
 */
public final class ToDoListImpl extends PIMListImpl implements ToDoList
{
    // Constructors

    ToDoListImpl(int aListHandle, int aMode)
    {
        super(aListHandle, aMode);
    }

    // Methods from PIMListImpl

    int getListType()
    {
        return PIM.TODO_LIST;
    }

    protected PIMItemImpl createAssociatedItem(int aItemHandle)
    {
        return new ToDoImpl(aItemHandle, this);
    }

    // Methods from ToDoList

    public synchronized ToDo createToDo()
    {
        return (ToDoImpl) createItem();
    }

    public synchronized ToDo importToDo(ToDo aToDo)
    {
        if (aToDo == null)
        {
            throw new NullPointerException(ErrorString.IMPORTING_FAILED_COLON +
                                           ErrorString.IMPORT_ITEM_IS_NULL);
        }
        ToDoImpl todoItem = (ToDoImpl) createItem();
        todoItem.copyFrom(aToDo);

        // Set REVISION field
        updateRevisionField(todoItem, ToDo.REVISION, PIMItem.ATTR_NONE);

        return todoItem;
    }

    public synchronized void removeToDo(ToDo aToDo) throws PIMException
    {
        checkModeMinimum(PIM.WRITE_ONLY);
        removeItem(aToDo);
    }

    public synchronized Enumeration items(int aField, long aStartDate,
                                          long aEndDate) throws PIMException
    {
        ensurePimPermission(PIM.READ_ONLY);

        if (getFieldDataType(aField) != PIMItem.DATE)
        {
            throw new IllegalArgumentException(ErrorString.INVALID_FIELD_COLON
                                               + aField);
        }

        if (aEndDate < aStartDate)
        {
            throw new IllegalArgumentException(ErrorString.SEARCHING_FAILED_COLON +
                                               ErrorString.START_DATE_IS_AFTER_END_DATE);
        }

        updateList(0);

        int[] error = new int[1];

        int[] itemHandles = _itemsByDate(iListHandle, aField, aStartDate,
                                         aEndDate, error);

        return processItemsResults(itemHandles, error[0]);
    }

    // Native operations

    private native int[] _itemsByDate(int aToDoListHandle, int aField,
                                      long aStartDate, long aEndDate, int[] aError);

}