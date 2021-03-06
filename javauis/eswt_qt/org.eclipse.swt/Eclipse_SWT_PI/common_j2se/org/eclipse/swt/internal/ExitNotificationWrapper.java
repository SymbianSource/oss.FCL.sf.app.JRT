/*******************************************************************************
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Nokia Corporation - initial implementation 
 *******************************************************************************/
package org.eclipse.swt.internal;
import org.eclipse.swt.widgets.Display;


public final class ExitNotificationWrapper {	
    
    public static void notifyExit() {
        Display display = Display.getCurrent();
        if(display != null) {
            display.dispose();
        }
    }
    
    public static void uiDisposed() {
        notifyExit();
    }
}
