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
import com.nokia.mj.impl.rt.support.Finalizer;

public class Ticker
{
    final Toolkit iToolkit;
    private int iHandle;
    private String iText;
    private Finalizer mFinalizer;

    public Ticker(String aText)
    {
        if (null == aText)
        {
            throw new NullPointerException();
        }
        mFinalizer = new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };
        Toolkit toolkit = Toolkit.getToolkit();
        synchronized (toolkit)
        {
            iToolkit = toolkit;
            iHandle  = Toolkit.checkHandle(_create(toolkit.getHandle(), aText));
            iText    = aText;
        }
    }

    public void setString(String aText)
    {
        if (null == aText)
        {
            throw new NullPointerException();
        }
        synchronized (iToolkit)
        {
            if (iHandle <= 0) throw new RuntimeException("bad handle");
            NativeError.check(_setText(iHandle,iToolkit.getHandle(),aText));
            iText = aText;
        }
    }

    public String getString()
    {
        return iText;
    }

    int getHandle()
    {
        if (iHandle <= 0)
        {
            throw new RuntimeException("Ticker: bad handle " + iHandle);
        }
        return iHandle;
    }

    private void doFinalize()
    {
        if (mFinalizer != null)
        {
            registeredFinalize();
            mFinalizer = null;
        }
    }

    void registeredFinalize()
    {
        synchronized (iToolkit)
        {
            iToolkit.disposeObject(iHandle);
            iHandle=0;
        }
    }

    private native int _create(int aToolkit,String aText);
    private native int _setText(int aHandle,int aToolkit,String aText);
} // class Ticker
