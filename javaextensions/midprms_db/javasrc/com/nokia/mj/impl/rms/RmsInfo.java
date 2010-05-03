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
* Description: RmsInfo
*
*/


package com.nokia.mj.impl.rms;
import javax.microedition.rms.*;

/**
 * RmsInfo provides access to common attributes of the record store
 */
public class RmsInfo
{
    private String iName;
    private long iLastModified;
    private int iVersion;
    private int iAuthMode;
    private int iWritable;
    private int iNextRecordId;
    private int iEncrypted;
    private String iAlgorithm;

    public RmsInfo(String aName, long aLastModified,
                   int aVersion, int aAuthMode, int aWritable, int aNextRecordId,
                   int aEncrypted, String aAlgorithm)
    {
        iName = aName;
        iLastModified = aLastModified;
        iVersion = aVersion;
        iAuthMode = aAuthMode;
        iWritable = aWritable;
        iNextRecordId = aNextRecordId;
        iEncrypted = aEncrypted;
        iAlgorithm = aAlgorithm;
    }

    public String getName()
    {
        return iName;
    }

    public long getLastModified()
    {
        return iLastModified;
    }

    public int getVersion()
    {
        return iVersion;
    }

    public int getNextRecordId()
    {
        return iNextRecordId;
    }

    public int getAuthMode()
    {
        return iAuthMode;
    }

    public boolean isWriteable()
    {
        return iWritable != 0;
    }

    public boolean isEncrypted()
    {
        return iEncrypted != 0;
    }

    public String getAlgorithm()
    {
        return iAlgorithm;
    }

}

