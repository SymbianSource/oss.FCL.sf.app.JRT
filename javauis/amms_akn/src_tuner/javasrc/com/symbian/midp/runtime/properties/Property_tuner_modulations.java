/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements tuner.modulations property
*
*/

package com.symbian.midp.runtime.properties;

import com.symbian.midp.runtime.PropertyHandler;

public final class Property_tuner_modulations
        implements PropertyHandler
{
    public String getProperty()
    {
        return "fm";
    }
}
