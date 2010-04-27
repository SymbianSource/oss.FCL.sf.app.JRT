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

package com.nokia.mj.impl.gcf.protocol.btspp;

import java.io.IOException;
import java.util.Hashtable;
import java.util.Vector;

import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.gcf.PushValidator;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.bluetooth.BTPermission;
import com.nokia.mj.impl.bluetooth.utils.DebugLog;
import com.nokia.mj.impl.bluetooth.utils.PushUrlValidator;
import com.intel.bluetooth.UtilsStringTokenizer;

/**
 * Used to validate url passed by apps trying to register for BTSPP push.
 * <p>
 * ABNF form of SPP server and client URLS:
 *
 * <pre>
 *    srvString = protocol colon slashes srvHost 0*5(srvParams)
 *    cliString = protocol colon slashes cliHost 0*3(cliParams)
 *
 *    protocol = btspp
 *    btspp = %d98.116.115.112.112 ; defines the literal btspp
 *
 *    cliHost = address colon channel
 *    srvHost = “localhost” colon uuid
 *
 *    channel = %d1-30
 *    uuid = 1*32(HEXDIG)
 *    colon = “:”
 *    slashes = “//”
 *    bool = “true” / “false”
 *    address = 12*12(HEXDIG)
 *    text = 1*( ALPHA / DIGIT / SP / “-” / “_” )
 *
 *    name = “;name=” text
 *    master = “;master=” bool ; see constraints noted below
 *    encrypt = “;encrypt=” bool ; see constraints noted below
 *    authorize = “;authorize=” bool ; see constraints noted below
 *    authenticate = “;authenticate=” bool ; see constraints noted below
 *    cliParams = master / encrypt / authenticate
 *    srvParams = name / master / encrypt / authorize / authenticate
 * </pre>
 *
 * <p>
 * The AllowedSender Filter string MUST conform to the following Augmented
 * Backus-Naur Form (ABNF) :
 *
 * <pre>
 *     filter = directfilter / directfilter blacklist
 *     directfilter = address / address authenticated / address authorized
 *     address = 1*12 (HEXDIG / ”?” / ”*”)
 *     blacklist = blacklistword list
 *     blacklistword = “;blacklist=”
 *     list = 0*1023 (address semicolon) address
 *     semicolon = “;”
 *     authenticated = “;authenticated”
 *     authorized = “;authorized”
 * </pre>
 *
 * According to RFC 2234 HEXDIG stands for hexadecimal digits (0-9, a-f, A-F).
 */
public class PushValidatorImpl extends PushValidator
{
    private static final String BTSPP_PREFIX = "btspp://";

    // Common Server parameters
    private static final String AUTHENTICATE = "authenticate";

    private static final String AUTHORIZE = "authorize";

    private static final String ENCRYPT = "encrypt";

    private static final String MASTER = "master";

    private static final String NAME = "name";

    private Vector pushBaseUris;

    // Only used for BTSPP
    private static final String RECEIVE_MTU = "receivemtu";

    private static final String TRANSMIT_MTU = "transmitmtu";

    private static final String BLACKLIST = "blacklist";

    private static final String AUTHENTICATED = "authenticated";

    private static final String AUTHORIZED = "authorized";

    // Hash table to store these
    private static Hashtable/* <String, String> */srvParams = new Hashtable();

    private static Hashtable/* <String, String> */filterParams = new Hashtable();

    static
    {
        // Populate values for server
        srvParams.put(AUTHENTICATE, AUTHENTICATE);
        srvParams.put(ENCRYPT, ENCRYPT);
        srvParams.put(MASTER, MASTER);
        srvParams.put(AUTHORIZE, AUTHORIZE);
        srvParams.put(NAME, NAME);
        srvParams.put(RECEIVE_MTU, RECEIVE_MTU);
        srvParams.put(TRANSMIT_MTU, TRANSMIT_MTU);

        // Populate values for filter
        filterParams.put(AUTHENTICATED, AUTHENTICATED);
        filterParams.put(AUTHORIZED, AUTHORIZED);
        filterParams.put(BLACKLIST, BLACKLIST);
    }

    public PushValidatorImpl()
    {
    }

