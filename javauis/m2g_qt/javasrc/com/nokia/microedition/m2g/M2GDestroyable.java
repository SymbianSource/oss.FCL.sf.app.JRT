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


/**
 * Interface
 */
public interface M2GDestroyable
{
    /**
     * Finalization
     */
    void doDestroy();
}

//--------------------------------------------------
// OTHER CLASSES
//--------------------------------------------------
/**
 * M2GDestroyer implements the ToolkitObserver interface.
 */
class M2GDestroyer
{
    private M2GDestroyable iDestroyable;

    /**
     * Ctor
     * @param aDestroyable Destroy target
     */
    M2GDestroyer(M2GDestroyable aDestroyable)
    {
        if (aDestroyable != null)
        {
            iDestroyable = aDestroyable;
        }
    }
}


