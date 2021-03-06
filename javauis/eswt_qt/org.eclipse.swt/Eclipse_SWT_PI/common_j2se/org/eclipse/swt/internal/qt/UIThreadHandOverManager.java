/*******************************************************************************
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

package org.eclipse.swt.internal.qt;

import org.eclipse.swt.internal.qt.midp.UIThreadLauncher;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Shell;

/**
 * A class that manages the UI thread hand-over from the internal UI event 
 * loop to the application. 
 */
public final class UIThreadHandOverManager {

    /**
     * Called indirectly by the application when it's requesting the UI thread. 
     * Application will pass a Runnable as a parameter and expects it to get called
     * in the UI thread. 
     * 
     * @param runnable
     *            The Runnable to call in the UI thread.
     * @return True if obtaining the UI thread was successful and the runnable
     *         will be called.
     */
    public static boolean startInUIThread(Runnable runnable) {
        if(!UIThreadLauncher.startInUIThread(runnable)) {
            return false;
        }
        
        return true;
    }
}
