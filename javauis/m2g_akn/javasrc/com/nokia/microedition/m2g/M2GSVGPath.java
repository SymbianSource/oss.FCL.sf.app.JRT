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

import org.w3c.dom.svg.*;
import org.w3c.dom.DOMException;
import java.io.IOException;
import com.nokia.mj.impl.utils.Logger;

/**
 * This interface represents an "SVGPath" datatype used to define the path geometry.
 * Corresponds to SVG path specification or the "d" attribute.
 */
public class M2GSVGPath extends M2GObject implements SVGPath
{
    //--------------------------------------------------
    // STATIC CONTANTS
    //--------------------------------------------------
    /* Optimization: static finals changed to local variables
    private static final String SEGMENT_INDEX_SIZE_ESTR =
    "The segment index is out of bounds.";
    private static final String PARAMETER_INDEX_SIZE_ESTR =
    "The parameter index is out of bounds for this segment's type.";
    */
    private static final int CLOSE_PARAMETER_COUNT = 0;
    private static final int MOVE_TO_PARAMETER_COUNT = 2;
    private static final int LINE_TO_PARAMETER_COUNT = 2;
    private static final int CURVE_TO_PARAMETER_COUNT = 6;
    private static final int QUAD_TO_PARAMETER_COUNT = 4;

    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------

    /**
     * Constructor
     * @param aHandle - native path object handle
     * @param aDoCleanup - true if the native object needed to be deleted
     */
    protected M2GSVGPath(int aHandle)
    {
        super();
        setHandle(aHandle);
        doConstruct();
    }

    /**
    * @see com.nokia.microedition.m2g.M2GObject#doCheckValidity()
     */
    public boolean doCheckValidity()
    {
        // SVGPath is invalid if it begins with any segment other
        // than MOVE_TO segment. Note that an empty SVGPath is still a valid value.
        try
        {
            if (!super.doCheckValidity() ||
                    ((getNumberOfSegments() != 0) && (getSegment(0) != MOVE_TO)))
            {
                return false;
            }
        }
        catch (IOException e)
        {
            return false;
        }
        return true;
    }

    /**
     * @see com.nokia.microedition.m2g.M2GObject#doCleanup()
     */
    protected void doCleanup()
    {
        _destroyPath(
            getNativeEventSourceHandle(),
            getNativeSVGProxyHandle(),
            getHandle(),
            iUIToolkit);
        resetHandles();
    }

    /**
     * @see com.nokia.microedition.m2g.M2GObject#doConstruct()
     */
    protected void doConstruct()
    {
        super.doConstruct();
        register(this);
    }


    /**
     * @see org.w3c.dom.svg.SVGPath#getNumberOfSegments()
     */
    public int getNumberOfSegments()
    {
        return _getNumberOfSegments(getNativeEventSourceHandle(),
                                    getNativeSVGProxyHandle(),
                                    getHandle(),
                                    iUIToolkit);
    }

    /**
     * @see org.w3c.dom.svg.SVGPath#getSegment()
     */
    public short getSegment(int index) throws DOMException
    {
        if ((index < 0) ||
                (index >= getNumberOfSegments()))
        {
            Logger.ELOG(Logger.EJavaUI, "getSegment() - exception:"
                        + /*SF*/"The segment index is out of bounds."/*SF*/);
            throw new DOMException(
                DOMException.INDEX_SIZE_ERR,
                /*SF*/"The segment index is out of bounds."/*SF*/);
        }
        return M2GSVGConstants.parsePathCommand(
                   _getSegmentType(getNativeEventSourceHandle(),
                                   getNativeSVGProxyHandle(),
                                   getHandle(),
                                   index,
                                   iUIToolkit)
               );
    }

    /**
     * Checks that parameter index is valid
     * @param aCommand -
     * @param aParameterIndex -
     */
    protected void checkParameterIndex(int aCommand, int aParameterIndex) throws DOMException
    {
        if ((aCommand == CLOSE && aParameterIndex >= CLOSE_PARAMETER_COUNT) ||
                (aCommand == MOVE_TO && aParameterIndex >= MOVE_TO_PARAMETER_COUNT)
                || (aCommand == LINE_TO && aParameterIndex >= LINE_TO_PARAMETER_COUNT)
                || (aCommand == CURVE_TO && aParameterIndex >= CURVE_TO_PARAMETER_COUNT)
                || (aCommand == QUAD_TO && aParameterIndex >= QUAD_TO_PARAMETER_COUNT))
        {
            throw new DOMException(
                DOMException.INDEX_SIZE_ERR,
                /*SF*/"The parameter index is out of bounds for this segment's type."/*SF*/);
        }
    }

