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
* Description:  Unsupported field exception.
 *
*/


// PACKAGE
package javax.microedition.pim;

// CLASS DEFINITION
/**
 * PIM API 1.0 compliant UnsupportedFieldException.
 */
public class UnsupportedFieldException extends java.lang.RuntimeException
{
    private int iOffendingField;

    public UnsupportedFieldException()
    {
        super();
        iOffendingField = -1;
    }

    public UnsupportedFieldException(String aDetailMessage)
    {
        super(aDetailMessage);
        iOffendingField = -1;
    }

    public UnsupportedFieldException(String aDetailMessage, int aField)
    {
        super(aDetailMessage);
        iOffendingField = aField;
    }

    public int getField()
    {
        return iOffendingField;
    }
}