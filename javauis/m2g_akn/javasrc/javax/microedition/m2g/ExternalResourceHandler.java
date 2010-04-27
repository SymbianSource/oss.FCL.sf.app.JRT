/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


package javax.microedition.m2g;


/**
* <p>This interface is used to load external resources that are referenced
* within a ScalableImage, namely within an SVG document in the case of SVG.
* In SVGT 1.1, these resources are not required for displaying the SVG document.
* If no ExternalResourceHandler is registered when creating the image,
* the engine will retrieve these resources using its own implementation
* of the requestResource method.</p>
*<p>
* The default engine behaviour can be overwritten by passing to {@link javax.microedition.m2g.ScalableImage#createImage createImage}
* an implementation of this interface as paramater when the default implementation
* is insufficient. For example, if the SVGImage is loaded from within a
* Jar file, the implementation will not be able to load images that
* have relative URIs (e.g., &lt;image xlink:href="myImage.png" /&gt;).
* All the external resources are requested by the engine at the time of
* the <code>createImage</code> call. Note that SVGT 1.1 does
* not contain external references to SVG files, therefore an external
* resource can not contain subsequent external resources. </p>
*
* @see      javax.microedition.m2g.ScalableImage
*/
public interface ExternalResourceHandler
{

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
     * @param URI the URI for the external resource.
     */
    public void requestResource(ScalableImage image, String URI);

};
