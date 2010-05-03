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


import org.eclipse.swt.SWT;
import org.eclipse.swt.animation.internal.qt.OS;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Widget;

public class PropertyAnimation extends Animation {
	
	private static final int PROPERTY_POS = 1;
	private static final int PROPERTY_SIZE = 2;
	private static final int PROPERTY_ALPHA = 3;
	
	Object from, to;
	int duration;
	String name, property;
	Object target;
	Class paramType;
	int propertyType;

	Object nextValue;
	IInterpolator interpolator;
		
	public PropertyAnimation(Display display) {
		init(display);
	}
	
	void updateAnimation() {
		if ( property == null ) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
		updateDuration();
		setParamType();
		setValues();
		registerInterpolation();
	}
	
	private void registerInterpolation() {
		if(interpolator == null )return;
		if(interpolator instanceof BuiltInInterpolator ){
			BuiltInInterpolator bi = (BuiltInInterpolator)interpolator;
			OS.QVariantAnimation_setEasingCurve(handle, bi.handle);
		}
	}
	
	void setValues() {
		switch (propertyType) {
		case PROPERTY_POS:
			if (to != null){
				Point p = (Point)to;
				OS.QVariantAnimation_swt_setEndValuePoint(handle, p.x, p.y );
			}
			if (from != null ){
				Point p = (Point) from;
				OS.QVariantAnimation_swt_setStartValuePoint(propertyType, p.x, p.y);
			}
			break;
		case PROPERTY_ALPHA:
			if(to != null){
				Double d = (Double)to;
				OS.QVariantAnimation_swt_setEndValueDouble(handle, d.doubleValue() );
			}
			if(from != null){
				Double d = (Double)from;
				OS.QVariantAnimation_swt_setStartValueDouble(handle, d.doubleValue() );
			}			
		case PROPERTY_SIZE:
			//TODO:
		default:
			break;
		}	
	}

	private void updateDuration() {
		if (duration != 0 )
		OS.QVariantAnimation_setDuration(handle, this.duration );
	}
	
	void setWidget(Widget widget) {
		super.setWidget(widget);
		OS.QPropertyAnimation_setTargetObject(handle,Internal_PackageSupport.topHandle(widget));
	}

	public int getDuration() {
		return OS.QAbstractAnimation_duration(handle);
	}
	
	
	public void setDuration(int duration) {
		checkAnimation();
		this.duration = duration;
	}

	public void setFrom(Object from) {
		checkAnimation();
		this.from = from;
	}
	
	public void setInterpolator(IInterpolator interpolator) {
		checkAnimation();
		this.interpolator = interpolator;
	}
	
	void setParamType() {
		String qtProperty="";
		if( "location".equals(property) ){
			propertyType = PROPERTY_POS;
			qtProperty = "pos";
		}
		if("alpha".equals(property)){
			propertyType = PROPERTY_ALPHA;
			qtProperty = "windowOpacity";
		}
		if("size".equals(property)){
			propertyType = PROPERTY_SIZE;
			qtProperty = "size";
		}
		OS.QPropertyAnimation_setPropertyName(handle,qtProperty);	
	}
	
	public void setProperty(String property) {
		checkAnimation();
		this.property = property;
	}
	
	void create() {
		if (handle == 0 ) handle = OS.QPropertyAnimation_new( Internal_PackageSupport.handle(display) );
		if (handle == 0) SWT.error(SWT.ERROR_NO_HANDLES);
	}

	public void setTo(Object to) {
		checkAnimation();
		this.to = to;
	}
	
}
