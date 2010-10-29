/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

//--------------------------------------------------
//Imports
//--------------------------------------------------
import java.io.*;
import java.io.InputStream;
import org.w3c.dom.Document;
import org.w3c.dom.DOMException;
import org.w3c.dom.svg.SVGElement;
import com.nokia.microedition.m2g.M2GSVGImage;

/**
* This class represents an SVG image conforming to the W3C SVG Tiny 1.1 Profile. Applications can access the Document
* associated with this SVGImage using <code>getDocument</code> method. Using the <code>Document</code> class,
* it is possible to interact with the content and perform operations such as zoom, pan, and rotate
* apart from various other functions. The event dispatching methods as described
* in this class must be used for dispatching user specific events to the underlying SVG engine.
*
* <p>
* The width and height values specified in the SVGImage are only used to compute the initial viewport width/height.
* It is also important to note that according to the SVG 1.1 specification, if viewBox is not defined, then
* the preserveAspectRatio attribute is ignored. Therefore, the contents should not be scaled if there is no
* viewBox defined.
* </p>
* <p><b>Note:</b> If the application does not have the necessary privilege rights to access this (SVG)
* content, a SecurityException may be thrown by the underlying implementation. This is applicable
* to all the tree navigation (see {@link org.w3c.dom.Node Node}) and trait acessor methods (see {@link org.w3c.dom.svg.SVGElement SVGElement}).
* Features such as zooming, panning and playing of animations will not be affected.
* <p>
* Here is the list of events supported in this specification:
* <p>
* <table  cellpadding="2" cellspacing="2" border="1"
 *  style="border-collapse: collapse;">
 *   <tbody>
 *     <tr>
 *       <th width="150" valign="top" bgcolor="#999999"><b>Events<br>
 *       </b> </th>
 *       <th valign="top" bgcolor="#999999"><b>Description<br>
 *       </b> </th>
 *     </tr>
 *     <tr>
 *       <td valign="top"> <b>"DOMFocusIn"</b><br>
 *       </td>
 *       <td valign="top"> Occurs when the element comes into focus.<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td valign="top"> <b>"DOMFocusOut"</b><br>
 *       </td>
 *       <td valign="top"> Occurs when an element goes out of focus.<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td valign="top"> <b>"DOMActivate"</b><br>
 *       </td>
 *       <td valign="top"> Occurs when an element is activated, for instance, through a keypress (select)<br>
 *       </td>
 *     </tr>
 *     <tr>
 *       <td valign="top"> <b>"click"</b><br>
 *       </td>
 *       <td valign="top"> Occurs when the pointing device button is clicked over the SVG content (or particular SVGElement) <br>
 *       </td>
 *     </tr>
 *   </tbody>
 * </table>
* @see      javax.microedition.m2g.ScalableImage
* @see      org.w3c.dom.Document
*/

public class SVGImage extends ScalableImage
{
    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    private M2GSVGImage iImg = null;

    /**
     * Constructor
     */
    protected SVGImage()
    {
        super();
    }

    /**
     * This method is used to dispatch an "DOMActivate" event to the document.
     * The element that has focus is activated, which means that a
     * "DOMActivate" event with the the currently focused element as target is
     * dispatched. If no element has focus, this method does not do anything
     * and no event is dispatched.
     */
    public void activate()
    {
        iImg.activate();
    }

    /**
     * This method creates and loads an empty SVGImage (skeleton) that can be used to
     * programatically construct a simple SVG image. Note: A root &lt;svg&gt; element
     * with default viewport size of 100x100 is also created inside this method.
     *
     * <p>If an handler is specified, the engine will invoke it for any external resource
     * referenced in the document. If the handler is set to null, the SVGImage will try to
     * load images automatically using the engine default implementation,but might not be
     * able to load all of them.</p>
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
     * @param handler implementation of the ExternalResourceHandler interface
     * @see      javax.microedition.m2g.ExternalResourceHandler
     *
     * @return an empty SVGImage
     */
    public static SVGImage createEmptyImage(ExternalResourceHandler handler)
    {
        M2GSVGImage image = M2GSVGImage.buildEmptyImage(handler);
        if (image == null)
        {
            throw new NullPointerException();
        }
        SVGImage proxy = new SVGImage();
        proxy.iImg = image;
        return proxy;
    }

