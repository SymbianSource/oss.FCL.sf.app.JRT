/*******************************************************************************
 * Copyright (c) 2004 IBM Corp.
 * Portion Copyright (c) 2004, 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Mark Rogalski (IBM Corp.) - initial API specification
 *     Nokia Corporation - S60 implementation
 *     Nokia Corporation - Qt implementation
 *******************************************************************************/

package org.eclipse.ercp.swt.mobile;

import java.util.Vector;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Internal_PackageSupport;


/**
 * Instances of this class represent display screens available for application
 * use.
 * 
 * <dl>
 * <dt><b>Screen usage types: </b></dt>
 * <dd>PRIMARY</dd>
 * <dd>SECONDARY</dd>
 * <dd>STATUS</dd>
 * </dl>
 * 
 * <dl>
 * <dt><b>Screen orientation styles: </b></dt>
 * <dd>PORTRAIT</dd>
 * <dd>LANDSCAPE</dd>
 * </dl>
 * 
 * @see MobileDevice
 * @see MobileDeviceListener
 */
public class Screen {
	/**
	 * Handle of this Object Screen.
	 */
	int 			screenHandle;
	
	/**
	 * Indicate if the screen is disposed.
	 */
	private boolean			isDisposed;
	
	/**
	 * All ScreenListener on this Screeen.
	 */
	private	Vector			screenListeners;

	/**
	 * one primary screen may be active at a time
	 */
	public static final int PRIMARY = 1;

	/**
	 * multiple secondary screens may be simultaneously active
	 */
	public static final int SECONDARY = 2;

	/**
	 * sub type of secondary screen that shows minimal content for notification
	 * purposes
	 */
	public static final int STATUS = 3;

	/**
	 * indicates text is normally written across the shortest width
	 */
	public static final int PORTRAIT = 0;

	/**
	 * indicates text is normally written across the longest width
	 */
	public static final int LANDSCAPE = 1;

	/**
	 * The Screen used by the Runnable.
	 */

	/**
	 * The bounds used by the Runnable.
	 */
	private Rectangle boundsForRunnable;

	/**
	 * The Color depth used by the Runnable.
	 */
	private int colorDepthForRunnable;

	/**
	 * The location used by the Runnable.
	 */
	private int locationForRunnable;

	/**
	 * The orientation used by the Runnable.
	 */
	private int orientationGetForRunnable;

	/**
	 * The usage used by the Runnable.
	 */
	private int usageForRunnable;

	/**
	 * The activation state used by the Runnable.
	 */
	boolean isActiveForRunnable;

	boolean isActive;
	
	/**
	 * The Touchscreen state used by the Runnable.
	 */
	private boolean isTouchScreenForRunnable;

	/**
	 * The Orientation state used by the Runnable.
	 */
	private int orientationSetForRunnable;
	
	/**
	 * Screen id.
	 */
	int id;
	
	private int desktopWidgetHandle;
	
	/**
	 * Constructs a new instance of this class. Non-public to prevent
	 * instantiation outside of this package.
	 * 
	 * @see MobileDevice#getLocalScreens
	 */
	Screen( int aScreenId, int aDesktopWidgetHandle){
	    id = aScreenId;
	    desktopWidgetHandle = aDesktopWidgetHandle;
		(getDisplay()).syncExec(new Runnable() {
			public void run() {
				screenHandle = OS.QDesktopWidget_screen(desktopWidgetHandle, id);
			}
		});
		screenListeners = new Vector();
		isDisposed = false;
	}

