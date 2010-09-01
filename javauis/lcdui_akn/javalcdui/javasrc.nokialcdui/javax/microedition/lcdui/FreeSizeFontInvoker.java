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


package javax.microedition.lcdui;

// FreeSizeFontInvoker class allows to access package private members
// of javax.microedition.lcdui package from com.nokia.mid.ui package.
// This class is shared by these two packages.
// Class from javax.microedition.lcdui package is extended by class from
// com.nokia.mid.ui package.
// Invoker is instantiated by javax.microedition.lcdui.Toolkit.
final class FreeSizeFontInvoker
        extends com.nokia.mid.ui.FreeSizeFontInvoker
{

    // this method is called from javax.microedition.lcdui.Toolkit
    static void createInvoker()
    {
        FreeSizeFontInvoker.setInvoker(
            new javax.microedition.lcdui.FreeSizeFontInvoker());
    }

    // returns new font instance via calling package private method
    // getFreeSizeFont defined in javax.microedition.lcdui.Font class
    protected Font getFreeSizeFont(int aFace, int aStyle, int aHeight)
    {
        return javax.microedition.lcdui.Font.getFreeSizeFont(
                   aFace, aStyle, aHeight);
    }
}
