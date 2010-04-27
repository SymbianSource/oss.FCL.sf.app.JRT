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
* Description: RecordStoreException
*
*/


package javax.microedition.rms;
import java.lang.Exception;


/**
 * Thrown to indicate record store related error.
 */
public class RecordStoreException extends Exception
{
    public RecordStoreException(String aMessage)
    {
        super(aMessage);
    }

    public RecordStoreException()
    {
        super();
    }
}
