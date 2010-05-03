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

import com.nokia.mj.impl.rt.legacy.NativeError;

public class TextField extends Item
{
    public static final int ANY = 0;
    public static final int EMAILADDR = 1;
    public static final int NUMERIC = 2;
    public static final int PHONENUMBER = 3;
    public static final int URL = 4;
    public static final int DECIMAL = 5;
    //
    public static final int PASSWORD = 0x10000;
    public static final int UNEDITABLE = 0x20000;
    public static final int SENSITIVE = 0x40000;
    public static final int NON_PREDICTIVE = 0x80000;
    public static final int INITIAL_CAPS_WORD = 0x100000;
    public static final int INITIAL_CAPS_SENTENCE = 0x200000;
    //
    public static final int CONSTRAINT_MASK = 0x0FFFF;
    //
    private int iMaxSize;
    private int iConstraints;

    public TextField(String aLabel,String aText,int aMaxSize,int aConstraints)
    {
        TextComponent.checkText(aText,aMaxSize);
        TextComponent.checkConstraints(aConstraints);

        synchronized (iToolkit)
        {
            final int toolkit = iToolkit.getHandle();
            final int handle = _create(toolkit, this, aLabel, aText, aConstraints, aMaxSize);
            iHandle  = Toolkit.checkHandle(handle);
            iMaxSize = NativeError.check(_getMaxSize(iHandle, toolkit));
        }

        iConstraints = aConstraints;
        iLabel       = aLabel;
        // assigned maximum capacity may be smaller than requested so must
        // go native to get real max size.
    }

    public int getCaretPosition()
    {
        synchronized (iToolkit)
        {
            return _getCaretPosition(getHandle(), iToolkit.getHandle());
        }
    }

    public String getString()
    {
        synchronized (iToolkit)
        {
            return GetString();
        }
    }

    final String GetString()
    {
        final String s = _getString(getHandle(),iToolkit.getHandle());
        return (s == null ? "" : s);
    }

    public void setString(String aText)
    {
        synchronized (iToolkit)
        {
            TextComponent.checkText(aText,iMaxSize);
            NativeError.check(_setString(getHandle(),iToolkit.getHandle(),aText));
            RefreshForm();
        }
    }

    public int getChars(char[] aData)
    {
        final String string;
        final int    length;
        synchronized (iToolkit)
        {
            string = GetString();
            length = Size();
        }
        return TextComponent.getCharsFromString(string,length,aData);
    }

    public void setChars(char[] aData,int aOffset,int aLength)
    {
        setString(TextComponent.getStringFromChars(aData,aOffset,aLength));
    }

    public void insert(String aSrc,int aPosition)
    {
        synchronized (iToolkit)
        {
            if (aSrc.length()+size() > iMaxSize)
            {
                throw new IllegalArgumentException();
            }
            NativeError.check(_insertString(getHandle(),iToolkit.getHandle(),aSrc,aPosition));
        }
    }

    public void insert(char[] aData,int aOffset,int aLength,int aPosition)
    {
        try
        {
            insert(new String(aData,aOffset,aLength),aPosition);
        }
        catch (StringIndexOutOfBoundsException aException)
        {
            throw new ArrayIndexOutOfBoundsException();
        }
    }

    public void delete(int aOffset,int aLength)
    {
        synchronized (iToolkit)
        {
            final int size = size();
            TextComponent.checkOffset(aOffset,aLength,size);
            NativeError.check(_delete(getHandle(),iToolkit.getHandle(),aOffset,aLength));
        }
    }

    public int getMaxSize()
    {
        return iMaxSize;
    }

    public int setMaxSize(int aMaxSize)
    {
        if (aMaxSize <= 0)
        {
            throw new IllegalArgumentException();
        }
        synchronized (iToolkit)
        {
            iMaxSize = NativeError.check(_setMaxSize(getHandle(),iToolkit.getHandle(),aMaxSize));
            return iMaxSize;
        }
    }

    public int size()
    {
        synchronized (iToolkit)
        {
            return Size();
        }
    }

    final int Size()
    {
        return _size(getHandle(),iToolkit.getHandle());
    }

    public void setConstraints(int aConstraints)
    {
        TextComponent.checkConstraints(aConstraints);
        synchronized (iToolkit)
        {
            NativeError.check(_setConstraints(getHandle(),iToolkit.getHandle(),aConstraints));
            iConstraints = aConstraints;
        }
    }

    public int getConstraints()
    {
        return iConstraints;
    }

    public void setInitialInputMode(String aCharacterSubset)
    {
        final String subset = aCharacterSubset==null?"":aCharacterSubset;
        synchronized (iToolkit)
        {
            NativeError.check(_setInitialInputMode(getHandle(), iToolkit.getHandle(), subset));
        }
    }

    private native int _create(int aToolkit,TextField aTextField,String aLabel,String aText,int aConstraints,int aMaxSize);
    private native int _getCaretPosition(int aTextField,int aToolkit);
    private native String _getString(int aTextField,int aToolkit);
    private native int _setString(int aTextField,int aToolkit,String aText);
    private native int _insertString(int aTextField,int aToolkit,String aText,int aPosition);
    private native int _delete(int aTextField,int aToolkit,int aOffset,int aLength);
    private native int _size(int aTextField,int aToolkit);
    private native int _setMaxSize(int aTextField,int aToolkit,int aMaxSize);
    private native int _getMaxSize(int aTextField,int aToolkit);
    private native int _setConstraints(int aTextField,int aToolkit,int aConstraints);
    private native int _setInitialInputMode(int aTextField,int aToolkit,String aCharacterSubset);
}
