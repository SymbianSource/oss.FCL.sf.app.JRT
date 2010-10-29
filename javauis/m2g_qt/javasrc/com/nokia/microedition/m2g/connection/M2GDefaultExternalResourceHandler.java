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

package com.nokia.microedition.m2g.connection;


import java.io.*;
import javax.microedition.io.*;
import javax.microedition.m2g.ExternalResourceHandler;
import javax.microedition.m2g.*;


/**
* <p>This private class is the default resource handler
* used to load external resources that are referenced
* within a ScalableImage, namely within a SVG document in the case of SVG.
* The default resource handler will be used by the platform in the following cases:
*   - if the MIDlet has not explicitly set its own handler to load external resources
*   - if the resource handler specified by the MIDlet is not able to load the
*     external resources (i.e. the MIDlet's SVGResourceHandle.getExternalResource()
*     returns null).</p>
* @see javax.microedition.m2g.ExternalResourceHandler
*/
public class M2GDefaultExternalResourceHandler implements ExternalResourceHandler
{

    /*
     * According to RFC 2396, the rules for determening the base URI can be summarized
     * as follows (highest priority to lowest):
     * 1.The base URI is embedded in the document's content.
     * 2.The base URI is that of the encapsulating document.
     * 3.The base URI is the URI used to retrieve  the entity.
     * 4.The base URI is defined by the context of the application
     *
     * Note that in the case of the M2GDefaultExternalResourceHandler  4. is not
     * applicable.
     */

    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    private M2GUrlTokenizer iTokenizer = null;

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
     * This constructor is used to create a default resource handler that can
     * load resources from absolute URLs and from URLs that are relative
     * to the aUrl.
     * NOTE: This handler will not be allowed to load resources from the application's
     * jar file.
     *
     * @param aUrl The already validated url used to load the svg document
     */
    M2GDefaultExternalResourceHandler(String aUrl)
    {
        if (aUrl == null || aUrl.equals(""))
        {
            iTokenizer = new M2GUrlTokenizer();
        }
        else
        {
            iTokenizer = new M2GUrlTokenizer(aUrl);
        }
    }

    /**
     * This constructor is used to create a default resource handler that can
     * load external resources from fully qualified URLs ONLY. Any relative URL
     * will be considered to be a link to a resource within the application's jar file.
     */
    M2GDefaultExternalResourceHandler()
    {
        this(null);
    }

    /**
      * This method is invoked when an external resource is required by the underlying implementation.
      * When the request is completed by the implementation of this handler, a notification
      * must be sent to the SVG engine through the requestCompleted() method of <code>ScalableImage</code>.
      * To get a synchronous behaviour, requestCompleted() can be called in the implementation
      * of requestResource. If called later, rendering the document before
      * completing all the requests will just display the currently available content.
      * Once the request is completed, the image will have to be redrawn to reflect the newly
      * available data.
      * For more details on required resources, please refer to the <code>externalResourcesRequired</code>
      * attribute description in the SVG specification.
      *
      * @param image image that originated the external data request
      * @param URL the an absolute or a relative URL for the external resource + drm mode
      */
    public void requestResource(ScalableImage aImage, String aUrl)
    {
        M2GConnectionProxy proxy = null;

        /* IMPLEMENTATION NOTE:
         * The default resource handler assumes that if there is an embedded base URL within the
         * document then is upon the svg engine to provide the absolute URL.
         * If the given URL is a relative URL then this handler will attempt to use as a base URL
         * to retrieve the containing svg document. However, if the document's base
         * URL is null then this handler will assume the given relative URL to be a file name
         * within the application's jar file.
         */
        try
        {
            // NOTE: if exception is thrown during the resolveExternalResourceUrl() call then
            // the requestCompleted should be called again with a null input stream.
            proxy = M2GConnectionFactory.resolveExternalResourceUrl(iTokenizer, aUrl);
            aImage.requestCompleted(aUrl, proxy.getInputStream());
        }
        catch (Exception e)
        {
            // e.printStackTrace();
            // The requestCompleted() failed
            try
            {
                aImage.requestCompleted(aUrl, null);
            }
            catch (Exception ee)
            {
                // ee.printStackTrace();
            }
        }
        // Close streams
        finally
        {
            try
            {
                if (proxy != null)
                {
                    proxy.close();
                }
            }
            catch (Exception e)
            {
                // e.printStackTrace();
            }
        }
    }
}


