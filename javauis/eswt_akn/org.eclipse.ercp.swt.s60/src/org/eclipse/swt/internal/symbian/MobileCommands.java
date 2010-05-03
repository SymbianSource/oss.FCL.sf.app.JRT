/*******************************************************************************
 * Copyright (c) 2005, 2007 Nokia Corporation.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.swt.internal.symbian;


/**
 * <code>CommandsFromMobileExtensions</code> is an interface through which
 * Control class, that belongs to eSWT Core package, may communicate with
 * a class that belongs to eSWT Mobile Extensions package.
 * <p>
 * <b>IMPORTANT:</b> This class is <em>not</em> part of the SWT
 * public API. It is marked public only so that it can be shared
 * within the packages provided by SWT. It should never be
 * referenced from application code.
 * </p>
 */
public interface MobileCommands
{

    /**
     * Invokes disposal of a collection of Command widgets (Command class
     * is defined in eSWT Mobile Extensions package).
     * <p>
     * <b>IMPORTANT:</b> This method is <em>not</em> part of the public
     * API for <code>CommandsFromMobileExtensions</code>. It is marked
     * public only so that it can be shared within the packages provided
     * by SWT. It is not available on all platforms, and should never be
     * called from application code.
     * </p>
     */
    public void release();
}
