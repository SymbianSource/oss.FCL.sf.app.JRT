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

package com.nokia.mj.impl.security.midp.common;

import java.util.Hashtable;
import java.util.Enumeration;

/**
 * Mapping between named permissions and class named permissions
 */
public class PermissionMappingTable
{
    /**
     * The mapping between (MIDP2 and MIDP3 permissions) and the internal
     * class permissions
     */
    static Hashtable PERMISSION_MAPPING_TABLE;

    static
    {
        PERMISSION_MAPPING_TABLE = new Hashtable();
        // add MIDP2 named permissions
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.socket", new MIDPPermission(
                "com.nokia.mj.impl.gcf.protocol.socket.SocketPermissionImpl", "socket://*"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.serversocket", new MIDPPermission(
                "com.nokia.mj.impl.gcf.protocol.socket.SocketPermissionImpl", "socket://"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.http", new MIDPPermission(
                "com.nokia.mj.impl.gcf.protocol.http.HttpPermissionImpl", "http://*"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.https", new MIDPPermission(
                "com.nokia.mj.impl.gcf.protocol.https.HttpsPermissionImpl", "https://*"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.ssl", new MIDPPermission(
                "com.nokia.mj.impl.gcf.protocol.ssl.SSLPermissionImpl", "ssl://*"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.datagram", new MIDPPermission(
                "com.nokia.mj.impl.gcf.protocol.datagram.DatagramPermissionImpl", "datagram://*"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.datagramreceiver", new MIDPPermission(
                "com.nokia.mj.impl.gcf.protocol.datagram.DatagramPermissionImpl", "datagram://"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.comm", new MIDPPermission(
                "com.nokia.mj.impl.gcf.protocol.comm.CommPermissionImpl", "comm:*"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.PushRegistry", new MIDPPermission(
                "com.nokia.mj.impl.gcf.PushRegistryPermissionImpl", "*"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.sms", new MIDPPermission(
                "com.nokia.mj.impl.sms.SMSPermissionImpl", "sms://*", "open"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.wireless.messaging.sms.send", new MIDPPermission(
                "com.nokia.mj.impl.sms.SMSPermissionImpl", "sms://*", "send"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.wireless.messaging.sms.receive", new MIDPPermission(
                "com.nokia.mj.impl.sms.SMSPermissionImpl", "sms://*", "receive"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.mms", new MIDPPermission(
                "com.nokia.mj.impl.mms.MMSPermissionImpl", "mms://*", "open"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.wireless.messaging.mms.send", new MIDPPermission(
                "com.nokia.mj.impl.mms.MMSPermissionImpl", "mms://*", "send"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.wireless.messaging.mms.receive", new MIDPPermission(
                "com.nokia.mj.impl.mms.MMSPermissionImpl", "mms://*", "receive"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.cbs", new MIDPPermission(
                "com.nokia.mj.impl.cbs.CBSPermissionImpl", "cbs://*", "open"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.wireless.messaging.cbs.receive", new MIDPPermission(
                "com.nokia.mj.impl.cbs.CBSPermissionImpl", "cbs://*", "receive"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.file.read", new MIDPPermission(
                "com.nokia.mj.impl.file.FileInternalPermission", "file://*", "read"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.file.write", new MIDPPermission(
                "com.nokia.mj.impl.file.FileInternalPermission", "file://*", "write"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.pim.ContactList", new MIDPPermission(
                "com.nokia.mj.impl.pim.PIMPermissionImpl", "pim://*", "read_contacts,write_contacts"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.pim.ContactList.read", new MIDPPermission(
                "com.nokia.mj.impl.pim.PIMPermissionImpl", "pim://*", "read_contacts"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.pim.EventList", new MIDPPermission(
                "com.nokia.mj.impl.pim.PIMPermissionImpl", "pim://*", "read_events,write_events"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.pim.EventList.read", new MIDPPermission(
                "com.nokia.mj.impl.pim.PIMPermissionImpl", "pim://*", "read_events"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.pim.ToDoList", new MIDPPermission(
                "com.nokia.mj.impl.pim.PIMPermissionImpl", "pim://*", "read_todos,write_todos"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.pim.ToDoList.read", new MIDPPermission(
                "com.nokia.mj.impl.pim.PIMPermissionImpl", "pim://*", "read_todos"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.pim.ContactList.write", new MIDPPermission(
                "com.nokia.mj.impl.pim.PIMPermissionImpl", "pim://*", "write_contacts"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.pim.EventList.write", new MIDPPermission(
                "com.nokia.mj.impl.pim.PIMPermissionImpl", "pim://*", "write_events"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.pim.ToDoList.write", new MIDPPermission(
                "com.nokia.mj.impl.pim.PIMPermissionImpl", "pim://*", "write_todos"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.obex.server", new MIDPPermission(
                "com.nokia.mj.impl.bluetooth.BTOBEXPermission", "*", "server"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.obex.client", new MIDPPermission(
                "com.nokia.mj.impl.bluetooth.BTOBEXPermission", "*", "client"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.bluetooth.server", new MIDPPermission(
                "com.nokia.mj.impl.bluetooth.BTPermission", "*", "server"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.bluetooth.client", new MIDPPermission(
                "com.nokia.mj.impl.bluetooth.BTPermission", "*", "client"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.location.LandmarkStore.read", new MIDPPermission(
                "com.nokia.mj.impl.location.LocationPermission", "landmarks://*", "read"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.location.LandmarkStore.write", new MIDPPermission(
                "com.nokia.mj.impl.location.LocationPermission", "landmarks://*", "write"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.location.LandmarkStore.category", new MIDPPermission(
                "com.nokia.mj.impl.location.LocationPermission", "landmarks://*", "category"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.location.LandmarkStore.management", new MIDPPermission(
                "com.nokia.mj.impl.location.LocationPermission", "landmarks://*", "management"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.location.Location", new MIDPPermission(
                "com.nokia.mj.impl.location.LocationPermission", "location://*", "position"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.location.Orientation", new MIDPPermission(
                "com.nokia.mj.impl.location.LocationPermission", "location://*", "orientation"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.location.ProximityListener", new MIDPPermission(
                "com.nokia.mj.impl.location.LocationPermission", "proximity://*", "registerlistener"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.apdu.aid", new MIDPPermission(
                "com.nokia.mj.impl.satsa.APDUPermission", "apdu://*", "aid"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.apdu.sat", new MIDPPermission(
                "com.nokia.mj.impl.satsa.APDUPermission", "apdu://*", "sat"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.securityservice.CMSMessageSignatureService", new MIDPPermission(
                "com.nokia.mj.impl.pki.SignaturePermission", "pki://*", "authenticate"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.media.control.VideoControl.getSnapshot", new MIDPPermission(
                "com.nokia.mj.impl.media.PlayerPermission", "*", "snapshot"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.media.control.RecordControl", new MIDPPermission(
                "com.nokia.mj.impl.media.PlayerPermission", "*", "record"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.rtsp", new MIDPPermission(
                "com.nokia.mj.impl.media.RTSPPermission", "*"));
        PERMISSION_MAPPING_TABLE.put(
            "com.nokia.mid.Mobinfo.IMSI", new MIDPPermission(
                "com.nokia.mj.impl.properties.mobinfo.MobileInfoPermission", "mobinfo.imsi", "read"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.PropertyPermission", new MIDPPermission(
                "com.nokia.mj.impl.properties.mobinfo.MobileInfoPermission", "*", "read"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.contactless.ndef.NDEFTagConnection.write", new MIDPPermission(
                "com.nokia.mj.impl.nfc.utils.ContactlessCommunicationPermission", "ndef://*", "write"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.contactless.DiscoveryManager", new MIDPPermission(
                "com.nokia.mj.impl.nfc.utils.ContactlessCommunicationPermission", "DiscoveryManager", "get_instance"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.ndef", new MIDPPermission(
                "com.nokia.mj.impl.nfc.utils.ContactlessCommunicationPermission", "ndef://*", "open"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.rf", new MIDPPermission(
                "com.nokia.mj.impl.nfc.utils.ContactlessCommunicationPermission", "rf://*", "open"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.sc", new MIDPPermission(
                "com.nokia.mj.impl.nfc.utils.ContactlessCommunicationPermission", "sc://*", "open"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.Connector.vtag", new MIDPPermission(
                "com.nokia.mj.impl.nfc.utils.ContactlessCommunicationPermission", "vtag://*", "open"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.SocketProtocolPermission", new MIDPPermission(
                "com.nokia.mj.impl.gcf.protocol.socket.SocketPermissionImpl", "socket://*"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.HttpProtocolPermission", new MIDPPermission(
                "com.nokia.mj.impl.gcf.protocol.http.HttpPermissionImpl", "http://*"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.HttpsProtocolPermission", new MIDPPermission(
                "com.nokia.mj.impl.gcf.protocol.https.HttpsPermissionImpl", "https://*"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.SSLProtocolPermission", new MIDPPermission(
                "com.nokia.mj.impl.gcf.protocol.ssl.SSLPermissionImpl", "ssl://*"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.DatagramProtocolPermission", new MIDPPermission(
                "com.nokia.mj.impl.gcf.protocol.datagram.DatagramPermissionImpl", "datagram://*"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.CommProtocolPermission", new MIDPPermission(
                "com.nokia.mj.impl.gcf.protocol.comm.CommPermissionImpl", "comm://*"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.FileProtocolPermission", new MIDPPermission(
                "com.nokia.mj.impl.file.FileInternalPermission", "file://*"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.io.PushRegistryPermission", new MIDPPermission(
                "com.nokia.mj.impl.gcf.PushRegistryPermissionImpl", "*"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.location.LocationPermission", new MIDPPermission(
                "com.nokia.mj.impl.location.LocationPermission", "*"));
        PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.location.LandmarkPermission", new MIDPPermission(
                "com.nokia.mj.impl.location.LocationPermission", "*"));
       PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.midlet.AutoStartPermission", new MIDPPermission(
                "com.nokia.mj.impl.installer.utils.AutoStartPermission", null));
       PERMISSION_MAPPING_TABLE.put(
            "AutoStartPermission", new MIDPPermission(
                "com.nokia.mj.impl.installer.utils.AutoStartPermission", null));
       PERMISSION_MAPPING_TABLE.put(
            "javax.microedition.midlet.CmdLineArgsPermission", new MIDPPermission(
                "com.nokia.mj.impl.rt.utils.CmdLineArgsPermission", null));
       PERMISSION_MAPPING_TABLE.put(
            "com.nokia.webstart", new MIDPPermission(
                "com.nokia.mj.impl.rt.utils.CmdLineArgsPermission", null));
    }

    /**
     * Getter for permission mappings
     */
    public static MIDPPermission get(String permissionName)
    {
        return (MIDPPermission)PERMISSION_MAPPING_TABLE.get(permissionName);
    }

    /**
     * Setter for the mapping table
     *
     * @param  aMappingTable The new mapping table
     * @return the "old" mapping table = the mapping table which was replaced
     *         by aMappingTable or NULL if the mapping table was not set
     */
    public static Hashtable setMappingTable(Hashtable aMappingTable)
    {
        if (aMappingTable == null || aMappingTable.size() == 0)
        {
            return null;
        }
        Hashtable oldMappingTable = new Hashtable(PERMISSION_MAPPING_TABLE.size());
        for (Enumeration e = PERMISSION_MAPPING_TABLE.keys() ; e.hasMoreElements() ;)
        {
            Object key = e.nextElement();
            oldMappingTable.put(key, PERMISSION_MAPPING_TABLE.remove(key));
        }
        PERMISSION_MAPPING_TABLE = new Hashtable(aMappingTable.size());
        for (Enumeration e = aMappingTable.keys() ; e.hasMoreElements() ;)
        {
            Object key = e.nextElement();
            PERMISSION_MAPPING_TABLE.put(key, aMappingTable.get(key));
        }
        return oldMappingTable;
    }
}
