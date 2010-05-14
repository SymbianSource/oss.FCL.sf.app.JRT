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

import org.eclipse.swt.*;
import org.eclipse.swt.widgets.*;
import javax.microedition.lcdui.*;
import org.eclipse.ercp.swt.mobile.MobileShell;
import org.eclipse.swt.graphics.Rectangle;



public class MMACanvasDisplay extends BaseDisplay
{

  private int iEventSourceHandle;
	private MMAPIeSWTObserver eswtObserver;
	private int x;
	private int y;
	private int qwidgetHandle;
	private int eSWTcontrolLocationX;
	private int eSWTcontrolLocationY;

	// index 0 : x-coordinate of topleft corner of display
	// index 1 : y-coordinate of topleft corner of display
	// index 3 : width of display
	// index 4 : height of display

	private int[] displayboundarr = new int[ 4 ];
	// represents native side of display


    public MMACanvasDisplay(int aEventSourceHandle , javax.microedition.lcdui.Canvas canvas)
    {
    iEventSourceHandle = aEventSourceHandle;
    eswtObserver = new MMAPIeSWTObserver();
		eswtCanvasControl = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtControl(canvas);
		//super.setESWTControl(eswtCanvasControl);
		}

	private void addListeners()
	{
	eswtObserver.addControlListenerToControl(eswtCanvasControl);
	eswtObserver.addShellListenerToControl(eswtCanvasControl);
	eswtObserver.addDisposeListenerToControl(eswtCanvasControl);
	}

	public void setNativeHandle( int handle)
	{
		System.out.println("MMACanvasDisplay.java : setNativeHandle :" + handle);
		nativeDisplayHandle = handle;
		eswtObserver.setDisplayToObserver(this);
		addListeners();
	}

  public void setDisplaySize(int aWidth, int aHeight)
	{
		final int width = aWidth;
		final int height = aHeight;
		try{
		 final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
		 disp.syncExec(new Runnable() {
							 public void run() {
							   org.eclipse.swt.graphics.Point size = new org.eclipse.swt.graphics.Point(width,height);
							   eswtCanvasControl.redraw();
							   
								System.out.println("inside videoControl's setDisplaySize redraw called");
								        		 }
			 });
		   }
		catch(Exception e)
		{
			System.out.println("inside videoControl's setDisplaySize....exception is " + e.toString());
		}


System.out.println("inside videoControl's setDisplaySize +");
		_setDisplaySize(nativeDisplayHandle,
							iEventSourceHandle,
		 						aWidth,
		 							aHeight);
System.out.println("inside videoControl's setDisplaySize -");		 							
	}

	public void setDisplayFullScreen(final boolean aFullScreenMode)
	{
		try{

			final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
			disp.syncExec(new Runnable() {
			 public void run() {
			  eswtCanvasControl.setBounds(disp.getClientArea());
			  new MobileShell(disp).setFullScreenMode(aFullScreenMode);
			  // instruct native to switch to full screen mode
			  _setFullScreenMode(nativeDisplayHandle,aFullScreenMode);
	 			             }
						 });
		   }
		 catch(Exception e)
		 {
			System.out.println("inside videoControl's setDisplaySize....exception is " + e.toString());
		 }

	}

	public void setDisplayLocation(int aX, int aY)
	{

	final int videoControlLocationX = aX;
	final int videoControlLocationY = aY;

		try{
			 final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
			 disp.syncExec(new Runnable() {
							 public void run() {
												   org.eclipse.swt.graphics.Point topleftposition = eswtCanvasControl.getLocation();
												   eSWTcontrolLocationX = topleftposition.x;
												   eSWTcontrolLocationY = topleftposition.y;

								    		   }
			    });
		 }
		 catch(Exception e)
		 {
			System.out.println("inside videoControl's setDisplaySize....exception is " + e.toString());
		 }
		// To Avoid UI thread block
		System.out.println("inside videoControl's setDisplaySize before calling _setPosition " );
		_setPosition(nativeDisplayHandle,
							iEventSourceHandle,
		 						eSWTcontrolLocationX,
		 							eSWTcontrolLocationY,
		 								videoControlLocationX,
		 									videoControlLocationY);
		System.out.println("inside videoControl's setDisplaySize after calling _setPosition " );

		try{
		 final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
		 disp.syncExec(new Runnable() {
						 public void run() {
							 System.out.println("inside videoControl's setDisplaySize before redraw() " );
									eswtCanvasControl.redraw();
							 System.out.println("inside videoControl's setDisplaySize after redraw() " );
										   }
					    });
		 }
		 catch(Exception e)
		 {
			System.out.println("inside videoControl's setDisplayLocation in redraw()....exception is " + e.toString());
		 }


	System.out.println("inside videoControl's setDisplayLocation coming out of setDisplayLocation()");
	}


	public void setVisible(boolean aVisible)
	{
		try{
			final boolean visible = aVisible;
		    final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
				 disp.asyncExec(new Runnable() {
								 public void run() {
									   //eswtCanvasControl.setVisible(visible);
									   // call native side to set the visibiity
									   _setVisible(nativeDisplayHandle,visible);
								 }
			    });
		   }
	    catch(Exception e)
		   {
				System.out.println("inside videoControl's setDisplaySize....exception is " + e.toString());
		   }
		   //System.out.println("inside videoControl's setVisible before native call");
		  // _setVisible(nativeDisplayHandle,aVisible);
		  // System.out.println("inside videoControl's setVisible after native call");
	}

