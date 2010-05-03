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

package com.nokia.amms.control.audio3d;

import com.nokia.mj.impl.rt.legacy.NativeError;

public class OrientationControl
        extends com.nokia.amms.control.ControlImpl
        implements javax.microedition.amms.control.audio3d.OrientationControl
{
    private static final int REQUIRED_VECTOR_SIZE = 3;
    private static final int COMPONENT_X = 0;
    private static final int COMPONENT_Y = 1;
    private static final int COMPONENT_Z = 2;

    /**
     * Constructor
     */
    public OrientationControl()
    {
    }

    /**
    * Please refer to JSR 234 for more details.
    */
    public void setOrientation(int aHeading, int aPitch, int aRoll)
    {
        checkValid();

        int err = _setOrientation(
                      iEventSource,
                      iControlHandle,
                      aHeading,
                      aPitch,
                      aRoll);

        NativeError.check(err);
    }

    /**
    * Please refer to JSR 234 for more details.
    */
    public void setOrientation(int[] aFrontVector, int[] aAboveVector)
    throws IllegalArgumentException
    {
        checkValid();

        if ((aFrontVector == null) ||
                (aAboveVector == null))
        {
            throw new IllegalArgumentException("Argument is null");
        }

        if ((aFrontVector.length < REQUIRED_VECTOR_SIZE) ||
                (aAboveVector.length < REQUIRED_VECTOR_SIZE) ||
                (aFrontVector.length > REQUIRED_VECTOR_SIZE) ||
                (aAboveVector.length > REQUIRED_VECTOR_SIZE))
        {
            throw new IllegalArgumentException("Required orientation vector size is 3");
        }

        // Vector is a zero vector if all the components are zero
        if (((aFrontVector[ COMPONENT_X ] == 0) &&
                (aFrontVector[ COMPONENT_Y ] == 0) &&
                (aFrontVector[ COMPONENT_Z ] == 0)) ||
                ((aAboveVector[ COMPONENT_X ] == 0) &&
                 (aAboveVector[ COMPONENT_Y ] == 0) &&
                 (aAboveVector[ COMPONENT_Z ] == 0)))
        {
            throw new IllegalArgumentException("Orientation vector cannot be a zero vector");
        }

        // Two vectors are parallel if their cross product is zero vector.
        // Cross product of vectors a1*i + a2*j + a3*k and b1*i + b2*j + b3*k :
        // (a2*b3 - a3*b2)i + (a3*b1 - a1*b3)j + (a1*b2 - a2*b1)k
        if ((
                    (aFrontVector[ COMPONENT_Y ] * aAboveVector[ COMPONENT_Z ]) -
                    (aFrontVector[ COMPONENT_Z ] * aAboveVector[ COMPONENT_Y ]) == 0
                )
                &&
                (
                    (aFrontVector[ COMPONENT_Z ] * aAboveVector[ COMPONENT_X ]) -
                    (aFrontVector[ COMPONENT_X ] * aAboveVector[ COMPONENT_Z ]) == 0
                )
                &&
                (
                    (aFrontVector[ COMPONENT_X ] * aAboveVector[ COMPONENT_Y ]) -
                    (aFrontVector[ COMPONENT_Y ] * aAboveVector[ COMPONENT_X ]) == 0
                ))
        {
            throw new IllegalArgumentException("Orientation vectors cannot be parallel");
        }

        int err = _setOrientationVectors(
                      iEventSource,
                      iControlHandle,
                      aFrontVector,
                      aAboveVector);

        NativeError.check(err);
    }

    /**
    * Please refer to JSR 234 for more details.
    */
    public int[] getOrientationVectors()
    {
        checkValid();

        int[] error = new int[ 1 ];

        int[] orientation = _getOrientationVectors(
                                iEventSource,
                                iControlHandle,
                                error);

        if (error[ 0 ] != NativeError.KErrNone)
        {
            throw new OutOfMemoryError("Obtaining orientation vectors failed: Symbian OS error " +
                                       error[ 0 ]);
        }

        if (orientation == null)
        {
            throw new OutOfMemoryError("Unable to obtain orientation vectors");
        }
        return orientation;
    }

    private static native int _setOrientation(
        int aEventSource,
        int aControlHandle,
        int heading,
        int pitch,
        int roll);

    private static native int _setOrientationVectors(
        int aEventSource,
        int aControlHandle,
        int[] frontVector,
        int[] aboveVector);

    private static native int[] _getOrientationVectors(
        int aEventSource,
        int aControlHandle,
        int[] error);
}
