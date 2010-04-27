/*******************************************************************************
 * Copyright (c) 2007 Nokia Corporation.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.swt.internal.symbian;


import com.nokia.mj.impl.rt.legacy.LegacyRtPort;


public class ApplicationIdWrapper
{

    public static final int NO_APPLICATION = -1;

    public static int getApplicationId()
    {
        int applicationUid = LegacyRtPort.getMidletUid();
        if (applicationUid != -1)
        {
            return applicationUid;
        }
        else
        {
            return NO_APPLICATION;
        }
    }
}
