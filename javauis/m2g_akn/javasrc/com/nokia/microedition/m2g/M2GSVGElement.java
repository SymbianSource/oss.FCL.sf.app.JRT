/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.microedition.m2g;

import org.w3c.dom.*;
import org.w3c.dom.svg.*;
import org.w3c.dom.events.*;
import com.nokia.mj.impl.rt.legacy.ToolkitInvoker;
import java.io.IOException;
import org.eclipse.swt.widgets.*;
import org.eclipse.swt.widgets.Display;
import com.nokia.mj.impl.rt.legacy.LegacyRtPort;
import com.nokia.mj.impl.utils.Logger;

public class M2GSVGElement extends M2GObject implements SVGElement
{
    //--------------------------------------------------
    // STATIC CONSTANTS
    //--------------------------------------------------
    // Error constants
    static final String TYPE_IS_NULL_ESTR =
        "The type is null.";
    static final String LISTENER_IS_NULL_ESTR =
        "The listener is null.";
    static final String CAPTURE_NOT_SUPPORTED_ESTR =
        "The capture phase is not supported in SVG Tiny.";
    static final String ESWT_PACKAGE =
        "org.eclipse.swt.widgets.Display" ;
    static boolean iIsToolkitAvailable     = false;
    static boolean iStaticUIToolkit        = LCDUI_TOOLKIT;
    /* Optimization: static finals changed to local variables
    static final String TRAIT_IS_NULL_ESTR =
      "The trait is null.";
    static final String TRAIT_NOT_SUPPORTED_ON_ELEM_ESTR =
      "The trait is not supported on this element.";
    static final String TRAIT_NOT_SUPPORTED_IN_NS_ESTR =
      "The trait is not supported in this namespace.";
    static final String GET_FLOAT_MISMATCH_ESTR =
      "Trait's computed value cannot be converted to a float.";
    static final String SET_FLOAT_MISMATCH_ESTR =
      "Trait's value cannot be specified as a float.";
    static final String GET_MATRIX_MISMATCH_ESTR =
      "Trait's computed value cannot be converted to a SVGMatrix.";
    static final String SET_MATRIX_MISMATCH_ESTR =
      "Trait's value cannot be specified as a SVGMatrix.";
    static final String GET_PATH_MISMATCH_ESTR =
      "Trait's computed value cannot be converted to a SVGPath.";
    static final String SET_PATH_MISMATCH_ESTR =
      "Trait's value cannot be specified as a SVGPath.";
    static final String GET_RECT_MISMATCH_ESTR =
      "Trait's computed value cannot be converted to a SVGRect.";
    static final String SET_RECT_MISMATCH_ESTR =
      "Trait's value cannot be specified as a SVGRect.";
    static final String GET_RGB_MISMATCH_ESTR =
      "Trait's computed value cannot be converted to a SVGRGBColor.";
    static final String SET_RGB_MISMATCH_ESTR =
      "Trait's value cannot be specified as a SVGRGBColor.";
    static final String NS_NOT_SUPPORTED_ESTR =
      "The namespace is not supported.";
    static final String GET_TRAIT_NS_STRING_MISMATCH_ESTR =
      "Trait's computed value cannot be converted to a String.";
    static final String SET_TRAIT_NS_STRING_MISMATCH_ESTR =
      "Trait's value cannot be specified as a String.";
    static final String ID_IS_NULL_ESTR =
      "The id is null.";
    static final String EXISTING_ELEM_ID_CHANGE_ESTR =
      "Existing element id cannot be changed.";
    static final String ELEM_ID_EXIST_IN_DOCUMENT_ESTR =
      "Element id already exists in the document.";
    static final String INVALID_INPUT_VALUE_ESTR =
      "The input value is an invalid value for the given trait.";
    static final String VALUE_IS_NULL_ESTR =
      "Value cannot be set to null.";
    static final String SET_READONLY_MISMATCH_ESTR =
      "Attempt to change readonly trait.";
    static final String INVALID_VALUE_ESTR =
      "The value is invalid.";
    static final String INVALID_USE_ELEMENT_ESTR =
      "The <use> element is hooked into the document tree and the the value of xlink:href is set invalid.";
    static final String CANNOT_REMOVE_NODE_ESTR =
      "Cannot remove this type of node.";
    static final String CANNOT_REMOVE_NOT_CHILD_ESTR =
      "Not a child of this node.";
    static final String CANNOT_REMOVE_NON_NULL_ID_ESTR =
      "The element being removed or one of its decendants have non-null id.";
    static final String INSERT_NODE_OF_THAT_TYPE_ESTR =
      "Cannot insert node of that type.";
    static final String APPEND_DOCUMENT_ESTR =
      "Cannot append Document elements.";
    static final String DOCUMENT_HIERARCHY_ESTR =
      "Hierarchy request error in Document.";
    static final String CHILD_IS_WRONG_TYPE_ESTR =
      "Child is wrong type (Document).";
    static final String CHILD_BELONG_TO_DIFFERENT_DOCUMENT_ESTR =
      "Child belongs to different document.";
    static final String CHILD_NOT_FOUND_ESTR =
      "The child to insert before doesn't exist in this current node.";
    static final String INVALID_ELEMENT_ID_VALUE =
      "Invalid element id value.";
    */

    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    protected M2GDocument iDocument;
    protected short iElementTypeId;

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
     * Constructor
     * @param aHandle is native element handle
     * @param aDocument is a reference to a document that owns this element.
     */
    M2GSVGElement(int aElementHandle, M2GDocument aDocument)
    {
        super(aElementHandle);
        iDocument = aDocument;
        doConstruct();
    }


    /**
     * @see org.w3c.dom.events.EventTarget#addEventListener()
     */
    public void addEventListener(String type, EventListener listener, boolean useCapture)
    {
        if (type == null || type.equals(""))
        {
            throw new NullPointerException(TYPE_IS_NULL_ESTR);
        }
        if (listener == null)
        {
            throw new NullPointerException(LISTENER_IS_NULL_ESTR);
        }
        if (useCapture == true)
        {
            throw new DOMException(
                DOMException.NOT_SUPPORTED_ERR,
                CAPTURE_NOT_SUPPORTED_ESTR);
        }
        iDocument.register(this, type, listener);
    }

