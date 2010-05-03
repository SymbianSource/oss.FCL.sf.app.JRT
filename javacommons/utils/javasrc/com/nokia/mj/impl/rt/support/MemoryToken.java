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
 * MemoryToken is a class which is used to inform the runtime about native
 * memory allocations happening outside of the JVM. The runtime uses the
 * information collected through MemoryToken to trigger garbage collections,
 * when needed.
 *
 * When the memory allocation regarding to a token changes, use {@link #setSize}
 * to update the information. The memory allocation is automatically reduced
 * when the object referencing MemoryToken instance is garbage collected, thus
 * there is no need to set the size to zero in this case.
 *
 * Add a MemoryToken instance to the Java class holding the native memory
 * handle. E.g.
 *
 * <pre>
 *
 * import com.nokia.mj.impl.rt.support.MemoryToken;
 * class GraphicsObject
 * {
 *     MemoryToken memoryToken = MemoryToken();
 *
 *     public GraphicsObject(int height, int width) {
 *         int sizeEstimate = height * width * 32;
 *         memoryToken.setSize(sizeEstimate)
 *
 *         // ...
 *     }
 * }
 *</pre>
 * @author Nokia Corporation
 * @version 1.0
 */

public class MemoryToken
{
    /** Assigned size of this memory token */
    private int tokenSize = 0;

    /** Enables finalization */
    private Finalizer finalizer = new Finalizer()
    {
        public void finalizeImpl()
        {
            doFinalize();
        }
    };

    static MemoryManager manager = new MemoryManager();

    /**
     * Creates a new token with zero sized memory.
     */
    public MemoryToken()
    {
    }

    /**
     * Creates a new token with given memory size. The call may take some time
     * if the construction triggers a garbage collection.
     *
     * @param size number of bytes assigned for this token
     */
    public MemoryToken(int size)
    {
        setSize(size);
    }

    /**
     * Changes the token's size. The call may take some time if it triggers a
     * garbage collection.
     *
     * @param newSize new number of bytes assigned for this token
     */
    public void setSize(int newSize)
    {
        // Get the old size and change size
        int oldSize;
        synchronized (this)
        {
            oldSize = tokenSize;
            tokenSize = newSize;
        }

        // Inform the manager about the size change
        manager.changeSize(oldSize, newSize);
    }

    private void doFinalize()
    {
        // Set size to zero
        setSize(0);
    }
}
