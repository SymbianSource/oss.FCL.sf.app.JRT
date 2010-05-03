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
* Description:  Event item interface.
 *
*/


// PACKAGE
package javax.microedition.pim;

// CLASS DEFINITION
/**
 * PIM API 1.0 compliant PIMItem interface.
 */
public interface Event extends PIMItem
{
    public static final int ALARM = 100;

    public static final int CLASS = 101;

    public static final int END = 102;

    public static final int LOCATION = 103;

    public static final int NOTE = 104;

    public static final int REVISION = 105;

    public static final int START = 106;

    public static final int SUMMARY = 107;

    public static final int UID = 108;

    public static final int CLASS_CONFIDENTIAL = 200;

    public static final int CLASS_PRIVATE = 201;

    public static final int CLASS_PUBLIC = 202;

    public RepeatRule getRepeat();

    public void setRepeat(RepeatRule aValue);
}
