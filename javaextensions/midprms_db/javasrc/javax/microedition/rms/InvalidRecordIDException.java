/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: InvalidRecordIDException
*
*/


package javax.microedition.rms;

/**
 * InvalidRecordIDException is thrown if record ID used in an operation
 * is invalid.
 */

public class InvalidRecordIDException extends RecordStoreException
{
    public InvalidRecordIDException(String aMessage)
    {
        super(aMessage);
    }

    public InvalidRecordIDException()
    {
        super();
    }
}
