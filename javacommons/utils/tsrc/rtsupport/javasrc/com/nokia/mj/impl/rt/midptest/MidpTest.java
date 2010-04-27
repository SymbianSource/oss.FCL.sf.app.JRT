/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.rt.midptest;

public final class MidpTest
{
    public static void main(String[] args)
    {
        try
        {
            Class.forName("com.nokia.mj.impl.rt.test.TestMain").newInstance();
        }
        catch (Throwable th)
        {
        }
        ApplicationUtilsImpl.doShutdownImpl();
    }
}

