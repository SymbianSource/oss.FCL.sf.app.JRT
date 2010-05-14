/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


package com.nokia.microedition.media.control;

import org.eclipse.swt.widgets.*;
import javax.microedition.lcdui.*;
import org.eclipse.swt.graphics.Rectangle;


public abstract class BaseDisplay
{
	// represents native side of display and initialized by derived class
	protected int nativeDisplayHandle;
	protected org.eclipse.swt.widgets.Control eswtCanvasControl;
	private Rectangle rect;
	private int[] rectDimension = new int[ 4 ];
	public BaseDisplay()
    {
    	rect = new Rectangle(0,0,0,0);
   	}
/*	protected void setESWTControl()
	{
		eswtCanvasControl =
	}
	*/
 	public void GetCallbackInUiThread(int placeholder)
	{
		System.out.println("inside BaseDisplay : GetCallbackInUiThread +");
		final int val = placeholder;
		try{
			final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
			disp.syncExec(new Runnable() {
			 public void run() {
			  			  _nativeMethodCallInUiThread(nativeDisplayHandle , val);
	 			             }
						 });
		   }
		 catch(Exception e)
		 {
			System.out.println("inside BaseDisplay : GetCallbackInUiThread....exception is " + e.toString());
		 }
		System.out.println("inside BaseDisplay : GetCallbackInUiThread -");
	}
	
	public void setContentBound()
	{
		System.out.println("inside BaseDisplay : setContentBound +");
		try{

			final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
			disp.syncExec(new Runnable() {
			 public void run() {
			  eswtCanvasControl.setBounds(rect);
			      }
						 });
		   }
		 catch(Exception e)
		 {
			System.out.println("inside BaseDisplay : setContentBound....exception is  " + e.toString());
		 }
		 System.out.println("inside BaseDisplay : setContentBound -");
	}


	public void removeContentBound()
	{
		System.out.println("inside BaseDisplay : removeContentBound +");
		try{

			final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
			disp.syncExec(new Runnable() {
			 public void run() {
			 	Rectangle emptyRect = new Rectangle(0,0,0,0);
			  eswtCanvasControl.setBounds(emptyRect);
			      }
						 });
		   }
		 catch(Exception e)
		 {
			System.out.println("inside BaseDisplay : removeContentBound....exception is  " + e.toString());
		 }
		 System.out.println("inside BaseDisplay : removeContentBound -");
	}
	
	public void redrawControl()
	{
		System.out.println("inside BaseDisplay : redrawControl +");
		try{

			final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
			disp.syncExec(new Runnable() {
			 public void run() {
			 				// redraw entire bounds of receiver
			 			  eswtCanvasControl.redraw();
			      }
						 });
		   }
		 catch(Exception e)
		 {
			System.out.println("inside BaseDisplay : redrawControl....exception is  " + e.toString());
		 }
		 System.out.println("inside BaseDisplay : redrawControl -");
	}
		/* called from native to reset the java side rect
	 Before the call of this function array is updated from native side
	 which can be used to refresh the UI screen rect and/or 
	 set/remove bound rect
	*/
	public void setRect()
	{
		
			rect.x = rectDimension[0];
			rect.y = rectDimension[1];
			rect.width = rectDimension[2];
			rect.height = rectDimension[3];
			
		System.out.println("BaseDisplay : setRect rect is reset with values: x =" + rect.x +"y ="+ rect.y +"width ="+rect.width+"height = "+ rect.height);	
			
	}

	// abstract functions implemented by concrete class
	protected abstract void setDisplaySize(int aWidth, int aHeight);
	protected abstract void setDisplayFullScreen(final boolean aFullScreenMode);
	protected abstract void setDisplayLocation(int aX, int aY);
	protected abstract void setVisible(boolean aVisible);
	protected abstract int getDisplayHeight();
	protected abstract int getDisplayWidth();
	protected abstract int getDisplayX();
	protected abstract int getDisplayY();
	protected abstract int getSourceWidth();
	protected abstract int getSourceHeight();
	protected abstract void getBoundRect();
	protected abstract void setWindowResources();
	protected abstract void setNativeHandle(int handle);
    // Native methods
	 private native void _nativeMethodCallInUiThread(int nativeDisplayHandle , int placeholder);

}
