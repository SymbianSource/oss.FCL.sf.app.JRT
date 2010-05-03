/*******************************************************************************
 * Copyright (c) 2000, 2008 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.eclipse.swt.animation;

import org.eclipse.swt.animation.internal.qt.OS;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Internal_PackageSupport;


public class SequenceAnimation extends CompositeAnimation {
	
	public SequenceAnimation(Display display) {
		super(display);
	}

	void create() {
		handle = OS.QSequentialAnimationGroup_new(Internal_PackageSupport.handle(display));
	}
	
	public void addPause ( int msecs ){
		checkAnimation();
		OS.QSequentialAnimationGroup_addPause(handle,msecs);
	}
	
	
}
