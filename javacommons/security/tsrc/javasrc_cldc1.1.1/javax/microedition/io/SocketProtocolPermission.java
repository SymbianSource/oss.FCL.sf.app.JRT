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

package javax.microedition.io;

import java.security.Permission;
import java.security.PermissionCollection;

/**
 * This class represents access rights to connections via the "socket"
 * protocol. A SocketProtocolPermission consists of a URI string but no
 * actions list.
 * <p>
 * The URI string specifies a socket stream connection. It takes the following
 * general form:
 * <p>
 * socket://{host}:{portspec} | socket://[:{portspec}]
 * <p>
 * The exact syntax for the SocketProtocolPermission URI is provided by this
 * BNF. The value of the {host} field must be a symbolic hostname, a literal
 * IPv4 address or a literal IPv6 address, as specified by RFC 3986. The
 * {host} field is omitted to indicate a "server mode" connection. Server-mode
 * URIs may also omit the {portspec} field to indicate a system-assigned port
 * number. In such a case, the SocketProtocolPermission is normalized to the
 * equivalent URI "socket://:1024-65535".
 * <p>
 * If the {host} string is a DNS name, an asterisk may appear in the leftmost
 * position to indicate a match of 1 or more entire domain labels. Partial
 * domain label matches are not permitted, therefore "*.sun.com" is valid,
 * but "*sun.com" is not. An asterisk by itself matches all hosts in
 * "client-mode" connections;
 * <p>
 * The {portspec} string takes the following form:
 * <p>
 * portnumber | -portnumber | portnumber-[portnumber] | "*"
 * <p>
 * A {portspec} specification of the form "N-" (where N is a port number)
 * signifies all ports numbered N and above, while a specification of the
 * form "-N" indicates all ports numbered N and below. A single asterisk may
 * be used in place of the {portspec} field to indicate all ports. Therefore,
 * the URI "socket://:*" matches server-mode socket connections to all ports,
 * and the URI "socket://*:*" matches client-mode socket connections to all
 * hosts on all ports.
 *
 * @see Connector.open(java.lang.String)
 * @see "javax.microedition.io.SocketConnection" in MIDP 3.0 Specification
 *
 * @author Nokia Corporation
 * @version 1.0
 */
public final class SocketProtocolPermission
        extends GCFPermission
{
    /**
     * Creates a new SocketProtocolPermission with the specified URI as its
     * name. The URI string must conform to the specification given above,
     * and is normalized  to facilitate subsequent comparisons.
     *
     * @param uri the URI string.
     * @throws IllegalArgumentException if uri is malformed.
     * @throws NullPointerException if uri is null.
     */
    public SocketProtocolPermission(String uri)
    {
        super(uri);
    }

    /**
     * Checks if this SocketProtocolPermission object "implies" the
     * specified permission.
     * <p>
     * Because the {host} field of the permission URI may indicate an
     * individual host by means of either a symbolic hostname or an IP
     * address, a thorough evaluation of the implies relation between
     * two SocketProtocolPermission objects requires the underlying
     * platform to be able to transform symbolic hostnames into IP
     * addresses (via address lookup) and vice-versa (via name lookup).
     * Either service may be unavailable on a given platform. As a result,
     * the comparison of URI strings from two permissions can only be
     * best-effort. For each SocketProtocolPermission constructed with a
     * URI that contains a {host} field, the implementation will attempt
     * to determine both the fully-qualified domain name (the "canonical
     * name") and the IP address(es) of the host. However, either value
     * may be unknown.
     * <p>
     * To determine whether this object implies another permission, p,
     * this method first ensures that all of the following are true
     * (and returns false if any of them are not):
     * <ul>
     * <li>p is an instanceof SocketProtocolPermission, and</li>
     * <li>p's port range is included in this port range</li>
     * </ul>
     * Then implies checks each of the following, in order, and for each
     * returns true if the stated condition is true:
     * <ul>
     * <li>If both this object and p lack the {host}  field.</li>
     * <li>If this object has a single IP address and one of p's IP addresses
     * is equal to this object's IP address.</li>
     * <li>If this object has a wildcard domain (such as *.sun.com), and p's
     * absolute name (the name with any preceding * removed) ends with this
     * object's absolute name. For example, *.sun.com implies *.eng.sun.com.
     * </li>
     * <li>If this object has multiple IP addresses, and one of this object's
     * IP addresses equals one of p's IP addresses.</li>
     * <li>If this object's canonical name equals p's canonical name.</li>
     * </ul>
     * If none of the above are true, implies returns false.
     *
     * @param p the permission to check against.
     * @return true if the specified permission is implied by this object,
     *        false if not.
     */
    public boolean implies(Permission p)
    {
        if (p instanceof SocketProtocolPermission)
        {
            return true;
        }
        return false;
    }

    /**
     * Checks two SocketProtocolPermission objects for equality.
     *
     * @param obj the object we are testing for equality with this object.
     * @return true if obj is a SocketProtocolPermission and has the same
     *         URI string as this SocketProtocolPermission object.
     */
    public boolean equals(Object obj)
    {
        return true;
    }

    /**
     * Returns the hash code value for this object.
     *
     * @return a hash code value for this object.
     */
    public int hashCode()
    {
        return 0;
    }

    /**
     * Returns the canonical string representation of the actions, which
     * currently is the empty string "", since there are no actions defined
     * for SocketProtocolPermission.
     *
     * @return the empty string "".
     */
    public String getActions()
    {
        return "";
    }

    /**
     * Returns a new PermissionCollection for storing
     * SocketProtocolPermission objects.
     * <p>
     * SocketProtocolPermission objects must be stored in a manner that allows
     * them to be inserted into the collection in any order, but that also
     * enables the PermissionCollection implies method to be implemented in an
     * efficient (and consistent) manner.
     *
     * @return a new PermissionCollection suitable for storing
     *         SocketProtocolPermission objects.
     */
    public PermissionCollection newPermissionCollection()
    {
        return null;
    }
}
