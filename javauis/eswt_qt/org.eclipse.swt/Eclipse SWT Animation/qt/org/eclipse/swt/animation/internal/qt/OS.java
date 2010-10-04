package org.eclipse.swt.animation.internal.qt;


public final class OS {
	
	static {
		System.loadLibrary("swt-animations");    
	}

	// QAbstractAnimation
	public static final native void QAbstractAnimation_start(int handle);
	public static final native void QAbstractAnimation_stop(int handle) ;
	public static final native void QAbstractAnimation_setLoopCount(int handle, int repeatCount) ;
	public static final native int QAbstractAnimation_duration(int handle);

	// QPropertyAnimation 
	public static final native int QPropertyAnimation_new(int parent);
	public static final native void QPropertyAnimation_setTargetObject(int handle, int target);
	public static final native void QPropertyAnimation_setPropertyName(int handle, String string);
	
	// QVariantAnimation
	public static final native void QVariantAnimation_setDuration(int handle, int duration);
	public static final native void QVariantAnimation_setEasingCurve(int handle, int interpolator) ;
	public static final native void QVariantAnimation_swt_setEndValuePoint(int handle, int x, int y) ;
	public static final native void QVariantAnimation_swt_setStartValuePoint(int propertyType, int x, int y);
	public static final native void QVariantAnimation_swt_setEndValueDouble(int handle,	double doubleValue);
	public static final native void QVariantAnimation_swt_setStartValueDouble(int handle, double doubleValue);
	
	// QEasingCurve
	public static final native int QEasingCurve_new(int type) ;
	
	// QAnimationGroup
	public static final native void QAnimationGroup_addAnimation(int handle, int child);

	// QSequentialAnimationGroup
	public static final native int QSequentialAnimationGroup_new(int i) ;
	public static final native void QSequentialAnimationGroup_addPause(int handle, int msecs) ;

	// QParallelAnimationGroup
	public static final native int QParallelAnimationGroup_new(int i);






}
