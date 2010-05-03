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

package com.nokia.mj.impl.uitestutils;

/**
 * Class for storing point, i.e. x and y values.
 * This is used in matric calculations.
 *
 * @author ktuokkol
 */
public class Point {
    public float x;
    public float y;
    
    /**
     * Ctor
     */
    public Point() {
        x = 0f;
        y = 0f;
    }

    /**
     * Ctor
     * @param x
     * @param y
     */
    public Point(float x, float y) {
        this.x = x;
        this.y = y;
    }
    
}
