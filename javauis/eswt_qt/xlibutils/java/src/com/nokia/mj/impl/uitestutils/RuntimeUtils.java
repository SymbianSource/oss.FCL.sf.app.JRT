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
package com.nokia.mj.impl.uitestutils;

public class RuntimeUtils {

    public static final int S60 = 1;
    public static final int X11 = 2;
    
    public static int getPlatform() {
        return X11;
    }

    // We don't support InstallerMain-based test running for X11 target,
    // so only return 0 here.
    public synchronized static final int startUI(
            final Runnable callback) {
        return 0;
    }
}
