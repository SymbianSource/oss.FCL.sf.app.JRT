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


import org.eclipse.swt.widgets.Display;
import com.nokia.mj.impl.rt.legacy.LegacyRtPort;
import java.util.Hashtable;


public class DefaultDisplayWrapper
{

    static Hashtable sDefaultDisplayMap = new Hashtable();

    public static Display get()
    {
        Object instance = LegacyRtPort.getApplicationId();
        synchronized (sDefaultDisplayMap)
        {
            Display display = (Display)sDefaultDisplayMap.get(instance);
            return display;
        }
    }

    public static void set(Display display)
    {

        Object instance = LegacyRtPort.getApplicationId();
        synchronized (sDefaultDisplayMap)
        {
            if (display != null)
            {
                sDefaultDisplayMap.put(instance, display);
            }
            else
            {
                sDefaultDisplayMap.remove(instance);
            }
        }
    }

}
