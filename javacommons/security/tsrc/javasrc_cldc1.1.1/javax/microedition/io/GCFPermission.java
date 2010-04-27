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

/**
 * An abstract class that is the superclass of all permissions used with the
 * Generic Connection Framework. A GCFPermission  consists of a URI string
 * representing access rights to a connection based on the specified protocol
 * scheme. The URI must conform to the syntax requirements of RFC 3986  with
 * extensions  to permit wildcard matching.
 * <p>
 * A specification that extends the Generic Connection Framework with a new
 * protocol scheme should define a subclass of GCFPermission to guard access
 * to the protocol.
 *
 * @see Connector.open(java.lang.String)
 *
 * @author Nokia Corporation
 * @version 1.0
 */
public abstract class GCFPermission
        extends Permission
{
    /**
     * Constructs a GCFPermission with the specified URI. The URI must conform
     * to the syntax requirements of RFC 3986  with extensions  to permit
     * wildcard matching. This constructor performs protocol-independent
     * normalization  of the specified URI string; subsequent queries for the
     * URI or portions of the URI will reflect the normalized form.
     *
     * @param uri the URI string.
     * @throws NullPointerException - if uri is null.
     * @throws IllegalArgumentException - if uri is malformed.
     */
    public GCFPermission(String uri)
    {
        super(uri);
    }

    /**
     * Returns the URI of this GCFPermission.
     *
     * @return the URI string, identical to Permission.getName().
     */
    public String getURI()
    {
        return null;
    }

    /**
     * Returns the protocol scheme of this GCFPermission. The protocol scheme
     * is the string preceding the first ':' in the URI.
     *
     * @return the protocol scheme portion of the URI string.
     */
    public String getProtocol()
    {
        return null;
    }
}
