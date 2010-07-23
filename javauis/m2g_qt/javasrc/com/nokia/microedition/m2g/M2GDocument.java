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
import org.w3c.dom.events.*;
import org.w3c.dom.svg.SVGElement;
import java.io.IOException;
import java.util.Hashtable;
import java.lang.ref.WeakReference;
import java.util.Vector;
import java.util.Enumeration;
import javax.microedition.m2g.*;
import com.nokia.microedition.m2g.connection.*;
import com.nokia.mj.impl.utils.Logger;

public class M2GDocument extends M2GObject
        implements Document, M2GConnectionPolicyHandler
{
    //--------------------------------------------------
    // STATIC CONSTANTS
    //--------------------------------------------------
    // Error constants
    /* Optimization: static finals changed to local variables
    public static final String UNSUPORTED_ELEMENT_TYPE_ESTR =
      "The type of element is not supported by the implementation.";
    public static final String UNSUPORTED_NAMESPACE_ESTR =
      "The URI is not the SVG namespace URI.";
    public static final String ID_IS_NULL_ESTR =
      "The ID is null.";
    public static final String NAMESPACE_URI_IS_NULL_ESTR =
      "The namespace URI is null.";
    public static final String QUALIFIED_NAME_IS_NULL_ESTR =
      "The qualifiedName is null.";
    public static final String CANNOT_APPEND_CHILD_ESTR =
      "Cannot appendChild to a Document node.";
    public static final String CANNOT_INSERT_BEFORE_ESTR =
      "Cannot insertBefore a Document node.";
    public static final String CANNOT_REMOVE_CHILD_ESTR =
      "Cannot removeChild from a Document node.";
    public static final String DOCUMENT_IS_ILLEGAL_ESTR =
      "The SVG document does not conform to the XML 1.0 specification.";
    public static final String HEIGHT_IS_ILLEGAL_ESTR =
      "The height is negative.";
    public static final String WIDTH_IS_ILLEGAL_ESTR =
      "The width is negative.";
    */

    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    private M2GEventDispatcher iEventDispatcher = null;
    private ExternalResourceHandler iResourceHandler = null;
    private M2GSVGImage iImage = null;
    private M2GSVGSVGElement iRootElement = null;
    private final M2GLiveElements iLiveElements = new M2GLiveElements(true);
    private M2GConnectionPolicy iConnectionPolicy =
        M2GConnectionFactory.getDefaultPolicy();
    boolean iConnectionRight = true;
    Vector iExternalResources = new Vector();

    /** A base url from where a document was downloaded */
    String iBaseUrl;

    /** The suffix of the url from where the document was open*/
    String iSuffixUrl;

    /**
       * Observer to be notified about changes in DOM
       * @see M2GDOMChangeObserver.notifyDOMChange()
       */
    private M2GDOMChangeObserver  iDOMChangeObserver = null;

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
     * Creates new document object. If aData is null or empty then
     * an empty document is create.
     * @param aImage Svg image
    * @param aBaseUrl A base url from where a document is downloaded.
     * @param aSuffixUrl A suffix url from where a document is downloaded
     * @param aData Plain text svg data
     */
    public M2GDocument(M2GSVGImage aImage, String aBaseUrl, String aSuffixUrl,
                       String aData)
    {
        super();
        
     		System.out.println("P-M2GDocument Constructor getting Called.");
        iImage      = aImage;
        iBaseUrl    = aBaseUrl;
        iSuffixUrl  = aSuffixUrl;

        setHandle(_createDocument(
                      getNativeSVGProxyHandle(),
                      aData)
                 );

        doConstruct();
    }

    /**
    * @see org.w3c.dom.Node#appendChild()
     */
    public Node appendChild(Node newChild) throws DOMException
    {
        // Optimization: if(!getConnectionPolicy().getAccessRight())
        if (!iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI, "appendChild() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }
        Logger.ELOG(Logger.EJavaUI, "appendChild() - exception: "
                    + /*SF*/"Cannot appendChild to a Document node."/*SF*/);
        throw new DOMException(
            DOMException.HIERARCHY_REQUEST_ERR,
            /*SF*/"Cannot appendChild to a Document node."/*SF*/);
    }

    /**
     * Appends external resource uri to internal container
     * @param aUri External resource uri
     * @return true if succeeds
       */
    boolean appendExternalResourceURI(String aUri)
    {
        boolean result = false;
        if (aUri != null && !aUri.equals(""))
        {
            synchronized (iExternalResources)
            {
                if (!iExternalResources.contains(aUri))
                {
                    iExternalResources.addElement(aUri);
                    result = true;
                }
            }
        }
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "appendExternalResourceURI() uri=" +
                   ((aUri != null) ? aUri : "null") + ", result=" + result + " - end");
        return result;
    }

    /**
     * Checks if document contains the element
     * @param id
     * @return true if document contains the element
     */
    public boolean containsElement(String id)
    {
        if (id == null || id.equals(""))
        {
            return false;
        }
        int handle = _getElementById(
                         getNativeSVGProxyHandle(),
                         getHandle(), id);
        return M2GObject.checkHandle(handle);
    }

    /**
     * Check if external resource uri exists in internal container
     * @param aUri External resource uri
     * @return true if uri exists in container
     */
    boolean containsExternalResourceURI(String aUri)
    {
        boolean result = false;
        if (aUri != null && !aUri.equals(""))
        {
            synchronized (iExternalResources)
            {
                result = iExternalResources.contains(aUri);
            }
        }
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "containsExternalResourceURI() uri=" +
                   ((aUri != null) ? aUri : "null") + ", result=" + result + " - end");
        return result;
    }


    /**
     * @see org.w3c.dom.Document#createElementNS()
     */
    public Element createElementNS(String namespaceURI, String qualifiedName)
    {
        if (namespaceURI == null || namespaceURI.equals(""))
        {
            Logger.ELOG(Logger.EJavaUI,
                        "createElementNS() exception because namespace is null. qualifiedName="
                        + qualifiedName);
            throw new NullPointerException(/*SF*/"The namespace URI is null."/*SF*/);
        }
        if (qualifiedName == null || qualifiedName.equals(""))
        {
            Logger.ELOG(Logger.EJavaUI,
                        "createElementNS() exception - " + namespaceURI + ", "
                        + qualifiedName);
            throw new NullPointerException(/*SF*/"The qualifiedName is null."/*SF*/);
        }
        // Checks element validity
        if (!M2GSVGConstants.isQualifiedElement(qualifiedName))
        {
            Logger.ELOG(Logger.EJavaUI, "createElementNS() exception - "
                        + namespaceURI + ", " + qualifiedName);
            throw new DOMException(
                DOMException.NOT_SUPPORTED_ERR,
                /*SF*/"The type of element is not supported by the implementation."/*SF*/);
        }
        // Checks namespace validity
        if (!namespaceURI.equals(M2GSVGConstants.SVG_NAMESPACE_URI))
        {
            Logger.ELOG(Logger.EJavaUI, "createElementNS() exception - "
                        + namespaceURI + ", " + qualifiedName);
            throw new DOMException(
                DOMException.NOT_SUPPORTED_ERR,
                /*SF*/"The URI is not the SVG namespace URI."/*SF*/);
        }
        // Creates native element. A native document owns the new element.
        // NOTE: The element is NOT located in the DOM tree yet.
        int elementHandle = _createElementNS(
                                getNativeSVGProxyHandle(),
                                M2GSVGConstants.parseElementTypeId(qualifiedName),
                                getHandle());
        M2GSVGLocatableElement element = null;
        // Checks native handle validity
        if (M2GObject.checkHandle(elementHandle))
        {
            // Creates java element object
            element = new M2GSVGLocatableElement(elementHandle, this);
            registerLiveElement(element, new Integer(elementHandle));
        }
        else
        {
            Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                       "createElementNS(): " + qualifiedName + " - end, NOK!!!");
        }
        return element;
    }

    /**
     * Checks document validity.
     * @return true if valid
     */
    public boolean doCheckValidity() throws IOException
    {
        boolean result = false;
        if (isHandleValid())
        {
            M2GSVGSVGElement root = getRootElement();
            if (root != null)
            {
                // Check svg version and base profile
                root.checkVersionAndProfile();
                result = true;
            }
        }
        return result;
    }

    /**
     * Cleanup
     * @see com.nokia.microedition.m2g.M2GObject#doCleanup()
     */
    public void doCleanup()
    {
        if (isHandleValid())
        {
            _deleteDocument(
                getNativeSVGProxyHandle(),
                getHandle());
        }
        iImage = null;
        if (iEventDispatcher != null)
        {
            iEventDispatcher.clear();
            iEventDispatcher = null;
        }
        resetHandles();
    }

    /**
     * @see com.nokia.microedition.m2g.M2GObject#doConstruct()
     */
    protected void doConstruct()
    {
        super.doConstruct();
        iEventDispatcher = new M2GEventDispatcher();
    }

    /**
     * Find an element from the handle-to-element map.
     * @see M2GLiveElements#findElement()
     */
    public SVGElement findLiveElement(Integer aElementHandle)
    {
        return iLiveElements.findElement(aElementHandle);
    }

    /**
     * Get connection policy
     * @see M2GConnectionListener#getConnectionPolicy()
     */
    public M2GConnectionPolicy getConnectionPolicy()
    {
        return iConnectionPolicy;
    }

    /**
     * @see org.w3c.dom.Document#getDocumentElement()
       */
    public Element getDocumentElement()
    {
        // Optimization: if(!getConnectionPolicy().getAccessRight())
        if (!iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "getDocumentElement() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }
        return getRootElement();
    }

    /**
    * @see org.w3c.dom.Document#getElementById()
     */
    public Element getElementById(String id)
    {
        // Optimization: if(!getConnectionPolicy().getAccessRight())
        if (!iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI, "getElementById() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }
        if (id == null || id.equals(""))
        {
            Logger.ELOG(Logger.EJavaUI, "getElementById() - exception");
            throw new NullPointerException(/*SF*/"The ID is null."/*SF*/);
        }
        int elementHandle = _getElementById(
                                getNativeSVGProxyHandle(),
                                getHandle(),
                                id );
        return M2GSVGElement.buildElement(elementHandle, this);
    }

    /**
     * Returns the image
     */
    public M2GSVGImage getImage()
    {
        return iImage;
    }

    /**
     * @see org.w3c.dom.Node#getLocalName()
     */
    public String getLocalName()
    {
        // Optimization: if(!getConnectionPolicy().getAccessRight())
        if (!iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI, "getLocalName() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }
        return null;
    }

    /**
    * @see org.w3c.dom.Node#getNamespaceURI()
     */
    public String getNamespaceURI()
    {
        // Optimization: if(!getConnectionPolicy().getAccessRight())
        if (!iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI, "getNamespaceURI() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }
        return null;
    }

    /**
     * @see org.w3c.dom.Node#getParentNode()
     */
    public Node getParentNode()
    {
        // Optimization: if(!getConnectionPolicy().getAccessRight())
        if (!iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "getParentNode() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }
        return null;
    }

    /**
     * Get root element
     * @return M2GSVGSVGElement
       */
    public M2GSVGSVGElement getRootElement()
    {
        if (iRootElement == null)
        {
            iRootElement = M2GSVGSVGElement.buildRootElement(this);
        }
        return iRootElement;
    }

    /**
     * @see javax.microedition.m2g.ScalableImage#getViewportHeight()
     */
    public int getViewportHeight()
    {
        return _getViewportHeight(
                   getNativeSVGProxyHandle(),
                   getHandle());
    }

    /**
     * @see javax.microedition.m2g.ScalableImage#getViewportWidth()
     */
    public int getViewportWidth()
    {
        return _getViewportWidth(
                   getNativeSVGProxyHandle(),
                   getHandle() );
    }


    /**
     * Called by SVGImage upon creating an Event instance
     * Calls the handleEvent() of all the listeners of the event
     */
    public void handleEvent(Event aEvent)
    {
        iEventDispatcher.handleEvent(aEvent);
    }

    /**
    * @see org.w3c.dom.Node#insertBefore()
     */
    public Node insertBefore(Node newChild, Node refChild)
    {
        // Optimization: if(!getConnectionPolicy().getAccessRight())
        if (!iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI, "insertBefore() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }

        Logger.ELOG(Logger.EJavaUI, "insertBefore() DOMException");
        throw new DOMException(
            DOMException.HIERARCHY_REQUEST_ERR,
            /*SF*/"Cannot insertBefore a Document node."/*SF*/);
    }

    /**
     * Checks if element is in DOM
     * @param aElementHandle -
     * @return true if element is in DOM
     */
    public boolean isElementInDOM(int aElementHandle)
    {
        if (!M2GObject.checkHandle(aElementHandle))
        {
            return false;
        }
        return (_isElementInDOM(
                    getNativeSVGProxyHandle(),
                    getHandle(),
                    aElementHandle) > 0 ? true : false);
    }

    /**
     * The external resource handler is invoked
     * for any external resource referenced in the document.
     * @param aUri External resource referenced in the document
     */
    public void invokeResourceHandler(String aUri)
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "invokeResourceHandler() - URI="
                   + aUri + " - begin");

        if ((iImage != null) && (iResourceHandler != null) &&
                (aUri != null) && (aUri.length() > 4))
        {
            StringBuffer urlBuf = new StringBuffer();
            // The uri format for external resource must include the base url
            // from which the document was loaded. It's also checked that the
            // url read from svg doesn't contain already the base part.
            if ((iBaseUrl != null) &&
                    !iBaseUrl.equals("") &&
                    (aUri.indexOf(iBaseUrl) == -1))
            {
                Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                           "invokeResourceHandler() append base url=" + iBaseUrl);
                urlBuf.append(iBaseUrl);
            }

            urlBuf.append(aUri);

            // append the url suffix in case of drm open mode
            if (iSuffixUrl != null && !iSuffixUrl.equals(""))
            {
                Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                           "invokeResourceHandler() append suffix url=" + iSuffixUrl);
                urlBuf.append(iSuffixUrl);
            }

            Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                       "invokeResourceHandler() handler uri=" + urlBuf.toString());

            iResourceHandler.requestResource(iImage, urlBuf.toString());
            appendExternalResourceURI(aUri);
        }

        Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                   "invokeResourceHandler() - URI:" + aUri + " - end");
    }

    /**
     * Registers an event target and an event listener.
     * @param aTarget Target element
     * @param aType Event type
     * @param aListener Event listener
     */
    void register(EventTarget aTarget, String aType, EventListener aListener)
    {
        iEventDispatcher.register(aTarget, aType, aListener);
    }

    /**
     * Register element
     * @see M2GLiveElements#registerElement()
     */
    public void registerLiveElement(SVGElement aElement, Integer aElementHandle)
    {
        iLiveElements.registerElement(aElement, aElementHandle);
    }

    /**
     * @see org.w3c.dom.Node#removeChild()
     */
    public Node removeChild(Node oldChild) throws DOMException
    {
        // Optimization: if(!getConnectionPolicy().getAccessRight())
        if (!iConnectionRight)
        {
            Logger.ELOG(Logger.EJavaUI, "removeChild() - access rights failure");
            throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
        }
        Logger.ELOG(Logger.EJavaUI, "removeChild() DOMException");
        throw new DOMException(
            DOMException.NOT_SUPPORTED_ERR,
            /*SF*/"Cannot removeChild from a Document node."/*SF*/);
    }

    /**
     * Removes external resource uri from internal container
     * @param aUri External resource uri
     * @return true if succeeds
     */
    boolean removeExternalResourceURI(String aUri)
    {
        boolean result = false;
        if (aUri != null && !aUri.equals(""))
        {
            synchronized (iExternalResources)
            {
                result = iExternalResources.removeElement(aUri);
            }
        }
        Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                   "removeExternalResourceURI() uri="
                   + ((aUri != null) ? aUri : "null") + ", result=" + result + " - end");
        return result;
    }

    /*
     * @see com.nokia.microedition.m2g.M2GSVGImage#requestCompleted()
     */
    public int requestCompleted(String aUri, byte[] aResourceData)
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                   "requestCompleted() uri=" + aUri + " - begin");
        // If the aUri contains a base part it will be cut off
        if (!aUri.equals("") &&
                (iBaseUrl != null) &&
                !containsExternalResourceURI(aUri) &&
                (aUri.indexOf(iBaseUrl, 0) != -1))
        {
            aUri = aUri.substring(iBaseUrl.length(), aUri.length());
            Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                       "requestCompleted() uri when base part is cut off=" + aUri);
        }

        // Execute tha request command.
        int status = _requestCompleted(
                         getNativeSVGProxyHandle(),
                         getHandle(), aUri, aResourceData);
        if ((status == 0) || aUri.equals(""))
        {
            Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                       "requestCompleted() remove URI=" + aUri);
            removeExternalResourceURI(aUri);
        }
        return status;
    }

    /**
     * Set connection policy
     * @see M2GConnectionListener#setConnectionPolicy()
     */
    public void setConnectionPolicy(M2GConnectionPolicy aConnectionPolicy)
    {
        iConnectionPolicy = aConnectionPolicy;
        if (iConnectionPolicy != null)
        {
            iConnectionRight = iConnectionPolicy.getAccessRight();
        }
        else
        {
            iConnectionRight = true;
        }
    }

    /**
     * Set image
     * @param aImage Image
     */
    public void setImage(M2GSVGImage aImage)
    {
        iImage = aImage;
    }

    /**
     * Set external resource handler
     * @param aHandler External resource handler
     */
    public void setResourceHandler(ExternalResourceHandler aHandler)
    {
        iResourceHandler = aHandler;
    }

    /**
     * @see javax.microedition.m2g.ScalableImage#setViewportHeight()
     */
    public void setViewportHeight(int height)
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                   "setViewportHeight():" + height + " - begin");
        if (height < 0)
        {
            Logger.ELOG(Logger.EJavaUI, "setViewportHeight():"
                        + height + " - IllegalArgumentException is thrown");
            throw new IllegalArgumentException(/*SF*/"The height is negative."/*SF*/);
        }
        _setViewportHeight(
            getNativeSVGProxyHandle(),
            getHandle(),
            height);
        Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                   "setViewportHeight():" + height + " - end");
    }

    /**
     * @see javax.microedition.m2g.ScalableImage#setViewportWidth()
     */
    public void setViewportWidth(int width)
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "setViewportWidth:" + width + " - begin");
        if (width < 0)
        {
            Logger.ELOG(Logger.EJavaUI, "setViewportWidth():"
                        + width + " - IllegalArgumentException is thrown");
            throw new IllegalArgumentException(/*SF*/"The width is negative."/*SF*/);
        }
        _setViewportWidth(
            getNativeSVGProxyHandle(),
            getHandle(),
            width );
        Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                   "setViewportWidth:" + width + " - end");
    }

    /**
     * Unregister event listener
     * @param aTarget Target element
     * @param aType Event type
     * @param aListener Event listener
     */
    void unregister(EventTarget aTarget, String aType, EventListener aListener)
    {
        iEventDispatcher.unregister(aTarget, aType, aListener);
    }

    /**
     * Unregister an element from the handle-to-element map
     * @see M2GLiveElements#unregisterElement()
     */
    public void unregisterLiveElement(Integer aElementHandle)
    {
        iLiveElements.unregisterElement(aElementHandle);
    }

    //--------------------------------------------------
    // INNER CLASSES
    //--------------------------------------------------
    /**
     * M2GLiveElements
     */
    class M2GLiveElements
    {
        private Hashtable iLiveElements = new Hashtable();
        private boolean iUseWeakReference;

        /**
         * Constructor
         */
        public M2GLiveElements(boolean aUseWeakReference)
        {
            iUseWeakReference = aUseWeakReference;
        }

        /**
        * Find an element from the handle-to-element map.
        * @param aElementHandle Handle
        * @return SVGElement. Null if not found
        */
        public SVGElement findElement(Integer aElementHandle)
        {
            SVGElement element = null;
            Object weakObject = null;
            Object object = null;

            synchronized (this)
            {
                Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                           "** findElement() handle: " + aElementHandle + " - begin **");
                object = iLiveElements.get(aElementHandle);
                if (object != null)
                {
                    if (iUseWeakReference)
                    {
                        weakObject = ((WeakReference)object).get();
                        if (weakObject == null)
                        {
                            Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                                       "** findElement() handle: " + aElementHandle
                                       + " is garbage collected. **");
                            iLiveElements.remove(aElementHandle);
                        }
                        else
                        {
                            element = (SVGElement)weakObject;
                        }
                    }
                    else
                    {
                        element = (SVGElement)object;
                    }
                }
                Logger.LOG(Logger.EJavaUI, Logger.EInfo,"** findElement() handle: " + aElementHandle
                           + ((element != null) ? " FOUND" : " NOT FOUND") + " - end **");
            }
            return element;
        }

        /**
         * Register element
         * @param aElement Element
         * @param aElementHandle Handle
         */
        public void registerElement(SVGElement aElement, Integer aElementHandle)
        {
            if (aElement == null)
            {
                return;
            }
            synchronized (this)
            {
                if (iUseWeakReference)
                {
                    iLiveElements.put(aElementHandle, new WeakReference(aElement));
                }
                else
                {
                    iLiveElements.put(aElementHandle, aElement);
                }
            }
        }

        /**
         * Unregister an element from the handle-to-element map
         * @param aElementHandle Handle
         */
        public void unregisterElement(Integer aElementHandle)
        {
            synchronized (this)
            {
                iLiveElements.remove(aElementHandle);
            }
        }
    }


    /**
     * M2GEventDispatcher
     */
    protected class M2GEventDispatcher
    {
        static public final int INDEX_UNDEFINED = -1;
        static public final int INDEX_CLICK = 0;
        static public final int INDEX_ACTIVATE = 1;
        static public final int INDEX_FOCUS_IN = 2;
        static public final int INDEX_FOCUS_OUT = 3;

        Hashtable[] iEventTypes;
        int iCurrentIndex;
        Hashtable iDelayedRegistrations = new Hashtable();
        M2GEventData iCurrentEventData = new M2GEventData();

        /**
         * Constructor
         */
        public M2GEventDispatcher()
        {
            iEventTypes = new Hashtable[4];
            iCurrentIndex = INDEX_UNDEFINED;
        }

        /**
         * Clear all
         */
        public void clear()
        {
            synchronized (this)
            {
                for (int index = 0; index < iEventTypes.length; index++)
                {
                    if (iEventTypes[index] != null)
                    {
                        iEventTypes[index].clear();
                        iEventTypes[index] = null;
                    }
                }
            }
        }

        /**
         * Execute delayed registration
         * If an EventListener is added to an EventTarget while it is
         * processing an event, it will not be triggered by the current actions.
         */
        private void doDelayedRegistration()
        {
            try
            {
                Enumeration registrations = iDelayedRegistrations.elements();
                while (registrations.hasMoreElements())
                {
                    M2GEventData eventData =
                        (M2GEventData)registrations.nextElement();
                    register(
                        eventData.iTarget, eventData.iType, eventData.iListener);
                }
            }
            catch (NullPointerException e)
            {
            }
            iDelayedRegistrations.clear();
        }


        /**
         * Handle event
         * @param aEvent Event
         */
        public void handleEvent(Event aEvent)
        {
            synchronized (this)
            {
                if ((aEvent == null) || !prepare(aEvent.getType()))
                {
                    Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                               "handleEvent() FAILED - event invalid");
                    return;
                }
                Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                           "handleEvent(): " + aEvent.getType() + " - begin");
                EventTarget target = aEvent.getCurrentTarget();
                if (target == null)
                {
                    Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                               "handleEvent() FAILED - event's target is null");
                    return;
                }
                // Check if wrapped event target
                if (target instanceof M2GSVGElementWrapper)
                {
                    target = ((M2GSVGElementWrapper)target).getWrappedTarget();
                    if (target == null)
                    {
                        Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                                   "handleEvent() FAILED - wrapped event's target is null");
                        return;
                    }
                }
                if (iEventTypes[iCurrentIndex].containsKey(target))
                {
                    Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                               "handleEvent() - target handling begin");
                    iCurrentEventData.set(target, aEvent.getType(), null);
                    Vector listeners =
                        (Vector)iEventTypes[iCurrentIndex].get(iCurrentEventData.iTarget);

                    if (listeners != null)
                    {
                        Enumeration index = listeners.elements();
                        while (index.hasMoreElements())
                        {
                            Object listener = index.nextElement();
                            if ((listener != null) && (listener instanceof EventListener))
                            {
                                ((EventListener)listener).handleEvent(aEvent);
                            }
                            else
                            {
                                Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                                           "handleEvent(): listener is null!");
                            }
                        }
                    }
                    else
                    {
                        Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                                   "handleEvent(): listeners is null!");
                    }
                    iCurrentEventData.clear();
                }
                else
                {
                    Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                               "handleEvent(): target not found");
                }
                // Execute the delayed registration.
                doDelayedRegistration();
            }
        }

        /**
         * Check is event targets are equal
         * @param aLhs Left hand side
         * @param aRhs Right hand side
         * @return True if equal
         */
        private boolean isEquals(EventTarget aLhs, EventTarget aRhs)
        {
            int lhsHandle = M2GObject.INVALID_NATIVE_HANDLE;
            int rhsHandle = M2GObject.INVALID_NATIVE_HANDLE;
            if (aLhs instanceof M2GSVGElement)
            {
                lhsHandle = ((M2GSVGElement)aLhs).getHandle();
            }
            if (aRhs instanceof M2GSVGElement)
            {
                rhsHandle = ((M2GSVGElement)aRhs).getHandle();
            }
            if ((lhsHandle == rhsHandle) && (lhsHandle != M2GObject.INVALID_NATIVE_HANDLE))
            {
                return true;
            }
            return false;
        }

        /**
         * Set an index that points to an event type hash table.
         * The hash table is created if needed.
         * @param aType Type
         */
        private boolean prepare(String aType)
        {
            if (!setIndex(aType))
            {
                return false;
            }
            if (iEventTypes[iCurrentIndex] == null)
            {
                iEventTypes[iCurrentIndex] = new Hashtable();
            }
            return true;
        }

        /**
         * Register an event target and an event listener pair
         * @param aTarget Target
         * @param aType Type
         * @param aListener Listener
         */
        public void register(EventTarget aTarget,
                             String aType,
                             EventListener aListener)
        {
            // NOTE: no need to check validity of aTarget & aListener. It's
            // already done.
            synchronized (this)
            {
                if (!prepare(aType))
                {
                    Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                               "register() - failed: " + aType);
                    return;
                }
                // Check if delayed registration is needed
                // If an EventListener is added to an EventTarget while it is
                // processing an event, it will not be triggered by
                // the current actions.
                if ((iCurrentEventData.iType == aType) &&
                        isEquals(iCurrentEventData.iTarget, aTarget))
                {
                    try
                    {
                        iDelayedRegistrations.put(
                            aListener,
                            new M2GEventData(aTarget, aType, aListener));
                    }
                    catch (NullPointerException e)
                    {
                    }
                    return;
                }
                Vector listeners = null;
                if (iEventTypes[iCurrentIndex].containsKey(aTarget))
                {
                    listeners = (Vector)iEventTypes[iCurrentIndex].get(aTarget);
                }
                else
                {
                    listeners = new Vector();
                    iEventTypes[iCurrentIndex].put(aTarget, listeners);
                }
                // Don't add if the event target already contains the same listener
                // for the same event.
                if (!listeners.contains(aListener))
                {
                    listeners.addElement(aListener);
                }
            }
        }

        /**
         * Set an event type container index
         * @param aType Type
         * @return False if the type is invalid
         */
        private boolean setIndex(String aType)
        {
            if (aType.equals(M2GSVGConstants.EVENT_ACTIVATE))
            {
                iCurrentIndex = INDEX_ACTIVATE;
            }
            else if (aType.equals(M2GSVGConstants.EVENT_CLICK))
            {
                iCurrentIndex = INDEX_CLICK;
            }
            else if (aType.equals(M2GSVGConstants.EVENT_FOCUS_IN))
            {
                iCurrentIndex = INDEX_FOCUS_IN;
            }
            else if (aType.equals(M2GSVGConstants.EVENT_FOCUS_OUT))
            {
                iCurrentIndex = INDEX_FOCUS_OUT;
            }
            else
            {
                Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                           "setIndex() type:" + aType + " FALSE - end");
                // Wrong event type
                iCurrentIndex = INDEX_UNDEFINED;
                return false;
            }
            return true;
        }

        /**
         * Unregister an event target and an event listener pair
         * @param aTarget Target
         * @param aType Type
         * @param aListener Listener
         */
        public void unregister(EventTarget aTarget,
                               String aType,
                               EventListener aListener)
        {
            synchronized (this)
            {
                if (!prepare(aType))
                {
                    return;
                }
                // Remove delayed registration
                iDelayedRegistrations.remove(aListener);
                // Remove real registration
                Vector listeners = (Vector)iEventTypes[iCurrentIndex].get(aTarget);
                boolean removed = false;
                if (listeners != null)
                {
                    removed = listeners.removeElement(aListener);
                    if (listeners.isEmpty())
                    {
                        iEventTypes[iCurrentIndex].remove(aTarget);
                    }
                }
                if (removed)
                {
                    Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                               "unregister() ok - end");
                }
                else
                {
                    Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                               "unregister() failed - end");
                }
            }
        }

        /**
         * Helper class for handling delayed registration
         */
        class M2GEventData
        {
            public EventTarget iTarget = null;
            public String iType = null;
            public EventListener iListener = null;

            /**
             * Ctor
             */
            public M2GEventData()
            {
            }

            /**
             * Ctor
             * @param aTarget Target
             * @param aType Event type
             * @param aListener Event listener
             */
            public M2GEventData(
                EventTarget aTarget,
                String aType,
                EventListener aListener)
            {
                set(aTarget, aType, aListener);
            }

            /**
             * Clearing
             */
            public void clear()
            {
                iTarget = null;
                iType = null;
                iListener = null;
            }

            /**
             * Setter
             * @param aTarget Target
             * @param aType Event type
             * @param aListener Event listener
             */
            public void set(EventTarget aTarget,
                            String aType,
                            EventListener aListener)
            {
                iTarget = aTarget;
                iType = aType;
                iListener = aListener;
            }
        }
    }

    //--------------------------------------------------
    // STATIC METHODS
    //--------------------------------------------------

    /**
     * Creates an empty SVG document. The document contains a
     * root <svg> element with default viewport size of 100x100.
       * @param aImage -
       * @param aHander -
       * @return document object
       * @throws IllegalArgumentException
       * @throws IOException
       */
    static public M2GDocument buildEmptyDocument(M2GSVGImage aImage,
            ExternalResourceHandler aHandler)
    throws IllegalArgumentException, IOException
    {
        return M2GDocument.buildDocument(
                   aImage,
                   null,
                   null,
                   M2GSVGConstants.getInstance().iEmptySvgDocument,
                   aHandler);
    }

    /**
     * Builds document
     * @param aImage Svg image
    * @param aBaseUrl A base url from where a document is downloaded.
    * @param aSuffixUrl The suffix url from where a document is downloaded.
     * @param aData Plain text svg data
     * @param aHandler Resource handler. If null the default resource handler is
     * used.
     * @return document object
     * @throws IllegalArgumentException
     * @throws IOException
     */
    static public M2GDocument buildDocument(M2GSVGImage aImage,
                                            String aBaseUrl,
                                            String aSuffixUrl,
                                            String aData,
                                            ExternalResourceHandler aHandler)
    throws IOException
    {
    		System.out.println("P-M2GDocument");
        if ((aData == null) || (aData.equals("")))
        {
        		System.out.println("P-M2GDocument (aData == null) || (aData.equals()");
            throw new IOException(
                /*SF*/"The SVG document does not conform to the XML 1.0 specification."/*SF*/);
        }

        M2GDocument document = new M2GDocument(aImage, aBaseUrl, aSuffixUrl, aData);

        if (document.doCheckValidity())
        {
            document.setResourceHandler(aHandler);
            return document;
        }
        throw new IOException(
            /*SF*/"The SVG document does not conform to the XML 1.0 specification."/*SF*/);
    }

    /**
     * Registers an observer to any DOM change
     * @since S60 3.2
     */
    public void registerDOMChangeObserver(M2GDOMChangeObserver aDOMChangeObserver)
    {
        iDOMChangeObserver = aDOMChangeObserver;
    }

    /**
     * Notify the observer about any change in DOM
     * SVGSVGElement is always part of DOM
     * @since S60 3.2
     */
    public void notifyDOMChangeObserver()
    {
        if (iDOMChangeObserver != null)
        {
            iDOMChangeObserver.notifyDOMChange();
        }
    }

    /**
     * Notify the observer about any change in DOM only if the node is in DOM
     * @param aNativeHandle the handle of a node
     * @since S60 3.2
     */
    public void notifyDOMChangeObserver(int aNativeHandle)
    {
        if (iDOMChangeObserver != null && isElementInDOM(aNativeHandle))
        {
            iDOMChangeObserver.notifyDOMChange();
        }
    }


    //--------------------------------------------------
    // NATIVE METHODS
    //--------------------------------------------------
    private native static int _createDocument(
        int aSvgProxyHandle, String aString);

    private native static int _createElementNS(
        int aSvgProxyHandle, short aType,
        int aDocumentHandle);

    private native static void _deleteDocument(
        int aSvgProxyHandle, int aDocumentHandle);

    private native static int _getElementById(
        int aSvgProxyHandle, int aDocumentHandle,
        String aId);

    private static native int _getViewportHeight(
         int aSvgProxyHandle, int aDocumentHandle);

    private static native int _getViewportWidth(
         int aSvgProxyHandle, int aDocumentHandle);

    private native static int _isElementInDOM(
         int aSvgProxyHandle, int aDocumentHandle,
        int aElementHandle);

    private static native int _requestCompleted(
        int aSvgProxyHandle, int aDocumentHandle,
        String aUri, byte[] aResourceData);

    private static native void _setViewportHeight(
        int aSvgProxyHandle, int aDocumentHandle, int aHeight);

    private static native void _setViewportWidth(
        int aSvgProxyHandle, int aDocumentHandle, int aWidth);
}

