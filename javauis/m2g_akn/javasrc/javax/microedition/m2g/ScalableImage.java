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

import java.io.IOException;

/**
 * This class models images in vector format, such as the Scalable Vector
 * Graphics (SVG) image format.  Therefore, it is required that all classes
 * representing "scalable" images extend this class.
 * @see      javax.microedition.m2g.SVGImage
 */
public abstract class ScalableImage
{
    /**
     * Constructor
     */
    protected ScalableImage()
    {
    }

    /**
     * This method creates and loads a ScalableImage (e.g. SVG) from the specified stream.
     * A ScalableImage can only be rendered using the render() method in the ScalableGraphics
     * context.The image size is determined by the content specification
     * (eg: width/height attributes on root SVG element). The default viewport size
     * of 100-by-100 pixels is used when the size is unspecified. This method will throw an exception
     * when the image is in error. For SVG document, the document is in error when:
     * <ul>
     * <li>the SVG document does not conform to the XML 1.0 specification</li>
     * <li>an element has an attribute or property value which is not permissible according to the SVG specification </li>
     * <li>the required or mandatory attributes according to the SVG specification are missing</li>
     * <li>the document contains circular references on the &lt;use&gt; element</li>
     * <li>the document contains &lt;image&gt; element(s) with local references </li>
     * <li>the document contains elements with duplicate Id's</li>
     * <li>the document contains animation(s) in error</li>
     * </ul>
     *
     * <p>If an <code>handler</code> is specified, the engine will invoke it for any
     * external resource referenced in the document. However, please note that data URIs
     * (for ex: base64 encoded images like &lt;image xlink:href="data:image/png;base64,/9j/4AAQ..."/&gt;)
     * are required to be decoded by the engine, and therefore the handler will not be invoked
     * in such cases. All these requests must occur before createImage returns, and the
     * engine must only make one call if there exist multiple resources
     * with the same URI. If the <code>handler</code> is set to null, the ScalableImage
     * will try to load images automatically using the engine's default implementation,
     * but might not be able to load all of them. For example, if the ScalableImage
     * is loaded from a file in a Jar file, the implementation will not be able
     * to load images that have relative URIs (ex; &lt;image xlink:href="myImage.png" /&gt;).
     * If the handler is not able to locate a requested resource, it informs the SVG engine
     * by calling the requestCompleted method with <code>null</code> for resourceData. </p>
     *
     * <p>Note that the <code>handler</code> is also called when the xlink:href attribute on
     * &lt;image&gt; is set or changed by the application, but the call is made only when the
     * element is hooked into the document tree i.e. when the ancestors go all the way up to
     * the root &lt;svg&gt; element. There are two cases:
     * <ul>
     * <li>When changing the xlink:href attribute of an existing &lt;image&gt; element that is already hooked or part of the tree.</li>
     * <li>When creating a new &lt;image&gt;, setting its xlink:href and hooking it to the document tree.</li>
     * </ul>
     * </p>
     *
     * @param stream The stream from which the SVG content should be read.
     * @param handler implementation of the ExternalResourceHandler interface
     * @see      javax.microedition.m2g.ExternalResourceHandler
     *
     * @return the loaded ScalableImage.
     * @throws IOException if an error occurs while loading the content.
     * @throws NullPointerException if <code>stream</code> is null.
     */
    public static ScalableImage createImage(java.io.InputStream stream,
                                            ExternalResourceHandler handler) throws IOException
    {
        return SVGImage.createImage(stream, handler);
    }