	/**
	 * Adds the listener to the collection of listeners that will be notified
	 * when screen events occur by sending it one of the messages defined in the
	 * ScreenListener interface.
	 * <p>
	 * <code>screenActivated</code> is called when a screen becomes active. That
	 * is, the screen is powered and contents are visible to the user.
	 * <br>
	 * <code>screenDeactivated</code> is called when a screen is no longer
	 * active. <br>
	 * 
	 * @param listener
	 *            instance called when screen events occur
	 * 
	 * @exception IllegalArgumentException
	 *                <ul>
	 *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
	 *                </ul>
	 * @exception SWTException
	 *                <ul>
	 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
	 *                disposed</li>
	 *                </ul>
	 * @exception SWTError
	 *                <ul>
	 *                <li>ERROR_ITEM_NOT_ADDED - if the operation fails
	 *                because of an operating system failure</li>
	 *                </ul>
	 * 
	 * @see ScreenListener
	 * @see Screen#removeEventListener
	 */
	public void addEventListener(ScreenListener listener) {
		check();
		
		if (listener == null) {
			SWT.error (SWT.ERROR_NULL_ARGUMENT);
		}
		
		screenListeners.addElement(listener);
	}

	/**
	 * Returns the bounds of the screen. Application contents must be within
	 * these bounds to be visible on the given device screen. Note, the bounds
	 * may not have a 0,0 origin.
	 * 
	 * @return Rectangle The bounds of the sceen.
	 * @exception SWTException
	 *                <ul>
	 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
	 *                disposed</li>
	 *                </ul>
	 * @exception SWTError
	 *                <ul>
	 *                <li>ERROR_CANNOT_GET_SELECTION - if the operation fails
	 *                because of an operating system failure</li>
	 *                </ul>
	 *  
	 */
	
	public Rectangle getBounds() {
		check();
		(getDisplay()).syncExec(new Runnable() {
			public void run() {
				boundsForRunnable = OS.QDesktopWidget_screenGeometry(screenHandle, id);
			}
		}); 
		
		return boundsForRunnable;
	}

	/**
	 * Returns the color depth of the screen in bits per pixel.
	 * 
	 * @return int the color depth.
	 * @exception SWTException
	 *                <ul>
	 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
	 *                disposed</li>
	 *                </ul>
	 * @exception SWTError
	 *                <ul>
	 *                <li>ERROR_CANNOT_GET_SELECTION - if the operation fails
	 *                because of an operating system failure</li>
	 *                </ul>
	 *  
	 */
	public int getColorDepth() {
		check();
		
		(getDisplay()).syncExec(new Runnable() {
			public void run() {
				if(isActive()){
					colorDepthForRunnable = OS.getColorDepth();
				} else{ 
					colorDepthForRunnable = -1;
				}
			}
		});  

		return colorDepthForRunnable;
	}

	/**
	 * Returns the location of the screen device.
	 * 
	 * @return int The screen location. The value is one of
	 *         {@link MobileDevice#LOCAL LOCAL},
	 *         {@link MobileDevice#INTERNAL INTERNAL},
	 *         {@link MobileDevice#EXTERNAL EXTERNAL}, or
	 *         {@link MobileDevice#REMOTE REMOTE}
	 * 
	 * @exception SWTException
	 *                <ul>
	 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
	 *                disposed</li>
	 *                </ul>
	 * @exception SWTError
	 *                <ul>
	 *                <li>ERROR_CANNOT_GET_SELECTION - if the operation fails
	 *                because of an operating system failure</li>
	 *                </ul>
	 * 
	 * @see MobileDevice#LOCAL
	 * @see MobileDevice#INTERNAL
	 * @see MobileDevice#EXTERNAL
	 * @see MobileDevice#REMOTE
	 */
	public int getLocation() {
		check();
		
		(getDisplay()).syncExec(new Runnable() {
			public void run() {
				if(OS.QDesktopWidget_screenCount(desktopWidgetHandle)>1){
					if(OS.QDesktopWidget_primaryScreen(desktopWidgetHandle) == id){
						locationForRunnable = MobileDevice.INTERNAL;
					}else{
						locationForRunnable = MobileDevice.EXTERNAL;
					}
				} else{
					locationForRunnable = MobileDevice.LOCAL;
				}
			}
		}); 

		return locationForRunnable;
	}

