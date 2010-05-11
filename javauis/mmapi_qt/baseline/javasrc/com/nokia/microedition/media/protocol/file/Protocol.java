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
* Description:  file protocol
*
*/


package com.nokia.microedition.media.protocol.file;

import javax.microedition.media.MediaException;

import com.nokia.microedition.media.Locator;
import com.nokia.microedition.media.InternalPlayer;
import com.nokia.microedition.media.ManagerImpl;
import com.nokia.microedition.media.NativePlayerFactory;
import com.nokia.microedition.media.ConnectorDataSource;
import com.nokia.microedition.media.tone.TonePlayer;
import javax.microedition.io.file.FileConnection;
import javax.microedition.io.Connector;
import java.io.IOException;


/**
 * This class presents file protocol
 */
public class Protocol implements
        com.nokia.microedition.media.protocol.Protocol
{
    private static final String FILE = "file://";
    private static final String DRM_SUFFIX = "dcf";
    private static final String DRM_PARAMS = "?drm=enc";
    private static final String JTS_SUFFIX = "jts";
    /**
     * Default constructor.
     */
    public Protocol()
    {
    }

    /**
     * From interface Protocol
     * @see Protocol
     */
    public InternalPlayer createPlayer(Locator aLocator)
    throws java.io.IOException,
                MediaException,
                java.lang.SecurityException
    {
        FileConnection fileConnection = null;
        try
        {
            if ((aLocator.getParameters() == null) &&
                    (aLocator.getMiddlePart().endsWith(DRM_SUFFIX)
                    ))
            {
                // DRM file, adding parameters to locator
                fileConnection = (FileConnection)Connector.open(aLocator.getLocatorString(),Connector.READ);
            }
            else if (aLocator.getMiddlePart().endsWith(JTS_SUFFIX))
            {
                // JTS file, this format is handled in Java side, so creating player in traditional way
                ConnectorDataSource dataSource = new ConnectorDataSource(aLocator);
                return new TonePlayer(dataSource);
            }
            else
            {
                fileConnection = (FileConnection)Connector.open(aLocator.getLocatorString(),
                                 Connector.READ);
            }
            String fileAndPath  = getPathAndFileName(fileConnection);
            Locator newLocator = new Locator(fileAndPath);
            return NativePlayerFactory.createPlayer(newLocator, null);
        }
        finally
        {
            if (null != fileConnection)
            {
                fileConnection.close();
            }
        }
    }

    /**
     * Internal operation to construct path + fileName from the URI of the file.
     * Note: This operation removes parameter of the URI just like old
     * FileConnectionImpl.getPathAndFileName() operation (used in older releases).
     */
    private String getPathAndFileName(FileConnection aFileConn)
    {
        String path = aFileConn.getPath();
        // Path always starts with '/' character. This has to be skipped away because
        // otherwise this is converted to '\\' below. FILE constant contains this first
        // character('/') of the path.
        path = path.substring(1);
        String pathAndFileName = path + aFileConn.getName();
        pathAndFileName = pathAndFileName.replace('/','\\');
        return FILE + pathAndFileName;
    }
}