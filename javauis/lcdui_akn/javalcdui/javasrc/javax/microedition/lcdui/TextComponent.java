/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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

// Utility class for TextField and TextBox

final class TextComponent
{

    public static void checkText(String aText,int aMaxSize)
    {
        if (aText == null)
            aText = "";
        if (aMaxSize <= 0 || aText.length() > aMaxSize)
            throw new IllegalArgumentException();
    }

    public static void checkOffset(int aOffset,int aLength,int aSize)
    {
        if (aOffset == 0 && aLength == 0 && aSize == 0)
        {
            return;
        }

        if (aOffset <0 || aOffset > aSize-1 || (aOffset+aLength) > aSize || aLength <0)
        {
            throw new StringIndexOutOfBoundsException();
        }
    }

    public static int getCharsFromString(String aString,int aSize,char[] aData)
    {
        if (aData.length < aSize)
            throw new ArrayIndexOutOfBoundsException();
        aString.getChars(0,aSize,aData,0);
        return aSize;
    }

    public static String getStringFromChars(char[] aChar,int aOffset,int aLength)
    {
        String string;
        if (aChar == null)
            return "";
        try
        {
            string = new String(aChar,aOffset,aLength);
        }
        catch (StringIndexOutOfBoundsException aException)
        {
            throw new ArrayIndexOutOfBoundsException();
        }
        return string;
    }

    public static void checkConstraints(int aConstraints)
    {
        final int constraints = (aConstraints & TextField.CONSTRAINT_MASK);
        //
        switch (constraints)
        {
        case TextField.ANY:
        case TextField.EMAILADDR:
        case TextField.NUMERIC:
        case TextField.PHONENUMBER:
        case TextField.URL:
        case TextField.DECIMAL:
        {
            final int modifier = (aConstraints & ~TextField.CONSTRAINT_MASK);
            if (modifier == 0 || (modifier >= 0x10000 && modifier <= 0x3F0000))
                return;
            else
                throw new IllegalArgumentException();
        }
        default:
            throw new IllegalArgumentException();
        }
    }

} // class TextComponent