	/**
	 * Returns the screen orientation.
	 * 
	 * @return int The screen orientation. The value must be either PORTRAIT or
	 *         LANDSCAPE.
	 * @exception SWTException
	 *                <ul>
	 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
	 *                disposed</li>
	 *                </ul>
	 * @exception SWTError
	 *                <ul>
	 *                <li>ERROR_CANNOT_GET_SELECTION - if the operation fails
	 *                because of an operating system failure</li>
	 *                </ul>
	 * 
	 * @see Screen#PORTRAIT
	 * @see Screen#LANDSCAPE
	 * @see #setOrientation(int)
	 */
	public int getOrientation() {
		check();
		(getDisplay()).syncExec(new Runnable() {
			public void run() {
				Rectangle bounds= getBounds();
				if(bounds.width>bounds.height){
					orientationGetForRunnable = Screen.LANDSCAPE;
				}else {
					orientationGetForRunnable = Screen.PORTRAIT;
				}
			}
		});
		return orientationGetForRunnable;
	}

	/**
	 * Returns the usage type of the screen.
	 * 
	 * @return int The type of the screen. The value must be one of PRIMARY,
	 *         SECONDARY and STATUS</dd>
	 * @exception SWTException
	 *                <ul>
	 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
	 *                disposed</li>
	 *                </ul>
	 * @exception SWTError
	 *                <ul>
	 *                <li>ERROR_CANNOT_GET_SELECTION - if the operation fails
	 *                because of an operating system failure</li>
	 *                </ul>
	 * 
	 * @see Screen#PRIMARY
	 * @see Screen#SECONDARY
	 * @see Screen#STATUS
	 */
	public int getUsage() {
		check();
		
		(getDisplay()).syncExec(new Runnable() {
			public void run() {
				if(OS.QDesktopWidget_primaryScreen(desktopWidgetHandle) == 
					OS.QDesktopWidget_screenNumber(desktopWidgetHandle, screenHandle)){
					usageForRunnable = PRIMARY;	
				} else {
					usageForRunnable = SECONDARY;
				}
			}
		});

		return usageForRunnable;
	}

	/**
	 * Returns whether the screen is currently active. That is, whether it is
	 * powered up and contents are visible to the user.
	 * 
	 * @return boolean The screen status.
	 * @exception SWTException
	 *                <ul>
	 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
	 *                disposed</li>
	 *                </ul>
	 * @exception SWTError
	 *                <ul>
	 *                <li>ERROR_CANNOT_GET_SELECTION - if the operation fails
	 *                because of an operating system failure</li>
	 *                </ul>
	 *  
	 */
	public boolean isActive() {
		check();
		
		(getDisplay()).syncExec(new Runnable() {
			public void run() {
				if(!(OS.QDesktopWidget_screenCount(desktopWidgetHandle)==1)){
					if(OS.getScreenDeviceNumber() == id){
						isActive = isActiveForRunnable;
					} else {
						isActive = false;
					}
				}else{
					isActive = true;
				}
			}
		}); 

		return isActive;
	}

	/**
	 * Returns whether the screen has touch screen capability.
	 * 
	 * @return boolean The touch screen capability.
	 * @exception SWTException
	 *                <ul>
	 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
	 *                disposed</li>
	 *                </ul>
	 * @exception SWTError
	 *                <ul>
	 *                <li>ERROR_CANNOT_GET_SELECTION - if the operation fails
	 *                because of an operating system failure</li>
	 *                </ul>
	 *  
	 */
	public boolean isTouchScreen() {
		check();
		
		(getDisplay()).syncExec(new Runnable() {
			public void run() {
				if(isActive()){
					isTouchScreenForRunnable = true;
				}else {
					isTouchScreenForRunnable = false;
				}
			}
		});  

		return isTouchScreenForRunnable;
	}

