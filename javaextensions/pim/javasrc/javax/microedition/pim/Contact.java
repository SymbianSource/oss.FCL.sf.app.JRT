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
* Description:  Contact item interface.
 *
*/


// PACKAGE
package javax.microedition.pim;

// CLASS DEFINITION
/**
 * PIM API 1.0 compliant Contact interface.
 */
public interface Contact extends PIMItem
{
    public static final int ADDR = 100;

    public static final int BIRTHDAY = 101;

    public static final int CLASS = 102;

    public static final int EMAIL = 103;

    public static final int FORMATTED_ADDR = 104;

    public static final int FORMATTED_NAME = 105;

    public static final int NAME = 106;

    public static final int NICKNAME = 107;

    public static final int NOTE = 108;

    public static final int ORG = 109;

    public static final int PHOTO = 110;

    public static final int PHOTO_URL = 111;

    public static final int PUBLIC_KEY = 112;

    public static final int PUBLIC_KEY_STRING = 113;

    public static final int REVISION = 114;

    public static final int TEL = 115;

    public static final int TITLE = 116;

    public static final int UID = 117;

    public static final int URL = 118;

    public static final int ATTR_ASST = 1;

    public static final int ATTR_AUTO = 2;

    public static final int ATTR_FAX = 4;

    public static final int ATTR_HOME = 8;

    public static final int ATTR_MOBILE = 16;

    public static final int ATTR_OTHER = 32;

    public static final int ATTR_PAGER = 64;

    public static final int ATTR_PREFERRED = 128;

    public static final int ATTR_SMS = 256;

    public static final int ATTR_WORK = 512;

    public static final int ADDR_POBOX = 0;

    public static final int ADDR_EXTRA = 1;

    public static final int ADDR_STREET = 2;

    public static final int ADDR_LOCALITY = 3;

    public static final int ADDR_REGION = 4;

    public static final int ADDR_POSTALCODE = 5;

    public static final int ADDR_COUNTRY = 6;

    public static final int NAME_FAMILY = 0;

    public static final int NAME_GIVEN = 1;

    public static final int NAME_OTHER = 2;

    public static final int NAME_PREFIX = 3;

    public static final int NAME_SUFFIX = 4;

    public static final int CLASS_CONFIDENTIAL = 200;

    public static final int CLASS_PRIVATE = 201;

    public static final int CLASS_PUBLIC = 202;

    public int getPreferredIndex(int aField);
}
