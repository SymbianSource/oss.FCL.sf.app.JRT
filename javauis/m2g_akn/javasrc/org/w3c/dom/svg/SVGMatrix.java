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

import org.w3c.dom.svg.SVGException;
import org.w3c.dom.DOMException;

/**
 * This interface represents an "SVGMatrix" datatype, identified by an affine transform.
 * It can be used to read and modify the values of transform attribute as per SVG specification.
 * Note that the mTranslate, mMultiply, mScale and mRotate methods in this interface
 * mutate the SVGMatrix object and return a reference to the SVGMatrix instance itself, after
 * performing the necessary matrix operation.
 * <p>This matrix transforms source coordinates (x, y) into destination coordinates (x', y') by
 * considering them to be a column vector and multiplying the coordinate vector by the matrix
 * according to the following process:</p>
 *
 * <p>
 * <pre>
 *  [ x' ]    [  a  c  e  ]   [ x ]    [ a.x + c.y + e ]
 *  [ y' ] =  [  b  d  f  ]   [ y ] =  [ b.x + d.y + f ]
 *  [ 1  ]    [  0  0  1  ]   [ 1 ]    [        1      ]
 * </pre>
 * </p>
 */
public interface SVGMatrix
{

    /**
     * Returns a component of the matrix by component's zero-based index. <code>getComponent(0)</code> is a, <code>getComponent(1)</code> is b, etc.
     *
     * @param index the index of the matrix component to retrieve.
     * @return the component for the specified index.
     * @throws DOMException  - INDEX_SIZE_ERR if the <code>index</code> is invalid.
     */
    public float getComponent(int index)
    throws DOMException;

    /**
     * Performs matrix multiplication. This matrix is post-multiplied by another matrix, returning the resulting current matrix.
     *
     * @param secondMatrix the matrix to post-multiply with.
     * @return the resulting current matrix after post-multiplication.
     * @throws NullPointerException  - if secondMatrix is null.
     */
    public SVGMatrix mMultiply(SVGMatrix secondMatrix);

    /**
     * Returns a new instance of SVGMatrix containing the inverse of the current matrix.
     *
     * @return the inverse of the current matrix.
     * @throws SVGException  - SVG_MATRIX_NOT_INVERTABLE when determinant of this matrix is zero.
     */
    public SVGMatrix inverse()
    throws SVGException;

    /**
     * Post-multiplies a translation transformation on the current matrix and returns the resulting current matrix.
     * This is equivalent to calling <code>multiply(T)</code>, where <code>T</code> is an
     * <code>SVGMatrix</code> object represented by the following
     * matrix:
     *
     * <p>
     * <pre>
     *      [   1    0    x  ]
     *      [   0    1    y  ]
     *      [   0    0    1   ]
     * </pre>
     * </p>
     *
     * @param x the distance by which coordinates are translated
     * in the X axis direction.
     * @param y the distance by which coordinates are translated
     * in the Y axis direction.
     * @return the resulting current matrix after post-multiplication.
     */
    public SVGMatrix mTranslate(float x, float y);

    /**
     * Post-multiplies a uniform scale transformation on the current matrix and returns the resulting current matrix.
     * This is equivalent to calling <code>multiply(S)</code>, where <code>S</code> is an <code>SVGMatrix</code>
     * object represented by the following matrix:
     *
     * <p>
     * <pre>
     *      [   scaleFactor      0          0   ]
     *      [   0          scaleFactor      0   ]
     *      [   0                0          1   ]
     * </pre>
     * </p>
     *
     * @param scaleFactor the factor by which coordinates are scaled along the
     * X and Y axis.
     * @return the resulting current matrix after post-mutiplication.
     */
    public SVGMatrix mScale(float scaleFactor);

    /**
     * Post-multiplies a rotation transformation on the current matrix and returns the resulting current matrix.
     * This is equivalent to calling <code>multiply(R)</code>, where <code>R</code> is an
     * <code>SVGMatrix</code> object represented by the following matrix:
     *
     * <p>
     * <pre>
     *      [ cos(angle) -sin(angle) 0 ]
     *      [ sin(angle)  cos(angle) 0 ]
     *      [ 0           0          1 ]
     * </pre>
     * </p>
     *
     * @param angle the angle of rotation in degrees.
     * @return the resulting current matrix after post-multiplication.
     */
    public SVGMatrix mRotate(float angle);
}