    /**
    * @see org.w3c.dom.svg.SVGPath#close()
     */
    public void close()
    {
        _addClose(
            getNativeEventSourceHandle(), getNativeSVGProxyHandle(), getHandle(), iUIToolkit);
    }

    /**
     * @see org.w3c.dom.svg.SVGPath#curveTo()
     */
    public void curveTo(float x1, float y1, float x2, float y2, float x3, float y3)
    {
        _addCurveTo(
            getNativeEventSourceHandle(), getNativeSVGProxyHandle(),
            getHandle(), x1, y1, x2, y2, x3, y3, iUIToolkit);
    }

    /**
     * @see org.w3c.dom.svg.SVGPath#getSegmentParam()
     */
    public float getSegmentParam(int cmdIndex, int paramIndex)
    {
        if ((cmdIndex >= getNumberOfSegments()) ||
                (cmdIndex < 0) ||
                (paramIndex < 0))
        {
            Logger.ELOG(Logger.EJavaUI, "getSegmentParam() - end, exception:"
                        + /*SF*/"The segment index is out of bounds."/*SF*/);
            throw new DOMException(
                DOMException.INDEX_SIZE_ERR,
                /*SF*/"The segment index is out of bounds."/*SF*/);
        }
        short command = getSegment(cmdIndex);
        // Checks the validity of the parameter index.
        checkParameterIndex(command, paramIndex);
        return _getSegmentParameter(getNativeEventSourceHandle(),
                                    getNativeSVGProxyHandle(),
                                    getHandle(),
                                    cmdIndex,
                                    paramIndex,
                                    iUIToolkit
                                   );
    }

    /**
     * @see org.w3c.dom.svg.SVGPath#lineTo()
     */
    public void lineTo(float x, float y)
    {
        _addLineTo(
            getNativeEventSourceHandle(), getNativeSVGProxyHandle(), getHandle(), x, y, iUIToolkit);
    }

    /**
     * @see org.w3c.dom.svg.SVGPath#moveTo()
     */
    public void moveTo(float x, float y)
    {
        _addMoveTo(
            getNativeEventSourceHandle(), getNativeSVGProxyHandle(), getHandle(), x, y, iUIToolkit);
    }

    /**
     * @see org.w3c.dom.svg.SVGPath#quadTo()
     */
    public void quadTo(float x1, float y1, float x2, float y2)
    {
        _addQuadTo(getNativeEventSourceHandle(),
                   getNativeSVGProxyHandle(),
                   getHandle(),
                   x1,
                   y1,
                   x2,
                   y2,
                   iUIToolkit);
    }

    //--------------------------------------------------
    // NATIVE METHODS
    //--------------------------------------------------
    private static native void _addClose(
        int aToolitHandle, int aSVGProxyHandle, int aPathHandle, boolean iUIToolKit);

    private static native void _addCurveTo(
        int aToolitHandle, int aSVGProxyHandle, int aPathHandle,
        float aX1, float aY1, float aX2, float aY2, float aX3, float aY3, boolean iUIToolKit);

    private static native void _addLineTo(
        int aToolitHandle, int aSVGProxyHandle, int aPathHandle, float aX, float aY, boolean iUIToolKit);

    private static native void _addMoveTo(
        int aToolitHandle, int aSVGProxyHandle, int aPathHandle, float aX, float aY, boolean iUIToolKit);

    private static native void _addQuadTo(
        int aToolitHandle, int aSVGProxyHandle, int aPathHandle,
        float aX1, float aY1, float aX2, float aY2, boolean iUIToolKit);

    static native int _createPath(
        int aToolitHandle, int aSVGProxyHandle, boolean iUIToolKit);

    private static native void _destroyPath(
        int aToolitHandle, int aSVGProxyHandle, int aPathHandle, boolean iUIToolKit);

    private static native int _getNumberOfSegments(
        int aToolitHandle, int aSVGProxyHandle, int aPathHandle, boolean iUIToolKit);

    private static native float _getSegmentParameter(
        int aToolitHandle, int aSVGProxyHandle, int aPathHandle, int aCmdIndex, int aParamIndex, boolean iUIToolKit);

    private static native short _getSegmentType(
        int aToolitHandle, int aSVGProxyHandle, int aPathHandle, int aIndex, boolean iUIToolKit);
}

