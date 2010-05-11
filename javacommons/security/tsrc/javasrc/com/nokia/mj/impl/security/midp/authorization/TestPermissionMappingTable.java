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

package com.nokia.mj.impl.security.midp.authorization;

import java.util.Hashtable;
import java.util.Enumeration;
import com.nokia.mj.impl.security.midp.common.*;

/**
 * Mapping between named permissions and class named permissions
 */
public class TestPermissionMappingTable
{
    /**
     * The mapping between (MIDP2 and MIDP3 permissions) and the internal
     * class permissions
     */
    private static Hashtable TEST_PERMISSION_MAPPING_TABLE;

    // The permission table must be extandable with permissions of the APIs outside MIDP
    // The table could be initialized from an external config file?
    // When the internal permissions are ready, change the mapping from MIDP2 into internal permissions instead of public permissions
    static
    {
        TEST_PERMISSION_MAPPING_TABLE = new Hashtable();
        // add MIDP2 named permissions
        TEST_PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.socket", new MIDPPermission(
                "com.nokia.mj.impl.security.midp.authorization.SocketProtocolPermissionImpl", "socket://*"));
        TEST_PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.serversocket", new MIDPPermission(
                "com.nokia.mj.impl.security.midp.authorization.SocketProtocolPermissionImpl", "socket://"));
        TEST_PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.http", new MIDPPermission(
                "com.nokia.mj.impl.security.midp.authorization.HttpProtocolPermissionImpl", "http://*"));
        TEST_PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.https", new MIDPPermission(
                "com.nokia.mj.impl.security.midp.authorization.HttpsProtocolPermissionImpl", "https://*"));
        TEST_PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.ssl", new MIDPPermission(
                "com.nokia.mj.impl.security.midp.authorization.SSLProtocolPermissionImpl", "ssl://*"));
        TEST_PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.datagram", new MIDPPermission(
                "com.nokia.mj.impl.security.midp.authorization.DatagramProtocolPermissionImpl", "datagram://*"));
        TEST_PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.datagramreceiver", new MIDPPermission(
                "com.nokia.mj.impl.security.midp.authorization.DatagramProtocolPermissionImpl", "datagram://"));
        TEST_PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.comm", new MIDPPermission(
                "com.nokia.mj.impl.security.midp.authorization.CommProtocolPermissionImpl", "comm://*"));
        TEST_PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.PushRegistry", new MIDPPermission(
                "javax.microedition.io.PushRegistryPermission", "*"));
        TEST_PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.sms", new MIDPPermission(
                "com.nokia.mj.impl.sms.SMSPermissionImpl", "sms://*", "open"));
        TEST_PERMISSION_MAPPING_TABLE.put(
            "javax.wireless.messaging.sms.send", new MIDPPermission(
                "com.nokia.mj.impl.sms.SMSPermissionImpl", "sms://*", "send"));
        TEST_PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.mms", new MIDPPermission(
                "com.nokia.mj.impl.mms.MMSPermissionImpl", "mms://*", "open"));
        TEST_PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.file.read", new MIDPPermission(
                "com.nokia.mj.impl.security.midp.authorization.FileProtocolPermissionImpl", "file://*", "read"));
        TEST_PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.securityservice.CMSMessageSignatureService", new MIDPPermission(
                "javax.microedition.securityservice.SignaturePermission", "pki://*", "authenticate"));
        // add name of MIDP3 class permissions
        TEST_PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.SocketProtocolPermission", new MIDPPermission(
                "com.nokia.mj.impl.security.midp.authorization.SocketProtocolPermissionImpl", "socket://*"));
        TEST_PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.HttpProtocolPermission", new MIDPPermission(
                "com.nokia.mj.impl.security.midp.authorization.HttpProtocolPermissionImpl", "http://*"));
        TEST_PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.HttpsProtocolPermission", new MIDPPermission(
                "com.nokia.mj.impl.security.midp.authorization.HttpsProtocolPermissionImpl", "https://*"));
        TEST_PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.SSLProtocolPermission", new MIDPPermission(
                "com.nokia.mj.impl.security.midp.authorization.SSLProtocolPermissionImpl", "ssl://*"));
        TEST_PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.DatagramProtocolPermission", new MIDPPermission(
                "com.nokia.mj.impl.security.midp.authorization.DatagramProtocolPermissionImpl", "datagram://*"));
        TEST_PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.CommProtocolPermission", new MIDPPermission(
                "com.nokia.mj.impl.security.midp.authorization.CommProtocolPermissionImpl", "comm://*"));
        TEST_PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.FileProtocolPermission", new MIDPPermission(
                "com.nokia.mj.impl.security.midp.authorization.FileProtocolPermissionImpl", "file://*"));
        TEST_PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.PushRegistryPermission", new MIDPPermission(
                "javax.microedition.io.PushRegistryPermission", "*"));
    }

    /**
     * Getter for permission mapping table
     */
    public static Hashtable getMappingTable()
    {
        return TEST_PERMISSION_MAPPING_TABLE;
    }
}
