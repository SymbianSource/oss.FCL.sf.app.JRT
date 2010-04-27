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
* Description: RecordStoreFullException
*
*/


package javax.microedition.rms;

/**
 * Thrown if record store is full.
 */
public class RecordStoreFullException extends RecordStoreException
{
    public RecordStoreFullException(String aMessage)
    {
        super(aMessage);
    }

    public RecordStoreFullException()
    {
        super();
    }
}
