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

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.security.midp.authorization.SecurityPolicyModule;
import com.nokia.mj.impl.security.midp.common.PolicyBasedPermission;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettings;
import com.nokia.mj.impl.security.midp.common.PermissionMappingTable;
import com.nokia.mj.impl.security.midp.common.MIDPPermission;
import com.nokia.mj.impl.rt.support.ApplicationInfo;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * SecurityPolicyModule unit tests.
 */
public class SecurityPolicyModuleTests extends TestCase implements InstallerMain
{

    static final SecurityPolicyPermission[] MANUFACTURER_PERMS =
    {
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.HttpProtocolPermission"),"http://*", null, null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.HttpsProtocolPermission"),"https://*", null, null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.DatagramProtocolPermission"),"datagram://*", null, null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.DatagramProtocolPermission"),"datagram://", null, null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.SocketProtocolPermission"),"socket://*", null, null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.SocketProtocolPermission"),"socket://", null, null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.SSLProtocolPermission"),"ssl://*", null, null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.CommProtocolPermission"),"comm:*", null, null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.FileProtocolPermission"),"*", "read", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.FileProtocolPermission"),"*", "write", null),
        new SecurityPolicyPermission(getPermissionName("java.util.PropertyPermission"),"microedition.*","read",null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.sms"), "sms://*" , "open", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.sms"), "sms://*" , "send", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.sms"), "sms://*" , "receive", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.mms"), "mms://*" , "open", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.mms"), "mms://*" , "send", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.mms"), "mms://*" , "receive", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.cbs"), "cbs://*" , "open", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.cbs"), "cbs://*" , "receive", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.ContactList"), "pim://*" , "read_contacts", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.EventList"), "pim://*" , "read_events", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.ToDoList"), "pim://*" , "read_todos", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.ContactList"), "pim://*" , "write_contacts", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.EventList"), "pim://*" , "write_events", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.ToDoList"), "pim://*" , "write_todos", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.PushRegistryPermission"),"*","staticregistration",null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.PushRegistryPermission"),"*","dynamicregistration",null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.PushRegistryPermission"),"*","autoinvocation",null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.obex.server"),"*", "server", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.obex.client"),"*", "client", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.bluetooth.server"),"*", "server", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.bluetooth.client"),"*", "client", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"location://*", "position", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"location://*", "orientation", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"landmarks://*", "read", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"landmarks://*", "write", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"landmarks://*", "category", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"landmarks://*", "management", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"proximity://*", "registerlistener", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.apdu.aid"),"apdu://*", "aid", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.apdu.sat"),"apdu://*", "sat", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.securityservice.CMSMessageSignatureService"),"pki://*", "authenticate", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.media.control.RecordControl"),"*", "record", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.media.control.VideoControl.getSnapshot"),"*", "snapshot", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.rtsp"),"*", null, null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.PropertyPermission"),"mobinfo.imsi","read",null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.PropertyPermission"),"mobinfo.msisdn","read",null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.PropertyPermission"),"mobinfo.publicinfo","read",null),
    };
    static final SecurityPolicyPermission[] OPERATOR_PERMS =
    {
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.HttpProtocolPermission"),"http://*", null, null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.HttpsProtocolPermission"),"https://*", null, null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.DatagramProtocolPermission"),"datagram://*", null, null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.DatagramProtocolPermission"),"datagram://", null, null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.SocketProtocolPermission"),"socket://*", null, null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.SocketProtocolPermission"),"socket://", null, null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.SSLProtocolPermission"),"ssl://*", null, null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.CommProtocolPermission"),"comm:*", null, null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.FileProtocolPermission"),"*", "read", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.FileProtocolPermission"),"*", "write", null),
        new SecurityPolicyPermission(getPermissionName("java.util.PropertyPermission"),"microedition.*","read",null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.sms"), "sms://*" , "open", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.sms"), "sms://*" , "send", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.sms"), "sms://*" , "receive", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.mms"), "mms://*" , "open", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.mms"), "mms://*" , "send", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.mms"), "mms://*" , "receive", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.cbs"), "cbs://*" , "open", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.cbs"), "cbs://*" , "receive", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.ContactList"), "pim://*" , "read_contacts", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.EventList"), "pim://*" , "read_events", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.ToDoList"), "pim://*" , "read_todos", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.ContactList"), "pim://*" , "write_contacts", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.EventList"), "pim://*" , "write_events", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.ToDoList"), "pim://*" , "write_todos", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.PushRegistryPermission"),"*","staticregistration",null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.PushRegistryPermission"),"*","dynamicregistration",null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.PushRegistryPermission"),"*","autoinvocation",null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.obex.server"),"*", "server", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.obex.client"),"*", "client", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.bluetooth.server"),"*", "server", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.bluetooth.client"),"*", "client", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"location://*", "position", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"location://*", "orientation", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"landmarks://*", "read", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"landmarks://*", "write", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"landmarks://*", "category", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"landmarks://*", "management", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"proximity://*", "registerlistener", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.apdu.aid"),"apdu://*", "aid", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.apdu.sat"),"apdu://*", "sat", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.securityservice.CMSMessageSignatureService"),"pki://*", "authenticate", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.media.control.RecordControl"),"*", "record", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.media.control.VideoControl.getSnapshot"),"*", "snapshot", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.rtsp"),"*", null, null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.PropertyPermission"),"mobinfo.imsi","read",null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.PropertyPermission"),"mobinfo.msisdn","read",null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.PropertyPermission"),"mobinfo.publicinfo","read",null),
    };
    static final SecurityPolicyPermission[] IDENTIFIED_THIRD_PARTY_PERMS =
    {
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.HttpProtocolPermission"),"http://*", null,
        new SecurityPolicyPermissionSettings("Net Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
        new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.HttpsProtocolPermission"),"https://*", null,
                                     new SecurityPolicyPermissionSettings("Net Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.DatagramProtocolPermission"),"datagram://*", null,
                                     new SecurityPolicyPermissionSettings("Net Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.DatagramProtocolPermission"),"datagram://", null,
                                     new SecurityPolicyPermissionSettings("Net Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.SocketProtocolPermission"),"socket://*", null,
                                     new SecurityPolicyPermissionSettings("Net Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.SocketProtocolPermission"),"socket://", null,
                                     new SecurityPolicyPermissionSettings("Net Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.SSLProtocolPermission"),"ssl://*", null,
                                     new SecurityPolicyPermissionSettings("Net Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.CommProtocolPermission"),"comm:*", null,
                                     new SecurityPolicyPermissionSettings("Local Connectivity",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.obex.server"),"*", "server",
                                     new SecurityPolicyPermissionSettings("Local Connectivity",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.obex.client"),"*", "client",
                                     new SecurityPolicyPermissionSettings("Local Connectivity",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.bluetooth.server"),"*", "server",
                                     new SecurityPolicyPermissionSettings("Local Connectivity",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.bluetooth.client"),"*", "client",
                                     new SecurityPolicyPermissionSettings("Local Connectivity",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.FileProtocolPermission"),"*", "read",
                                     new SecurityPolicyPermissionSettings("Read User Data Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.FileProtocolPermission"),"*", "write",
                                     new SecurityPolicyPermissionSettings("Write User Data Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.sms"), "sms://*" , "open", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.sms"), "sms://*" , "send",
                                     new SecurityPolicyPermissionSettings("Messaging",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.sms"), "sms://*" , "receive", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.mms"), "mms://*" , "open", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.mms"), "mms://*" , "send",
                                     new SecurityPolicyPermissionSettings("Messaging",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.mms"), "mms://*" , "receive", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.cbs"), "cbs://*" , "open", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.cbs"), "cbs://*" , "receive", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.ContactList"), "pim://*" , "read_contacts",
                                     new SecurityPolicyPermissionSettings("Read User Data Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.EventList"), "pim://*" , "read_events",
                                     new SecurityPolicyPermissionSettings("Read User Data Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.ToDoList"), "pim://*" , "read_todos",
                                     new SecurityPolicyPermissionSettings("Read User Data Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.ContactList"), "pim://*" , "write_contacts",
                                     new SecurityPolicyPermissionSettings("Write User Data Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.EventList"), "pim://*" , "write_events",
                                     new SecurityPolicyPermissionSettings("Write User Data Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.ToDoList"), "pim://*" , "write_todos",
                                     new SecurityPolicyPermissionSettings("Write User Data Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("java.util.PropertyPermission"),"microedition.*","read",null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.PushRegistryPermission"),"*", "staticregistration",
                                     new SecurityPolicyPermissionSettings("Application Auto Invocation",UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.PushRegistryPermission"),"*", "dynamicregistration",
                                     new SecurityPolicyPermissionSettings("Application Auto Invocation",UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.PushRegistryPermission"),"*", "autoinvocation",
                                     new SecurityPolicyPermissionSettings("Application Auto Invocation",UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"location://*", "position",
                                     new SecurityPolicyPermissionSettings("Location",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"location://*", "orientation",
                                     new SecurityPolicyPermissionSettings("Location",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"proximity://*", "registerlistener",
                                     new SecurityPolicyPermissionSettings("Location",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"landmarks://*", "read",
                                     new SecurityPolicyPermissionSettings("Landmark",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"landmarks://*", "write",
                                     new SecurityPolicyPermissionSettings("Landmark",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"landmarks://*", "category",
                                     new SecurityPolicyPermissionSettings("Landmark",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"landmarks://*", "management",
                                     new SecurityPolicyPermissionSettings("Landmark",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.apdu.aid"),"apdu://*", "aid",
                                     new SecurityPolicyPermissionSettings("Smart Card Communication",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.securityservice.CMSMessageSignatureService"),"pki://*", "authenticate",
                                     new SecurityPolicyPermissionSettings("Authentication",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.media.control.RecordControl"),"*", "record",
                                     new SecurityPolicyPermissionSettings("Multimedia Recording",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.media.control.VideoControl.getSnapshot"),"*", "snapshot",
                                     new SecurityPolicyPermissionSettings("Multimedia Recording",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.rtsp"),"*", null,
                                     new SecurityPolicyPermissionSettings("Net Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.PropertyPermission"),"mobinfo.publicinfo","read",null),
    };
    static final SecurityPolicyPermission[] UNIDENTIFIED_THIRD_PARTY_PERMS =
    {
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.HttpProtocolPermission"),"http://*", null,
        new SecurityPolicyPermissionSettings("Net Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
        new int[]{UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.HttpsProtocolPermission"),"https://*", null,
                                     new SecurityPolicyPermissionSettings("Net Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.DatagramProtocolPermission"),"datagram://*", null,
                                     new SecurityPolicyPermissionSettings("Net Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.DatagramProtocolPermission"),"datagram://", null,
                                     new SecurityPolicyPermissionSettings("Net Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.SocketProtocolPermission"),"socket://*", null,
                                     new SecurityPolicyPermissionSettings("Net Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.SocketProtocolPermission"),"socket://", null,
                                     new SecurityPolicyPermissionSettings("Net Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.SSLProtocolPermission"),"ssl://*", null,
                                     new SecurityPolicyPermissionSettings("Net Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.CommProtocolPermission"),"comm:*", null,
                                     new SecurityPolicyPermissionSettings("Local Connectivity",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.obex.server"),"*", "server",
                                     new SecurityPolicyPermissionSettings("Local Connectivity",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.obex.client"),"*", "client",
                                     new SecurityPolicyPermissionSettings("Local Connectivity",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.bluetooth.server"),"*", "server",
                                     new SecurityPolicyPermissionSettings("Local Connectivity",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.bluetooth.client"),"*", "client",
                                     new SecurityPolicyPermissionSettings("Local Connectivity",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.FileProtocolPermission"),"*", "read",
                                     new SecurityPolicyPermissionSettings("Read User Data Access",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.FileProtocolPermission"),"*", "write",
                                     new SecurityPolicyPermissionSettings("Write User Data Access",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.sms"), "sms://*" , "open", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.sms"), "sms://*" , "send",
                                     new SecurityPolicyPermissionSettings("Messaging",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.sms"), "sms://*" , "receive", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.mms"), "mms://*" , "open", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.mms"), "mms://*" , "send",
                                     new SecurityPolicyPermissionSettings("Messaging",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.mms"), "mms://*" , "receive", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.cbs"), "cbs://*" , "open", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.cbs"), "cbs://*" , "receive", null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.ContactList"), "pim://*" , "read_contacts",
                                     new SecurityPolicyPermissionSettings("Read User Data Access",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.EventList"), "pim://*" , "read_events",
                                     new SecurityPolicyPermissionSettings("Read User Data Access",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.ToDoList"), "pim://*" , "read_todos",
                                     new SecurityPolicyPermissionSettings("Read User Data Access",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.ContactList"), "pim://*" , "write_contacts",
                                     new SecurityPolicyPermissionSettings("Write User Data Access",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.EventList"), "pim://*" , "write_events",
                                     new SecurityPolicyPermissionSettings("Write User Data Access",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.pim.ToDoList"), "pim://*" , "write_todos",
                                     new SecurityPolicyPermissionSettings("Write User Data Access",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("java.util.PropertyPermission"),"microedition.*","read",null),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.PushRegistryPermission"),"*", "staticregistration",
                                     new SecurityPolicyPermissionSettings("Application Auto Invocation",UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.PushRegistryPermission"),"*", "dynamicregistration",
                                     new SecurityPolicyPermissionSettings("Application Auto Invocation",UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.PushRegistryPermission"),"*", "autoinvocation",
                                     new SecurityPolicyPermissionSettings("Application Auto Invocation",UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"location://*", "position",
                                     new SecurityPolicyPermissionSettings("Location",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"location://*", "orientation",
                                     new SecurityPolicyPermissionSettings("Location",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"proximity://*", "registerlistener",
                                     new SecurityPolicyPermissionSettings("Location",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"landmarks://*", "read",
                                     new SecurityPolicyPermissionSettings("Landmark",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"landmarks://*", "write",
                                     new SecurityPolicyPermissionSettings("Landmark",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"landmarks://*", "category",
                                     new SecurityPolicyPermissionSettings("Landmark",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.location.LocationPermission"),"landmarks://*", "management",
                                     new SecurityPolicyPermissionSettings("Landmark",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.media.control.RecordControl"),"*", "record",
                                     new SecurityPolicyPermissionSettings("Multimedia Recording",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.media.control.VideoControl.getSnapshot"),"*", "snapshot",
                                     new SecurityPolicyPermissionSettings("Multimedia Recording",UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.io.Connector.rtsp"),"*", null,
                                     new SecurityPolicyPermissionSettings("Net Access",UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                          new int[]{UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
        new SecurityPolicyPermission(getPermissionName("javax.microedition.PropertyPermission"),"mobinfo.publicinfo","read",null),
    };

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new SecurityPolicyModuleTests("testDefaultPolicies", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SecurityPolicyModuleTests)tc).testDefaultPolicies();
            }
        }));
        suite.addTest(new SecurityPolicyModuleTests("testMisc", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SecurityPolicyModuleTests)tc).testMisc();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public SecurityPolicyModuleTests()
    {
    }

    public SecurityPolicyModuleTests(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    // End j2meunit test framework setup

    protected void setUp()
    {
    }

    protected void tearDown()
    {
    }

    // tests for the default policies from the system
    protected void testDefaultPolicies()
    {
        SecurityPolicyPermission[] perms = (SecurityPolicyPermission[])SecurityPolicyModule.getInstance().getPermissions(SecurityPolicyModule.MANUFACTURER_DOMAIN);
        assertTrue(perms != null && checkPermissions(MANUFACTURER_PERMS, perms, getExtPermissions(ApplicationInfo.MANUFACTURER_DOMAIN)));
        perms = (SecurityPolicyPermission[])SecurityPolicyModule.getInstance().getPermissions(SecurityPolicyModule.OPERATOR_DOMAIN);
        assertTrue(perms != null && checkPermissions(OPERATOR_PERMS, perms, getExtPermissions(ApplicationInfo.OPERATOR_DOMAIN)));
        perms = (SecurityPolicyPermission[])SecurityPolicyModule.getInstance().getPermissions(SecurityPolicyModule.IDENTIFIED_THIRD_PARTY_DOMAIN);
        assertTrue(perms != null && checkPermissions(IDENTIFIED_THIRD_PARTY_PERMS, perms, getExtPermissions(ApplicationInfo.IDENTIFIED_THIRD_PARTY_DOMAIN)));
        perms = (SecurityPolicyPermission[])SecurityPolicyModule.getInstance().getPermissions(SecurityPolicyModule.UNIDENTIFIED_THIRD_PARTY_DOMAIN);
        assertTrue(perms != null && checkPermissions(UNIDENTIFIED_THIRD_PARTY_PERMS, perms, getExtPermissions(ApplicationInfo.UNIDENTIFIED_THIRD_PARTY_DOMAIN)));
    }

    protected void testMisc()
    {
        // check policy for null protection domain
        PolicyBasedPermission[] perms  = (SecurityPolicyPermission[])SecurityPolicyModule.getInstance().getPermissions(null);
        assertTrue(perms == null);
        // check the policies for unknown protection domain
        perms  = (SecurityPolicyPermission[])SecurityPolicyModule.getInstance().getPermissions("blahblah");
        assertTrue(perms == null);
    }
    private SecurityPolicyPermission[] getExtPermissions(String domain)
    {
        if (domain.equalsIgnoreCase(ApplicationInfo.IDENTIFIED_THIRD_PARTY_DOMAIN))
        {
            return new SecurityPolicyPermission[]
                   {
                       new SecurityPolicyPermission("com.nokia.ext1.public.Ext1Perm", "ext1.target1", "ext1.action1",new SecurityPolicyPermissionSettings("Ext1",UserSecuritySettings.BLANKET_INTERACTION_MODE, new int[]{UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.BLANKET_INTERACTION_MODE})),
                       new SecurityPolicyPermission("com.nokia.ext2.internal.Ext2Perm", "ext2.target1", "ext2.action1",new SecurityPolicyPermissionSettings("Ext1",UserSecuritySettings.BLANKET_INTERACTION_MODE, new int[]{UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.BLANKET_INTERACTION_MODE})),
                       new SecurityPolicyPermission("com.nokia.ext2.internal.Ext2Perm", "ext2.target2", "ext2.action2",new SecurityPolicyPermissionSettings("Ext2",UserSecuritySettings.ONESHOT_INTERACTION_MODE, new int[]{UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE})),
                       new SecurityPolicyPermission("com.nokia.ext2.internal.Ext2Perm", "ext2.target3", "ext2.action3", null),
                       new SecurityPolicyPermission("com.nokia.ext2.internal.Ext2Perm", "ext2.target4", "ext2.action4", null),
                       new SecurityPolicyPermission("com.nokia.ext2.internal.Ext2Perm", "ext2.target5", "ext2.action5", new SecurityPolicyPermissionSettings("Messaging",UserSecuritySettings.SESSION_INTERACTION_MODE, new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE,UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.SESSION_INTERACTION_MODE})),
                   };
        }
        else if (domain.equalsIgnoreCase(ApplicationInfo.UNIDENTIFIED_THIRD_PARTY_DOMAIN))
        {
            return new SecurityPolicyPermission[]
                   {
                       new SecurityPolicyPermission("com.nokia.ext1.internal.Ext1Perm", "ext1.target1", "ext1.action1",new SecurityPolicyPermissionSettings("Ext1",UserSecuritySettings.ONESHOT_INTERACTION_MODE, new int[]{UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.BLANKET_INTERACTION_MODE, UserSecuritySettings.ONESHOT_INTERACTION_MODE, UserSecuritySettings.SESSION_INTERACTION_MODE})),
                       new SecurityPolicyPermission("com.nokia.mj.impl.security.midp.authorization.HttpProtocolPermissionImpl", "http://", null,new SecurityPolicyPermissionSettings("Ext1",UserSecuritySettings.ONESHOT_INTERACTION_MODE, new int[]{UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.BLANKET_INTERACTION_MODE, UserSecuritySettings.ONESHOT_INTERACTION_MODE, UserSecuritySettings.SESSION_INTERACTION_MODE})),
                       new SecurityPolicyPermission("com.nokia.mj.impl.security.midp.authorization.FileProtocolPermissionImpl", "PUBLIC_DIRS", "read",new SecurityPolicyPermissionSettings("Net Access",UserSecuritySettings.SESSION_INTERACTION_MODE, new int[]{UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE, UserSecuritySettings.SESSION_INTERACTION_MODE})),
                       new SecurityPolicyPermission("com.nokia.ext1.internal.Ext1Perm", "ext1.target2", "ext1.action2",new SecurityPolicyPermissionSettings("Net Access",UserSecuritySettings.SESSION_INTERACTION_MODE, new int[]{UserSecuritySettings.NO_INTERACTION_MODE,UserSecuritySettings.ONESHOT_INTERACTION_MODE, UserSecuritySettings.SESSION_INTERACTION_MODE})),
                       new SecurityPolicyPermission("com.nokia.ext1.internal.Ext1Perm", "ext1.target3", "ext1.action3", null)
                   };
        }
        return new SecurityPolicyPermission[0];
    }
    private boolean checkPermissions(SecurityPolicyPermission[] perms1, SecurityPolicyPermission[] perms2, SecurityPolicyPermission[] perms3)
    {
        if (perms2.length != (perms1.length + perms3.length))
        {
            return false;
        }
        SecurityPolicyPermission[] combinedPerms = new SecurityPolicyPermission[perms1.length + perms3.length];
        int i;
        for (i=0; i<perms1.length; i++)
        {
            combinedPerms[i] = perms1[i];
        }
        for (int j=0; j<perms3.length; j++)
        {
            combinedPerms[i + j] = perms3[j];
        }
        boolean[] permsChecked = new boolean[perms2.length];
        initBoolArray(permsChecked);
        for (i=0; i<perms2.length; i++)
        {
            for (int j=0; j<combinedPerms.length; j++)
            {
                if (perms2[i].equals(combinedPerms[j]))
                {
                    if (permsChecked[i] == true)
                    {
                        return false;
                    }
                    permsChecked[i] = true;
                    break;
                }
            }
        }
        return checkBoolArray(permsChecked);
    }

    private void initBoolArray(boolean[] boolArray)
    {
        for (int i=0; i<boolArray.length; i++)
        {
            boolArray[i] = false;
        }
    }

    private boolean checkBoolArray(boolean[] boolArray)
    {
        for (int i=0; i<boolArray.length; i++)
        {
            if (boolArray[i] == false)
            {
                return false;
            }
        }
        return true;
    }

    private static String getPermissionName(String pName)
    {
        MIDPPermission mappedPerm = PermissionMappingTable.get(pName);
        if (mappedPerm != null)
        {
            return mappedPerm.getName();
        }
        return pName;
    }

}
