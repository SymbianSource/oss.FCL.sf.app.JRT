/*
* Copyright (c) 1999 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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

import com.nokia.mj.impl.rt.legacy.NativeError;

public class Spacer extends Item
{
    public Spacer(int aWidth,int aHeight)
    {
        checkSize(aWidth,aHeight);
        synchronized (iToolkit)
        {
            iHandle = Toolkit.checkHandle(_create(iToolkit.getHandle(),aWidth,aHeight));
        }
    }

    public void setLabel(String aLabel)
    {
        invalidOperation();
    }

    public void addCommand(Command aCommand)
    {
        invalidOperation();
    }

    public void setDefaultCommand(Command aCommand)
    {
        invalidOperation();
    }

    public void setMinimumSize(int aWidth,int aHeight)
    {
        checkSize(aWidth,aHeight);
        synchronized (iToolkit)
        {
            NativeError.check(_setMinimumSize(getHandle(),iToolkit.getHandle(),aWidth,aHeight));
            UpdateForm();
        }
    }

    private void invalidOperation()
    {
        throw new IllegalStateException();
    }

    private void checkSize(int aWidth,int aHeight)
    {
        if (aWidth < 0 || aHeight < 0)
            throw new IllegalArgumentException();
    }

    private native int _create(int aToolkit,int aWidth,int aHeight);
    private native int _setMinimumSize(int aSpacer,int aToolkit,int aWidth,int aHeight);
}
