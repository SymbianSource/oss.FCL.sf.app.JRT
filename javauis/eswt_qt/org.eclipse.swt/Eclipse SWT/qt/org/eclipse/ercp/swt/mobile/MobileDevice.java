/*******************************************************************************
 * Copyright (c) 2004 IBM Corp.
 * Portion Copyright (c) 2005, 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Mark Rogalski (IBM Corp.) - initial API specification
 *    Nokia Corporation - S60 implementation
 *    Nokia Corporation - QT implementation
 *******************************************************************************/

package org.eclipse.ercp.swt.mobile;

import java.util.Vector;

import org.eclipse.swt.SWT;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Internal_PackageSupport;

/** 
 * Instances of this class represent the device that is being used.
 * It provides methods that enable applications to learn more about the device 
 * specific characteristics and capabilities.
 * <p>
 * Applications can query what input features and display screens are permanently 
 * part of the device. These are considered <i>local</i> features. Some devices also
 * allow input mechanisms or screens to be attached to the device at runtime. These
 * are considered <i>remote</i> features.  Since local features do not come and go,
 * it is sufficient to query for them once. On the other hand, since remote devices
 * can be added or removed at any time, an application needs to add a <code>
 * MobileDeviceListener</code> to be informed of these events.
 * <p> 
 * Local features may also be <i>internal</i> or <i>external</i>. 
 * External features are only available when a device is closed.
 * Internal features are only available when a device is opened.
 *
 * @see Screen
 * @see Input
 * @see MobileDeviceListener
 */
public class MobileDevice {

    private static int 		handleMobileDevice;
    private Vector		mobileDeviceListeners;
    private Screen[]	screens;
    private Input[]		inputs;
    private int			alertLevel;
    private boolean		isOpen;
    private static int desktopWidgetHandle;
    static MobileDevice currentMobileDevice;
    private int activeScreenId;
    
    

    
    /** 
     * feature which is permanently attached to the mobile device and always available
     */	
    public static final int LOCAL = 0;

	/**
	 * local feature which is only available when mobile device is opened
	 */
	public static final int INTERNAL = 1;

	/**
	 * local feature which is only available when mobile device is closed
	 */
	public static final int EXTERNAL = 2;
	
	/**
	 * feature which is not part of the mobile device unless connected via wire or wireless signal
	 */
	public static final int REMOTE = 3;
	
	/**
	 * alert level indicating simple notification (ex. e-mail arrival)
	 */
	public static final int ALERT_LOW_IMPORTANCE = 1;
	
	/**
	 * alert level indicating user attention desired (ex. instant message arrival)
	 */
	public static final int ALERT_HIGH_IMPORTANCE = 2;
	
	/**
	 * alert level indicating immediate attention is required (ex. battery level at 1%)
	 */
	public static final int ALERT_CRITICAL = 3;
   /**
    * virtual keyboard status indicating normal show and hide operation
    */
	public static final int VK_NORMAL = 1;

	/**
	 * virtual keyboard status indicating keyboard is always visible
	 */
	public static final int VK_ALWAYS_ON = 2;

	/**
	 * virtual keyboard status indicating keyboard is always hidden
	 */
	public static final int VK_ALWAYS_OFF = 3;	  
	
	/**
	 * Constructs a new instance of this class. Non-public to prevent
	 * instantiation outside this package.
	 * 
	 */
	MobileDevice () {
	   
	    (getDisplay()).syncExec(new Runnable() {
			public void run() {
				handleMobileDevice = Internal_PackageSupport.initializeMobileDevice(
						Internal_PackageSupport.getDisplayInstance());
				OS.MobileDevice_createFlipWatch(handleMobileDevice);
			}
		}); 
	    mobileDeviceListeners = new Vector();
	    isOpen = true;
	}
	
	private static Display getDisplay(){
		Display display = Internal_PackageSupport.getDisplayInstance();
		
		// Following if-statement (getting default display) should be removed
		// if agreed that MobileDevice shouldn't create the display.
		if(display==null){
			display = Display.getDefault();
		}
		
		if(display == null || display.isDisposed()){
	    	SWT.error(SWT.ERROR_DEVICE_DISPOSED);
	    }
		return display;
	}
	
	private void sendMobileDeviceChangedEvent(int eventType) {
		// MobileDevice changed event for open and closed
		int count = mobileDeviceListeners.size();
		for (int i = 0; i <count; ++i) {
			MobileDeviceEvent event = new MobileDeviceEvent(this);
			event.type = eventType;
			MobileDeviceListener listener = (MobileDeviceListener) mobileDeviceListeners.elementAt(i);
			listener.deviceChanged(event);
		}
	}
	
