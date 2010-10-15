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


package com.nokia.mj.impl.utils;

/**
 * Localisation identifier.
 */
public class Id
{
    private String iAvkonLocString = null;
    private String iQtLocString = null;

    /**
     * Default constructor.
     */
    private Id()
    {
    }

    /**
     * Localisation identifier with Avkon and Qt type text identifiers.
     *
     * @param aAvkonLocString text id fof Avkon based localisation
     * @param aQtLocString text id for Qt based localisation
     */
    public Id(String aAvkonLocString, String aQtLocString)
    {
        this.iAvkonLocString = aAvkonLocString;
        this.iQtLocString = aQtLocString;
    }

    String getString(int aPlatform)
    {
        if (aPlatform == ResourceLoader.QT)
        {
            return iQtLocString;
        }
        else
        {
            return iAvkonLocString;
        }
    }
}
