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
* Description: SecureRecordStoreException
*
*/


package javax.microedition.rms;

/**
 * Thrown if there is problem in encrypting or decrypting data.
 */
/*public in MIPD3*/
class SecureRecordStoreException extends RecordStoreException
{
    public SecureRecordStoreException(String aMessage)
    {
        super(aMessage);
    }

    public SecureRecordStoreException()
    {
        super();
    }
}
