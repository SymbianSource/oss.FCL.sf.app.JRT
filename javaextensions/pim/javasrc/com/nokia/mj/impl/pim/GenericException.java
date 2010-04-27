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
* Description:
*
*/


// PACKAGE
package com.nokia.mj.impl.pim;

// CLASS DEFINITION

public final class GenericException extends java.lang.RuntimeException
{
    public GenericException()
    {
        super();
    }

    public GenericException(String aDetailMessage)
    {
        super(aDetailMessage);
    }
}
