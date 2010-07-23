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
 * This interface represents an "SVGPath" datatype used to define the path geometry.
 * Corresponds to SVG path specification or the "d" attribute.
 *
 *
 * <p>The native implementations must support the following simplifications or
 * canonicalization of path segments. Any simplifications should be lossless.
 *
 * <ul>
 * <li>Relative commands (c, h, l, m, q, s, t, and v) must be converted to their absolute counterparts·</li>
 * <li>Horizontal and Vertical lines (H, h, V, and v) must be converted to general lines (L and l)·</li>
 * <li>Translate command S to command C·</li>
 * <li>Translate command T to command Q.</li>
 * </ul>
 * </p>
 */
public interface SVGPath
{

    /**
     * Numeric value is ASCII code of the letter 'M'.
     */
    public static final short MOVE_TO = 77;

    /**
     * Numeric value is ASCII code of the letter 'L'.
     */
    public static final short LINE_TO = 76;

    /**
     * Numeric value is ASCII code of the letter 'C'.
     */
    public static final short CURVE_TO = 67;

    /**
     * Numeric value is ASCII code of the letter 'Q'.
     */
    public static final short QUAD_TO = 81;

    /**
     * Numeric value is ASCII code of the letter 'Z'.
     */
    public static final short CLOSE = 90;


    /**
     * Return number of segments in this path.
     *
     * @return the number of segments in this path.
     */
    public int getNumberOfSegments();

    /**
     * Returns segment command by zero-based command index. Returns one of MOVE_TO, LINE_TO, CURVE_TO, QUAD_TO or CLOSE.
     *
     * @param cmdIndex the command index for the segment command to retrieve.
     * @return the segment command for the specified cmdIndex.
     * @throws DOMException with error code INDEX_SIZE_ERR if segment index out of bounds.
     *
     */
    public short getSegment(int cmdIndex)
    throws DOMException;

    /**
     * Returns segment parameter by zero-based command index and zero-based parametr index.
     *
     * @param cmdIndex the command index for the segment parameter to retrieve.
     * @param paramIndex the parameter index for the segment parameter to retrieve.
     * @return the segment parameter for the specified cmdIndex and paramIndex.
     * @throws DOMException with error code INDEX_SIZE_ERR if segment index out of bounds or param index out of bounds for this segment's type.
     */
    public float getSegmentParam(int cmdIndex, int paramIndex)
    throws DOMException;

    /**
     * Appends 'M' (absolute move) segment to the path with the specified coordinates.
     *
     * @param x the x-axis coordinate for the specified point.
     * @param y the y-axis coordinate for the specified point.
     */
    public void moveTo(float x, float y);

    /**
     * Appends 'L' (absolute line) segment to the path with the specified coordinates.
     *
     * @param x the x-axis coordinate of the specified point.
     * @param y the y-axis coordinate of the specified point.
     */
    public void lineTo(float x, float y);

    /**
     * Appends 'Q' (absolute quadratic curve) segment to the path.
     *
     * @param x1 the x-axis coordinate of the first control point.
     * @param y1 the y-axis coordinate of the first control point.
     * @param x2 the x-axis coordinate of the final end point.
     * @param y2 the y-axis coordinate of the final end point.
     *
     */
    public void quadTo(float x1, float y1, float x2, float y2);

    /**
     * Appends 'C' (absolute cubic curve) segment to the path.
     *
     * @param x1 the x-axis coordinate of the first control point.
     * @param y1 the y-axis coordinate of the first control point.
     * @param x2 the x-axis coordinate of the second end point.
     * @param y2 the y-axis coordinate of the second end point.
     * @param x3 the x-axis coordinate of the final end point.
     * @param y3 the y-axis coordinate of the final end point.
     *
     */
    public void curveTo(float x1, float y1, float x2, float y2, float x3, float y3);

    /**
     * Appends 'Z' (close path) segment to the path
     */
    public void close();
}
