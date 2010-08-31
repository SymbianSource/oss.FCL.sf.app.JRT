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
* Description:  This class implements functionalities defined in the
*                MetaDataControl interface (included in
*                javax.microedition.media.control package).
*
*/


package com.nokia.microedition.media.control;

import java.util.Vector;
import java.util.Enumeration;

// CLASS DEFINITION
/**
 * The <code>MetaDataControl</code> class implements functionalities defined
 * in the <code>MetaDataControl</code> interface (included in
 * <code>javax.microedition.media.control</code> package).
 */

public class MetaDataControl
        extends ControlImpl
        implements javax.microedition.media.control.MetaDataControl
{

    /**
     * The metadata keys received from the native side. It is assumed that
     * the keys will not change during the lifetime of a media clip or
     * stream. Key data is used so frequently that we want to buffer it
     * here at Java side.
     */
    private String[] iKeys = null;
    private static final String[] DEFAULT_DATA = { MetaDataControl.AUTHOR_KEY,
            MetaDataControl.COPYRIGHT_KEY,
            MetaDataControl.DATE_KEY,
            MetaDataControl.TITLE_KEY
                                                 };
    private static final String DEFAULT_DATA_VALUE = "unknown";

    // Static initialization

    /**
     * Creates an instance of <code>MetaDataControl</code>.
     */
    public MetaDataControl()
    {
    }

    /**
     * Returns acceptable metadata keys.
     *
     * The keys can be used for getting specific metadata value associated to each key.
     *
     * @See <code>javax.microedition.media.control.MetaDataControl</code>
     * interface documentation for details.
     *
     * @return Acceptable key values.
     *
     */
    public String[] getKeys()
    {
        checkState();
        // Get the keys only if not yet buffered.
        if (null == iKeys)
        {
            int keyCount = _getKeysCount(iEventSource, iControlHandle);

            if (keyCount < 0)    // can't read key count
            {
                iKeys = DEFAULT_DATA;
                return iKeys;
            }

            Vector keys = new Vector(keyCount + 1);

            for (int i = 0; i < keyCount; i++)
            {
                String currentKey = _getKey(iEventSource, iControlHandle, i);
                if (currentKey == null)  // can't read key value
                {
                    iKeys = DEFAULT_DATA;
                    return iKeys;
                }
                keys.addElement(currentKey);
            }

            // Check if the default keys are already in the key vector
            // and if not, then add them

            for (int i = 0; i < DEFAULT_DATA.length; i++)
            {
                checkAddKey(keys, DEFAULT_DATA[ i ]);
            }

            iKeys = new String[ keys.size()];
            keys.copyInto(iKeys);
        }
        return iKeys;
    }

    /**
     * Check if there is already a key in vector and if not, then
     * add one.
     * @param aKeys vector containing keys to be checked and which to add
     * new key
     * @param aKey key string to be added if not already present in aKeys
     */
    private void checkAddKey(Vector aKeys, String aKey)
    {
        for (Enumeration e = aKeys.elements(); e.hasMoreElements();)
        {
            String currentElement = (String)e.nextElement();
            if (currentElement.equals(aKey))
            {
                return;
            }
        }
        // Otherwise add the key
        aKeys.addElement(new String(aKey));
    }

    /**
     * Fetches a value of the given metadata key.
     *
     * See <code>javax.microedition.media.control.MetaDataControl</code>
     * interface documentation for details.
     *
     * @param aKey Key of the metadata value to be returned.
     *
     * @return Value of the metadata.
     *
     * @exception IllegalArgumentException Thrown if the given key is
     * <code>null</code> or invalid.
     */
    public String getKeyValue(String aKey)
    {
        checkState();
        if (null == aKey)
        {
            throw new IllegalArgumentException("Key cannot be null");
        }

        getKeys();   // Buffer the keys if not present

        int arrLen = iKeys.length;
        int i = 0;

        // Check if the given key is valid and return the value.
        while (i < arrLen)
        {
            if (iKeys[ i ].equals(aKey))
            {
                String value =  _getKeyValue(iEventSource, iControlHandle, aKey);

                // if key is ok but value is null then this key does not really
                // exist and it must be checked whether it is one from DEFAULT_DATA
                if (value == null)
                {
                    for (int j = 0; j < DEFAULT_DATA.length; j++)
                    {
                        if (aKey.equals(DEFAULT_DATA[ j ]))
                        {
                            return(DEFAULT_DATA_VALUE);
                        }
                    }
                }
                return value;
            }
            ++i;
        }

        // No match - invalid key.
        throw new IllegalArgumentException("Invalid non-null key");
    }

    private native int _getKeysCount(int aEventSource,
                                     int aControlHandle);
    private native String _getKey(int aEventSource,
                                  int aControlHandle,
                                  int aIndex);
    private native String _getKeyValue(int aEventSource,
                                       int aControlHandle,
                                       String aKey);
}   //end of file

