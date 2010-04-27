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
* Description: RecordStoreOpenArgs
*
*/


package com.nokia.mj.impl.rms;
import javax.microedition.rms.*;
import com.nokia.mj.impl.rt.support.*;

/**
 * RecordStoreOpenArgs is a wrapper for RecordStore.openRecordStore() arguments.
 * RecordStore can be opened number of ways and RecordStoreOpenArgs can be used to
 * hide the details.
 */
public class RecordStoreOpenArgs
{
    private RmsName iName;
    private boolean iCreateIfNecessary;
    private int iAuthmode;
    private boolean iWritable;
    private String iPassword;

    public RecordStoreOpenArgs(String aName, boolean aCreateIfNecessary,
                               int aAuthmode, boolean aWritable, String aPassword)
    {
        iName = new RmsName(aName);

        iCreateIfNecessary = aCreateIfNecessary;
        iAuthmode = aAuthmode;
        iWritable = aWritable;
        iPassword = aPassword;
    }

    public RecordStoreOpenArgs(String aName, String aVendorName, String aSuiteName, String aPassword)
    {
        iName = new RmsName(aName, aVendorName, aSuiteName);

        iCreateIfNecessary = false;
        iAuthmode = RecordStore.AUTHMODE_PRIVATE;
        iWritable = true;
        iPassword = aPassword;
    }

    public RmsName getRecordStoreName()
    {
        return iName;
    }

    public boolean getCreateFlag()
    {
        return iCreateIfNecessary;
    }

    public boolean getWritableFlag()
    {
        return iWritable;
    }

    public int getAuthmode()
    {
        return iAuthmode;
    }

    public String getPassword()
    {
        return iPassword;
    }

}

