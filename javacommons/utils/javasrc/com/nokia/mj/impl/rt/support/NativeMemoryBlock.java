/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.rt.support;

/**
 * JAVADOCS ARE UNDER CONSTRUCTION!!
 *
 * @author Nokia Corporation
 * @version 1.0
 */

public interface NativeMemoryBlock
{

    /**
     * Get the pointer to the native memory.
     */
    public long getPointer();

    /**
     * Get the size of the allocated native memory.
     */
    public int getSize();

    /**
     * Frees the native memory.
     */
    public void freeMemory();
}
