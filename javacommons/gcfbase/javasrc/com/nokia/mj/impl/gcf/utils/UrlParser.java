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



package com.nokia.mj.impl.gcf.utils;

import java.lang.*;
import java.io.IOException;
import com.nokia.mj.impl.utils.Logger;

public class UrlParser
{
    public String uri = null;
    public String scheme = null;
    public String authority = null;
    public String path = null;
    public String query = null;
    public String fragment = null;
    public String userinfo = null;
    public String host = null;
    public String portString = null;
    public static final int NOT_SPECIFIED = -1;
    public int port = NOT_SPECIFIED;
    private int startScheme = 0;
    private int endScheme = 0;
    private int startAuthority = 0;
    private int endAuthority = 0;
    private int startPath = 0;
    private int endPath = 0;
    private int startQuery = 0;
    private int endQuery = 0;
    private int startFragment = 0;
    private int endFragment = 0;
    private int startPort = 0;
    private int endUserinfo = 0;
    private int startHost = 0;
    private int endHost = 0;
    private int doubleSlash = 0;
    private boolean isPath = true;
    private boolean isQuery = true;
    private boolean isFragment = true;
    private boolean doubleColon = false;
    private boolean isPortNumber = true;

    public UrlParser(String absoluteUri)
    {
        Logger.LOG(Logger.ESOCKET,Logger.EInfo,"UrlParser(): uri = "+absoluteUri);
        String partialUri = null;
        uri = absoluteUri;
        //this.isPortNumber = isPortNumber;
        endScheme = uri.indexOf(":", startScheme);
        if (endScheme == -1)
            throw new IllegalArgumentException("Invalid url: "+absoluteUri);

        if (endScheme > startScheme)
            scheme = uri.substring(startScheme,endScheme);

        if (endScheme  == uri.length()-1)
            return; //only scheme present

        partialUri = uri.substring(endScheme + 1);
        if (partialUri.startsWith("//") == false)
            throw new IllegalArgumentException("Invalid url: "+absoluteUri);

        if (uri.charAt(endScheme+1)=='/' && uri.charAt(endScheme+2) == '/')
        {
            doubleSlash = endScheme + 1;
            if (endScheme > startScheme)
                scheme = uri.substring(startScheme,endScheme);
        }


        parse(absoluteUri);
        Logger.LOG(Logger.ESOCKET,Logger.EInfo,"scheme : "+scheme+ "host: "+host+ "port: "+port+ "authority: "+authority+ "query: "+query+"fragment: "+fragment);
    }

    public UrlParser(String scheme, String relativeUri)
    {
        Logger.LOG(Logger.ESOCKET,Logger.EInfo,"partial uri");
        uri = relativeUri;
        //this.isPortNumber = isPortNumber;
        this.scheme = scheme;
        if (uri.startsWith("//") == false)
            throw new IllegalArgumentException("Invalid Url: "+scheme+":"+relativeUri);
        parse(scheme+":"+relativeUri);
    }