    /**
     * @see org.w3c.dom.Node#appendChild()
     */
    public Node appendChild(Node newChild)
    {
        // If refChild is null, newChild is inserted at the end of the list
        return insertBefore(newChild, null);  // will inform about DOM changes
    }

    /**
     * Checks attribute validity
     * @param aAttributeTypeName
     * @return attribute's type id
     * @throws DOMException
     */
    protected short check(String aAttributeTypeName) throws DOMException
    {
        short attributeTypeId = M2GSVGConstants.AT_UNDEFINED;
        if (aAttributeTypeName == null || aAttributeTypeName.equals(""))
        {
            Logger.ELOG(Logger.EJavaUI,
                        "check() exception: " + /*SF*/"The trait is null."/*SF*/);
            throw new DOMException(
                DOMException.NOT_SUPPORTED_ERR,
                /*SF*/"The trait is null."/*SF*/);
        }
        // Converts attribute's name
        attributeTypeId = M2GSVGConstants.parseAttributeTypeId(aAttributeTypeName);
        // Checks that element has this attribute
        if (!M2GSVGConstants.checkElementAttribute(attributeTypeId, iElementTypeId))
        {
            Logger.ELOG(Logger.EJavaUI,
                        "check() exception: " + /*SF*/"The trait is not supported on this element."/*SF*/);
            throw new DOMException(
                DOMException.NOT_SUPPORTED_ERR,
                /*SF*/"The trait is not supported on this element."/*SF*/);
        }
        return attributeTypeId;
    }

    /**
     * @see com.nokia.microedition.m2g.M2GObject#doCheckValidity()
     */
    public boolean doCheckValidity()
    {
        try
        {
            if (super.doCheckValidity() &&
                    (iDocument != null) &&
                    (iElementTypeId != M2GSVGConstants.EL_UNDEFINED))
            {
                return true;
            }
        }
        catch (IOException e)
        {
        }
        return false;
    }

    /**
     * @see com.nokia.microedition.m2g.M2GObject#doCleanup()
     */
    protected void doCleanup()
    {
        iDocument = null;
        iElementTypeId = M2GSVGConstants.EL_UNDEFINED;
        resetHandles();
    }

    /**
     * @see com.nokia.microedition.m2g.M2GObject#doConstruct()
     */
    public void doConstruct()
    {
        // No need to register for finalization
        // super.doConstruct();
        iElementTypeId = _getElementTypeId(
                             getNativeEventSourceHandle(),
                             getNativeSVGProxyHandle(), getHandle(), iUIToolkit);
    }

    /**
     * Returns a document
     */
    public M2GDocument getDocument()
    {
        return iDocument;
    }

    /*
     * Retuns element's type id
     * @return  element's type id
     */
    public short getElementTypeId()
    {
        return iElementTypeId;
    }

    /**
     * @see org.w3c.dom.svg.SVGElement#getFirstElementChild()
     */
    public Element getFirstElementChild() throws DOMException
    {
        // Get child element's handle
        int childHandle = _getFirstElementChild(
                              getNativeEventSourceHandle(), getNativeSVGProxyHandle(), getHandle(), iUIToolkit);
        if (M2GObject.checkHandle(childHandle))
        {
            return (Element)M2GSVGElement.buildElement(childHandle, iDocument);
        }
        return null;
    }

