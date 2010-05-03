package org.eclipse.swt.animation;

import org.eclipse.swt.animation.internal.qt.OS;

public class BuiltInInterpolator implements IInterpolator {
	
	int type;
	int handle;

	public BuiltInInterpolator(int type ){
		this.type = type;
		create();
	}
	
	void create() {
		handle = OS.QEasingCurve_new(type);
		
	}

	public Object getCurrentValue(Object from, Object to, long currentTime,
			long duration) {
		return null;
	}

}
