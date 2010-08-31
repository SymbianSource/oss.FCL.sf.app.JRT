/*******************************************************************************
 * Copyright (c) 2000, 2007 IBM Corporation and others.
 * Portion Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Nokia Corporation - Qt implementation
 *******************************************************************************/
package org.eclipse.swt.internal.qt;


import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Device;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.qt.graphics.GraphicsContext;

/**
 * Instances of this class are descriptions of GCs in terms
 * of unallocated platform-specific data fields.
 * <p>
 * <b>IMPORTANT:</b> This class is <em>not</em> part of the public
 * API for SWT. It is marked public only so that it can be shared
 * within the packages provided by SWT. It is not available on all
 * platforms, and should never be called from application code.
 * </p>
 */
public final class GCData {
	
	public static final int NOT_DEFINED = -1;
	public static final int XOR_MODE_ON = 1;
	public static final int XOR_MODE_OFF = 0;
	
	/*
	 * Stroke style mappings
	 */
	static final int [][] StrokeStyleTable = {
		{GraphicsContext.STROKE_SOLID,	      SWT.LINE_SOLID},
		{GraphicsContext.STROKE_DASH,	      SWT.LINE_DASH},
		{GraphicsContext.STROKE_DOT,	      SWT.LINE_DOT},
		{GraphicsContext.STROKE_DASH_DOT,     SWT.LINE_DASHDOT},
		{GraphicsContext.STROKE_DASH_DOT_DOT, SWT.LINE_DASHDOTDOT}};
	
	public Device device;
	public GraphicsContext internalGc;
	public Color foreground;
	public Color background;
	public int drawable; // QPaintDevice
	public int alpha;
	public Font font;
	public int lineStyle;
	public int lineWidth;
	public int style, state;
	public int xorMode;
	public Rectangle clippingRect;
	public boolean buffered;

	public GCData() {
	    drawable = NOT_DEFINED;
	    alpha = 0xFF;
	    lineStyle = NOT_DEFINED;
	    lineWidth = NOT_DEFINED;
	    style = NOT_DEFINED;
	    xorMode = NOT_DEFINED;
	}
	
	/*
	 * Releases resources
	 */
	public void destroy() {
		internalGc = null;
		device     = null;
		foreground = null;
		background = null;
		drawable   = NOT_DEFINED;
		alpha      = 0xFF;
		font       = null;
		lineStyle  = NOT_DEFINED;
		lineWidth  = NOT_DEFINED;
		style      = NOT_DEFINED;
		xorMode    = NOT_DEFINED;
		clippingRect = null;
	}

	/*
	 * Converts stroke style to and from SWT.
	 * <p>
	 * <b>IMPORTANT:</b> This table is should never be accessed 
	 * from application code.
	 * </p>
	 *
	 * @param style Stroke style
	 * @param fromSWT if true translates the given SWT stroke style
	 * to a corresponding native stroke style and vice versa if 
	 * the <code>fromSWT</code> is false.
	 * @return Translated stroke. 
	 * @throws IllegalArgumentException if no match

	 */
	public static int translateStrokeStyle(int style, boolean fromSWT) {
		int source = 0;
		int target = 1;
		if( fromSWT ) {
			source = 1;
			target = 0;
		}
		for (int i = 0; i < GCData.StrokeStyleTable.length; i++) {
			if (GCData.StrokeStyleTable[i][source] == style) {
				return GCData.StrokeStyleTable[i][target];
			}
		}
		throw new IllegalArgumentException("Invalid style: " + style);
	}
}
