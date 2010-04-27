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

package com.nokia.mj.impl.security.midp.authorization;

/**
 * Localized security settings info
 */
public final class LocalizedSecuritySettingsInfo
{
    private String language;
    private String name;
    private String promptText;

    public LocalizedSecuritySettingsInfo(
        String language, String name, String promptText)
    {
        this.language = language;
        this.name = name;
        this.promptText = promptText;
    }

    public String getName()
    {
        return name;
    }

    public String getLanguage()
    {
        return language;
    }

    public String getPromptText()
    {
        return promptText;
    }
}