	void qt_swt_event_mobiledevice_changed(boolean aStatus) {
		// update MobileDevice status to opened/closed
		isOpen = aStatus;
		
		if(isOpen){
			sendMobileDeviceChangedEvent(MobileDeviceEvent.OPENED);
		}else{
			sendMobileDeviceChangedEvent(MobileDeviceEvent.CLOSED);
		}
	}
	
	void qt_swt_event_mobiledevice_screen_activate(boolean aStatus) {
		
		if(screens.length > 0 && activeScreenId<screens.length){
			if(aStatus){
				// update the active screen
				activeScreenId = OS.getScreenDeviceNumber();
				//forward to screen activated event to new active screen
				screens[activeScreenId].internal_sendScreenEventActivated();
			}else {
				//forward to screen deactivated event to old active screen
				screens[activeScreenId].internal_sendScreenEventDeactivated();
			}
		}
	}
	
	void qt_swt_event_mobiledevice_orientationchanged() {
		activeScreenId = OS.getScreenDeviceNumber();
		if(activeScreenId<screens.length){
			// forward to orientation change event to active screen
			screens[activeScreenId].internal_sendOrientationScreenChanged();
		}
	}
	
	private void sendMobileDeviceScreenChangedEvent(int eventType, Screen screen) {
		int count = mobileDeviceListeners.size();
		for (int i = 0; i <count; ++i) {
			MobileDeviceEvent event = new MobileDeviceEvent(this);
			event.type = eventType;
			event.data = screen;
			event.feature = screen;
			MobileDeviceListener listener = (MobileDeviceListener) mobileDeviceListeners.elementAt(i);
			listener.screenChanged(event);
		}
	}
	
	void qt_signal_screen_changed(int aCount) {
		int type = 0;
		Screen screen = null;
		Screen []newScreens = new Screen[aCount];
		
		// if getScreen is not called and if still want 
		// screen changed event in the MobileDevice
		if(screens.length==0){
			getScreens();
		}
		
		if(screens.length > aCount){
			// screen removed
			int screenHandle = 0;
			for(int j=0; j<screens.length; j++) {
				// found removed screen
				boolean found = false;
				for(int i=0; i<aCount; i++) {     
					screenHandle = OS.QDesktopWidget_screen(desktopWidgetHandle, i);
					if(screens[j].screenHandle== screenHandle){
						newScreens[i] = screens[j];
						newScreens[i].id = i;
						found = true;
						break;
		        	 }
				}
		        if(!found){
		        	screen = screens[j];
		        	screens[j].internal_dispose();
		        } 
		    }
			screens = new Screen[aCount];
			// remove detached screen from 'screens'
			System.arraycopy(newScreens, 0, screens, 0, aCount);
			type = MobileDeviceEvent.REMOVED;
		} else if( screens.length < aCount){
			// screen attached
			for(int i=0; i<aCount; i++) {   
				// found added screen
				int screenHandle = OS.QDesktopWidget_screen(desktopWidgetHandle, i);
				boolean found=false;
				for(int j=0; j<screens.length; j++) {
					if(screens[j].screenHandle== screenHandle){
						newScreens[i] = screens[j];
						newScreens[i].id = i;
						found = true;
						break;
					}
				}
				if(!found){
					screen = screens[i];
					newScreens[i] = new Screen(i, desktopWidgetHandle);
				} 
			}
			screens = new Screen[aCount];
			// add attached screen to 'screens'
			System.arraycopy(newScreens, 0, screens, 0, aCount);
			type = MobileDeviceEvent.ADDED;
		}
		sendMobileDeviceScreenChangedEvent(type, screen);
		activeScreenId = OS.getScreenDeviceNumber();
	}
	
	private static void hookEvents() {
	    int screenSignalProxy = OS.SignalHandler_new(desktopWidgetHandle, getDisplay(), OS.QSIGNAL_QDESKTOPWIDGET_SCREENCOUNTCHANGED);
	    OS.QObject_connectOrThrow(desktopWidgetHandle, "screenCountChanged(int)", screenSignalProxy,
	            "widgetSignal(int)", OS.QT_AUTOCONNECTION);
	}

