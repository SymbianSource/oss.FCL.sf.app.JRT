/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Nokia Corporation - initial implementation 
 *******************************************************************************/
package org.eclipse.swt.snippets;

import org.eclipse.ercp.swt.midp.UIThreadSupport;;

/**
 * <p>
 * A wrapper for org.eclipse.ercp.swt.midp.UIThreadSupport. 
 * </p><p>
 * This class can be compiled using an SDK where UIThreadSupport is available 
 * and then distributed as a class file with a MIDlet. This enables the same 
 * MIDlet to run on platforms that require the use of UIThreadSupport and on 
 * those that don't have it. This can be achieved by checking at runtime if
 * the UIThreadSupport class is available then using through this wrapper to 
 * avoid compilation time dependency to it. 
 * </p>
 */
public class UIThreadSupportWrapper {

public static void startInUIThread(Runnable r) {
    UIThreadSupport.startInUIThread(r);
}

}
