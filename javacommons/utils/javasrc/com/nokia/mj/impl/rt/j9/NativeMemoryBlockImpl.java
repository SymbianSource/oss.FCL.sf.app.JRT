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

package com.nokia.mj.impl.rt.j9;

import java.io.InputStream;
import java.io.IOException;

import com.nokia.mj.impl.rt.support.NativeMemoryBlock;
import com.ibm.oti.vm.VM;
import com.ibm.oti.vm.OSMemoryAccessor;
/**
 *
 * @author Nokia Corporation
 * @version 1.0
 */

class NativeMemoryBlockImpl implements NativeMemoryBlock
{

    InputStream mMemInputStream = null;

    /**
     * Default constructor not allowed
     */
    private NativeMemoryBlockImpl()
    {
    }

    /**
     * ctor
     */
    NativeMemoryBlockImpl(String jarPath, String resName)
    {
        mMemInputStream = VM.getResourceAsMemInputStream(jarPath, resName);
    }

    /**
     * Get the pointer to the native memory.
     */
    public long getPointer()
    {
        if (mMemInputStream != null)
        {
            return ((OSMemoryAccessor)mMemInputStream).getPointer();
        }
        return 0;
    }

    /**
     * Get the size of the allocated native memory.
     */
    public int getSize()
    {
        if (mMemInputStream != null)
        {
            return ((OSMemoryAccessor)mMemInputStream).getSize();
        }
        return 0;
    }

    /**
     * Frees the native memory.
     */
    public void freeMemory()
    {
        if (mMemInputStream != null)
        {
            try
            {
                mMemInputStream.close();
            }
            catch (IOException ioe)
            {
                // Should write a log or silently discard?
            }
        }
    }
}
