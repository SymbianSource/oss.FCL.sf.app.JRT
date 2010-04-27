/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.gcf.utils;

import com.nokia.mj.impl.rt.support.Jvm;

/**
Parses the basic elements of a URI, i.e. scheme, target, parameters
*/
public final class URI
{
    private static final int PROTOCOL = 1;
    private static final int TARGET = 2;
    private static final int HOST = 3;
    private static final int PORT = 4;
    private static final int PATH = 5;
    private static final int PARAMETER = 6;
    private static final int QUERY = 7;

    private static final String EMPTY_STRING = "";



    private final String iFullURI;
    private final int[] iErrorCache;


    static
    {
        try
        {
            // Load the native libraries
            Jvm.loadSystemLibrary("javagcf");
        }
        catch (Exception e)
        {
            StreamsLogger.ELog("Unable to load native libraries!");
        }
    }

    public URI(String aURIString)
    {
        iErrorCache = new int[1];
        iFullURI = aURIString;
    }



    private String parse(int aUriPortion)
    {
        return parse(aUriPortion, null);
    }



    private String parse(int aUriPortion, String aParameterName)
    {
        int error = 0;
        String value = null;
        synchronized (this)
        {
            value = _parse(iFullURI, aUriPortion, aParameterName, iErrorCache);
            error = iErrorCache[0];
        }
        //JDEBUG( "parse : value = " + value );
        if (value == null)
        {
            if (error == -1)    //KErrNotFound
            {
                return EMPTY_STRING;
            }
            if (error != 0)
            {
                throw new IllegalArgumentException();
            }
        }
        return value;
    }



    private native String _parse(String aURI, int aUriPortion, String aParameterName,
                                 int[] aError);



    public String getProtocol()
    {
        return parse(PROTOCOL);
    }



    public String getHost()
    {
        return parse(HOST);
    }



    public String getPort()
    {
        return parse(PORT);
    }



    public String getPath()
    {
        return parse(PATH);
    }


    public String getParameter(String aParameterName)
    {
        return parse(PARAMETER, aParameterName);
    }


    public String getQuery()
    {
        return parse(QUERY);
    }



    public String toString()
    {
        return iFullURI;
    }
}