    /**
     * @see javax.microedition.m2g.ScalableImage
     */
    public static ScalableImage createImage(
        InputStream stream, ExternalResourceHandler handler) throws IOException
    {
        M2GSVGImage image = M2GSVGImage.buildImage(stream, handler, null, null);
        if (image == null)
        {
            throw new NullPointerException();
        }
        SVGImage proxy = new SVGImage();
        proxy.iImg = image;
        return proxy;
    }

    /**
     * @see javax.microedition.m2g.ScalableImage
     */
    public static ScalableImage createImage(
        String url, ExternalResourceHandler handler) throws IOException
    {
        M2GSVGImage image = M2GSVGImage.buildImage(url, handler);
        if (image == null)
        {
            throw new NullPointerException();
        }
        SVGImage proxy = new SVGImage();
        proxy.iImg = image;
        return proxy;
    }

    /**
     * This method is used to dispatch a mouse event of the specified <code>type</code> to the
     * document. The mouse position is given as screen coordinates <code>x, y</code>. If the
     * x, y values are outside the viewport area or no target is available for the x, y
     * coordinates, the event is not dispatched. Note that when a "click" event is dispatched,
     * a "DOMActivate" is automatically dispatched by the underlying implementation. The only
     * required mouse event type is "click". Therefore, if an unsupported type is specified,
     * a DOMException with error code NOT_SUPPORTED_ERR is thrown.
     *
     *
     * @param type the type of mouse event.
     * @param x the x location of the mouse/pointer in viewport coordinate
     *        system.
     * @param y the y location of the mouse/pointer in viewport coordinate
     *        system.
     * @throws DOMException with error code NOT_SUPPORTED_ERR: if the event <code>type</code> is not supported.
     * @throws NullPointerException if <code>type</code> is null.
     * @throws IllegalArgumentException if the x or y values are negative.
     *
     */
    public void dispatchMouseEvent(String type, int x, int y) throws DOMException
    {
        iImg.dispatchMouseEvent(type, x, y);
    }

    /**
     * Returns the associated <code>Document</code>.
     * @return the associated <code>Document</code>.
     */
    public Document getDocument()
    {
        return iImg.getDocument();
    }

    /**
     * @see javax.microedition.m2g.ScalableImage
     */
    public int getViewportHeight()
    {
        return iImg.getViewportHeight();
    }

    /**
     * @see javax.microedition.m2g.ScalableImage
     */
    public int getViewportWidth()
    {
        return iImg.getViewportWidth();
    }

    /**
     * This method triggers a "DOMFocusIn" event with the specified element as
     * the event target. That element becomes the element with focus. This
     * method also triggers a "DOMFocusOut" event with the element which
     * previous had focus as target. When the activate method is called, a
     * "DOMActivate" event is triggered with the currently focused element as
     * the target. The initial focus is always <code>null</code> and setting
     * <code>null</code> will remove the current focus.
     *
     * @param element the element to set the focus on.
     * @throws DOMException with error code WRONG_DOCUMENT_ERR: if invalid element is passed (for
     * ex: an element that does not belong to this document).
     */
    public void focusOn(SVGElement element)  throws DOMException
    {
        iImg.focusOn(element);
    }

    /**
     * Increments the animation or media timeline for this SVGImage (in seconds). As the name
     * implies, this method is intended to move only forward in the timeline and typically should be used
     * to animate SVG content when the SVGAnimator class in not used. Setting negative values will throw
     * an Exception. It is important to note that setting large increments of time would result in
     * skipping parts of the animation as per the SVG animation model.
     *
     * @param seconds the value of time to increment in seconds.
     * @throws IllegalArgumentException if the specified time is negative.
     */
    public void incrementTime(float seconds)
    {
        iImg.incrementTime(seconds);
    }

    /**
     * @see javax.microedition.m2g.ScalableImage
     */
    public void requestCompleted(
        String URI, InputStream resourceData) throws IOException
    {
        iImg.requestCompleted(URI, resourceData);
    }

    /**
     * @see javax.microedition.m2g.ScalableImage
     */
    public void setViewportHeight(int height)
    {
        iImg.setViewportHeight(height);
    }

    /**
     * @see javax.microedition.m2g.ScalableImage
     */
    public void setViewportWidth(int width)
    {
        iImg.setViewportWidth(width);
    }
}

