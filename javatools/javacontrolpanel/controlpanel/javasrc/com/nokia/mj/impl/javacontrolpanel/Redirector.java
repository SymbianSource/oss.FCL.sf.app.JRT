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
* Description:
*
*/

package com.nokia.mj.impl.javacontrolpanel;

class Redirector
{
    private int iHandle;

    public Redirector()
    {
    }

    public void start()
    {
        stop();
        iHandle = _start();
    }

    public void stop()
    {
        if (isRedirecting())
        {
            _stop(iHandle);
            iHandle = 0;
        }
    }

    public boolean isRedirecting()
    {
        if (iHandle != 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    private native int _start();
    private native void _stop(int aHandle);

}
