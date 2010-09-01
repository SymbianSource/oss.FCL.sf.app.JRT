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

import com.nokia.mj.impl.rt.support.MemoryManagerCtr;
import com.nokia.mj.impl.rt.legacy.NativeError;

public class TextBox extends Screen
{
    private int iMaxSize;
    private int iConstraints;
    private static MemoryManagerCtr iMemoryManager = new MemoryManagerCtr(10, 25);
    private Object iMemoryToken = iMemoryManager.getMemoryToken();

    public TextBox(String aTitle,String aText,int aMaxSize,int aConstraints)
    {
        super();
        synchronized (iToolkit)
        {
            TextComponent.checkText(aText, aMaxSize);
            TextComponent.checkConstraints(aConstraints);
            iConstraints = aConstraints;
            int[] createTextBoxReturn = new int[4];
            NativeError.check(_create(iToolkit.getHandle(), getContainerHandle(), aConstraints, aMaxSize, aText, createTextBoxReturn));
            setContent(createTextBoxReturn[0], createTextBoxReturn[1], createTextBoxReturn[2]);
            iMaxSize = createTextBoxReturn[3];
        }
        setTitle(aTitle);
    }

    public int getCaretPosition()
    {
        synchronized (iToolkit)
        {
            return _getCaretPosition(getContentHandle(), iToolkit.getHandle());
        }
    }

    public String getString()
    {
        final String s;
        synchronized (iToolkit)
        {
            s = _getString(getContentHandle(),iToolkit.getHandle());
        }
        return (s == null ? "" : s);
    }

    public void setString(String aText)
    {
        TextComponent.checkText(aText, iMaxSize);
        synchronized (iToolkit)
        {
            NativeError.check(_setString(getContentHandle(),iToolkit.getHandle(),aText));
        }
    }

    public int getChars(char[] aData)
    {
        return TextComponent.getCharsFromString(getString(),size(),aData);
    }

    public void setChars(char[] aData,int aOffset,int aLength)
    {
        setString(TextComponent.getStringFromChars(aData, aOffset,aLength));
    }

    public void insert(String aSrc,int aPosition)
    {
        if (aSrc.length()+size() > iMaxSize)
            throw new IllegalArgumentException();
        synchronized (iToolkit)
        {
            NativeError.check(_insertString(getContentHandle(),iToolkit.getHandle(),aSrc,aPosition));
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
        final int size = size();
        TextComponent.checkOffset(aOffset, aLength, size);
        synchronized (iToolkit)
        {
            NativeError.check(_delete(getContentHandle(),iToolkit.getHandle(),aOffset,aLength));
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
            iMaxSize = NativeError.check(_setMaxSize(getContentHandle(),iToolkit.getHandle(),aMaxSize));
            aMaxSize = iMaxSize;
        }

        return aMaxSize;
    }

    public int size()
    {
        synchronized (iToolkit)
        {
            return _size(getContentHandle(),iToolkit.getHandle());
        }
    }

    public void setConstraints(int aConstraints)
    {
        TextComponent.checkConstraints(aConstraints);
        synchronized (iToolkit)
        {
            NativeError.check(_setConstraints(getContentHandle(),iToolkit.getHandle(),aConstraints));
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
            final int toolkit = iToolkit.getHandle();
            NativeError.check(_setInitialInputMode(getContentHandle(),toolkit, subset));
        }
    }

    private native int _create(int aToolkit,int aDisplayable,int aConstraints,int aMaxSize,String aText, int[] aCreateTextBoxReturn);
    private native int _getCaretPosition(int aTextBox,int aToolkit);
    private native String _getString(int aTextBox,int aToolkit);
    private native int _setString(int aTextBox,int aToolkit,String aText);
    private native int _insertString(int aTextBox,int aToolkit,String aText,int aPosition);
    private native int _delete(int aTextBox,int aToolkit,int aOffset,int aLength);
    private native int _size(int aTextBox,int aToolkit);
    private native int _setMaxSize(int aTextBox,int aToolkit,int aMaxSize);
    private native int _getMaxSize(int aTextBox,int aToolkit);
    private native int _setConstraints(int aTextBox,int aToolkit,int aConstraints);
    private native int _setInitialInputMode(int aTextField,int aToolkit,String aCharacterSubset);
} // class TextBox


