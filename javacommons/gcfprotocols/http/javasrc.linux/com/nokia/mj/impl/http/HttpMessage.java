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
* Description:
*
*/


package com.nokia.mj.impl.http;

import java.util.Vector;

/**
 * Http class responsible for storing HTTP message.
 */
public class HttpMessage
{
    private Vector mNameList;

    private Vector mValueList;

    /**
     * Constructs the Storage class.
     */
    public HttpMessage()
    {
        mNameList = new Vector();
        mValueList = new Vector();
    }

    /**
     * parse the protocol version.
     *
     * @param aKey
     *            the name of the http header.
     * @param aVal
     *            the value of the corresponding header.
     * @param mNameList
     *            the list into which the header key will be added.
     * @param mValueList
     *            the list into which the header value will be added.
     */
    public final void addElement(String aKey, String aVal)
    {
        String key = aKey.toLowerCase();
        String val = aVal;
        mNameList.addElement(key);
        mValueList.addElement(val);
    }

    /**
     * parse the protocol version.
     *
     * @param key
     *            the name of the http header to be removed.
     * @param mNameList
     *            the list from which header correspoding to key will be removed.
     * @param mValueList
     *            the list from which the header value corresponding to the key will be removed.
     */
    public final void removeElement(String aKey)
    {
        String keyLowerCase = aKey.toLowerCase();
        int index = mNameList.indexOf(keyLowerCase);
        if (index > -1)
        {
            mNameList.removeElementAt(index);
            mValueList.removeElementAt(index);
        }
    }

    /**
     * Returns the value of the http header.
     *
     * @param key
     *            the name of the http header.
     * @param mNameList
     *            the list of headers.
     * @param mValueList
     *            the list of values corresponding to header names.
     * @return the header value.
     */
    public final String getElement(String key)
    {
        if (key == null)
        {
            return null;
        }
        String key1 = key.toLowerCase();
        int index = mNameList.indexOf(key1);
        String hv = null;
        if (index > -1)
        {
            hv = (String) mValueList.elementAt(index);
        }
        return hv;
    }

    /**
     * Returns the name of the http header at the specified index.
     *
     * @param aIndex
     *            index into the list supplied.
     * @param mNameList
     *            the list of headers.
     * @return the header name.
     */
    public final String getNameAt(int aIndex)
    {
        String nv = (String) mNameList.elementAt(aIndex);
        return nv;
    }

    /**
     * Returns the value of the http header at the specified index.
     *
     * @param aIndex
     *            index into the list supplied.
     * @param mValueList
     *            the list of header values.
     * @return the header value.
     */
    public final String getValueAt(int aIndex)
    {
        String hv = (String) mValueList.elementAt(aIndex);
        return hv;
    }

    /**
     * Sets the name,value of the http header.
     *
     * @param aKey
     *            name of the http header.
     * @param aVal
     *            the header value.
     * @param mNameList
     *            the list of header names(keys).
     * @param mValueList
     *            the list of header values.
     */
    public final void setElement(String aKey, String aValue)
    {
        String keyLowerCase = aKey.toLowerCase();
        int index = mNameList.indexOf(keyLowerCase);
        if (index == -1)
        {
            addElement(keyLowerCase, aValue);
        }
        else
        {
            // for certain headers, it can have muliple value
            // so there will be multiple entries for the same key
            if (aKey.equals("User-Agent"))
            {
                String temp1 = (String) mValueList.elementAt(index);
                String temp2 = temp1.concat(", ");
                String multipleValues = temp2.concat(aValue);
                mValueList.setElementAt(multipleValues, index);
            }
            else
            {
                // else replace the old value with the new value
                mValueList.setElementAt(aValue, index);
            }
        }
    }

    /**
     * Sets the value of the http header at the specified index.
     *
     * @param aIndex
     *            the index of the http header.
     * @param aVal
     *            the header value.
     * @param mValueList
     *            the list of header values.
     */
    public final void setElementAt(String aVal, int aIndex)
    {
        mValueList.setElementAt(aVal, aIndex);
    }

    /**
     * Sets the value of the http header at the specified index.
     *
     * @param mNameList
     *            the list containing all the header names.
     * @return the number of elements present in the input list.
     */
    public final int numberOfElements()
    {
        return mNameList.size();
    }
}
