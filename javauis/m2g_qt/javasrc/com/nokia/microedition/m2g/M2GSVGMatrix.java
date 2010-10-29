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

import org.w3c.dom.svg.*;
import org.w3c.dom.DOMException;

/**
 * This interface represents an "SVGMatrix" datatype, identified by an affine transform.
 * It can be used to read and modify the values of transform attribute as per SVG specification.
 *
 * The transformation can be represented using matrix math on a 3x3 array.
 * Given (x,y), the transformation (x',y') can be found by:
 * [ x']   [ m00 m01 m02 ] [ x ]   [ m00*x + m01*y + m02 ]
 * [ y'] = [ m10 m11 m12 ] [ y ] = [ m10*x + m11*y + m12 ]
 * [ 1 ]   [  0   0   1  ] [ 1 ]   [          1          ]
 *
 * The bottom row of the matrix is constant, so a transform can be uniquely
 * represented by "[[m00, m01, m02], [m10, m11, m12]]".
 */
public class M2GSVGMatrix implements SVGMatrix
{
    //--------------------------------------------------
    // STATIC CONSTANTS
    //--------------------------------------------------
    /* Optimization: static finals changed to local variables
    static private final String INVALID_INDEX_ESTR =
    "The index is invalid.";
    static private final String DET_IS_ZERO_ESTR =
    "The determinant of this matrix is zero.";
    */
    static final int ARRAY_SIZE = 6;
    // Array index
    static private final int M00 = 0;
    static private final int M10 = 1;
    static private final int M01 = 2;
    static private final int M11 = 3;
    static private final int M02 = 4;
    static private final int M12 = 5;

    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    private float iComponents[];

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
     * Construct a matrix with the following components:
     * <pre>
     * [1 0 0]
     * [0 1 0]
     * </pre>
     */
    protected M2GSVGMatrix()
    {
        iComponents = new float[ARRAY_SIZE];
        iComponents[M00] = iComponents[M11] = 1;
    }

    /**
    /**
     * Construct a matrix with the following components:
     * <pre>
     * [aM00 aM01 aM02]
     * [aM10 aM11 aM12]
     * </pre>
     * @param aM00 the x scaling component
     * @param aM10 the y shearing component
     * @param aM01 the x shearing component
     * @param aM11 the y scaling component
     * @param aM02 the x translation component
     * @param aM12 the y translation component
     */
    protected M2GSVGMatrix(float aM00, float aM10, float aM01,
                           float aM11, float aM02, float aM12)
    {
        iComponents = new float[ARRAY_SIZE];
        iComponents[M00] = aM00;
        iComponents[M01] = aM01;
        iComponents[M02] = aM02;
        iComponents[M10] = aM10;
        iComponents[M11] = aM11;
        iComponents[M12] = aM12;
    }

    /**
     * Constructor
     * Create a new matrix by coping the given one.
     * @param aMatrix the matrix to copy
     * @throws NullPointerException if aMatrix is null
     * @throws DOMException if index sizes are not same
     */
    protected M2GSVGMatrix(M2GSVGMatrix aMatrix)
    {
        if (aMatrix == null)
        {
            throw new NullPointerException();
        }
        iComponents = new float[aMatrix.iComponents.length];
        for (int index = 0; index < iComponents.length; index++)
        {
            iComponents[index] = aMatrix.iComponents[index];
        }
    }

    /**
     * @see org.w3c.dom.svg.SVGMatrix#getComponent()
     */
    public float getComponent(int index) throws DOMException
    {
        if ((index < 0) || (index >= iComponents.length))
        {
            throw new DOMException(
                DOMException.INDEX_SIZE_ERR,
                /*SF*/"The index is invalid."/*SF*/);
        }
        return iComponents[index];
    }

    /**
     * Return the matrix of components used in this transform. The resulting
     * values are:
     * <pre>
     * [array[0] array[2] array[4]]
     * [array[1] array[3] array[5]]
     * </pre>
     * @return array that contains the matrix components.
     */
    float[] getComponents()
    {
        return iComponents;
    }

    /**
     * Get the matrix of components used in this transform. The resulting
     * values are:
     * <pre>
     * [aComponents[0] aComponents[2] aComponents[4]]
     * [aComponents[1] aComponents[3] aComponents[5]]
     * </pre>
     * @param aComponents Float array for matrix components
     */
    void getComponentsToArray(float[] aComponents)
    {
        if (aComponents == null)
        {
            return;
        }
        for (int index = 0; index < aComponents.length; index++)
        {
            aComponents[index] = iComponents[index];
        }
    }

    /**
     * Return the determinant of this transform matrix. If the determinant is
     * non-zero, the transform is invertible.
     * The determinant is calculated as:
     * <pre>
     * [m00 m01 m02]
     * [m10 m11 m12] = m00 * m11 - m01 * m10
     * [ 0   0   1 ]
     * </pre>
     * @return the determinant
     */
    public float getDeterminant()
    {
        return ((iComponents[M00] * iComponents[M11]) -
                (iComponents[M01] * iComponents[M10]));
    }

