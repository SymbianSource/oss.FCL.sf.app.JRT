/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
import java.io.IOException;
import org.eclipse.swt.widgets.*;
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
                             getNativeSVGProxyHandle(), getHandle());
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
                              getNativeSVGProxyHandle(), getHandle());
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
                   getNativeSVGProxyHandle(), getHandle(), attributeTypeId);
    }

    /**
     * @see org.w3c.dom.svg.SVGElement#getId()
     */
    public String getId()
    {
        return _getStringTrait(
                   getNativeSVGProxyHandle(), getHandle(), M2GSVGConstants.AT_ID);
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
            getNativeSVGProxyHandle(),
            getHandle(),
            attributeTypeId,
            matrix.getComponents());
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
                                getNativeSVGProxyHandle(), getHandle());
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
                               getNativeSVGProxyHandle(), getHandle());
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
                             getNativeSVGProxyHandle(),
                             getHandle(),
                             attributeTypeId);
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
                    getNativeSVGProxyHandle(),
                    getHandle(),
                    attributeTypeId,
                    rect.getComponents()) == -1)
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
                         getNativeSVGProxyHandle(),
                         getHandle(),
                         attributeTypeId,
                         rgb.getComponents());
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
                        getNativeSVGProxyHandle(),
                        getHandle(),
                        id);

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
                        getNativeSVGProxyHandle(),
                        getHandle(),
                        id);
        }
        // Reads enum attribute
        else
        {
            short valueId = _getEnumTrait(
                                getNativeSVGProxyHandle(),
                                getHandle(),
                                id);

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
                                getNativeSVGProxyHandle(),
                                getHandle());
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
                getNativeSVGProxyHandle(),
                getHandle(),
                ((M2GSVGElement)newChild).getHandle());
        }
        else
        {
            _insertBefore(
                getNativeSVGProxyHandle(),
                getHandle(),
                ((M2GSVGElement)newChild).getHandle(),
                ((M2GSVGElement) refChild).getHandle());
        }
        String href = _getStringTrait(
                          getNativeSVGProxyHandle(),
                          ((M2GSVGElement)newChild).getHandle(),
                          M2GSVGConstants.AT_XLINKHREF);
        // call resource handler call if element is elementindom, image,
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
                    getNativeSVGProxyHandle(),
                    getHandle()) > 0 ? true : false);
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
                getNativeSVGProxyHandle(),
                getHandle(), M2GSVGConstants.AT_ID, id);
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
        _setFloatTrait(getNativeSVGProxyHandle(),
                       getHandle(), id, value);

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

        _setMatrixTrait(getNativeSVGProxyHandle(),
                        getHandle(), id,
                        ((M2GSVGMatrix)matrix).getComponents());

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

        _setPathTrait(getNativeSVGProxyHandle(),
                      getHandle(), id, ((M2GSVGPath)path).getHandle());

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
            getNativeSVGProxyHandle(), getHandle(), id,
            rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight());

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
            getNativeSVGProxyHandle(), getHandle(), id,
            color.getRed(), color.getGreen(), color.getBlue());

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
                getNativeSVGProxyHandle(), getHandle(), id, value);

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
                getNativeSVGProxyHandle(), getHandle(), id, value);
        }
        else
        {
            _setEnumTrait(
                getNativeSVGProxyHandle(), getHandle(), id, valueId);
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
                         getNativeSVGProxyHandle(),
                         removableChild.getHandle());
        if (result <= 0)
        {
            throw new DOMException(
                DOMException.INVALID_ACCESS_ERR,
                /*SF*/"The element being removed or one of its decendants have non-null id."/*SF*/);
        }
        // The removedChildHandle handle should be same as the removableChild handle
        int removedChildHandle  = removedChildHandle = _removeChild(
                                      getNativeSVGProxyHandle(),
                                      getHandle(), removableChild.getHandle());
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

            String id = M2GSVGElement._getStringTrait(
                            M2GManager.getInstance().getSVGProxyHandle(),
                            aElementHandle,
                            M2GSVGConstants.AT_ID);
            if ((id != null) && id.equals("text_use_svg_default_font"))
            {
                return buildElement(
                           M2GSVGElement._getNextElementSibling(
                               aDocument.getNativeSVGProxyHandle(), aElementHandle),
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
        if (aDocument != null)
        {
            elemenTypeId = M2GSVGElement._getElementTypeId(
                               aDocument.getNativeSVGProxyHandle(),
                               aElementHandle);
        }
        else
        {
            elemenTypeId = M2GSVGElement._getElementTypeId(
                               M2GManager.getInstance().getSVGProxyHandle(),
                               aElementHandle);
        }

        return M2GSVGConstants.parseElementTypeName(elemenTypeId);
    }

    //--------------------------------------------------
    // NATIVE METHODS
    //--------------------------------------------------
    private static native void _appendChild(
        int aSvgProxyHandle, int aElementHandle, int aNewChild);

    private static native int _checkRemoveable(
        int aSvgProxyHandle, int aElementHandle);

    static native int _getBBox(
        int aSvgProxyHandle, int aElementHandle,
        short aAttributeTypeId, float[] aComponents);

    private static native int _getColorTrait(
        int aSvgProxyHandle, int aElementHandle,
        short aAttributeTypeId, int[] aColorComponents);

    private static native short _getElementTypeId(
        int aSvgProxyHandle, int aElementHandle);

    static native short _getEnumTrait(
        int aSvgProxyHandle, int aElementHandle, short aAttributeTypeId);

    private static native int _getFirstElementChild(
        int aSvgProxyHandle, int aElementHandle);

    private static native float _getFloatTrait(
        int aSvgProxyHandle, int aElementHandle, short aAttributeTypeId);

    static native int _getMatrixTrait(
        int aSvgProxyHandle, int aElementHandle,
        short aAttributeTypeId, float[] aComponents);

    private static native int _getNextElementSibling(
        int aSvgProxyHandle, int aElementHandle);

    private static native int _getParent(
        int aSvgProxyHandle, int aElementHandle);

    private static native int _getPathTrait(
        int aSvgProxyHandle, int aElementHandle, short aAttributeTypeId);

    static native int _getRectTrait(
        int aSvgProxyHandle, int aElementHandle,
        short aAttributeTypeId, float[] aComponents);

    native static int _getRootElement(
        int aSvgProxyHandle, int aDocumentHandle);

    static native void _getScreenBBox(
        int aSvgProxyHandle, int aElementHandle, float[] aComponents);

    protected static native String _getStringTrait(
        int aSvgProxyHandle, int aElementHandle, short aAttributeTypeId);

    private static native int _getUsedFromElement(
        int aSvgProxyHandle, int aElementHandle);

    private static native void _insertBefore(
        int aSvgProxyHandle, int aElementHandle,
        int aChildElementHandle, int aReferenceElementHandle);

    private static native int _isUsed(
        int aSvgProxyHandle, int aElementHandle);

    private static native int _removeChild(
        int aSvgProxyHandle, int aElementHandle, int aChildElementHandle);

    private static native void _setColorTrait(
        int aSvgProxyHandle, int aElementHandle, short aAttributeTypeId,
        int aRed, int aGreen, int aBlue);

    private static native void _setEnumTrait(
        int aSvgProxyHandle, int aElementHandle, short aAttributeTypeId,
        short aValue);

    private static native void _setFloatTrait(
        int aSvgProxyHandle, int aElementHandle, short aAttributeTypeId, float aValue);

    static native void _setMatrixTrait(
        int aSvgProxyHandle, int aElementHandle,
        short aAttributeTypeId, float[] aComponents);

    private static native void _setPathTrait(
        int aSvgProxyHandle, int aElementHandle, short aAttributeTypeId, int aPathHandle);

    private static native void _setRectTrait(
        int aSvgProxyHandle, int aElementHandle, short aAttributeTypeId,
        float aX, float aY, float aWidth, float aHeight);

    private static native void _setStringTrait(
        int aSvgProxyHandle, int aElementHandle, short aAttributeTypeId, String aValue);

}
