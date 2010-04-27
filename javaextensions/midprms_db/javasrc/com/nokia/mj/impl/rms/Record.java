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
* Description: Record
*
*/


package com.nokia.mj.impl.rms;

/**
 * Record is a helper class for record id, tag and record data.
 * Record related data can be accessed via this class, which simplifies
 * implementation of RecordStore related operations. For example,
 * there are two RecordStore.setRecord() operations which differ only
 * by whether tag is given or not.
 */
public class Record
{
    private int iId;
    private int iTag;
    private byte[] iData;
    private boolean iKeepExistingTag;

    public Record(int aId)
    {
        this(aId, null, 0, 0, 0);
    }

    public Record(int aId, byte[] aData, int aTag)
    {
        this(aId, aData, 0, aData == null ? 0 : aData.length, aTag);
    }

    public Record(byte[] aData, int aOffset, int aNumBytes, int aTag)
    {
        this(0, aData, aOffset, aNumBytes, aTag);
    }

    public Record(int aId, byte[] aData, int aOffset, int aNumBytes)
    {
        this(aId, aData, aOffset, aNumBytes, 0);
    }

    public Record(int aId, byte[] aData, int aOffset, int aNumBytes, int aTag)
    {
        iId = aId;
        iTag = aTag;
        setData(aData, aOffset, aNumBytes);
        iKeepExistingTag = false;
    }

    public int getId()
    {
        return iId;
    }

    public byte[] getData()
    {
        return iData;
    }

    public int getTag()
    {
        return iTag;
    }

    public void setTag(int aTag)
    {
        iTag = aTag;
    }

    public int size()
    {
        return iData != null ? iData.length : 0;
    }

    /**
     * The tag value associated with the record can be left unchanged
     * or replaced depending how RecordStore.setRecord() is called.
     * This method is used to set whether the tag value should be replaced
     */
    public void setKeepExistingTag(boolean aKeepExistingTag)
    {
        iKeepExistingTag = aKeepExistingTag;
    }

    public boolean keepExistingTag()
    {
        return iKeepExistingTag;
    }

    private void setData(byte[] aData, int aOffset, int aNumBytes)
    {
        if ((aNumBytes > 0) && (aData == null))
        {
            throw new NullPointerException(RmsErrorStrings.DATA_NULL);
        }

        if (aData != null)
        {
            iData = new byte[aNumBytes];
            System.arraycopy(aData, aOffset, iData, 0, aNumBytes);
        }
    }

}

