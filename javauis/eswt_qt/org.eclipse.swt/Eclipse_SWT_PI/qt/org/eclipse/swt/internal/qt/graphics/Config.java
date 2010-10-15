/*******************************************************************************
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/
package org.eclipse.swt.internal.qt.graphics;

final class Config {
	
	/**
	 * Enables or disables validation that calls to common graphics objects
	 * are made within UI thread. If this feature is enabled (true) an Error
	 * is thrown when common graphics is called outside ui thread. 
	 */
	static final boolean ENABLE_UI_THREAD_VALIDATION = false;
	
	/**
	 * Default fill color for images. Used when fill color is not specified as an argument in Image constructor.
	 */
	static final int IMAGE_DEFAULT_FILL_COLOR = 0xffffffff;
	
	/**
	 * Default type for images. Used when image type is not specified as an argument in Image constructor.
	 * This setting is also used for ImageLoader output Images (default), if no explicit target Image type is specified.
	 */
	static final int IMAGE_DEFAULT_TYPE = Image.IMAGE_TYPE_QIMAGE;
}
