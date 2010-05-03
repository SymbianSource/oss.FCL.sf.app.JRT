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
* Description:  ToDo list interface.
 *
*/


// PACKAGE
package javax.microedition.pim;

// IMPORTS
import java.util.Enumeration;

// CLASS DEFINITION
/**
 * PIM API 1.0 compliant PIMList interface.
 */
public interface ToDoList extends PIMList
{
    public ToDo createToDo();

    public ToDo importToDo(ToDo aItem);

    public void removeToDo(ToDo aItem) throws PIMException;

    public Enumeration items(int aField, long aStartDate, long aEndDate)
    throws PIMException;
}