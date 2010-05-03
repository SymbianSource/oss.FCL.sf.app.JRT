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

/*
 * Note that this might be called directly from native code. 
 * Be careful renaming or moving it.
 */ 
public class ExitNotificationWrapper {  
    
    public static void notifyExit() {
        com.nokia.mj.impl.rt.support.ApplicationUtils.getInstance().notifyExitCmd();
    }
    
}
