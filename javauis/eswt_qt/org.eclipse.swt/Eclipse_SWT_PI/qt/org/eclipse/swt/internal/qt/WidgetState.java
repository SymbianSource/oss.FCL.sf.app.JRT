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
package org.eclipse.swt.internal.qt;

public final class WidgetState {
/* Global state flags */
public static final int DISPOSED = 1<<0;
public static final int CANVAS = 1<<1;
public static final int KEYED_DATA = 1<<2;
public static final int HANDLE = 1<<3;
public static final int DISABLED = 1<<4;
public static final int MENU = 1<<5;
public static final int OBSCURED = 1<<6;
public static final int MOVED = 1<<7;
public static final int RESIZED = 1<<8;
public static final int ZERO_WIDTH = 1<<9;
public static final int ZERO_HEIGHT = 1<<10;
public static final int HIDDEN = 1<<11;
public static final int FOREGROUND = 1<<12;
public static final int BACKGROUND = 1<<13;
public static final int FONT = 1<<14;
public static final int PARENT_BACKGROUND = 1<<15;
public static final int THEME_BACKGROUND = 1<<16;

/* A layout was requested on this widget */
public static final int LAYOUT_NEEDED  = 1<<17;

/* The preferred size of a child has changed */
public static final int LAYOUT_CHANGED = 1<<18;

/* A layout was requested in this widget hierarchy */
public static final int LAYOUT_CHILD = 1<<19;

/* More global state flags */
public static final int RELEASED = 1<<20;
public static final int DISPOSE_SENT = 1<<21;
public static final int FOREIGN_HANDLE = 1<<22;
public static final int DRAG_DETECT = 1<<23;
public static final int NO_TRAVERSE = 1<<24;
public static final int NO_KEY_PROPAGATE = 1<<25;
public static final int NO_MOUSE_PROPAGATE = 1<<26;
public static final int EMBEDDED_SCROLLBARS = 1<<27;

// A flag telling if the widget is following the system bidi orientation. 
// If not set then the widget's orientation won't be changed when system 
// orientation changes. 
public static final int FOLLOWS_SYSLANG_DIRECTION = 1<<28;

// If extension functionality is enabled
public static final int IS_EXTENDED = 1<<29;
}