    /**
     * Validates the url and filter passed by Push Framework.
     *
     * @param aUri
     *            URI passed to register push connection
     * @param aFilter
     *            Contains list of allowed/forbidden clients
     */
    public void validate(String aUri, String aFilter, Uid aSuiteUid,
                         String aMidletName, boolean aIsStaticRegistration)
    throws IOException
    {
        DebugLog.debug(" +  BTSPP PushValidator: Validate(): Uri: " + aUri
                       + " Filter: " + aFilter);

        String host = null;
        String name = aUri;

        // Try to extract the string between scheme and :
        int hostEnd = name.indexOf(':', BTSPP_PREFIX.length());

        if (hostEnd > -1)
        {
            // Check to see if "localhost" is present in the url, soon after
            // btl2cap:// and before the next colon.
            host = name.substring(BTSPP_PREFIX.length(), hostEnd);
            if (!host.equalsIgnoreCase("localhost"))
            {
                // Connection url is not that of server.
                DebugLog.error(" -  BTSPP PushValidator: Validate():"
                               + " Not a host!");
                throw new IllegalArgumentException();
            }
        }
        else
        {
            // We dont have url of the form btl2cap://<host>:<something>
            DebugLog.error(" -  BTSPP PushValidator: Validate():"
                           + " Not proper format");
            throw new IllegalArgumentException();
        }

        // String after "btl2cap://localhost:"
        String paramsStr = name.substring(hostEnd + 1);

        // Tokenize the string. Semicolon is used as delimiter.
        UtilsStringTokenizer urlTokens = new UtilsStringTokenizer(paramsStr,
                ";");

        if (urlTokens.hasMoreTokens())
        {
            // The first token after localhost: will be the uuid
            String uuid = urlTokens.nextToken();
            // check if this is valid UUID
            PushUrlValidator.checkUuidLength(uuid);
            PushUrlValidator.checkHexDig(uuid);
            PushUrlValidator.validateParams(srvParams, urlTokens);
        }
        else
        {
            // There are no tokens to split
            // portORuuid = paramsStr;
        }

        // We not move to validating the filter parameters
        if (aFilter != null || !aFilter.equals("") || !aFilter.equals("*"))
        {
            UtilsStringTokenizer filterTokens = new UtilsStringTokenizer(
                aFilter, ";");
            PushUrlValidator.validateFilterParams(filterParams, filterTokens);
        }

        String baseUri = getBaseURI(aUri);
        if (aIsStaticRegistration)
        {
            if (pushBaseUris == null)
            {
                pushBaseUris = new Vector();
                pushBaseUris.addElement(baseUri);
            }
            else
            {
                for (int indx = 0; indx < pushBaseUris.size(); indx++)
                {
                    String uri = (String) pushBaseUris.elementAt(indx);
                    if (uri.equalsIgnoreCase(baseUri))
                    {
                        throw new IllegalArgumentException();
                    }
                }
                pushBaseUris.addElement(aUri);
            }
        }

        Uid suiteUid = null;
        String commsName = null;
        if (aIsStaticRegistration)
        {
            suiteUid = aSuiteUid;
            commsName = PushValidator.INSTALLER_COMMS;
        }
        else
        {
            commsName = PushValidator.RUNTIME_COMMS;
        }
        if (PushValidator.isRegisteredPushUriStartingWith(baseUri, suiteUid,
                commsName))
        {
            if (aIsStaticRegistration)
            {
                throw new IllegalArgumentException();
            }
            else
            {
                throw new IOException();
            }
        }

        // Checks whether permission to open a connection is available or not.
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        BTPermission permission = new BTPermission("*", "server");
        appUtils.checkPermission(permission);
        // Checks to see if the power and discoverability mode are in order.
        // If not, it prompts the user. On failure throws appropriate exception
        PushUrlValidator.checkPowerAndDiscoverability(aMidletName,
                aIsStaticRegistration);
    }

    /*
     * Returns baseURI string extracted from server URI. This function assumes
     * the given URI is valid. Base URI contains "protocol://localhost:uuid"
     *
     * @param aUri Valid Server URI @return Extracted Base URI
     */
    private String getBaseURI(String aUri)
    {
        String baseUri;

        // Searching for end of uuid
        int endIndex = aUri.indexOf(";");
        if (endIndex < 0)
        {
            baseUri = aUri;
        }
        else
        {
            baseUri = aUri.substring(0, endIndex);
        }

        return baseUri;
    }
}
