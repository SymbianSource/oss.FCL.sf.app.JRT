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

import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Internal_PackageSupport;

public class CanvasExtension extends Canvas {

	public CanvasExtension(Composite parent, int style) {
		super(parent, style);
	}
	
	public void redrawNow(int x, int y, int width, int height) {
		OS.QWidget_repaint(Internal_PackageSupport.handle(this), x, y, width, height);
	}
}
