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

package org.eclipse.swt.internal.extension;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Table;

public final class TableExtension extends Table {

    public TableExtension(Composite parent, int style) {
        super(parent, style, true);
    }

    public int getFocusIndex() {
        return Internal_PackageSupport.getFocusIndex(this);
    }

    public void setFocusIndex(int index) {
        Internal_PackageSupport.setFocusIndex(this, index);
    }

    public void setWordWrap(boolean on) {
        Internal_PackageSupport.setWordWrap(this, on);
    }

    public boolean getWordWrap() {
        return Internal_PackageSupport.getWordWrap(this);
    }
}
