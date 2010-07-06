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

package com.nokia.mj.impl.utils;

public abstract class LocalizedTextProvider
{

    /**
     * Provider for localized texts
     *
     * @param aTextId    identifier of the text to be localized
     * @param aTexParams parameters for the text to be localized
     * @return           the text associated with the provided
     *                   identifier and parameters
     */
    public abstract String getText(Id aTextId, Object[] aTextParams);
}
