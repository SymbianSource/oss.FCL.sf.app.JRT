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

import org.eclipse.swt.widgets.Widget;

public interface SymbianWindowVisibilityListener {
    /**
     * Notifies the listeners when the visibility of a window owing widget is changing.
     * May be sent either before or after the actual visibility is changed.
     * @param widget - the window owing widget who's visibility is being changed.
     * @param visible - true when the widget is becoming visible.
     */
    public abstract void handleSymbianWindowVisibilityChange(Widget widget, boolean visible);
}
