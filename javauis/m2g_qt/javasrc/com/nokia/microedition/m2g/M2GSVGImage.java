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

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import javax.microedition.m2g.ExternalResourceHandler;
import javax.microedition.m2g.SVGImage;
import org.w3c.dom.Document;
import org.w3c.dom.svg.SVGElement;
import org.w3c.dom.DOMException;
import com.nokia.microedition.m2g.connection.*;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.utils.StreamUtils;


/*
 * Image implementation
 */
public class M2GSVGImage
        extends SVGImage
{
    //--------------------------------------------------
    // STATIC CONSTANTS
    //--------------------------------------------------
    /* Optimization: static finals changed to local variables
    private static final String NULL_TYPE_ESTR =
      "The type is null.";
    private static final String ILLEGAL_VALUS_ESTR =
      "The x or y values are negative.";
    private static final String EVENT_TYPE_NOT_SUPPORTED_ESTR =
      "The event type is not supported.";
    private static final String INVALID_ELEMENT_ESTR =
      "Invalid element.";
    public static final String URI_IS_NULL_ESTR =
      "The URI is null.";
    */

    //--------------------------------------------------
    //  VARIABLES
    //--------------------------------------------------
    M2GDocument iDocument = null;
    private SVGElement iFocusedNode = null;

    //--------------------------------------------------
    //  METHODS
    //--------------------------------------------------
    /**
     * Constructor
     */
    M2GSVGImage()
    {
        super();
    }

    /**
     * @see javax.microedition.m2g.SVGImage#activate()
     */
    public void activate()
    {
        SVGElement node = iFocusedNode;
        while (node != null)
        {
            ((M2GDocument)getDocument()).handleEvent(new M2GEvent(
                        M2GSVGConstants.EVENT_ACTIVATE,
                        node,
                        M2GEvent.EVENT_TARGET));
            node = (SVGElement)node.getParentNode();
        }
    }

    /**
     * @see javax.microedition.m2g.SVGImage#dispatchMouseEvent()
     */
    public void dispatchMouseEvent(String type, int x, int y)
    {
        if (type == null || type.equals(""))
        {
            Logger.ELOG(Logger.EJavaUI, "dispatchMouseEvent() NullPointerException");
            throw new NullPointerException(/*SF*/"The type is null."/*SF*/);
        }
        if (x < 0 || y < 0)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "dispatchMouseEvent() IllegalArgumentException");
            throw new IllegalArgumentException(/*SF*/"The x or y values are negative."/*SF*/);
        }
        if (!type.equals(M2GSVGConstants.EVENT_CLICK))
        {
            Logger.ELOG(Logger.EJavaUI,
                        "dispatchMouseEvent() DOMException - only click event is supported");
            throw new DOMException(DOMException.NOT_SUPPORTED_ERR, /*SF*/"The event type is not supported."/*SF*/);
        }
        int elementHandle = _dispatchMouseEvent(
                                iDocument.getNativeSVGProxyHandle(),
                                iDocument.getHandle(), x, y);

        SVGElement targetElement = M2GSVGElement.buildElement(elementHandle, iDocument);
        SVGElement usedFromElement = null;
        if ((targetElement != null) && ((M2GSVGElement)targetElement).isUsed())
        {
            usedFromElement = ((M2GSVGElement)((M2GSVGElement)targetElement).getUsedFromElement());
        }
        if (targetElement instanceof M2GSVGElement)
        {
            iFocusedNode = targetElement;
        }
        while (targetElement != null)
        {
            // click event
            iDocument.handleEvent(new M2GEvent(
                                      M2GSVGConstants.EVENT_CLICK, targetElement, M2GEvent.EVENT_TARGET));
            // DOMActivate event
            iDocument.handleEvent(new M2GEvent(
                                      M2GSVGConstants.EVENT_ACTIVATE, targetElement, M2GEvent.EVENT_TARGET));
            // Gets parent node
            targetElement = (SVGElement)targetElement.getParentNode();
        }
        // The "target element" is only a clone. The "used from element" is the actual element.
        if (usedFromElement != null)
        {
            iDocument.handleEvent(new M2GEvent(
                                      M2GSVGConstants.EVENT_CLICK, usedFromElement, M2GEvent.WRAPPED_EVENT_TARGET));
            iDocument.handleEvent(new M2GEvent(
                                      M2GSVGConstants.EVENT_ACTIVATE, usedFromElement, M2GEvent.WRAPPED_EVENT_TARGET));
        }
    }

    /**
     * @see javax.microedition.m2g.SVGImage#focusOn()
     */
    public void focusOn(SVGElement element)
    {
        SVGElement tempNode = element;
        // If focusOn is called on the same element, do not focus out.
        boolean callFocusOut =
            (((iFocusedNode != null) && (iFocusedNode == element ? false : true)) || ((iFocusedNode != null) && (element == null)));
        //Checking to see if the element is hooked into the tree.
        if (element != null)
        {
            tempNode = element;
            while (tempNode != null)
            {
                if ((tempNode.getLocalName()).equals(M2GSVGConstants.ROOT_SVG_ELEMENT_NAME))
                {
                    // Breaks if tempNode is root svg
                    break;
                }
                tempNode = (SVGElement)tempNode.getParentNode();
            }
            // This indicates that the element does not contain a root element,
            // therefore does not belong to the document.
            if (tempNode == null)
            {
                Logger.ELOG(Logger.EJavaUI, "focusOn() - DOMException invalid element");
                throw new DOMException(DOMException.WRONG_DOCUMENT_ERR, /*SF*/"Invalid element."/*SF*/);
            }
        }
        // Focus out from the current focused node and its parents
        if (callFocusOut)
        {
            if (iFocusedNode instanceof M2GSVGElement)
            {
                _focusOut(
                    ((M2GSVGElement)iFocusedNode).getNativeSVGProxyHandle(),
                    ((M2GSVGElement)iFocusedNode).getDocument().getHandle(),
                    ((M2GSVGElement)iFocusedNode).getHandle());
            }

            // Focus out the current focused node and its parents
            tempNode = iFocusedNode;
            while (tempNode != null)
            {
                iDocument.handleEvent(new M2GEvent(
                                          M2GSVGConstants.EVENT_FOCUS_OUT, tempNode, M2GEvent.EVENT_TARGET));
                tempNode = (SVGElement)tempNode.getParentNode();
            }
        }
        //focus in for element and its parents
        if ((element != null) && (element instanceof M2GSVGElement))
        {
            _focusOn(
                ((M2GSVGElement)element).getNativeSVGProxyHandle(),
                ((M2GSVGElement)element).getDocument().getHandle(),
                ((M2GSVGElement)element).getHandle() );
        }
        tempNode = element;
        while (tempNode != null)
        {
            iDocument.handleEvent(new M2GEvent(
                                      M2GSVGConstants.EVENT_FOCUS_IN, tempNode, M2GEvent.EVENT_TARGET));
            tempNode = (SVGElement)tempNode.getParentNode();
        }
        // Sets the new focus
        iFocusedNode = element;
    }

    /**
     * @see javax.microedition.m2g.SVGImage#getDocument()
     */
    public Document getDocument()
    {
        return iDocument;
    }

    /**
     * @see javax.microedition.m2g.ScalableImage#getViewportHeight()
     */
    public synchronized int getViewportHeight()
    {
        return iDocument.getViewportHeight();
    }


    /*
     * @see javax.microedition.m2g.ScalableImage#getViewportWidth()
     */
    public synchronized int getViewportWidth()
    {
        return iDocument.getViewportWidth();
    }

    /**
     * @see javax.microedition.m2g.SVGImage#incrementTime()
     */
    public void incrementTime(float seconds)
    {
        ((M2GSVGSVGElement)iDocument.getDocumentElement()).incrementTime(seconds);
    }

    /**
     * @see javax.microedition.m2g.ScalableImage#requestCompleted()
     */
    public void requestCompleted(String uri, InputStream resourceData) throws IOException
    {
        if (uri == null)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "requestCompleted() throw NullPointerException");
            throw new NullPointerException(/*SF*/"The URI is null."/*SF*/);
        }

        if (resourceData != null)
        {
            byte receiveData[] = StreamUtils.readBytesFromStream(resourceData, -1);
            iDocument.requestCompleted(uri, receiveData);
        }
        else
        {
            // Indicates that the requested resource could not be
            // fetched by the ResourceHandler or application,
            // and in this event the SVG engine will not make further attempts
            // to load this resource.
            // Pass the null to the native side so it can delete the image
            iDocument.requestCompleted(uri, null);
        }
    }

    /**
     * Set document
     * @param aDocument -
     */
    public void setDocument(M2GDocument aDocument)
    {
        iDocument = aDocument;
    }

    /**
     * @see javax.microedition.m2g.ScalableImage#setViewportHeight()
     */
    public synchronized void setViewportHeight(int height)
    {
        iDocument.setViewportHeight(height);
    }

    /**
     * @see javax.microedition.m2g.ScalableImage#setViewportWidth()
     */
    public synchronized void setViewportWidth(int width)
    {
        iDocument.setViewportWidth(width);
    }

    //--------------------------------------------------
    //  STATIC METHODS
    //--------------------------------------------------
    /*
     * @see javax.microedition.m2g.SVGImage#createEmptyImage()
     */
    public static M2GSVGImage buildEmptyImage(ExternalResourceHandler handler)
    {
        try
        {
            return M2GSVGImage.buildImage(
                       new ByteArrayInputStream(
                           M2GSVGConstants.getInstance().iEmptySvgDocument.getBytes()),
                       handler, null, null);
        }
        catch (IOException e)
        {
            Logger.ELOG(Logger.EJavaUI, "M2GSVGImage: buildEmptyImage - IOEception: " + e.toString());
        }
        return null;
    }

    /**
     * Build svg image
     * @see javax.microedition.m2g.ScalableImage#createImage()
     * @param stream Plain text stream that contains svg data
     * @param handler External resource handler. If null then default
     * external resource handler is used.
     * @param baseUrl A base url from where a document is downloaded.
     * @param suffixUrl The suffix url according to the DRM opening mode of the SVG file
     * Null if it's not known.
     */
    public static M2GSVGImage buildImage(
        InputStream stream,
        ExternalResourceHandler handler,
        String baseUrl,
        String suffixUrl) throws IOException
    {
        if (stream == null)
        {
            throw new NullPointerException();
        }
				

        String strData = new String(StreamUtils.readBytesFromStream(stream, -1));

        // Default handler is used if the specified handler is null
        if (handler == null)
        {
            handler = M2GConnectionFactory.getExternalResourceHandler(null);
        }

        // Creates and setups svg image
        M2GSVGImage image = new M2GSVGImage();
        
        M2GDocument document = M2GDocument.buildDocument(
                                   image,
                                   baseUrl,
                                   suffixUrl,
                                   strData,
                                   handler);
        // Checks document validity
				
        image.setDocument(document);
        
        if (handler != null)
        {
            // Gets size of external resources
            int itemCount = _getExternalListSize(
                                document.getNativeSVGProxyHandle(),
                                document.getHandle());

            // Loops through external items
            for (int index = 0; index < itemCount; index++)
            {
                String url = _getExternalListItem(
                                 document.getNativeSVGProxyHandle(),
                                 document.getHandle(),
                                 index);
                // Calls an external implementation of the resource handler
                if ((url != null) && !(url.equals("")))
                {
                    document.invokeResourceHandler(url);
                }
            }
        }
        
        // initialise the viewport
        _initViewport(document.getNativeSVGProxyHandle(),
                      document.getHandle());
        

        return image;
    }

    /**
     * @see javax.microedition.m2g.ScalableImage#createImage()
     */
    public static M2GSVGImage buildImage(
        String url, ExternalResourceHandler handler) throws IOException
    {
        // Checks passed parameters
        if (url == null)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "M2GSVGImage: 2 buildImage() - url is null");
            throw new NullPointerException();
        }
        if (url.length() == 0)
        {
            Logger.ELOG(Logger.EJavaUI,
                        "M2GSVGImage: 2 buildImage() - url is empty");
            throw new IllegalArgumentException();
        }
        // Default handler is used if the specified handler is null
        if (handler == null)
        {
            handler = M2GConnectionFactory.getExternalResourceHandler(url);
        }

        // Open the url and get input stream
        M2GSVGImage image = null;
        M2GConnectionProxy connectionProxy = null;
        try
        {
            connectionProxy = M2GConnectionFactory.create(url);
            image = M2GSVGImage.buildImage(
                        connectionProxy.getInputStream(),
                        handler,
                        connectionProxy.getBaseUrl(),
                        connectionProxy.getSuffixUrl());
            // Set connection policy
            if (image != null)
            {
                Document doc = image.getDocument();
                if (doc != null)
                {
                    ((M2GDocument)doc).setConnectionPolicy(
                        connectionProxy.getConnectionPolicy());
                }
            }
            return image;
        }
        finally
        {
            if (connectionProxy != null)
            {
                connectionProxy.close();
                connectionProxy = null;
            }
        }
    }

    //--------------------------------------------------
    // NATIVE METHODS
    //--------------------------------------------------
    private native static int _dispatchMouseEvent(
        int aSvgProxyHandle,int aDocumentHandle, 
        int aX, int aY);

    private native static void _focusOn(
        int aSvgProxyHandle,int aDocumentHandle, 
        int aSvgElementHandle);

    private native static void _focusOut(
        int aSvgProxyHandle,int aDocumentHandle, 
        int aSvgElementHandle);

    native static int _getExternalListSize(
         int aSvgProxyHandle, int aDocumentHandle);

    native static String _getExternalListItem(
        int aSvgProxyHandle, int aDocumentHandle,
        int aIndex);

    private native static void _initViewport(
        int aSvgProxyHandle, int aDocumentHandle);
}
