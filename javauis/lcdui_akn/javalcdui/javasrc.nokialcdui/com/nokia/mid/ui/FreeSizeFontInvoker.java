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
* Description:  Invoker for access to lcdui package.
*
*/



package com.nokia.mid.ui;

import javax.microedition.lcdui.Font;

// FreeSizeFontInvoker class allows to access package private members
// of javax.microedition.lcdui package from com.nokia.mid.ui package.
// This class is shared by these two packages.
// Class from javax.microedition.lcdui package is extended by class from
// com.nokia.mid.ui package.
// Font class is accessed via getFreeSizeFont method.
public abstract class FreeSizeFontInvoker
{
    private static com.nokia.mid.ui.FreeSizeFontInvoker sInvoker;

    //stores instantiated invoker
    //called from javax.microedition.lcdui.FreeSizeFontInvoker createInvoker
    public static void setInvoker(com.nokia.mid.ui.FreeSizeFontInvoker invoker)
    {
        sInvoker = invoker;
    }

    public static javax.microedition.lcdui.Font getFont(
        int aFace, int aStyle, int aHeight)
    {
        //calls getFreeSizeFont from javax.microedition.lcdui.FreeSizeFontInvoker
        return sInvoker.getFreeSizeFont(aFace, aStyle, aHeight);
    }

    // method is defined in javax.microedition.lcdui.FreeSizeFontInvoker
    protected abstract javax.microedition.lcdui.Font getFreeSizeFont(
        int aFace, int aStyle, int aHeight);

}
