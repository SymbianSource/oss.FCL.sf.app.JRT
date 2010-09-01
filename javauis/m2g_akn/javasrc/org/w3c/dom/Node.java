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
 * The Node interface describes generic nodes in an SVG document tree.
 *
 * <p>This interface is a subset of the Node interface defined
 * in the <a href="http://www.w3.org/TR/2004/REC-DOM-Level-3-Core-20040407/idl-definitions.html">
 * DOM Level 3 Core</a>.</p>
 *
 */

public interface Node
{


    /**
     * Returns the namespace URI of the Node.
     *
     * @return the namespace URI of the Node.
     * @throws SecurityException if the application does not have the necessary privilege rights
     * to access this (SVG) content.
     */
    public String getNamespaceURI();

    /**
     * Returns the local part of the qualified name of this node. If the node is of type SVGElement, this returns the tag name without a prefix.
     * But, if the node is of type Document then <code>null</code> is returned.
     * @return the local part of the qualified name of this node.
     * @throws SecurityException if the application does not have the necessary privilege rights
     * to access this (SVG) content.
     */

    public String getLocalName();

    /**
     * Returns the parent <code>Node</code> of this <code>Node</code>.
     *
     * @return the parent node or null if there is no parent (i.e. if a node has
     * just been created and not yet added to the tree, or if it has been removed
     * from the tree, this is null).
     * @throws SecurityException if the application does not have the necessary privilege rights
     * to access this (SVG) content.
     */

    public Node getParentNode();

    /**
     * Appends a child to this <code>Node</code>.
     *
     * @param newChild the <code>Node</code> to be appended to this
     * <code>Node</code>. This is equivalent to insertBefore(newChild,null)
     * @return the added <code>Node</code>.
     *
     * @throws DOMException with error code HIERARCHY_REQUEST_ERR: Raised if this node is of a type that does not allow children
     * of the type of the newChild node, or if the node to append is one of this node's ancestors or this node itself,
     * or if this node is of type Document and the DOM application attempts to append a second Element node.
     * @throws DOMException with error code WRONG_DOCUMENT_ERR: Raised if newChild was created from a different document than
     * the one that created this node.
     * @throws DOMException with error code NOT_SUPPORTED_ERR: if the newChild node is a child of the Document node or if the
     * child is of a type that cannot be created with createElementNS.
     * @throws DOMException with error code INVALID_STATE_ERR: if the newChild node would cause the document to go into
     * error, for ex: when the newChild contains a &lt;use&gt; element with an invalid xlink:href attribute.
     * @throws NullPointerException if <code>newChild</code> is null.
     * @throws SecurityException if the application does not have the necessary privilege rights
     * to access this (SVG) content.
     */

    public Node appendChild(Node newChild) throws DOMException;

    /**
     * Removes the specified child associated with this Node. Elements that have ids cannot be
     * removed from the tree.
     *
     * @param oldChild the <code>Node</code> that is to be removed.
     * @return the node removed.
     * @throws DOMException with error code NOT_FOUND_ERR: Raised if oldChild is not a child of this node.
     * @throws DOMException with error code NOT_SUPPORTED_ERR: if this node is of type Document or if the
     * child, or any of its descendants, is of a type that cannot be created with createElementNS.
     * @throws DOMException with error code INVALID_ACCESS_ERR: if the element being removed or one of its decendants
     * have non-null id.
     * @throws NullPointerException if <code>oldChild</code> is null.
     * @throws SecurityException if the application does not have the necessary privilege rights
     * to access this (SVG) content.
     */

    public Node removeChild(Node oldChild) throws DOMException;

    /**
    * Inserts newChild before refChild in the child list for this node. If refChild is null,
    * newChild is inserted at the end of the list. If the newChild is already part of the
    * tree, it is first removed.
    *
    *
    * @param newChild the child to add
    * @param refChild the child before which the new child should be added.
    * @return the node being inserted.
    * @throws DOMException with error code HIERARCHY_REQUEST_ERR: if this node is of a type that does not allow children
    *   of the type of the newChild node, or if the node to append is one of this node's ancestors or this node itself,
    *   or if this node is of type Document and the DOM application attempts to append a second Element node.
    * @throws DOMException with error code WRONG_DOCUMENT_ERR: Raised if newChild was created from a different document than
    * the one that created this node.
    * @throws DOMException with error code NOT_FOUND_ERR: raised if refChild is not a child of this node.
    * @throws DOMException with error code NOT_SUPPORTED_ERR: if the newChild node is a child of the Document node or if the
    * child is of a type that cannot be created with createElementNS.
    * @throws DOMException with error code INVALID_STATE_ERR: if the newChild node would cause the document to go into
    * error, for ex: when the newChild contains a &lt;use&gt; element with an invalid xlink:href attribute.
    * @throws NullPointerException if <code>newChild</code> is null.
    * @throws SecurityException if the application does not have the necessary privilege rights
    * to access this (SVG) content.
    */

    public Node insertBefore(Node newChild, Node refChild) throws DOMException;


}
