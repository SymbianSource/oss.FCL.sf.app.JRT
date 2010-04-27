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
* Description:  ToDo item implementation.
 *
*/


// PACKAGE
package com.nokia.mj.impl.pim;

// IMPORTS
import javax.microedition.pim.PIM;
import javax.microedition.pim.ToDo;
import com.nokia.mj.impl.pim.GenericException;
import com.nokia.mj.impl.pim.utils.NativeError;

// CLASS DEFINITION
/**
 * ToDo item implementation.
 */
public final class ToDoImpl extends PIMItemImpl implements ToDo
{
    // Constructors

    /**
     * Creates new ToDoImpl and associates it with given native peer object.
     */
    ToDoImpl(int aItemHandle)
    {
        super(aItemHandle);
        iItemType = PIM.TODO_LIST;
    }

    /**
     * Creates new ToDoImpl and associates it with given native peer object and
     * given ToDoList.
     */
    ToDoImpl(int aItemHandle, ToDoListImpl aToDoList)
    {
        super(aItemHandle, aToDoList);
        iItemType = PIM.TODO_LIST;
    }

    /**
     * Creates new ToDoImpl and its native side peer object. The new TodoImpl
     * object is not associated with a list.
     */
    ToDoImpl()
    {
        super();
        iItemType = PIM.TODO_LIST;
        int[] error = new int[1];
        iItemHandle = _createNativePeer(

                          ((PIMManager) PIMManager.getInstance()).managerHandle(), error);
        if (!NativeError.checkSuccess(error[0]))
        {
            throw new GenericException(ErrorString.GENERAL_ERROR_COLON + error[0]);
        }
    }

    /**
     * Copies the contents of given ToDo-implementing object. New data is
     * appended to the existing data.
     */
    void copyFrom(ToDo aToDo)
    {
        baseCopyFrom(aToDo);
    }


    //  Methods from PIMItemImpl

    /**
     * Provides the essential info about this ToDo so that the user
     * can distinguish it among other ToDos.
     *
     * One of these values is returned (in preferred order):
     * @li Summary (Subject)
     * @li \c null
     */
    String getShortInfo()
    {
        String info = null;

        if (isFieldSupportedAndPresent(ToDo.SUMMARY))
        {
            info = getString(ToDo.SUMMARY, 0);
        }

        // else leave info null
        return info;
    }

    // Native operations
    /**
     * @return Handle to the native side peer object or < 0 in error.
     */
    private native int _createNativePeer(int aManagerhandle, int[] aError);

}
