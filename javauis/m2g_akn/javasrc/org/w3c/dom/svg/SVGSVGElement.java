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
package org.w3c.dom.svg;

import org.w3c.dom.DOMException;

/**
 * <p>This interface represents &lt;svg&gt; element in (SVG) document tree.</p>
 * <h4>User Agent Transforms</h4>
 * <p>
 * The DOM attributes currentScale, currentRotate and currentTranslate are combined to form user agent transformation
 * which is applied at the outermost level on the SVG document (i.e., outside the outermost 'svg' element) if "magnification"
 * is enabled (i.e., zoomAndPan attribute is set to "magnify"). Their values
 * can potentialy be modified through user-agent specific UI. User agent transformation can be obtained
 * by multiplying matrix
 * <p>
 * <pre>
 * [currentScale      0       currentTranslate.x]       [cos(currentRotate) -sin(currentRotate 0]
 * [     0      currentScale  currentTranslate.y]  by   [sin(currentRotate) cos(currentRotate) 0]
 * [     0            0               1         ]       [         0                  0         1]
 * </pre>
 * </p>
 *
 * i.e. (translate, then scale, then rotate the coordinate system). The reference point for scale and rotate operations is the origin (0, 0).
 * </p>
 *
 * <p><b>Note:</b> If the application does not have the necessary privilege rights to access this (SVG)
 * content, a SecurityException may be thrown by the underlying implementation. This is applicable
 * to all the Tree navigation and Trait acessor methods. Features such as zooming, panning and
 * playing of animations will not be affected.
 * </p>
 */
public interface SVGSVGElement extends SVGLocatableElement
{
    /**
     * Sets current user agent scale (zoom) coefficient.
     *
     * @param value the value of user agent scale coefficient to be set.
     * @throws DOMException with error code INVALID_ACCESS_ERR if the scale value is set to zero.
     */
    public void setCurrentScale(float value)
    throws DOMException;

    /**
     * Returns current user agent scale (zoom) coefficient. The initial value for currentScale is 1.
     *
     * @return the current user agent scale coefficient.
     */
    public float getCurrentScale();

    /**
     * Sets current user agent rotate coefficient in degrees.
     *
     * @param value the value of user agent rotate coefficient to be set.
     */
    public void setCurrentRotate(float value);

    /**
     * Returns current user agent rotation angle in degrees. The initial value for currentRotate is 0.
     *
     * @return the current user agent rotation coefficient in degrees.
     */
    public float getCurrentRotate();

    /**
     * Current user agent translation used for scrolling or panning (The returned {@link org.w3c.dom.svg.SVGPoint SVGPoint} object is "live" and setting its
     * x and y components will change user agent's translation). The initial values for currentTranslate is SVGPoint(0,0).
     *
     * @return returns the current user agent translation.
     */
    public SVGPoint getCurrentTranslate();

    /**
     * Returns current animation timeline time in seconds.
     *
     * @return the current animation timeline time in seconds.
     */
    public float getCurrentTime();

    /**
     * Sets current animation timeline time (in seconds). This API is required to support moving
     * forwards in timeline. The underlying implementations are normally designed to seek
     * forward in time and setting the time backwards is not meant to play the animation backwards.
     * Note: Moving backwards in time is a costly feature for the implementations to support.
     *
     * @param seconds the value of time to be set in seconds.
     */

    public void setCurrentTime(float seconds);

    /**
     * Creates new {@link org.w3c.dom.svg.SVGMatrix SVGMatrix} object. This object can be used to modify value of traits which are compatible with {@link org.w3c.dom.svg.SVGMatrix SVGMatrix}
     * type using {@link org.w3c.dom.svg.SVGElement#setMatrixTrait setMatrixTrait} method. The internal representation of the matrix is as follows:
     * <p>
     * <pre>
     *  [  a  c  e  ]
     *  [  b  d  f  ]
     *  [  0  0  1  ]
     * </pre>
     * </p>
     *
     * @param a the 'a' component of the matrix to be set.
     * @param b the 'b' component of the matrix to be set.
     * @param c the 'c' component of the matrix to be set.
     * @param d the 'd' component of the matrix to be set.
     * @param e the 'e' component of the matrix to be set.
     * @param f the 'f' component of the matrix to be set.
     *
     * @return the newly created SVGMatrix object.
     *
     * @see org.w3c.dom.svg.SVGMatrix
     */

    public SVGMatrix createSVGMatrixComponents(float a, float b, float c, float d, float e, float f);

    /**
     * Creates new {@link org.w3c.dom.svg.SVGRect SVGRect} object. This object can be used to modify value of traits which are compatible with {@link org.w3c.dom.svg.SVGRect SVGRect}
     * type using {@link org.w3c.dom.svg.SVGElement#setRectTrait setRectTrait} method. The intial values for x, y, width, height of this new SVGRect are zero.
     *
     * @return the newly created SVGRect object.
     */

    public SVGRect createSVGRect();

    /**
     * Creates new {@link org.w3c.dom.svg.SVGPath SVGPath} object. This object can be used to modify value of traits which are compatible with {@link org.w3c.dom.svg.SVGPath SVGPath}
     * type using {@link org.w3c.dom.svg.SVGElement#setPathTrait setPathTrait} method.
     *
     * @return the newly created SVGPath object with empty path commands.
     */

    public SVGPath createSVGPath();

    /**
     * Creates new {@link org.w3c.dom.svg.SVGRGBColor SVGRGBColor} object. This object can be used to modify value of traits which are compatible with {@link org.w3c.dom.svg.SVGRGBColor SVGRGBColor}
     * type using {@link org.w3c.dom.svg.SVGElement#setRGBColorTrait setRGBColorTrait} method.
     *
     * @param red the red component of SVGRGBColor object.
     * @param green the green component of SVGRGBColor object.
     * @param blue the blue component of SVGRGBColor object.
     *
     * @return the newly created SVGRGBColor object with specified (r,g,b) values.
     *
     * @throws SVGException with error code SVG_INVALID_VALUE_ERR: if any of the parameters is not in the 0..255 range.</li>
     */
    public SVGRGBColor createSVGRGBColor(int red, int green, int blue)
    throws SVGException;
}
