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

package org.eclipse.swt.internal.extension;

import org.eclipse.swt.internal.qt.EventLoop;
import org.eclipse.swt.widgets.Display;

/**
 * This class enables eSWT UI services to be used independently of the 
 * application's UI life-cycle in the implementation of the APIs. I.e. without 
 * having to know if the application has created a UI toolkit yet, if it has 
 * already destroyed it, or if it will ever create it. 
 * <p>
 * This is achieved by creating an 'internal' instance of the eSWT Display 
 * class. I.e. an instance that is not revealed to the application but is only 
 * used internally within the platform implementation. The application can 
 * create the actual 'public' Display instance. Thus there can be two Display 
 * objects but there's always only one set of UI resources such as  the native 
 * application, UI thread, native widgets, etc. These same resources are used 
 * by both the Displays. 
 * <p>
 * The users of the internal Display instance must take into account that 
 * the public eSWT APIs can never provide references to the internal Display 
 * instance. This has the implication that any other means of obtaining 
 * the Display besides the methods of this class will always use a reference 
 * to the public Display instance or null if it doesn't exist. E.g. 
 * Display.getCurrent() or Display.getDefault() can't be used to obtain the 
 * internal Display instance because they will return null if the application 
 * hasn't explicitly created the public Display instance. 
 * 
 * @see ApplicationUI
 */
final public class Toolkit {
    
    private static Display internalDisplay;

    /**
     * Returns an object reference to the internal Display instance. There's 
     * only one internal Display object in the process. The caller does not own 
     * the Display and must not dispose it. 
     * <p>
     * The returned Display shares the UI resources in the process with the
     * possibly existing public Display instance owned by the application. The
     * UI thread and the event loop maybe under control of the application.
     * <p>
     * This method is thread safe. 
     * 
     * @return The internal Display instance
     * 
     * @throws RuntimeException
     *             If the UI creation has failed and the Display can't be
     *             obtained.
     */
    static public synchronized Display getInternalDisplay() {
        if(internalDisplay == null) {
            internalDisplay = EventLoop.getInternalDisplay();
        }
        return internalDisplay;
    }
}
