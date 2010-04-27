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
 * The Document interface represents an XML Document.
 *
 * <p>This interface is a subset of the Document interface defined
 * in the <a href="http://www.w3.org/TR/2004/REC-DOM-Level-3-Core-20040407/idl-definitions.html">
 * DOM Level 3 Core</a>.</p>
 *
 *
 * <p>Note the behavior of the following methods from the
 * <code>Node</code> interface when called on a <code>Document</code>
 * object:
 *
 * <ul>
 * <li>getParentNode returns <code>null</code></li>
 * <li>appendChild throws <code>HIERARCHY_REQUEST_ERR</code></li>
 * <li>insertBefore throws <code>HIERARCHY_REQUEST_ERR</code></li>
 * <li>removeChild throws <code>NOT_SUPPORTED_ERR</code></li>
 * </ul>
 * </p>
 *
 */
public interface Document extends Node
{

    /**
     * Create a new <code>Element</code> based on the specified
     * (qualified) SVG tag name. This JSR does not require multiple
     * namespaces and may throw a <code>DOMException</code> with a
     * code of <code>NOT_SUPPORTED_ERR</code> if the URI is not the
     * SVG namespace URI, or if the specified name is not a valid SVG
     * Tiny element name. Only the following elements must be supported:
     * &lt;rect&gt;, &lt;circle&gt;, &lt;ellipse&gt;, &lt;line&gt;,
     * &lt;path&gt; &lt;use&gt; &lt;image&gt; &lt;text&gt;,
     * &lt;a&gt; and &lt;g&gt;.
     *
     * @param namespaceURI the namespace uri for the newly created
     * element. This should always be the SVG namespace URI "http://www.w3.org/2000/svg".
     * @param qualifiedName the qualified name for the newly created
     * element (For example: "rect", to create a &lt;rect&gt; element)
     *
     * @return the newly created SVG Element.
     *
     * @throws DOMException NOT_SUPPORTED_ERR if the type of element is
     * not supported by the implementation. JSR 226 only requires creation
     * support for some of the SVG namespace elements and only for a limited
     * number of local names in that namespace (see above documentation).Therefore, in
     * a conformant JSR 226 implementation, trying to create elements with a namespace
     * URIs other than the SVG namespace URI and with a qualified name not in the list
     * of required qualified names may result in this exception being thrown.
     * @throws NullPointerException if <code>namespaceURI</code> or
     * <code>qualifiedName</code> is null.
     *
     */
    public Element createElementNS(String namespaceURI, String qualifiedName)
    throws DOMException;

    /**
     * Return a child element of this document Node which corresponds to the top-most
     * tag in XML file. For SVG files it must be <code>SVGSVGElement</code>, but return
     * type is Element for DOM Core compatibility and to allow for future extensions.
     *
     * @return the root <code>Element</code> associated with this
     * document.
     * @throws SecurityException if the application does not have the necessary privilege rights
     * to access this (SVG) content.
     */

    public Element getDocumentElement();

    /**
     * Return the <code>Element</code> in the current document with
     * the given unique ID. If no such element exists, this returns null.
     *
     * @param id the ID of the object to be retrieved.
     * @return the Element that matches with the given ID or
     * <code>null</code> if the ID is not present.
     *
     * @throws NullPointerException if id is null
     * @throws SecurityException if the application does not have the necessary privilege rights
     * to access this (SVG) content.
     */

    public Element getElementById(String id);

}