    /**
    * The inverse is calculated as:
     * <pre>
     *     [m00 m01 m02]
     *  M= [m10 m11 m12]
     *     [ 0   0   1 ]
     *
     *              1                 [ m11/det  -m01/det   (m01*m12-m02*m11)/det]
     * inverse(M)= --- x adjoint(M) = [-m10/det   m00/det   (m10*m02-m00*m12)/det]
     *             det                [    0         0               1           ]
     * </pre>
     * @see org.w3c.dom.svg.SVGMatrix#inverse()
     */
    public SVGMatrix inverse() throws SVGException
    {
        // The inversion is useful for undoing transformations.
        float det = getDeterminant();
        if (det == 0)
        {
            throw new SVGException(
                SVGException.SVG_MATRIX_NOT_INVERTABLE,
                /*SF*/"The determinant of this matrix is zero."/*SF*/);
        }
        return new M2GSVGMatrix(
                   iComponents[M11] / det, // iMtx[M00]
                   (-iComponents[M10]) / det, // iMtx[M10]
                   (-iComponents[M01]) / det, // iMtx[M01]
                   iComponents[M00] / det, // iMtx[M11]
                   ((iComponents[M01] * iComponents[M12]) - (iComponents[M02] * iComponents[M11])) / det, // iMtx[M02]
                   ((iComponents[M10] * iComponents[M02]) - (iComponents[M00] * iComponents[M12])) / det); // iMtx[M12]
    }

    /**
     * The multiply is calculated as:
     * <pre>
     *       [a00 a01 a02]   [b00 b01 b02]
     *  this=[a10 a11 a12] B=[b10 b11 b12]
     *       [ 0   0   1 ]   [ 0   0   1 ]
     *
     *                       [(a00*b00+a01*b10) (a00*b01+a01*b11) (a00*b02+a01*b12+a02)]
     * [this] = [this]x[B] = [(a10*b00+a11*b10) (a10*b01+a11*b11) (a10*b02+a11*b12+a12)]
     *                       [       0                   0                     1       ]
     * </pre>
     * @see org.w3c.dom.svg.SVGMatrix#mMultiply()
     */
    public SVGMatrix mMultiply(SVGMatrix secondMatrix)
    {
        if (secondMatrix == null)
        {
            throw new NullPointerException();
        }
        M2GSVGMatrix b = (M2GSVGMatrix)secondMatrix;
        float a00 = iComponents[M00]; // a
        float a10 = iComponents[M10]; // b
        float a01 = iComponents[M01]; // c
        float a11 = iComponents[M11]; // d
        float a02 = iComponents[M02]; // e
        float a12 = iComponents[M12]; // f
        iComponents[M00] = (a00 * b.iComponents[M00]) + (a01 * b.iComponents[M10]); // a
        iComponents[M10] = (a10 * b.iComponents[M00]) + (a11 * b.iComponents[M10]); // b
        iComponents[M01] = (a00 * b.iComponents[M01]) + (a01 * b.iComponents[M11]); // c
        iComponents[M11] = (a10 * b.iComponents[M01]) + (a11 * b.iComponents[M11]); // d
        iComponents[M02] = (a00 * b.iComponents[M02]) + (a01 * b.iComponents[M12]) + a02; // e
        iComponents[M12] = (a10 * b.iComponents[M02]) + (a11 * b.iComponents[M12]) + a12; // f
        return this;
    }

    /**
     * The rotation is calculated as:
     * <pre>
     *          [ cos(angle) -sin(angle) 0 ]
     * [this] x [ sin(angle)  cos(angle) 0 ]
     *          [     0           0      1 ]
     * </pre>
     * @see org.w3c.dom.svg.SVGMatrix#mRotate()
     */
    public SVGMatrix mRotate(float angle)
    {
        if (angle % 360 == 0)
        {
            return this;
        }
        // must convert degrees to radians since java.lang.Math expects radians
        angle = angle * (float)Math.PI / 180.0f;
        float c = (float)Math.cos(angle);
        float s = (float)Math.sin(angle);
        float m00 = iComponents[M00];
        float m10 = iComponents[M10];
        float m01 = iComponents[M01];
        float m11 = iComponents[M11];
        iComponents[M00] = m00 * c + m01 * s;
        iComponents[M10] = m10 * c + m11 * s;
        iComponents[M01] = m01 * c - m00 * s;
        iComponents[M11] = m11 * c - m10 * s;
        return this;
    }

    /**
     * The multiply is calculated as:
     * <pre>
     *       [m00 m01 m02]   [scaleFactor      0      0]
     *  this=[m10 m11 m12] B=[    0       scaleFactor 0]
     *       [ 0   0   1 ]   [    0            0      1]
     *
     *                       [(a00*scaleFactor) (a01*scaleFactor) a02]
     * [this] = [this]x[B] = [(a10*scaleFactor) (a11*scaleFactor) a12]
     *                       [       0                   0         1 ]
     * </pre>
     * @see org.w3c.dom.svg.SVGMatrix#mScale()
     */
    public SVGMatrix mScale(float scaleFactor)
    {
        if (scaleFactor == 1)
        {
            return this;
        }
        iComponents[M00] *= scaleFactor;
        iComponents[M01] *= scaleFactor;
        iComponents[M10] *= scaleFactor;
        iComponents[M11] *= scaleFactor;
        return this;
    }

    /**
     * @see org.w3c.dom.svg.SVGMatrix#mTranslate()
     */
    public SVGMatrix mTranslate(float x, float y)
    {
        if (x == 0 && y == 0)
        {
            return this;
        }
        iComponents[M02] += (iComponents[M00] * x) + (iComponents[M01] * y);
        iComponents[M12] += (iComponents[M10] * x) + (iComponents[M11] * y);
        return this;
    }

    /**
     * To string
     */
    static String toString(float aComponents[])
    {
        if ((aComponents == null) || (M2GSVGMatrix.ARRAY_SIZE != aComponents.length))
        {
            return "";
        }
        StringBuffer buf = new StringBuffer();
        buf.append("[00]:").append(aComponents[M00]).append(", [10]:").append(aComponents[M10]);
        buf.append(", [01]:").append(aComponents[M01]).append(", [11]:").append(aComponents[M11]);
        buf.append(", [02]:").append(aComponents[M02]).append(", [12]:").append(aComponents[M12]);
        return buf.toString();
    }
}
