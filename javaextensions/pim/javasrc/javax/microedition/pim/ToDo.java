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
* Description:  ToDo item interface.
 *
*/


// PACKAGE
package javax.microedition.pim;

// CLASS DEFINITION
/**
 * PIM API 1.0 compliant ToDo interface.
 */
public interface ToDo extends PIMItem
{
    public static final int CLASS = 100;

    public static final int COMPLETED = 101;

    public static final int COMPLETION_DATE = 102;

    public static final int DUE = 103;

    public static final int NOTE = 104;

    public static final int PRIORITY = 105;

    public static final int REVISION = 106;

    public static final int SUMMARY = 107;

    public static final int UID = 108;

    public static final int CLASS_CONFIDENTIAL = 200;

    public static final int CLASS_PRIVATE = 201;

    public static final int CLASS_PUBLIC = 202;
}