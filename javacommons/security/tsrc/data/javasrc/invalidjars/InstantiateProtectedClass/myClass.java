/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

package mypackage;

import com.nokia.mj.impl.security.midp.common.MIDPPermission;

public class myClass
{

    public myClass()
    {
        o = new MIDPPermission("blah", "blah", "blah");
    }

    Object o;
}
