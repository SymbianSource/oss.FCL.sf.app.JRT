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

import org.eclipse.ercp.swt.mobile.MobileShell;
import org.eclipse.swt.internal.qt.SymbianWindowVisibilityListener;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Internal_PackageSupport;

public class MobileShellExtension extends MobileShell {

    public MobileShellExtension(Display display, int style) {
        super(display, style);
    }
    
    public void addSymbianWindowVisibilityListener(SymbianWindowVisibilityListener listener) {
        Internal_PackageSupport.addSymbianWindowVisibilityListener(this, listener);
    }

    public void removeSymbianWindowVisibilityListener(SymbianWindowVisibilityListener listener) {
        Internal_PackageSupport.removeSymbianWindowVisibilityListener(this, listener);
    }
}