    /**
     * @see org.w3c.dom.svg.SVGElement#getFloatTrait()
     */
    public float getFloatTrait(String name) throws DOMException
    {
        // Optimization: if(!getDocument().getConnectionPolicy().getAccessRight())
        if (!iDocument.iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI, "getFloatTrait() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }
        short attributeTypeId = check(name);
        if (!M2GSVGConstants.isGetFloatTraitAllowed(attributeTypeId))
        {
            throw new DOMException(
                DOMException.TYPE_MISMATCH_ERR,
                /*SF*/"Trait's computed value cannot be converted to a float."/*SF*/);
        }
        return _getFloatTrait(
                   getNativeEventSourceHandle(),
                   getNativeSVGProxyHandle(), getHandle(), attributeTypeId, iUIToolkit);
    }

    /**
     * @see org.w3c.dom.svg.SVGElement#getId()
     */
    public String getId()
    {
        return _getStringTrait(
                   getNativeEventSourceHandle(),
                   getNativeSVGProxyHandle(), getHandle(), M2GSVGConstants.AT_ID, iUIToolkit);
    }

    /**
     * @see org.w3c.dom.Node#getLocalName()
     */
    public String getLocalName()
    {
        // Optimization: if(!getDocument().getConnectionPolicy().getAccessRight())
        if (!iDocument.iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "getLocalName() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }
        return M2GSVGConstants.parseElementTypeName(iElementTypeId);
    }

    /**
     * @see org.w3c.dom.svg.SVGElement#getMatrixTrait()
     */
    public SVGMatrix getMatrixTrait(String name)
    {
        // Optimization: if(!getDocument().getConnectionPolicy().getAccessRight())
        if (!iDocument.iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "getMatrixTrait() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }
        short attributeTypeId = check(name);
        if (!M2GSVGConstants.isGetMatrixTraitAllowed(attributeTypeId))
        {
            throw new DOMException(
                DOMException.TYPE_MISMATCH_ERR,
                /*SF*/"Trait's computed value cannot be converted to a SVGMatrix."/*SF*/);
        }
        M2GSVGMatrix matrix = new M2GSVGMatrix();
        // If an error is occurred, then no values are put in the
        // matrix array by the native method
        _getMatrixTrait(
            getNativeEventSourceHandle(),
            getNativeSVGProxyHandle(),
            getHandle(),
            attributeTypeId,
            matrix.getComponents(),
            iUIToolkit);
        return matrix;
    }

    /**
     * @see org.w3c.dom.Node#getNamespaceURI()
     */
    public String getNamespaceURI()
    {
        // Optimization: if(!getDocument().getConnectionPolicy().getAccessRight())
        if (!iDocument.iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "getNamespaceURI() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }
        return M2GSVGConstants.SVG_NAMESPACE_URI;
        /*
          return _getStringTrait(
                  getNativeEventSourceHandle(),
                getNativeSVGProxyHandle(),
                getHandle(),
                id);
        */
    }

    /**
     * @see org.w3c.dom.svg.SVGElement#getNextElementSibling()
     */
    public Element getNextElementSibling()
    {
        int elementHandle = _getNextElementSibling(
                                getNativeEventSourceHandle(), getNativeSVGProxyHandle(), getHandle(),iUIToolkit);
        if (M2GObject.checkHandle(elementHandle))
        {
            return M2GSVGElement.buildElement(elementHandle, iDocument);
        }
        return null;
    }

    /**
     * @see org.w3c.dom.svg.SVGElement#getParentNode()
     */
    public Node getParentNode()
    {
        // Optimization: if(!getDocument().getConnectionPolicy().getAccessRight())
        if (!iDocument.iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "getParentNode() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }
        int parentHandle = _getParent(
                               getNativeEventSourceHandle(), getNativeSVGProxyHandle(), getHandle(), iUIToolkit);
        if (M2GObject.checkHandle(parentHandle))
        {
            return M2GSVGElement.buildElement(parentHandle, iDocument);
        }
        return null;
    }

    /**
     * @see org.w3c.dom.svg.SVGElement#getPathTrait()
     */
    public SVGPath getPathTrait(String name)
    {
        // Optimization: if(!getDocument().getConnectionPolicy().getAccessRight())
        if (!iDocument.iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "getPathTrait() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }
        short attributeTypeId = check(name);
        if (!M2GSVGConstants.isGetPathTraitAllowed(attributeTypeId))
        {
            throw new DOMException(
                DOMException.TYPE_MISMATCH_ERR,
                /*SF*/"Trait's computed value cannot be converted to a SVGPath."/*SF*/);
        }
        int pathHandle = _getPathTrait(
                             getNativeEventSourceHandle(),
                             getNativeSVGProxyHandle(),
                             getHandle(),
                             attributeTypeId,
                             iUIToolkit);
        if (pathHandle != 0)
        {
            // Path doesn't own the native object
            return new M2GSVGPath(pathHandle);
        }
        else
        {
            return null;
        }
    }

    /**
     * @see org.w3c.dom.svg.SVGElement#getRectTrait()
     */
    public SVGRect getRectTrait(String name)
    {
        // Optimization: if(!getDocument().getConnectionPolicy().getAccessRight())
        if (!iDocument.iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "getRectTrait() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }
        short attributeTypeId = check(name);
        if (!M2GSVGConstants.isGetRectTraitAllowed(attributeTypeId))
        {
            throw new DOMException(
                DOMException.TYPE_MISMATCH_ERR,
                /*SF*/"Trait's computed value cannot be converted to a SVGRect."/*SF*/);
        }
        M2GSVGRect rect = new M2GSVGRect();
        if (_getRectTrait(
                    getNativeEventSourceHandle(),
                    getNativeSVGProxyHandle(),
                    getHandle(),
                    attributeTypeId,
                    rect.getComponents(),iUIToolkit) == -1)
        {
            Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                       "getRectTrait() - returns null!!");
            return null;
        }
        else
        {
            return rect;
        }
    }

    /**
     * @see org.w3c.dom.svg.SVGElement#getRGBColorTrait()
     */
    public SVGRGBColor getRGBColorTrait(String name)
    {
        // Optimization: if(!getDocument().getConnectionPolicy().getAccessRight())
        if (!iDocument.iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "getRGBColorTrait() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }

        short attributeTypeId = check(name);
        if (!M2GSVGConstants.isGetRgbColorTraitAllowed(attributeTypeId))
        {
            throw new DOMException(
                DOMException.TYPE_MISMATCH_ERR,
                /*SF*/"Trait's computed value cannot be converted to a SVGRGBColor."/*SF*/);
        }
        M2GSVGRGBColor rgb = new M2GSVGRGBColor(0, 0, 0);
        int result = _getColorTrait(
                         getNativeEventSourceHandle(),
                         getNativeSVGProxyHandle(),
                         getHandle(),
                         attributeTypeId,
                         rgb.getComponents(), iUIToolkit);
        if (result == -1)
        {
            if ((attributeTypeId == M2GSVGConstants.AT_FILL) ||
                    (attributeTypeId == M2GSVGConstants.AT_COLOR))
            {
                return new M2GSVGRGBColor(0, 0, 0);
            }
            else
            {
                return null;
            }
        }
        else if (result == 0)
        {
            return null;
        }
        return rgb;
    }

    /**
     * Get trait
     * @see org.w3c.dom.svg.SVGElement#getTrait()
     */
    public String getTrait(String name)
    {
        return getTraitNS(null, name);
    }

    /**
     * Get trait
     * @see org.w3c.dom.svg.SVGElement#getTraitNS()
     */
    public String getTraitNS(String namespaceURI, String name)
    {
        // Optimization: if(!getDocument().getConnectionPolicy().getAccessRight())
        if (!iDocument.iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "getTraitNS() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }

        // Checks name validity
        short id = check(name);
        String value = null;

        // SVG traits are in the so-called element's 'per-element type partitions namespaces'
        // namespace must be always null
        // except for HREF traits which are from XLINK namespace?

        // XLINK_NAMESPACE
        if (namespaceURI != null &&
                namespaceURI.equals(M2GSVGConstants.XLINK_NAMESPACE_URI))
        {
            if (!M2GSVGConstants.isGetTraitNSAllowed(id))
            {
                Logger.ELOG(Logger.EJavaUI, "getTraitNS() XLINK eception: " +
                            "Trait's computed value cannot be converted to a String.");
                throw new DOMException(
                    DOMException.TYPE_MISMATCH_ERR,
                    "Trait's computed value cannot be converted to a String.");
            }

            value = _getStringTrait(
                        getNativeEventSourceHandle(),
                        getNativeSVGProxyHandle(),
                        getHandle(),
                        id, iUIToolkit);

            if (value == null)
            {
                value = "";
            }

            return value;
        }

        // Checks namespace validity: namespace must be null
        if (namespaceURI != null)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "getTraitNS() - namespace IS NOT xlink but something else.");
            Logger.ELOG(Logger.EJavaUI,
                        "getTraitNS() exception: " + /*SF*/"The namespace is not supported."/*SF*/);

            throw new DOMException(
                DOMException.NOT_SUPPORTED_ERR,
                "The namespace is not supported.");
        }

        // Checks that attribute is obtaineable as a string
        if (!M2GSVGConstants.isGetTraitNSStringAllowed(id))
        {
            Logger.ELOG(Logger.EJavaUI, "getTraitNS() exception: " +
                        "Trait's computed value cannot be converted to a String.");

            throw new DOMException(
                DOMException.TYPE_MISMATCH_ERR,
                "Trait's computed value cannot be converted to a String.");
        }

        if (id == M2GSVGConstants.AT_XLINKHREF)
        {
            Logger.ELOG(Logger.EJavaUI, "getTraitNS() exception: "
                        + "The trait is not supported in this namespace.");

            throw new DOMException(
                DOMException.NOT_SUPPORTED_ERR,
                "The trait is not supported in this namespace.");
        }

        // Checks if attribute is in string format
        if ((id == M2GSVGConstants.AT_STRING) ||
                (id == M2GSVGConstants.AT_VERSION) ||
                (id == M2GSVGConstants.AT_BASEPROFILE) ||
                (id == M2GSVGConstants.AT_TARGET) ||
                (id == M2GSVGConstants.AT_FONTFAMILY))
        {
            value = _getStringTrait(
                        getNativeEventSourceHandle(),
                        getNativeSVGProxyHandle(),
                        getHandle(),
                        id,iUIToolkit);
        }
        // Reads enum attribute
        else
        {
            short valueId = _getEnumTrait(
                                getNativeEventSourceHandle(),
                                getNativeSVGProxyHandle(),
                                getHandle(),
                                id,
                                iUIToolkit);

            // Convert value id to string
            value = M2GSVGConstants.parseAttributeValueName(id, valueId);
        }

        if (value == null)
        {
            value = "";
        }
        return value;
    }

    /*
     * Get element used from this element
     * @return Element.
     */
    public SVGElement getUsedFromElement()
    {
        int elementHandle = _getUsedFromElement(
                                getNativeEventSourceHandle(),
                                getNativeSVGProxyHandle(),
                                getHandle(),iUIToolkit);
        if (M2GObject.checkHandle(elementHandle))
        {
            return M2GSVGElement.buildElement(elementHandle, iDocument);
        }
        return null;
    }


    /**
     * @see org.w3c.dom.Node#insertBefore
     */
    public Node insertBefore(Node newChild, Node refChild)
    {
        // Optimization: if(!getDocument().getConnectionPolicy().getAccessRight())
        if (!iDocument.iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "insertBefore() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }

        if (newChild == null)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "insertBefore() exception - new child is null");
            throw new NullPointerException();
        }
        if (newChild instanceof M2GDocument)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "insertBefore() exception - child is document");
            throw new DOMException(
                DOMException.HIERARCHY_REQUEST_ERR,
                /*SF*/"Child is wrong type (Document)."/*SF*/);
        }

        if (!M2GSVGConstants.isAllowedToInsertElement(((M2GSVGElement)newChild).iElementTypeId))
        {
            Logger.ELOG(Logger.EJavaUI,
                        "insertBefore() exception - cannot insert this node type");
            throw new DOMException(
                DOMException.NOT_SUPPORTED_ERR,
                /*SF*/"Cannot insert node of that type."/*SF*/);
        }

        if (!M2GSVGConstants.checkElementAsChild(
                    iElementTypeId, ((M2GSVGElement)newChild).iElementTypeId))
        {
            Logger.ELOG(Logger.EJavaUI,
                        "insertBefore() exception - invalid parent-child relation");
            throw new DOMException(
                DOMException.HIERARCHY_REQUEST_ERR,
                /*SF*/"Cannot insert node of that type."/*SF*/);
        }
        // Ensure that the new child candidate wouldn't be an ancestor of this element
        if (IsAncestor(newChild, this))
        {
            Logger.ELOG(Logger.EJavaUI,
                        "insertBefore() exception - is ancestor");
            throw new DOMException(
                DOMException.HIERARCHY_REQUEST_ERR,
                /*SF*/"Hierarchy request error in Document."/*SF*/);
        }
        // Check that the child and this element belong to the same Document
        if (((M2GSVGElement)newChild).getDocument().getHandle() != iDocument.getHandle())
        {
            Logger.ELOG(Logger.EJavaUI,
                        "insertBefore() exception - document is not the same");
            throw new DOMException(
                DOMException.WRONG_DOCUMENT_ERR,
                /*SF*/"Child belongs to different document."/*SF*/);
        }
        if ((refChild != null) &&
                (((M2GSVGElement)refChild.getParentNode()).getHandle() != getHandle()))
        {
            Logger.ELOG(Logger.EJavaUI,
                        "insertBefore() exception - ref child is not found");
            throw new DOMException(
                DOMException.NOT_FOUND_ERR,
                /*SF*/"The child to insert before doesn't exist in this current node."/*SF*/);
        }
        // check that if <use> then xlink:href is an existing element id
        if (((M2GSVGElement)newChild).iElementTypeId == M2GSVGConstants.EL_USE)
        {
            String value = ((M2GSVGElement)newChild).getTraitNS(
                               M2GSVGConstants.XLINK_NAMESPACE_URI, "href");
            if (value.length() != 0)
            {
                if (!value.startsWith("#") ||
                        ((M2GSVGElement)newChild).iDocument.getElementById(value.substring(1)) == null)
                {
                    Logger.ELOG(Logger.EJavaUI,
                                "insertBefore() exception - \"href\" trait value="
                                + value + " is invalid");
                    throw new DOMException(
                        DOMException.INVALID_STATE_ERR,
                        /*SF*/"Invalid element id value."/*SF*/);
                }
            }
        }
        if (refChild == null)
        {
            _appendChild(
                getNativeEventSourceHandle(),
                getNativeSVGProxyHandle(),
                getHandle(),
                ((M2GSVGElement)newChild).getHandle(), iUIToolkit);
        }
        else
        {
            _insertBefore(
                getNativeEventSourceHandle(),
                getNativeSVGProxyHandle(),
                getHandle(),
                ((M2GSVGElement)newChild).getHandle(),
                ((M2GSVGElement) refChild).getHandle(),iUIToolkit);
        }
        String href = _getStringTrait(
                          getNativeEventSourceHandle(),
                          getNativeSVGProxyHandle(),
                          ((M2GSVGElement)newChild).getHandle(),
                          M2GSVGConstants.AT_XLINKHREF,iUIToolkit);
        // Need to call resource handler if element is elementindom, image,
        // and if it has an href attribute
        if ((href != null) &&
                (((M2GSVGElement)newChild).iElementTypeId == M2GSVGConstants.EL_IMAGE) &&
                iDocument.isElementInDOM(((M2GSVGElement)newChild).getHandle()))
        {
            // Call resource handler here
            iDocument.invokeResourceHandler(href);
        }

        // inform observer about changes in DOM only if element is in DOM
        iDocument.notifyDOMChangeObserver(iNativeHandle);

        return newChild;
    }

    /*
     * Check if used
     * @return True if used.
     */
    public boolean isUsed()
    {
        return (_isUsed(
                    getNativeEventSourceHandle(),
                    getNativeSVGProxyHandle(),
                    getHandle(),iUIToolkit) > 0 ? true : false);
    }

    /**
     * @see org.w3c.dom.events.EventTarget#removeEventListener()
     */
    public void removeEventListener(String type, EventListener listener, boolean useCapture)
    {
        if (type == null || type.equals("") || listener == null)
        {
            throw new NullPointerException();
        }
        if (useCapture == true)
        {
            throw new DOMException(
                DOMException.NOT_SUPPORTED_ERR,
                CAPTURE_NOT_SUPPORTED_ESTR);
        }
        iDocument.unregister(this, type, listener);
    }

    /**
     * @see org.w3c.dom.svg.SVGElement#setId()
     */
    public void setId(String id) throws DOMException
    {
        // Optimization: if(!getDocument().getConnectionPolicy().getAccessRight())
        if (!iDocument.iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "setId() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }
        if (id == null)
        {
            throw new NullPointerException(/*SF*/"The id is null."/*SF*/);
        }
        if (getId() != null)
        {
            throw new DOMException(
                DOMException.NO_MODIFICATION_ALLOWED_ERR,
                /*SF*/"Existing element id cannot be changed."/*SF*/);
        }
        if (iDocument.containsElement(id))
        {
            throw new DOMException(
                DOMException.INVALID_ACCESS_ERR,
                /*SF*/"Element id already exists in the document."/*SF*/);
        }
        else
        {
            _setStringTrait(
                getNativeEventSourceHandle(), getNativeSVGProxyHandle(),
                getHandle(), M2GSVGConstants.AT_ID, id,iUIToolkit);
        }
    }

    /**
     * @see org.w3c.dom.svg.SVGElement#setFloatTrait()
     */
    public void setFloatTrait(String name, float value)
    {
        // Optimization: if(!getDocument().getConnectionPolicy().getAccessRight())
        if (!iDocument.iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "setFloatTrait() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }

        short id = check(name);
        if (!M2GSVGConstants.isSetFloatTraitAllowed(id))
        {
            Logger.ELOG(Logger.EJavaUI,
                        "setFloatTrait() exception - set float (id=" + id + ") not allowed");
            throw new DOMException(
                DOMException.TYPE_MISMATCH_ERR,
                /*SF*/"Trait's value cannot be specified as a float."/*SF*/);
        }
        if ((Float.isNaN(value)) ||
                // value must be >= 1
                (value < 1 && (id == M2GSVGConstants.AT_STROKEMITERLIMIT)) ||
                // value must be >= 0
                (value < 0 && (id == M2GSVGConstants.AT_FONTSIZE ||
                               id == M2GSVGConstants.AT_STROKEWIDTH ||
                               id == M2GSVGConstants.AT_HEIGHT ||
                               id == M2GSVGConstants.AT_WIDTH ||
                               id == M2GSVGConstants.AT_RX ||
                               id == M2GSVGConstants.AT_RY ||
                               id == M2GSVGConstants.AT_R)))
        {
            Logger.ELOG(Logger.EJavaUI,
                        "setFloatTrait() exception - invalid value=" + value);
            throw new DOMException(
                DOMException.INVALID_ACCESS_ERR,
                /*SF*/"The input value is an invalid value for the given trait."/*SF*/);
        }
        _setFloatTrait(getNativeEventSourceHandle(), getNativeSVGProxyHandle(),
                       getHandle(), id, value,iUIToolkit);

        // inform observer about changes in DOM only if element is in DOM
        iDocument.notifyDOMChangeObserver(iNativeHandle);
    }

    /**
     * @see org.w3c.dom.svg.SVGElement#setMatrixTrait()
     */
    public void setMatrixTrait(String name, SVGMatrix matrix)
    {
        // Optimization: if(!getDocument().getConnectionPolicy().getAccessRight())
        if (!iDocument.iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "setMatrixTrait() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }
        short id = check(name);
        if (!M2GSVGConstants.isSetMatrixTraitAllowed(id))
        {
            throw new DOMException(
                DOMException.TYPE_MISMATCH_ERR,
                /*SF*/"Trait's value cannot be specified as a SVGMatrix."/*SF*/);
        }
        if ((matrix == null) ||
                !(matrix instanceof M2GSVGMatrix))
        {
            throw new DOMException(
                DOMException.INVALID_ACCESS_ERR,
                /*SF*/"The input value is an invalid value for the given trait."/*SF*/);
        }

        _setMatrixTrait(getNativeEventSourceHandle(), getNativeSVGProxyHandle(),
                        getHandle(), id,
                        ((M2GSVGMatrix)matrix).getComponents(),iUIToolkit);

        // inform observer about changes in DOM only if element is in DOM
        iDocument.notifyDOMChangeObserver(iNativeHandle);
    }

    /**
     * @see org.w3c.dom.svg.SVGElement#setPathTrait()
     */
    public void setPathTrait(String name, SVGPath path)
    {
        // Optimization: if(!getDocument().getConnectionPolicy().getAccessRight())
        if (!iDocument.iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "setPathTrait() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }
        short id = check(name);
        if (!M2GSVGConstants.isSetPathTraitAllowed(id))
        {
            throw new DOMException(
                DOMException.TYPE_MISMATCH_ERR,
                /*SF*/"Trait's value cannot be specified as a SVGPath."/*SF*/);
        }
        if ((path == null) ||
                !(path instanceof M2GSVGPath) ||
                !((M2GSVGPath)path).doCheckValidity())
        {
            Logger.ELOG(Logger.EJavaUI,
                        "setPathTrait() - exception M2GSVGPath not valid");
            throw new DOMException(
                DOMException.INVALID_ACCESS_ERR,
                /*SF*/"The input value is an invalid value for the given trait."/*SF*/);
        }

        _setPathTrait(getNativeEventSourceHandle(), getNativeSVGProxyHandle(),
                      getHandle(), id, ((M2GSVGPath)path).getHandle(),iUIToolkit);

        // inform observer about changes in DOM only if element is in DOM
        iDocument.notifyDOMChangeObserver(iNativeHandle);
    }


    /**
     * @see org.w3c.dom.svg.SVGElement#setPathTrait()
     */
    public void setRectTrait(String name, SVGRect rect)
    {
        // Optimization: if(!getDocument().getConnectionPolicy().getAccessRight())
        if (!iDocument.iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "setRectTrait() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }
        short id = check(name);
        if (!M2GSVGConstants.isSetRectTraitAllowed(id))
        {
            throw new DOMException(
                DOMException.TYPE_MISMATCH_ERR,
                /*SF*/"Trait's value cannot be specified as a SVGRect."/*SF*/);
        }
        if ((rect == null) ||
                (rect.getWidth() < 0 || rect.getHeight() < 0))
        {
            throw new DOMException(
                DOMException.INVALID_ACCESS_ERR,
                /*SF*/"The input value is an invalid value for the given trait."/*SF*/);
        }

        _setRectTrait(
            getNativeEventSourceHandle(), getNativeSVGProxyHandle(), getHandle(), id,
            rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight(),iUIToolkit);

        // inform observer about changes in DOM only if element is in DOM
        iDocument.notifyDOMChangeObserver(iNativeHandle);
    }

    /**
     * @see org.w3c.dom.svg.SVGElement#setRGBColorTrait()
     */
    public void setRGBColorTrait(String name, SVGRGBColor color)
    {

        // Optimization: if(!getDocument().getConnectionPolicy().getAccessRight())
        if (!iDocument.iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "setRGBColorTrait() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }
        short id = check(name);
        if (!M2GSVGConstants.isSetRgbColorTraitAllowed(id))
        {
            Logger.ELOG(Logger.EJavaUI,
                        "setRGBColorTrait() - exception: " + DOMException.TYPE_MISMATCH_ERR);
            throw new DOMException(
                DOMException.TYPE_MISMATCH_ERR,
                /*SF*/"Trait's value cannot be specified as a SVGRGBColor."/*SF*/);
        }
        if (color == null)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "setRGBColorTrait() - exception: " + DOMException.INVALID_ACCESS_ERR);
            throw new DOMException(
                DOMException.INVALID_ACCESS_ERR,
                /*SF*/"The trait is null."/*SF*/);
        }

        _setColorTrait(
            getNativeEventSourceHandle(), getNativeSVGProxyHandle(), getHandle(), id,
            color.getRed(), color.getGreen(), color.getBlue(),iUIToolkit);

        // inform observer about changes in DOM only if element is in DOM
        iDocument.notifyDOMChangeObserver(iNativeHandle);
    }

    /**
     * @see org.w3c.dom.svg.SVGElement#setTrait()
     */
    public void setTrait(String name, String value)
    {
        setTraitNS(null, name, value);
    }

    /**
     * @see org.w3c.dom.svg.SVGElement#setTraitNS()
     */
    public void setTraitNS(String namespaceURI, String name, String value)
    {
        // Optimization: if(!getDocument().getConnectionPolicy().getAccessRight())
        if (!iDocument.iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "setTraitNS() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }
        if (value == null)
        {
            throw new DOMException(
                DOMException.INVALID_ACCESS_ERR,
                /*SF*/"Value cannot be set to null."/*SF*/);
        }

        short id = check(name);

        // SVG traits are in the so-called element's 'per-element type partitions namespaces'
        // namespace must be always null
        // except for HREF traits which are from XLINK namespace?

        if (namespaceURI != null &&
                namespaceURI.equals(M2GSVGConstants.XLINK_NAMESPACE_URI))
        {
            if (!M2GSVGConstants.isGetTraitNSAllowed(id))
            {
                throw new DOMException(
                    DOMException.TYPE_MISMATCH_ERR,
                    "Trait's value cannot be specified as a String.");
            }

            // check that if <use> then xlink:href is an existing element id
            if ((iElementTypeId == M2GSVGConstants.EL_USE) &&
                    (iDocument.isElementInDOM(getHandle())))
            {
                if ((value.length() <= 1) ||
                        !value.startsWith("#") ||
                        (iDocument.getElementById(value.substring(1)) == null))
                {
                    Logger.ELOG(Logger.EJavaUI,
                                "setTraitNS() - DOMException");
                    throw new DOMException(
                        DOMException.INVALID_ACCESS_ERR,
                        /*SF*/"The <use> element is hooked into the document tree and the the value of xlink:href is set invalid."/*SF*/);
                }
            }

            _setStringTrait(
                getNativeEventSourceHandle(),
                getNativeSVGProxyHandle(), getHandle(), id, value,iUIToolkit);

            // inform observer about changes in DOM only if element is in DOM
            iDocument.notifyDOMChangeObserver(iNativeHandle);

            // Check if element is in DOM and in that case invokes
            // resource handler
            if ((iElementTypeId == M2GSVGConstants.EL_IMAGE) &&
                    (id == M2GSVGConstants.AT_XLINKHREF) &&
                    (iDocument.isElementInDOM(getHandle())))
            {
                if (!iDocument.containsExternalResourceURI(value))
                {
                    iDocument.invokeResourceHandler(value);
                }
            }

            return;
        } // namespaceURI != null &&  namespaceURI.equals(M2GSVGConstants.XLINK_NAMESPACE_URI)

        // all traits except HREF must have no namespace
        if (namespaceURI != null)
        {
            throw new DOMException(
                DOMException.NOT_SUPPORTED_ERR,
                "The namespace is not supported.");
        }

        if ((id == M2GSVGConstants.AT_VERSION) ||
                (id == M2GSVGConstants.AT_BASEPROFILE))
        {
            throw new DOMException(
                DOMException.NO_MODIFICATION_ALLOWED_ERR,
                /*SF*/"Attempt to change readonly trait."/*SF*/);
        }

        if (id == M2GSVGConstants.AT_XLINKHREF)
        {
            throw new DOMException(
                DOMException.NOT_SUPPORTED_ERR,
                /*SF*/"The trait is not supported in this namespace."/*SF*/);
        }

        if (!M2GSVGConstants.isSetTraitNSStringAllowed(id))
        {
            throw new DOMException(
                DOMException.TYPE_MISMATCH_ERR,
                /*SF*/"Trait's value cannot be specified as a String."/*SF*/);
        }

        short valueId = M2GSVGConstants.parseAttributeValueNameId(id, value);

        if (valueId == M2GSVGConstants.SVG_ERROR)
        {
            throw new DOMException(
                DOMException.INVALID_ACCESS_ERR,
                /*SF*/"The value is invalid."/*SF*/);
        }
        else if (valueId == M2GSVGConstants.TEXT_NOT_SHORT)
        {
            _setStringTrait(
                getNativeEventSourceHandle(),
                getNativeSVGProxyHandle(), getHandle(), id, value,iUIToolkit);
        }
        else
        {
            _setEnumTrait(
                getNativeEventSourceHandle(),
                getNativeSVGProxyHandle(), getHandle(), id, valueId,iUIToolkit);
        }

        // inform observer about changes in DOM only if element is in DOM
        iDocument.notifyDOMChangeObserver(iNativeHandle);
    }

    /**
     * @see org.w3c.dom.Node#removeChild
     */
    public Node removeChild(Node oldChild) throws DOMException
    {
        // Optimization: if(!getDocument().getConnectionPolicy().getAccessRight())
        if (!iDocument.iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "removeChild() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }
        if (oldChild == null)
        {
            throw new NullPointerException();
        }
        if (oldChild instanceof M2GDocument)
        {
            throw new DOMException(
                DOMException.NOT_SUPPORTED_ERR,
                /*SF*/"Cannot remove this type of node."/*SF*/);
        }

        M2GSVGElement removableChild = (M2GSVGElement)oldChild;

        if (!M2GSVGConstants.isAllowedToRemoveElement(
                    removableChild.iElementTypeId))
        {
            throw new DOMException(
                DOMException.NOT_SUPPORTED_ERR,
                /*SF*/"Cannot remove this type of node."/*SF*/);
        }
        M2GSVGElement parent = (M2GSVGElement)removableChild.getParentNode();
        if ((parent == null) ||
                (parent.getHandle() != getHandle()))
        {
            throw new DOMException(
                DOMException.NOT_FOUND_ERR,
                /*SF*/"Not a child of this node."/*SF*/);
        }
        // Check if removable
        int result = _checkRemoveable(
                         getNativeEventSourceHandle(),
                         getNativeSVGProxyHandle(),
                         removableChild.getHandle(), iUIToolkit);
        if (result <= 0)
        {
            throw new DOMException(
                DOMException.INVALID_ACCESS_ERR,
                /*SF*/"The element being removed or one of its decendants have non-null id."/*SF*/);
        }
        // The removedChildHandle handle should be same as the removableChild handle
        int removedChildHandle = _removeChild(
                                      getNativeEventSourceHandle(), getNativeSVGProxyHandle(),
                                      getHandle(), removableChild.getHandle(), iUIToolkit);
        // Remove the element from the live elements
        removableChild.getDocument().unregisterLiveElement(
            new Integer(removableChild.getHandle()));

        iDocument.notifyDOMChangeObserver(getHandle());

        return removableChild;
    }

    //--------------------------------------------------
    // STATIC METHODS
    //--------------------------------------------------

    /**
     * Gets the current Ui toolkit using RuntimeInfoFactory object.
     */
    public static boolean getCurrentUiToolkit()
    {
        if (!iIsToolkitAvailable)
        {
            String toolkit = LegacyRtPort.getRegisteredToolkit();
            if (ESWT_PACKAGE.equals(toolkit))
            {
                // Current UITOOLKIT is eSWT
                iStaticUIToolkit = eSWT_TOOLKIT;
            }
            else
            {
                // Current UITOOLKIT is LCDUI
                iStaticUIToolkit = LCDUI_TOOLKIT;
            }
            iIsToolkitAvailable = true;
        }
        return iStaticUIToolkit;
    }
    /**
     * Creates SVGElement instance according to the element type.
     * Possible element types are: SVGElement, SVGLocatableElement,
     * SVGSVGElement, SVGAnimationElement
     */
    public static SVGElement buildElement(int aElementHandle, M2GDocument aDocument)
    {
        // Check native handle
        if (!M2GObject.checkHandle(aElementHandle) || (aDocument == null))
        {
            return null;
        }

        // Check if the element already exists
        SVGElement svgElement = aDocument.findLiveElement(new Integer(aElementHandle));
        if (svgElement != null)
        {
            return svgElement;
        }

        String typeName = M2GSVGElement.getElementTypeName(aElementHandle, aDocument);

        if (M2GSVGConstants.isRootElement(typeName))
        {
            svgElement = M2GSVGSVGElement.buildRootElement(aDocument);
        }
        else if (M2GSVGConstants.isLocatableElement(typeName))
        {
            svgElement = new M2GSVGLocatableElement(aElementHandle, aDocument);
        }
        else if (M2GSVGConstants.isAnimatableElement(typeName))
        {
            svgElement = new M2GSVGAnimationElement(aElementHandle, aDocument);
        }
        else
        {
            boolean lUIToolkit = getCurrentUiToolkit();
            String id = M2GSVGElement._getStringTrait(
                            M2GManager.getInstance(lUIToolkit).getEventSourceHandle(),
                            M2GManager.getInstance(lUIToolkit).getSVGProxyHandle(),
                            aElementHandle,
                            M2GSVGConstants.AT_ID,
                            lUIToolkit);
            if ((id != null) && id.equals("text_use_svg_default_font"))
            {
                return buildElement(
                           M2GSVGElement._getNextElementSibling(
                               aDocument.getNativeEventSourceHandle(),
                               aDocument.getNativeSVGProxyHandle(), aElementHandle,  lUIToolkit),
                           aDocument);
            }
            else
            {
                svgElement = new M2GSVGElement(aElementHandle, aDocument);
            }
        }
        aDocument.registerLiveElement(svgElement, new Integer(aElementHandle));
        return svgElement;
    }

    /**
     * Helper function for appendChild and insertBefore
     * returns true if "ancestor" is the same node as "child" or
     * if "ancestor" is parent/ancestor of "child" else returns false
     */
    private static final boolean IsAncestor(Node aAncestor, Node aChild) throws DOMException
    {
        boolean result = false;
        if (aAncestor != null && aChild != null)
        {
            if (aChild instanceof M2GDocument)
            {
                throw new DOMException(
                    DOMException.HIERARCHY_REQUEST_ERR,
                    /*SF*/"Cannot append Document elements."/*SF*/);
            }
            if ((aChild instanceof M2GSVGElement) && (aAncestor instanceof M2GSVGElement))
            {
                // Should check all types or just parent type???
                if (((M2GSVGElement)aAncestor).getHandle() == ((M2GSVGElement)aChild).getHandle())
                {
                    result = true;
                }
                else
                {
                    result = IsAncestor(aAncestor, aChild.getParentNode());
                }
            }
        }
        return result;
    }


    /**
     * Helper function for removeChild.
     * @returns false if the node "child" and all its children have non-null ids
     * else returns true
     */
    private static final boolean IsReadOnly(Node aChild)
    {
        if (aChild != null)
        {
            // Should check all types or just parent type???
            if ((aChild instanceof M2GSVGElement) ||
                    (aChild instanceof SVGLocatableElement) ||
                    (aChild instanceof SVGSVGElement))
            {
                if (M2GSVGConstants.isElementReadOnly(((M2GSVGElement)aChild).getLocalName()))
                {
                    return true;
                }
                return IsReadOnly(aChild.getParentNode());
            }
        }
        return false;
    }

    /**
     * Get element's type name
     * @param aElementHandle
     * @param aDocument
     * @return element's type name
     */
    public static String getElementTypeName(int aElementHandle, M2GDocument aDocument)
    {
        short elemenTypeId = M2GSVGConstants.EL_UNDEFINED;
        boolean lUIToolkit = getCurrentUiToolkit();
        if (aDocument != null)
        {
            elemenTypeId = M2GSVGElement._getElementTypeId(
                               aDocument.getNativeEventSourceHandle(),
                               aDocument.getNativeSVGProxyHandle(),
                               aElementHandle,  lUIToolkit);
        }
        else
        {
            elemenTypeId = M2GSVGElement._getElementTypeId(
                               M2GManager.getInstance(lUIToolkit).getEventSourceHandle(),
                               M2GManager.getInstance(lUIToolkit).getSVGProxyHandle(),
                               aElementHandle,
                               lUIToolkit);
        }

        return M2GSVGConstants.parseElementTypeName(elemenTypeId);
    }

    //--------------------------------------------------
    // NATIVE METHODS
    //--------------------------------------------------
    private static native void _appendChild(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle, int aNewChild, boolean iUIToolkit);

    private static native int _checkRemoveable(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle, boolean iUIToolkit);

    static native int _getBBox(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle,
        short aAttributeTypeId, float[] aComponents, boolean iUIToolkit);

    private static native int _getColorTrait(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle,
        short aAttributeTypeId, int[] aColorComponents, boolean iUIToolkit);

    private static native short _getElementTypeId(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle, boolean iUIToolkit);

    static native short _getEnumTrait(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle, short aAttributeTypeId, boolean iUIToolkit);

    private static native int _getFirstElementChild(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle, boolean iUIToolkit);

    private static native float _getFloatTrait(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle, short aAttributeTypeId, boolean iUIToolkit);

    static native int _getMatrixTrait(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle,
        short aAttributeTypeId, float[] aComponents, boolean iUIToolkit);

    private static native int _getNextElementSibling(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle, boolean iUIToolkit);

    private static native int _getParent(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle,boolean iUIToolkit);

    private static native int _getPathTrait(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle, short aAttributeTypeId, boolean iUIToolkit);

    static native int _getRectTrait(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle,
        short aAttributeTypeId, float[] aComponents, boolean iUIToolkit);

    native static int _getRootElement(
        int aEventSourceHandle, int aSvgProxyHandle, int aDocumentHandle,boolean iUIToolkit);

    static native void _getScreenBBox(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle, float[] aComponents, boolean iUIToolkit);

    protected static native String _getStringTrait(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle, short aAttributeTypeId, boolean iUIToolkit);

    private static native int _getUsedFromElement(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle, boolean iUIToolkit);

    private static native void _insertBefore(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle,
        int aChildElementHandle, int aReferenceElementHandle, boolean iUIToolkit);

    private static native int _isUsed(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle,boolean iUIToolkit);

    private static native int _removeChild(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle, int aChildElementHandle,boolean iUIToolkit);

    private static native void _setColorTrait(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle, short aAttributeTypeId,
        int aRed, int aGreen, int aBlue, boolean iUIToolkit);

    private static native void _setEnumTrait(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle, short aAttributeTypeId,
        short aValue,boolean iUIToolkit);

    private static native void _setFloatTrait(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle, short aAttributeTypeId, float aValue, boolean iUIToolkit);

    static native void _setMatrixTrait(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle,
        short aAttributeTypeId, float[] aComponents, boolean iUIToolkit);

    private static native void _setPathTrait(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle, short aAttributeTypeId, int aPathHandle, boolean iUIToolkit);

    private static native void _setRectTrait(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle, short aAttributeTypeId,
        float aX, float aY, float aWidth, float aHeight, boolean iUIToolkit);

    private static native void _setStringTrait(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle, short aAttributeTypeId, String aValue,boolean iUIToolkit);

}