    /**
     * This method creates and loads a ScalableImage (e.g. SVG) with the specified
     * URI. A ScalableImage can only be rendered using the render() method in the ScalableGraphics
     * context.The image size is determined by the content specification
     * (eg: width/height attributes on root SVG element). The default viewport size
     * of 100-by-100 pixels is used when the size is unspecified. This method will throw an
     * exception when the image is in error. For SVG document, the document is in error when:
     * <ul>
     * <li>the SVG document does not conform to the XML 1.0 specification</li>
     * <li>an element has an attribute or property value which is not permissible according to the SVG specification </li>
     * <li>the required or mandatory attributes according to the SVG specification are missing</li>
     * <li>the document contains circular references on the &lt;use&gt; element</li>
     * <li>the document contains &lt;image&gt; element(s) with local references </li>
     * <li>the document contains elements with duplicate Id's</li>
     * <li>the document contains animation(s) in error</li>
     * </ul>
     *
     *
     * <p>If an <code>handler</code> is specified, the engine will invoke it for any
     * external resource referenced in the document. However, please note that data URIs
     * (for ex: base64 encoded images like &lt;image xlink:href="data:image/png;base64,/9j/4AAQ..."/&gt;)
     * are required to be decoded by the engine, and therefore the handler will not be invoked
     * in such cases. All these requests must occur before createImage returns, and the
     * engine must only make one call if there exist multiple resources
     * with the same URI. If the <code>handler</code> is set to null, the ScalableImage
     * will try to load images automatically using the engine's default implementation,
     * but might not be able to load all of them. For example, if the ScalableImage
     * is loaded from a file in a Jar file, the implementation will not be able
     * to load images that have relative URIs (ex: &lt;image xlink:href="myImage.png" /&gt;).
     * If the handler is not able to locate a requested resource, it informs the SVG engine
     * by calling the requestCompleted method with <code>null</code> for resourceData. </p>
     *
     * <p>Note that the <code>handler</code> is also called when the xlink:href attribute on
     * &lt;image&gt; is set or changed by the application, but the call is made only when the
     * element is hooked into the document tree i.e. when the ancestors go all the way up to
     * the root &lt;svg&gt; element. There are two cases:
     * <ul>
     * <li>When changing the xlink:href attribute of an existing &lt;image&gt; element that is already hooked or part of the tree.</li>
     * <li>When creating a new &lt;image&gt;, setting its xlink:href and hooking it to the document tree.</li>
     * </ul>
     * </p>
     *
     * <p>
     * If the platform implementation supports the JSR-75 FileConnection, then the file: URLs
     * as used by JSR-75 shall be supported and the images are loaded from files identified
     * by this locator. This can be used to load possible DRM protected image files from the
     * file system.
     *
     * @param url A string in URL syntax that identifies the image source.
     * @param handler implementation of the ExternalResourceHandler interface
     * @see      javax.microedition.m2g.ExternalResourceHandler
     *
     * @return the loaded ScalableImage
     * @throws IOException if an error occurs while loading the SVG content.
     * @throws NullPointerException if <code>URL</code> is null.
     * @throws java.lang.IllegalArgumentException if the specified locator is of unsupported type.
     * @throws SecurityException if the application does not have the privilege rights to access
     * the contents of this resource.
     *
     */
    public static ScalableImage createImage(String url,
                                            ExternalResourceHandler handler) throws IOException
    {
        return SVGImage.createImage(url, handler);
    }

    /**
     * This method returns the ScalableImage's viewport height. The initial
     * viewport height is taken from the "height" value specified in the Scalable Image. The value
     * returned is always in pixels. If the specified height is defined in percentages, the
     * values are mapped to the default view port size of 100x100. If the viewport height
     * is explicitly changed by the application, then the percentages are ignored and the content
     * is made to fit to this new viewport height.
     *
     * @return the current height of this ScalableImage.
     * @see #setViewportHeight
     */
    public abstract int getViewportHeight();

    /**
     * This method returns the ScalableImage's viewport width. The initial
     * viewport width is taken from the "width" value specified in the Scalable Image. The value
     * returned is always in pixels. If the specified width is defined in percentages, the
     * values are mapped to the default view port size of 100x100. If the viewport width
     * is explicitly changed by the application, then the percentages are ignored and the content
     * is made to fit to this new viewport width.
     *
     * @return the current width of this ScalableImage.
     * @see #setViewportWidth
     */
    public abstract int getViewportWidth();

    /**
    * Once the requested external resource is available, the application forwards this information
    * (resourceData) to the SVG engine. If this method is called a second time for a same URL of a
    * same <code>SVGImage</code>, the engine will replace the current resource data with the new
    * one. <b>Note:</b> Setting <code>null</code> for resourceData indicates that the requested resource
    * could not be fetched by the <code>ResourceHandler</code> or application, and in this
    * event the SVG engine will not make further attempts to load this resource.
    *
    * @param URI URI that was requested through requestExternalResource
    * @param resourceData inputstream containing the external resource
    *
    * @see      javax.microedition.m2g.ExternalResourceHandler
    *
    * @throws IOException if an error occurs while loading the resource.
    * @throws NullPointerException if URL is null.
    */
    public abstract void requestCompleted(String URI, java.io.InputStream resourceData) throws IOException;

    /**
     * This method sets the new (viewport) height of this ScalableImage.
     *
     * @param height the new height to be set.
     * @see #getViewportHeight
     * @throws IllegalArgumentException if height is negative.
     */
    public abstract void setViewportHeight(int height);

    /**
     * This method sets the new (viewport) width of this ScalableImage.
     * The viewport is the area where the ScalableImage is rendered. Any parts
     * of the viewport that lie outside the boundaries of the target clipping
     * rectangle are clipped. The viewport upper left corner (x, y) is given
     * relative to the upper left corner of the target rendering surface.
     *
     * @param width the new width to be set.
     * @see #getViewportWidth
     * @throws IllegalArgumentException if width is negative.
     */
    public abstract void setViewportWidth(int width);
}
