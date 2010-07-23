/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.installer.utils;

/**
 * Interface for listening property value changes.
 *
 * @see PropertyProvider
 */
public interface PropertyListener
{
    /**
     * Called when property value is changed.
     * This method must return quickly.
     *
     * @param aCategory property category
     * @param aKey property key
     * @param aValue property value
     */
    public void valueChanged(int aCategory, int aKey, int aValue);
}
