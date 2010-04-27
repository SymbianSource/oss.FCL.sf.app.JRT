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


package com.nokia.mj.impl.installer.applicationregistrator;

/**
 * Container class for a localized name and its locale.
 */
public final class LocalizedName
{
    private String iName = null;
    private String iLocale = null;
    private int iLanguage = 0;

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Constructor.
     */
    LocalizedName(String aName, String aLocale)
    {
        iName = aName;
        iLocale = aLocale;
        iLanguage = S60LangMapper.isoToLang(aLocale);
    }

    /**
     * Returns name.
     */
    public String getName()
    {
        return iName;
    }

    /**
     * Returns locale.
     */
    public String getLocale()
    {
        return iLocale;
    }

    /**
     * Returns S60 TLanguage value for the locale.
     */
    public int getLanguage()
    {
        return iLanguage;
    }

    /**
     * Returns string representation of this object.
     */
    public String toString()
    {
        StringBuffer buf = new StringBuffer();
        buf.append(getName());
        buf.append(", Locale: ").append(getLocale());
        buf.append(", Language: ").append(getLanguage());
        return buf.toString();
    }

    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Default constructor.
     */
    private LocalizedName()
    {
    }

    /*** ----------------------------- NATIVE ----------------------------- */
}