    private void parse(String aUri)
    {


        //  if(uri == null)
        //  throw new Exception("url is null");

        if (doubleSlash > 0)
            startAuthority = endScheme + 3;
        else
            startAuthority = endScheme + 2; // no authority present

        Logger.LOG(Logger.ESOCKET,Logger.EInfo,"uri = "+uri+ " starauth = "+startAuthority);
        int index = uri.indexOf('/', startAuthority);
        if (index == -1)
        {
            isPath = false;
            if (uri.indexOf('?', startAuthority)== -1)
            {
                isQuery = false;
                if (uri.indexOf('#', startAuthority) == -1)
                {
                    isFragment = false;
                    endAuthority = uri.length();
                }
                else
                {
                    endAuthority = uri.indexOf('#', startAuthority);
                }
            }
            else
            {
                endAuthority = uri.indexOf('?', startAuthority);
            }
        }
        else
        {
            endAuthority = uri.indexOf('/', startAuthority);
        }
        String bang = "bang";
        Logger.LOG(Logger.ESOCKET,Logger.EInfo,"startauth = "+startAuthority+ "  endauth = "+endAuthority);
        if (endAuthority == startAuthority)
        {
            throw new IllegalArgumentException("Invalid url: "+aUri);
        }
        if (endAuthority != -1 && startAuthority != -1 && endAuthority > startAuthority)
            authority = uri.substring(startAuthority,endAuthority);
        Logger.LOG(Logger.ESOCKET,Logger.EInfo,"authority = "+authority);

        // parse for userinfo
        endUserinfo = authority.indexOf('@',0);
        if (endUserinfo != -1)
        {
            if (endUserinfo > 0)
                userinfo = authority.substring(0,endUserinfo);
            startHost = endUserinfo + 1;
        }
        else
            startHost = 0;

        // if host is IPV6 address, it will contain ':'
        index = authority.indexOf(']');
        if (index != -1)
        {
            endHost = index;
            startPort = authority.indexOf(':', endHost); //in case of IPV6 address
        }
        else
            startPort = authority.indexOf(':'); // not an ipv6 address
        if (startPort != -1)
        {
            startPort++;
            //String p = null;
            if (authority.length() > startPort)
                Logger.LOG(Logger.ESOCKET,Logger.EInfo,"startPort = "+startPort+ "endAuthority = "+endAuthority);
            //p = authority.substring(startPort, endAuthority);
            portString = authority.substring(startPort, authority.length());
            Logger.LOG(Logger.ESOCKET,Logger.EInfo,"p = "+portString);
            //if(isPortNumber == true)
            if (portString.equals(""))
            {
                portString = null;
                Logger.LOG(Logger.ESOCKET,Logger.EInfo,"setting portstring to null");
            }
            if (portString != null)
            {
                try
                {
                    port = Integer.parseInt(portString);
                }
                catch (NumberFormatException e)
                {
                    throw new IllegalArgumentException("Port is not a number");
                }
                if (port < 1 || port > 65535)
                {
                    throw new IllegalArgumentException("Invalid Port number");
                }
            }
            endHost = startPort - 1;
        }
        else
            endHost = authority.length();
        Logger.LOG(Logger.ESOCKET,Logger.EInfo,"startport = "+startPort);
        Logger.LOG(Logger.ESOCKET,Logger.EInfo,"port = "+port);;
        Logger.LOG(Logger.ESOCKET,Logger.EInfo,"starthost = "+startHost+ "endhost= "+endHost);
        if (endHost > startHost)
            host = authority.substring(startHost, endHost);
        Logger.LOG(Logger.ESOCKET,Logger.EInfo,"host = "+host);
        if (host != null)
        {
            if (host.startsWith("[") && host.endsWith("]"))
            {
                if (host.length() < 4) // minimum ipv6 address '::'
                    throw new IllegalArgumentException("IPV6 adrress invalid");

                if (host.startsWith("::"))
                    doubleColon = true;
                if (endHost > startHost + 1)
                    checkIPv6format(host.substring(startHost + 1, endHost));
            }
        }

        if (isPath)
        {
            startPath = endAuthority;
            if (uri.indexOf('?', startPath) == -1)
            {
                isQuery = false;
                if (uri.indexOf('#',startPath) == -1)
                {
                    isFragment = false;
                    endPath = uri.length();
                }
                else
                {
                    endPath = uri.indexOf('#', startPath);
                }
            }
            else
            {
                endPath = uri.indexOf('?', startPath);
            }
        }
        if (endPath > startPath)
            path = uri.substring(startPath,endPath);


        if (isQuery)
        {
            startQuery = uri.indexOf('?', endAuthority);
            if (uri.indexOf('#', startQuery) == -1)
            {
                isFragment = false;
                endQuery = uri.length();
            }
            else
            {
                endQuery = uri.indexOf('#', startQuery);
            }
            if (endQuery > startQuery)
                query = uri.substring(startQuery+1, endQuery);
        }

        if (isFragment)
        {
            startFragment = uri.indexOf('#', endAuthority);
            endFragment = uri.length();
            if (endFragment > startFragment)
                fragment = uri.substring(startFragment+1, endFragment);
        }



    }
    public int getPort()
    {
        return port;
    }
    private void checkIPv6format(String ipv6addr)
    {

        int colon = 0;
        int doubleColon = 0;
        int start = 0;
        int end = 0;
        int index = 0;
        String fields = null;

        if (ipv6addr.startsWith("::") && ipv6addr.length() == 2)
            return; //minimum ipv6 address '::'

        end = ipv6addr.length() - 1;
        while (start <= end)
        {
            if (ipv6addr.indexOf("::", start) != -1)
                doubleColon++;
            if (ipv6addr.indexOf(':', start) != -1)
                colon++;
            start++;
        }



        if (doubleColon >= 2)   // only one '::' allowed, either in beginning, or middle or at the end
            throw new IllegalArgumentException("Invalid IPV6 address");



        if (doubleColon == 1)
        {
            if (colon < 2 || colon > 6) // minimum '::' or maximum of 6 colons for 8 fields
                throw new IllegalArgumentException("invalid IPV6 address");
        }
        else
        {
            if (colon < 6 || colon > 7) // 6 must in case of ending with IPv4address or 7 for 8 fields.
                throw new IllegalArgumentException("invalid IPV6 address");
        }

        start = 0;
        while (start <= end)
        {
            index = ipv6addr.indexOf(':', start);
            if (index != -1)
            {
                if (index > start)
                    fields = ipv6addr.substring(start, index);
                if (!validHex(fields) && !validIpv4(fields))
                    throw new IllegalArgumentException("invalid IPv6 address");

                start = index + 1;
            }
            else
                break;
        }
    }

    private boolean validHex(String fields)
    {
        int hex = 0;
        if (fields.length() > 4)
            return false;

        byte[] hexarray = fields.getBytes();

        for (int i =0; i< fields.length()-1; i++)
        {
            hex = (int) hexarray[i];
            if (!(hex >= 48) && !(hex <= 57) ||
                    !(hex >= 65) && !(hex <= 70) ||
                    !(hex >= 97) && !(hex <= 102))
                return false;
        }
        return true;
    }


    private boolean validIpv4(String fields)
    {
        int dot = 0;
        int start = 0;
        int end = 0;
        int len = fields.length();
        int index = 0;
        String dec = null;
        end = len - 1;
        if (len < 7 || len > 15) //minimum 0.0.0.0 and maximum 255.255.255.255
            return false;

        while (start <= end)
        {
            index = fields.indexOf('.', start);
            if (index != -1)
            {
                dot++;
                if (index > start)
                    dec = fields.substring(start,index);
                int decval = Integer.parseInt(dec);
                if (decval < 0 && decval >255)
                    return false;
                start = index + 1;
            }
            else
                break;
        }
        if (dot != 3)
            return false;

        return true;
    }


    /**
     * Converts this URL into a string.
     *
     * @return string representation of this URL
     */
    public String toString()
    {
        StringBuffer url = new StringBuffer();

        if (scheme != null)
        {
            url.append(scheme);
            url.append(':');
        }

        if (authority != null)
        {
            url.append('/');
            url.append('/');
            url.append(authority);
        }

        if (path != null)
        {
            url.append(path);
        }

        if (query != null)
        {
            url.append('?');
            url.append(query);
        }

        if (fragment != null)
        {
            url.append('#');
            url.append(fragment);
        }

        return url.toString();
    }
};







