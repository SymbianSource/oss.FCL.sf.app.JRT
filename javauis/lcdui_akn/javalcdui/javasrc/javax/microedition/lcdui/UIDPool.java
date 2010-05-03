/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


package javax.microedition.lcdui;


final class UIDPool
{
    UIDPool()
    {
        iNextUID = UID_LOW;
    }

    int allocUID()
    {
        if (iNextUID > UID_HI)
        {
            throw new RuntimeException("UIDPool: Unable to allocate UID");
        }
        return iNextUID++;
    }

    //

    private int iNextUID;

    //

    private final static int UID_LOW = 0x10283775;

    private final static int UID_HI  = 0x10285774;
}