	/**
	 * Adds the listener to the collection of listeners who will be notified when a device 
	 * configuration change occurs, by calling one of the methods defined in the 
	 * <code>MobileDeviceListener</code> interface. 
	 * <p>
	 * <code>screenChanged</code> is called when a monitor configuration changes.<br>
	 * <code>inputChanged</code> is called when an input configuration changes.<br>
	 * <code>deviceChanged</code> is called when the device is opened or closed.<br>
	 *
	 * @param listener instance called when device events occur
	 *
	 * @exception IllegalArgumentException <ul>
	 *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
	 * </ul>
	 * @exception SWTError <ul>
	 *    <li>ERROR_ITEM_NOT_ADDED - if the operation fails because of an operating system failure</li>
	 * </ul>
	 *
	 * @see #removeMobileDeviceListener
	 * @see MobileDeviceListener
	 */
	public void addMobileDeviceListener(MobileDeviceListener listener) {
	    if (listener == null) {
			SWT.error (SWT.ERROR_NULL_ARGUMENT);
		}
		mobileDeviceListeners.addElement(listener);
	}
	
	
	/**
	 * Alerts the user using device specific mechanisms such as sounds, flashing, or
	 * vibration. <code>Level</code> must be one of ALERT_LOW_IMPORTANCE,
	 * ALERT_HIGH_IMPORTANCE, or ALERT_CRITICAL. The method maps these levels to
	 * device specific features that may also be dependent upon user configuration
	 * or current profile.
	 * 
	 * @param level
	 *            constant describing the importance of the alert
	 * 
	 * @exception IllegalArgumentException
	 *                <ul>
	 *                <li>ERROR_INVALID_ARGUMENT - if level is not one of the
	 *                allowed class constants</li>
	 *                </ul>
	 * 
	 * @see #ALERT_LOW_IMPORTANCE
	 * @see #ALERT_HIGH_IMPORTANCE
	 * @see #ALERT_CRITICAL
	 */
	public void alert(int level) {
	    if (	level != ALERT_LOW_IMPORTANCE &&
	            level != ALERT_HIGH_IMPORTANCE &&
	            level != ALERT_CRITICAL) {
	        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
	    }
	    alertLevel = level;
		(getDisplay()).syncExec(new Runnable() {
			public void run() {
				if(alertLevel == ALERT_LOW_IMPORTANCE){
					OS.QApplication_beep();
				}else if(alertLevel == ALERT_HIGH_IMPORTANCE){
					OS.MobileDevice_flashLights(handleMobileDevice, 3000);
					OS.MobileDevice_vibration(handleMobileDevice, 3000);
				}else if(alertLevel == ALERT_CRITICAL){
					OS.QApplication_beep();
					OS.MobileDevice_flashLights(handleMobileDevice, 3000);
					OS.MobileDevice_vibration(handleMobileDevice, 3000);
				}
			   
			}
		}); 
	}
	
	
	/**
	 * Returns an array of Input objects describing the input features available to the device. 
	 * The return value may be <code>null</code> if there are no input features available.
	 * 
	 * @exception SWTError <ul>
	 *    <li>ERROR_CANNOT_GET_SELECTION - if the operation fails because 
	 * 		of an operating system failure</li>
	 * </ul> 
	 *
	 * @see Input
	 */
	public Input[] getInputs() {
		if(inputs == null) {
		    try {
			    (getDisplay()).syncExec(new Runnable() {
					public void run() {
						int mask = OS.getHwInputs();
						int nbInputs = 0;
						
						// finding out number of keyboards available
						if((mask & OS.SYMBIAN_KEYBOARD_FULL)!=0){
							nbInputs+=2;
						}
						
						if((mask & OS.SYMBIAN_KEYBOARD_KEYPAD)!=0){
							nbInputs+=2;
						}
						// create java counter part of Inputs and
						// add location and type
					    inputs = new Input[nbInputs];
					    int i=0;
					    int location;
					    if((mask & OS.SYMBIAN_KEYBOARD_FULL)!=0){
					    	if((mask & OS.SYMBIAN_KEYBOARD_KEYPAD)!=0){
								location = MobileDevice.INTERNAL;
							} else {
								location = MobileDevice.LOCAL;
							}
					    	inputs[i] = new Input(i);
					    	inputs[i].setLocation(location);
					    	inputs[i].setType(Input.SOFTKEYS);
					    	i+=1;
					    	inputs[i] = new Input(i);
					    	inputs[i].setLocation(location);
					    	inputs[i].setType(Input.FULL_KEYBOARD);
					    	i+=1;
						}
					    
					    if((mask & OS.SYMBIAN_KEYBOARD_KEYPAD)!=0){
					    	if((mask & OS.SYMBIAN_KEYBOARD_FULL)!=0){
								location = MobileDevice.EXTERNAL;
							} else {
								location = MobileDevice.LOCAL;
							}
					    	inputs[i] = new Input(i);
					    	inputs[i].setLocation(location);
					    	inputs[i].setType(Input.SOFTKEYS);
					    	i+=1;
					    	inputs[i] = new Input(i);
					    	inputs[i].setLocation(location);
					    	inputs[i].setType(Input.KEYPAD);
						}
					}
				});
		    } catch(Throwable e) {
		        SWT.error(SWT.ERROR_CANNOT_GET_SELECTION);	       
		    }
	    }
	    return inputs;
	}
	
	
	/**
	 * Returns an array of Screen objects describing the display features available to the device. 
	 * The return value may be <code>null</code> if there are no display screens available.
	 * 
	 * @exception SWTError <ul>
	 *    <li>ERROR_CANNOT_GET_SELECTION - if the operation fails because 
	 * 		of an operating system failure</li>
	 * </ul> 
	 *
	 * @see Screen
	 */
	public Screen[] getScreens() {
		if(screens == null) {
		    try {
			    (getDisplay()).syncExec(new Runnable() {
					public void run() {
					    int nbScreens = OS.QDesktopWidget_screenCount(desktopWidgetHandle);
					    activeScreenId = OS.getScreenDeviceNumber();
					    screens = new Screen[nbScreens];
					    for(int i=0; i<nbScreens; i++) {
					        screens[i] = new Screen(i, desktopWidgetHandle);
					    }
					}
				});
		    } catch(Throwable e) {
		        SWT.error(SWT.ERROR_CANNOT_GET_SELECTION);	       
		    }
	    }
	    return screens;
	}
	
	
	/**
	 * Returns singleton instance of MobileDevice class.
	 * 
	 * @return singleton of MobileDevice class. Must not be Null.
	 * 
	 * @exception SWTError <ul>
	 *    <li>ERROR_CANNOT_GET_SELECTION - if the operation fails because of 
	 * 		an operating system failure</li>
	 * </ul> 
	 */ 
	public static MobileDevice getMobileDevice() {
	    if(currentMobileDevice == null) {
	        try {
		        currentMobileDevice = new MobileDevice(); 
		        Internal_PackageSupport.setMobileDevice(getDisplay(),currentMobileDevice);
			    desktopWidgetHandle = OS.QApplication_desktop();
			    hookEvents();
	        } catch (Throwable e) {
	            SWT.error(SWT.ERROR_CANNOT_GET_SELECTION);
	        }
	    }
	    return currentMobileDevice;
	}
	 
	 
	/**
	 * Returns whether device is opened. For devices that have no internal screens 
	 * or input features, the method always returns <code>true</code>.
	 */
	public boolean isOpen() { 
	    getDisplay();

	    return isOpen; 
	}

	
	/**
	 * Call to dispose the Screen object.
	 *
	 */
	void internal_dispose() {
		if (inputs != null) {
			for (int i = 0; i < inputs.length; i++) {
				if (inputs[i] != null) {
					inputs[i].internal_dispose();
				}
			}
		}

		if (screens != null) {
			for (int i = 0; i < screens.length; i++) {
				if (screens[i] != null) {
					screens[i].internal_dispose();
				}
			}
		}
		// actual native counter part is deleted in display
		currentMobileDevice = null;
		handleMobileDevice = 0;
		inputs = null;
		screens = null;
		desktopWidgetHandle = 0;
	}
	
	
	/**
	 * Removes the listener from the collection of listeners who will be notified when a device 
	 * configuration change occurs. 
	 *
	 * @param listener instance called when device events occur
	 *
	 * @exception IllegalArgumentException <ul>
	 *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
	 * </ul>
	 * @exception SWTError <ul>
	 *    <li>ERROR_ITEM_NOT_REMOVED - if the operation fails because 
	 * 		of an operating system failure</li>
	 * </ul>
	 *
	 * @see #addMobileDeviceListener
	 * @see MobileDeviceListener
	 */
	public void removeMobileDeviceListener(MobileDeviceListener listener) {
		if (listener == null) {
			SWT.error (SWT.ERROR_NULL_ARGUMENT);
		}

		//remove listener
		//Remove method of Vector class never fails,
		//so ERROR_ITEM_NOT_REMOVED could never be thrown.
		mobileDeviceListeners.removeElement(listener);
	}
	
	 /**
	  * Sets the status of the system virtual keyboard (if one is available).
	  * By default, or when status is set to VK_NORMAL, a system virtual keyboard
	  * is displayed when a widget capable of text input gains focus and is
	  * hidden when that widget looses focus. However, there are cases where 
	  * an application may wish to continuously display the virtual keyboard
	  * or always keep it hidden. Setting the status to VK_ALWAYS_ON or VK_ALWAYS_OFF
	  * will accomplish this and the effect is immediate. Changing focus will
	  * then have no affect on the virtual keyboard. Note: By default, widgets which 
	  * normally accept input but are set to <i>read only</i> do not cause the 
	  * the virtual keyboard to display.
	  *
	  * @param status virtual keyboard mode. One of VK_NORMAL, VK_ALWAYS_ON, VK_ALWAYS_OFF
	  * 
	  * @exception IllegalArgumentException <ul>
	  *    <li>ERROR_INVALID_ARGUMENT - if the status parameter is not valid</li>
	  * </ul>
	  *
	  * @see #VK_NORMAL
	  * @see #VK_ALWAYS_ON
	  * @see #VK_ALWAYS_OFF
	  */
	public void setVKStatus(int status){
		
	}
}
