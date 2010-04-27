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
 * This class represents access rights to connections via the "file" protocol.
 * A FileProtocolPermission consists of a URI string indicating a
 * fully-qualified, absolute pathname and a set of actions desired for that
 * pathname.
 * <p>
 * The URI string takes the following general form:
 * <p>
 * file:{pathname} | file://[localhost]{pathname}
 * <p>
 * (The exact syntax is given by RFCs 1738 & 2396.) In addition, a pathname
 * that ends in "/*" indicates all the files and directories contained in
 * that directory. A pathname that ends with "/-" indicates (recursively)
 * all files and subdirectories contained in that directory.
 * <p>
 * The actions to be granted are passed to the constructor in a string
 * containing a list of one or more comma-separated keywords. The possible
 * keywords are "read" and "write". The actions string is converted to
 * lowercase before processing.
 *
 * @see Connector.open(java.lang.String)
 * @see "javax.microedition.io.file.FileConnection" in FileConnection
 *      Optional Package Specification
 *
 * @author Nokia Corporation
 * @version 1.0
 */
public final class FileProtocolPermission
        extends GCFPermission
{
    /**
     * Creates a new FileProtocolPermission with the specified actions.
     * The specified URI becomes the name of the FileProtocolPermission.
     * The URI string must conform to the specification given above, and
     * is normalized  to facilitate subsequent comparisons.
     *
     * @param uri the URI string
     * @param actions comma-separated list of desired actions
     * @throws NullPointerException if uri or actions is null.
     * @throws IllegalArgumentException if uri or actions is malformed.
     */
    public FileProtocolPermission(String uri, String actions)
    {
        super(uri);
    }

    /**
     * Checks if this FileProtocolPermission object "implies" the specified permission.
     * <p>
     * More specifically, this method returns true if:
     * <ul>
     * <li>p is an instanceof FileProtocolPermission,</li>
     * <li>p's actions are a proper subset of this object's actions, and</li>
     * <li>p's pathname is implied by this object's pathname. For example:
     * <li>"/tmp/*" implies "/tmp/foo", since "/tmp/*" encompasses the "/tmp"
     * directory and all files in that directory, including the one named
     * "foo".</li>
     * <li>path1 implies path2 if both strings are mapped to the same
     * filesystem entry. For example, on platforms with case-insensitive
     * filesystems, "/ABC" implies "/abc".</li>
     * </li>
     * </ul>
     *
     * @param p the permission to check against.
     * @return true if the specified permission is implied by this object,
     *        false if not.
     */
    public boolean implies(Permission p)
    {
        if (p instanceof FileProtocolPermission)
        {
            return true;
        }
        return false;
    }

    /**
     * Checks two FileProtocolPermission objects for equality.
     *
     * @param obj the object we are testing for equality with this object.
     * @return true if obj is a FileProtocolPermission, and has the same
     *         URI string and actions as this FileProtocolPermission object.
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
     * Returns the canonical string representation of the actions. If both read
     * and write actions are allowed, this method returns the string
     * "read,write".
     *
     * @return the canonical string representation of the actions.
     */
    public String getActions()
    {
        return null;
    }

    /**
     * Returns a new PermissionCollection for storing FileProtocolPermission
     * objects.
     * <p>
     * FileProtocolPermission objects must be stored in a manner that allows
     * them to be inserted into the collection in any order, but that also
     * enables the PermissionCollection implies method to be implemented in
     * an efficient (and consistent) manner.
     *
     * @return a new PermissionCollection suitable for storing
     *         FileProtocolPermission objects.
     */
    public PermissionCollection newPermissionCollection()
    {
        return null;
    }
}
