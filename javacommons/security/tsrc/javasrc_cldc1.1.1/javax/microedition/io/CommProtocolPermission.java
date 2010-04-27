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
 * This class represents access rights to connections via the "comm" protocol.
 * A CommProtocolPermission consists of a URI string but no actions list.
 * <p>
 * The URI string specifies a logical serial port connection and optional
 * parameters. It takes the following general form:
 * <p>
 * <code>comm:{port identifier}[{optional parameters}]</code>
 * <p>
 * The detailed syntax for the <code>CommProtocolPermission</code> URI is
 * provided by this BNF. In addition, an asterisk may terminate the URI
 * string within the <code>{port identifier}</code> field to indicate a
 * wildcard * match of 0 or more characters. Valid examples include
 * "comm:*" and "comm:port*".
 *
 * @see Connector.open(java.lang.String)
 * @see "javax.microedition.io.CommConnection" in MIDP 3.0 Specification
 *
 * @author Nokia Corporation
 * @version 1.0
 */
public final class CommProtocolPermission
        extends GCFPermission
{
    /**
     * Creates a new <code>CommProtocolPermission</code> with the specified URI
     * as its name. The URI string must conform to the specification given above,
     * and is normalized to facilitate subsequent comparisons.
     *
     * @param uri the URI string
     * @throws IllegalArgumentException - if uri is malformed.
     * @throws NullPointerException - if uri is null.
     */
    public CommProtocolPermission(String uri)
    {
        super(uri);
    }

    /**
     * Checks if this CommProtocolPermission object "implies" the specified
     * permission.
     * <p>
     * More specifically, this method returns true if:
     * <ul>
     * <li>p is an instanceof CommProtocolPermission</li>
     * <li>p's {port identifier} string equals or is implied by this object's
     * {port identifier} string. For example:<li>"comm:port*" implies
     * "comm:port1"</li>
     * <li>"comm:port1" implies "comm:port2" if both port identifiers are
     * mapped to the same logical port.</li></li>
     * <li>for each of the parameters permitted in the {optional parameters}
     * string, either:<li>the parameter value for this object and for p are
     * identical, or</li><li>this object does not define a value for the
     * parameter.</li><p>For example:<li>"comm:port1;baudrate=300;autorts=on"
     * implies "comm:port1;autorts=on;baudrate=300"</li><li>"comm:port*"
     * implies "comm:port1;baudrate=300"</li></li>
     * </ul>
     * @param p the permission to check against
     * @return true if the specified permission is implied by this object,
     *         false if not.
     */
    public boolean implies(Permission p)
    {
        if (p instanceof CommProtocolPermission)
        {
            return true;
        }
        return false;
    }

    /**
     * Checks two CommProtocolPermission objects for equality. Checks that
     * obj is a PropertyPermission, and has the same name and actions as
     * this object.
     *
     * @param obj the object we are testing for equality with this object.
     * @return true if obj is a CommProtocolPermission and has the same URI
     *         string as this CommProtocolPermission object.
     */
    public boolean equals(Object obj)
    {
        return true;
    }

    /**
     * Returns the hash code value for this object
     *
     * @return a hash code value for this object.
     */
    public int hashCode()
    {
        return 0;
    }

    /**
     * Returns the canonical string representation of the actions, which currently
     * is the empty string "", since there are no actions defined for
     * CommProtocolPermission.
     *
     * @return the empty string "".
     */
    public String getActions()
    {
        return "";
    }

    /**
     * Returns a new PermissionCollection for storing CommProtocolPermission
     * objects.
     * <p>
     * CommProtocolPermission objects must be stored in a manner that allows
     * them to be inserted into the collection in any order, but that also
     * enables the PermissionCollection implies method to be implemented in
     * an efficient (and consistent) manner.
     *
     * @return a new PermissionCollection suitable for storing
     *         CommProtocolPermission objects.
     */
    public PermissionCollection newPermissionCollection()
    {
        return null;
    }
}
