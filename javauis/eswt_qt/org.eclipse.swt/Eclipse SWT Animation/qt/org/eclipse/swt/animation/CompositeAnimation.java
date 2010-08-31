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

import org.eclipse.swt.*;
import org.eclipse.swt.animation.internal.qt.OS;
import org.eclipse.swt.widgets.*;

public abstract class CompositeAnimation extends Animation {
	
	public CompositeAnimation(Display display) {
		init(display);
	}

	int childCount = 0;
	Animation[] animations = new Animation[4];
	

	public void addAnimation(Animation animation) {
		if (animations.length == childCount) {
			Animation[] temp = new Animation[childCount + 4];
			System.arraycopy(animations, 0, temp, 0, childCount);
			animations = temp;
		}
		animations[childCount] = animation;
		childCount++;
	}

	void addChildren() {
		for (int i = 0; i < childCount; i++) {
			Animation animation = animations[i];
			OS.QAnimationGroup_addAnimation(this.handle, animation.handle);
		}

	}
	
		
	public Animation getAnimation(int i) {
		checkAnimation();
		if (0>i || i>=childCount) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
		return animations[i];
	}
	
	public int getDuration() {
		return OS.QAbstractAnimation_duration(handle);
	}
	
	void updateAnimation() {
		super.updateAnimation();
		for (int i = 0; i < childCount; i++) {
			Animation a = animations[i];
			a.updateAnimation();
		}

	}
	
	void setWidget(Widget widget) {
		this.widget = widget;
		for (int i = 0; i < childCount; i++) {
			Animation a = animations[i];
			a.setWidget(widget);
		}
	}
}
