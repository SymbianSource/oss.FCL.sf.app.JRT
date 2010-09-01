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

/**
Utility class for ChoiceGroup and List
Methods which require the LCDUI lock to be held have UpUp() style names
*/
final class ChoiceComponent
{
    private class ChoiceDataItem
    {
        String iString;
        Image iImage;
        Font iFont;

        ChoiceDataItem(String aString, Image aImage, Font aFont)
        {
            iString = aString;
            iImage = aImage;
            iFont = aFont;
        }
    }

    private static final int ARRAY_INCREMENT = 4;

    private ChoiceDataItem[] iChoiceArray;
    private int              iChoiceCount;
    private int              iFitPolicy;

    //
    // Validate choicegroup/list constructor array args
    //
    static Image[] validateElements(String[] aTextArray, Image[] aIconArray)
    {
        final int size = aTextArray.length;
        if (aIconArray == null)
        {
            aIconArray = new Image[size];
        }
        if (aIconArray.length != size)
        {
            throw new IllegalArgumentException();
        }
        for (int i=0; i<size; i++)
        {
            checkItem(aTextArray[i]);// could just call length() and let the VM raise NPE
        }
        return aIconArray;
    }

    ChoiceComponent()
    {
        iFitPolicy   = Choice.TEXT_WRAP_DEFAULT;
        iChoiceArray = new ChoiceDataItem[] {};
        iChoiceCount = 0;
    }

    final String GetString(int aIndex)
    {
        checkIndexInBounds(aIndex, iChoiceCount);
        return iChoiceArray[aIndex].iString;
    }

    final Image GetImage(int aIndex)
    {
        checkIndexInBounds(aIndex, iChoiceCount);
        return iChoiceArray[aIndex].iImage;
    }

    final int size()
    {
        return iChoiceCount;
    }

    final int  Append(String aText, Image aIcon)
    {
        final int pos = iChoiceCount;
        Insert(pos, aText, aIcon);
        return pos;
    }

    final void Insert(int aIndex, String aText, Image aIcon)
    {
        checkItem(aText);
        checkIndexInBounds(aIndex, iChoiceCount+1);
        ChoiceDataItem item = new ChoiceDataItem(aText, aIcon, Font.getDefaultFont());
        CheckCapacity(iChoiceCount+1);
        System.arraycopy(iChoiceArray, aIndex, iChoiceArray, aIndex+1, iChoiceCount-aIndex);
        iChoiceArray[aIndex] = item;
        iChoiceCount++;
    }

    final void Delete(int aIndex)
    {
        checkIndexInBounds(aIndex, iChoiceCount);
        if (aIndex != iChoiceCount-1)
        {
            System.arraycopy(iChoiceArray, aIndex+1, iChoiceArray, aIndex, iChoiceCount-aIndex-1);
        }
        iChoiceCount--;
    }

    final void DeleteAll()
    {
        for (int i=0; i<iChoiceCount; i++)
        {
            iChoiceArray[i] = null;
        }
        iChoiceCount=0;
    }

    final void Set(int aIndex, String aString,Image aImage)
    {
        checkIndexInBounds(aIndex, iChoiceCount);
        checkItem(aString);

        //
        // Snapshot
        //
        if (aImage != null)
        {
            aImage = Image.createImage(aImage);
        }

        final ChoiceDataItem item = iChoiceArray[aIndex];
        item.iString = aString;
        item.iImage  = aImage;
    }

    final void SetFitPolicy(int aFitPolicy)
    {
        if (aFitPolicy != Choice.TEXT_WRAP_DEFAULT &&
                aFitPolicy != Choice.TEXT_WRAP_ON &&
                aFitPolicy != Choice.TEXT_WRAP_OFF)
            throw new IllegalArgumentException();
        // SYNC NOTE: Requires lock to be held to ensure that if another
        // thread reads this variable several times while holding the lock,
        // the reads will return consistent values.
        iFitPolicy=aFitPolicy;
    }

    final int getFitPolicy()
    {
        return iFitPolicy;
    }

    final void SetFont(int aElementNum,Font aFont)
    {
        checkIndexInBounds(aElementNum, iChoiceCount);
        final ChoiceDataItem item = iChoiceArray[aElementNum];
        item.iFont = aFont;
    }

    final Font GetFont(int aElementNum)
    {
        checkIndexInBounds(aElementNum, iChoiceCount);
        return iChoiceArray[aElementNum].iFont;
    }

    final static void checkIndexInBounds(int aIndex,int aSize)
    {
        if (aIndex <0 || aIndex >= aSize)
            throw new IndexOutOfBoundsException();
    }

    private final static void checkItem(String aString)
    {
        if (aString == null)
            throw new NullPointerException();
    }

    /**
    Grows the array if required.
    */
    private final void CheckCapacity(int aRequiredLength)
    {
        final int length = iChoiceArray.length;
        if (aRequiredLength>length)
        {
            ChoiceDataItem[] newItems = new ChoiceDataItem[length + ARRAY_INCREMENT];
            System.arraycopy(iChoiceArray, 0, newItems, 0, length);
            iChoiceArray = newItems;
        }
    }
}
