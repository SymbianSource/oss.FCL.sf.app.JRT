/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.microedition.m2g.connection.file;

import javax.microedition.io.file.FileConnection;
import javax.microedition.io.Connector;
import java.io.IOException;
import com.nokia.microedition.m2g.connection.*;
import com.nokia.mj.impl.utils.Logger;

/**
 * File connection proxy class
 */
public class M2GConnectionProxyImpl extends M2GConnectionProxy
{

    //--------------------------------------------------
    // STATIC CONSTANTS
    //--------------------------------------------------
    public static final String READ_PERMISSION =
        "javax.microedition.io.Connector.file.read";

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
     * Ctor.
     */
    public M2GConnectionProxyImpl()
    {
        super();
    }

    /**
     * Open a connection.
     * @see connection.common.M2GConnectionProxy#open()
     */
    public void open(M2GUrlTokenizer aTokenizer) throws IOException
    {
        boolean openingStream = false;
        FileConnection connection = null;
        try
        {
            iBaseUrl = aTokenizer.getBaseUrl();

            // extract the suffix url from the initial url used to open the
            // file, and not the constructed (altered in default mode) url by
            // M2GConnectionPolicyImpl.
            // This suffix is passed to the external resource handler later
            iSuffixUrl = aTokenizer.getParameters();
            if (iSuffixUrl != null)
            {
                iSuffixUrl = M2GUrlTokenizer.PARAMETER_SEPARATOR + iSuffixUrl;
            }

            Logger.LOG(
                Logger.EJavaUI,
                Logger.EInfo,
                "file open(), suffix URI: "
                + (iSuffixUrl != null ? iSuffixUrl.toString() : "null")
                + ", create file connection - 0");

            M2GConnectionPolicyImpl connectionPolicy =
                new M2GConnectionPolicyImpl(aTokenizer);

            String uri = aTokenizer.getUrl();

            Logger.LOG(
                Logger.EJavaUI,
                Logger.EInfo,
                "file open(), URI: " + uri
                + ", create file connection - 2");

            connection = (FileConnection) Connector.open(uri,
                         Connector.READ);
            openingStream = true;
            iInputStream = connection.openInputStream();
            openingStream = false;
            iConnection = connection;
            iConnectionPolicy = connectionPolicy;
        }
        finally
        {
            if ((true == openingStream) && (null != connection))
            {
                connection.close();
            }
        }
    }
}
