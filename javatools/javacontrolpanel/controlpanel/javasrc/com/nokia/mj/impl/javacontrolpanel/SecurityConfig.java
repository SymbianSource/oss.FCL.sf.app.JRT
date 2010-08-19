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

package com.nokia.mj.impl.javacontrolpanel;

import com.nokia.mj.impl.utils.Logger;


class SecurityConfig
{
    private static final String SECURITY_WARNINGS_USER_DEFINED_MODE = "1";
    private static final String SECURITY_WARNINGS_DEFAULT_MODE = "2";

    private static final int S60_SECURITY_POLICY_ID = 1;
    private static final int MSA_SECURITY_POLICY_ID = 2;
    private static final int ATT_SECURITY_POLICY_ID = 3;

    public static final String MODES[] = { "Default", "User"};
    public static final String POLICIES[] = { "S60", "MSA", "AT&T"};

    public SecurityConfig()
    {
    }

    public int getSecurityPolicy()
    {
        int index = _getSecurityPolicy() - 1;
        return index;
    }

    public void setSecurityPolicy(int aPolicyIndex)
    {
        int id = aPolicyIndex + 1;
        _setSecurityPolicy(id);
    }

    public int getWarningsMode()
    {
        String mode = _getSecurityWarningsMode();
        int index = 0;
        if (mode.compareTo(SECURITY_WARNINGS_USER_DEFINED_MODE) == 0 )
        {
            index = 1;
        }
        return index;
    }

    public void setWarningsMode(int aModeIndex)
    {
        if (aModeIndex == 0)
        {
            _setSecurityWarningsMode(SECURITY_WARNINGS_DEFAULT_MODE);
        }
        else
        {
            _setSecurityWarningsMode(SECURITY_WARNINGS_USER_DEFINED_MODE);
        }
    }

    private native String _getSecurityWarningsMode();
    private native void _setSecurityWarningsMode(String aMode);
    private native int _getSecurityPolicy();
    private native void _setSecurityPolicy(int aPolicyId);

}