	/**
	 * Removes the listener from the collection of listeners that will be
	 * notified when screen events occur.
	 * 
	 * @param listener
	 *            instance called when screen events occur
	 * 
	 * @exception IllegalArgumentException
	 *                <ul>
	 *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
	 *                </ul>
	 * @exception SWTException
	 *                <ul>
	 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
	 *                disposed</li>
	 *                </ul>
	 * @exception SWTError
	 *                <ul>
	 *                <li>ERROR_ITEM_NOT_REMOVED - if the operation fails
	 *                because of an operating system failure</li>
	 *                </ul
	 * 
	 * @see ScreenListener
	 * @see Screen#addEventListener
	 */
	public void removeEventListener(ScreenListener listener) {
		check();
		
		if (listener == null) {
			SWT.error (SWT.ERROR_NULL_ARGUMENT);
		}
		
		screenListeners.removeElement(listener);
	}

	/**
	 * Sets the screen orientation if supported on this device. Note that this
	 * feature is implementation-dependent.
	 * 
	 * @param orientation
	 *            the caller's preferred orientation. Must be one of PORTRAIT and
	 *            LANDSCAPE.
	 * 
	 * @exception IllegalArgumentException
	 *                <ul>
	 *                <li>ERROR_INVALID_ARGUMENT - if the argument is not
	 *                either PORTRAIT or LANDSCAPE</li>
	 *                </ul>
	 * @exception SWTException
	 *                <ul>
	 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
	 *                disposed</li>
	 *                <li>ERROR_CANNOT_SET_SELECTION - if the device could not set
	 * 				  the orientation</li>
	 *                </ul>
	 * 
	 * @see Screen#PORTRAIT
	 * @see Screen#LANDSCAPE
	 * @see #getOrientation()
	 */
	public void setOrientation(int orientation) {
		check();
		if ( (orientation != PORTRAIT) && (orientation != LANDSCAPE) ) {
			SWT.error (SWT.ERROR_INVALID_ARGUMENT);
		}

		orientationSetForRunnable = orientation;
		getDisplay();
	}
	
	/**
	 * Call to dispose the Screen object.
	 *
	 */
	void internal_dispose() {		
		isDisposed = true;
		screenListeners = null;
		screenHandle = 0;
	}
	
	/**
	 * Verify if the object is disposed.
	 * 
	 */
	private void check() {
		if (isDisposed == true) {
			SWT.error (SWT.ERROR_WIDGET_DISPOSED);
		}
	}
	
	private ScreenEvent getAScreenEvent() {
		ScreenEvent screenEvent = new ScreenEvent(this);
		screenEvent.isActive = isActive();
		screenEvent.orientation = getOrientation();
		return screenEvent;
	}

	void internal_sendScreenEventActivated() {
		isActiveForRunnable = true;
		ScreenEvent screenEvent = getAScreenEvent();

		int count = screenListeners.size();
		for (int i = 0; i < count; ++i) {
			ScreenListener listener = (ScreenListener)screenListeners.elementAt(i);
			listener.screenActivated(screenEvent);	
		}
	}
	
	void internal_sendScreenEventDeactivated() {
		isActiveForRunnable = false;
		ScreenEvent screenEvent = getAScreenEvent();

		int count = screenListeners.size();
		for (int i = 0; i < count; ++i) {
			ScreenListener listener =(ScreenListener)screenListeners.elementAt(i);
			listener.screenDeactivated(screenEvent);	
		}
	}
	
	void internal_sendOrientationScreenChanged() {
		ScreenEvent screenEvent = getAScreenEvent();

		int count = screenListeners.size();
		for (int i = 0; i < count; ++i) {
			ScreenListener listener =(ScreenListener)screenListeners.elementAt(i);
			listener.screenOrientationChanged(screenEvent);	
		}
	}
	
	private static Display getDisplay(){
		Display display = Internal_PackageSupport.getDisplayInstance();
		
		// following if loop (getting default display) should be removed
		// if agreed that MobileDevice shouldn't create the display.
		if(display==null){
			display = Display.getDefault();
		}
		
		if(display == null || display.isDisposed()){
	    	SWT.error(SWT.ERROR_DEVICE_DISPOSED);
	    }
		return display;
	}
};