	public int getDisplayHeight()
	{


			try{
				final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
				disp.syncExec(new Runnable() {

								 public void run() {
									y = eswtCanvasControl.getSize().y;
											 }
				});
		   }
	     catch(Exception e)
		   {
				System.out.println("inside videoControl's setDisplaySize....exception is " + e.toString());
		   }
 	return y;

	}

   public int getDisplayWidth()
   {
	      try{
				final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
				 disp.syncExec(new Runnable() {
								 public void run() {
									  x = eswtCanvasControl.getSize().x;

								 }
				});
	   		   }
	   	     catch(Exception e)
	   		   {
	   			System.out.println("inside videoControl's setDisplaySize....exception is " + e.toString());
		       }

		       return x;

   }
	public int getDisplayX()
	{
		try{
				final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
				 disp.syncExec(new Runnable() {
								 public void run() {
								x = eswtCanvasControl.getLocation().x;

								 }
				});
		    }
		catch(Exception e)
			{
			System.out.println("inside videoControl's setDisplaySize....exception is " + e.toString());
	        }
    return x;

	}

	public int getDisplayY()
	{



				try{
						final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
						 disp.syncExec(new Runnable() {
										 public void run() {
											 y = eswtCanvasControl.getLocation().y;

										 }
						});
				    }
				catch(Exception e)
					{
					System.out.println("inside videoControl's setDisplaySize....exception is " + e.toString());
	        }
      return y;
	 }

	public int getSourceWidth()
	{
		// return width of the source video

		return 0;

	}

	public int getSourceHeight()
	{
		// return height of the source video

		return 0;
	}

 public void getBoundRect()
 {
   System.out.println("MMACanvasDisplay.java :getBoundRect()");

	try{
			final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
			 disp.syncExec(new Runnable() {
							 public void run() {
								 Rectangle boundrect  = eswtCanvasControl.getBounds();
								 displayboundarr[0] = boundrect.x ;
								 displayboundarr[1] = boundrect.y ;
								 displayboundarr[2] = boundrect.width ;
								 displayboundarr[3] = boundrect.height ;

							 }
			});
		}
	catch(Exception e)
		{
		System.out.println("inside videoControl's setDisplaySize....exception is " + e.toString());
		}
  System.out.println("MMACanvasDisplay.java :getBoundRect() x =" + displayboundarr[0] + "y ="+ displayboundarr[1] +"width ="+ displayboundarr[2] +"height =" + displayboundarr[3] );
 }


public void getContainerRect()
 {
    // this function is not used for canvas case assuming that in case of canvas Bound rect and container rect is same.
    // to do: need to confirm
   getBoundRect();

 }

public void setContainerVisibilityToNative(final boolean active)
 {
   System.out.println("MMACanvasDisplay.java : SetContainerVisibilityToNative + ");
   new Thread(){
   	public void run(){
   		System.out.println("MMACanvasDisplay.java : SetContainerVisibilityToNative execute the native function in new thread ");
   		 _setContainerVisible(iEventSourceHandle,nativeDisplayHandle,active);
   	} 
  }.start();
  
   System.out.println("MMACanvasDisplay.java : SetContainerVisibilityToNative - ");
 }

public void setWindowResources()
{
	System.out.println("MMACanvasDisplay.java: setWindowResources windowHandle ");
	try{
			final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();

			 disp.syncExec(new Runnable() {
							 public void run() {
								Shell shell = eswtCanvasControl.getShell();
								qwidgetHandle = Internal_PackageSupport.topHandle(shell);
								x = eswtCanvasControl.getSize().x;
								y = eswtCanvasControl.getSize().y;
								_setWindowToNative(nativeDisplayHandle,qwidgetHandle);
								System.out.println("MMACanvasDisplay.java: setWindowResources qwidgetHandle is " + qwidgetHandle);
							 }
			});
		}
	catch(Exception e)
		{
		System.out.println("inside videoControl's setDisplaySize....exception is " + e.toString());
		}
		_setNativeWindowSize(nativeDisplayHandle,iEventSourceHandle,x,y);

}

/*
// dummy

public void dummy()
{
	_nativeDummy();
}
*/
/*
	public void getDisplaySize(int width, int height)
	{

		try{
			final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
			 disp.syncExec(new Runnable() {
							 public void run() {
								  x = eswtCanvasControl.getSize().x;
								  y = eswtCanvasControl.getSize().y;

							 }
				});
			}
	   catch(Exception e)
			   		   {
			   			System.out.println("inside videoControl's setDisplaySize....exception is " + e.toString());
		       }

		       width = x;
		       height = y;
	}

	*/
    // Native methods


	 private native void _setVisible(int nativeDisplayHandle,
	                                         boolean value);

	 private native void _setContainerVisible(int iEventSourceHandle,
	 																						int nativeDisplayHandle,
		                                     					boolean value);
	private native void _setFullScreenMode(int nativeDisplayHandle,	boolean value);
	private native void _setWindowToNative(int nativeDisplayHandle,int qwidgetHandle);
	private native void _setNativeWindowSize(int nativeDisplayHandle,int iEventSourceHandle, int width,int height);
	private native void _setPosition(int nativeDisplayHandle,
																			int iEventSourceHandle,
																						int uiControlLocationX,
																										int uiControlLocationY,
																														int videoControlLocationX,
																																	int videoControlLocationY);
	private native void _setDisplaySize(int nativeDisplayHandle,
																						int iEventSourceHandle,
		 																							int	eSWTcontrolLocationX,
		 																										int	eSWTcontrolLocationY);																																
}
