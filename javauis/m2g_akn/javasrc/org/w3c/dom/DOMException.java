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

package org.w3c.dom;

/**
 *
 */
public class DOMException extends RuntimeException
{

    /**
     * If a node is used in a different document than the one that created it
     * (that doesn't support it).
     */
    public static final short WRONG_DOCUMENT_ERR          = 4;

    /**
     * If index or size is negative, or greater than the allowed value.
     */
    public static final short INDEX_SIZE_ERR = 1;

    /**
     * If any Node is inserted somewhere it doesn't belong.
     */
    public static final short HIERARCHY_REQUEST_ERR = 3;

    /**
     * If an attempt is made to modify an object where modifications are not allowed.
     */
    public static final short NO_MODIFICATION_ALLOWED_ERR = 7;

    /**
     * If an attempt is made to reference a {@link org.w3c.dom.Node Node} in a context where it does not exist. See {@link org.w3c.dom.Node#insertBefore insertBefore} for example.
     */
    public static final short NOT_FOUND_ERR = 8;

    /**
     * If the implementation does not support the requested type of object or operation.
     */
    public static final short NOT_SUPPORTED_ERR = 9;

    /**
     * If an attempt is made to use an object that is not, or is no longer, usable.
     */
    public static final short INVALID_STATE_ERR = 11;

    /**
     * If an attempt is made to modify the type of the underlying object.
     */
    public static final short INVALID_MODIFICATION_ERR = 13;

    /**
     * If a parameter or an operation is not supported by the underlying object.
     */
    public static final short INVALID_ACCESS_ERR = 15;

    /**
     * If the type of an object is incompatible with the expected type of the parameter associated to the object.
     */
    public static final short TYPE_MISMATCH_ERR = 17;

    /**
     *
     * The member variable to store exception's code, like INVALID_ACCESS_ERR.
     */
    public short code;

    /**
     * Constructs a DOMException with a detailed message.
     *
     * @param code the exception's error code.
     * @param message the exception's descriptive message.
     */
    public DOMException(final short code,
                        final String message)
    {
        super(message);
        this.code = code;
    }


}
