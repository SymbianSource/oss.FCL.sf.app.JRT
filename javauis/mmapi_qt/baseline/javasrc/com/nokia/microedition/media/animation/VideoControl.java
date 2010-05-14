/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
/**
 * 
 */
package com.nokia.microedition.media.animation;

import javax.microedition.lcdui.Item;
import javax.microedition.media.MediaException;
import javax.microedition.media.Player;
import javax.microedition.media.PlayerListener;

import org.eclipse.ercp.swt.mobile.MobileShell;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;

//import com.nokia.microedition.media.control.ApplicationUtils;
import com.nokia.microedition.media.control.ControlImpl;
import com.nokia.microedition.media.control.MMAGUIFactory;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.media.PlayerPermission;
import com.nokia.mj.impl.nokialcdui.ItemControlStateChangeListener;
import com.nokia.mj.impl.nokialcdui.LCDUIInvoker;

//import com.nokia.mj.impl.media.PlayerPermission;
import com.nokia.mj.impl.rt.support.ApplicationUtils;

/**
 * @author d35kumar
 *
 */
public class VideoControl extends ControlImpl implements
		javax.microedition.media.control.VideoControl , ItemControlStateChangeListener{

	//	Following variable has been taken from VideoControl class
	private static final String GUI_OBJECT_CLASS_NAME =
        "javax.microedition.lcdui.Item";
	// lcdui package used with UiToolkitRegister
    private static String LCDUI_PACKAGE =
        "javax.microedition.lcdui";
    // eswt package used with UiToolkitRegister
    private static String ESWT_PACKAGE = "org.eclipse.swt.widgets";
 // ToolkitRegister class name used with eswt and lcdui
    private static String DISPLAY = ".Display";

    // class name used to check if eswt is included
    private static String LISTENER = ".Listener";

    // class name used to check if eswt is included
    private static String ESWT_CONTROL = ".control";
    // class name used with dynamic display mode initialization
    private static String GUI_FACTORY_CLASS_NAME = ".Factory";
    
	private static final int NOT_INITIALIZED = -1;
	protected int iStatus = NOT_INITIALIZED;
	private static final int UNDEFINED_RETURN_VALUE=0;
	// For integrating with eSWT API  
	private Display iDisplay; 
//	private Shell iShell;
	
	private Control iControl;
	
	// Global??? yes because we need to remove it from player listener, while finalizer will be called
	private VideoItem iVideoItem;
	
	private Finalizer mFinalizer = new Finalizer(){
        public void finalizeImpl(){
            doFinalize();
        }
    };

	/**
	 * Constructor of VideoControl  
	 * @param player
	 */
	public VideoControl(Player player){
		this.iPlayer=player;
	}
	
	/**
	 * 
	 */
	private void doFinalize() {
		if (mFinalizer != null) {
			registeredFinalize();
			mFinalizer = null;
		}
	}
	
	/**
	 * 
	 */
	final void registeredFinalize() {
		if (iVideoItem != null) {
			iPlayer.removePlayerListener(iVideoItem);
		}
	}

	/* (non-Javadoc)
	 * @see javax.microedition.media.control.VideoControl#getDisplayHeight()
	 */
	public int getDisplayHeight() {
		checkState();
		if (iStatus == NOT_INITIALIZED) {
			throw new IllegalStateException(
					"VideoControl.initDisplayMode() not called yet");
		}
		return ((AnimationPlayer)iPlayer).getImageDimension().x;
	}

	/* (non-Javadoc)
	 * @see javax.microedition.media.control.VideoControl#getDisplayWidth()
	 */
	public int getDisplayWidth() {
		checkState();
		if (iStatus == NOT_INITIALIZED) {
			throw new IllegalStateException(
					"VideoControl.initDisplayMode() not called yet");
		}
		return ((AnimationPlayer)iPlayer).getImageDimension().x;
	}

	/* (non-Javadoc)
	 * @see javax.microedition.media.control.VideoControl#getDisplayX()
	 */
	public int getDisplayX() {
		checkState();
        if (iStatus == NOT_INITIALIZED)
        {
            return UNDEFINED_RETURN_VALUE;
        }
		return ((AnimationPlayer)iPlayer).getiDisplayLocation().x;
	}

	/* (non-Javadoc)
	 * @see javax.microedition.media.control.VideoControl#getDisplayY()
	 */
	public int getDisplayY() {
		checkState();
        if (iStatus == NOT_INITIALIZED)
        {
            return UNDEFINED_RETURN_VALUE;
        }
		return ((AnimationPlayer)iPlayer).getiDisplayLocation().y;
	}

	/* (non-Javadoc)
	 * @see javax.microedition.media.control.VideoControl#getSnapshot(java.lang.String)
	 */
	public byte[] getSnapshot(String aImageType) throws MediaException {
		final String DEBUG_STR="VideoControl::getSnapshot(String aImageType)";
		System.out.println(DEBUG_STR+"+");
		checkState();
		if (iStatus == NOT_INITIALIZED) {
			throw new IllegalStateException(
					"VideoControl.initDisplayMode() not called yet");
		}
		int imageData[]=((AnimationPlayer)iPlayer).getCurrentFrame(aImageType);
		byte bytArry[]= new byte[imageData.length];
		int pixelCount=bytArry.length;
		System.out.println(DEBUG_STR+"imageData receved is "+imageData);
		for(int i=0;i<pixelCount;i++)
			bytArry[i]=(byte)imageData[i];
        // Check the permission here, so 'the moment' is not lost?
        //Security.ensurePermission(PERMISSION, PERMISSION, PERM_ARGS);
//        ApplicationUtils appUtils = ApplicationUtils.getInstance();
//        PlayerPermission per = new PlayerPermission("audio/video recording","snapshot");
//        appUtils.checkPermission(per);
		return bytArry;
	}

	/* (non-Javadoc)
	 * @see javax.microedition.media.control.VideoControl#getSourceHeight()
	 */
	public int getSourceHeight() {
		checkState();
		return ((AnimationPlayer)iPlayer).getSourceDimension().y;
	}

	/* (non-Javadoc)
	 * @see javax.microedition.media.control.VideoControl#getSourceWidth()
	 */
	public int getSourceWidth() {
		checkState();
		return ((AnimationPlayer)iPlayer).getSourceDimension().x;
	}

	/* (non-Javadoc)
	 * @see javax.microedition.media.control.VideoControl#initDisplayMode(int, java.lang.Object)
	 */
	public Object initDisplayMode(int aMode, Object aArg) {
		final String DEBUG_STRING = "VideoControl::initDisplayMode(int, Object)";
		System.out.println(DEBUG_STRING + "+");
		// To check if state is in not closed state
		checkState();
		System.out.println(DEBUG_STRING+"After checking state ");
		if (iStatus != NOT_INITIALIZED) {
			// IllegalStateException - Thrown if initDisplayMode is
			// called again after it has previously been called successfully.
			throw new IllegalStateException(
					"initDisplayMode() already called successfully");
		}
		System.out.println(DEBUG_STRING+"Going to get the eswtDisplay, mode is  "+aMode+" argument is "+aArg);
		// Get the Display object of ESWT
		if(aArg!=null && aArg.equals(Control.class.getName())){
			iDisplay =Display.getDefault();	
//			System.out.println(DEBUG_STRING+"Display object created"+iDisplay );
		}else{ 
			iDisplay = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
		}
//		System.out.println(DEBUG_STRING+"After getting the display"+iDisplay);
		if (aMode == USE_GUI_PRIMITIVE) {
			System.out.println(DEBUG_STRING + " mode is USE_GUI_PRIMITIVE");
			Object guiObject = null;
			if (aArg == null) {
				guiObject = initNullMode();
			} else {
				if (aArg.equals(GUI_OBJECT_CLASS_NAME)) {
					guiObject = initLCDUI();
				} else // try load dynamic display mode
				{
					guiObject = initDynamicDisplayMode(aArg);
					System.out.println(DEBUG_STRING+"After returning from the initDynamicDisplayMode() "+guiObject);
					// Since it is eswtControl itself so no need of LCDUIInvoker here
					// assign it in iControl and return immediately from here
					iControl=(Control)guiObject;
					// Now we will change the status, when MIDlet developer will call the setParent
					iStatus = USE_GUI_PRIMITIVE;
					return iControl;
				}
			}
			// this will return the control as null, because the item might not
			// have been appended in form
			// Solution to this is as:- Add the listener through LCDUIInvoker
			// and when item will be appended
			// by midlet developer, notifyControlAvailable function of
			// ItemStateChangeListener will be called
			iControl = com.nokia.mj.impl.nokialcdui.LCDUIInvoker
					.getEswtControl(guiObject);
			// add this class as item state change listener
			com.nokia.mj.impl.nokialcdui.LCDUIInvoker
					.setItemControlStateChangeListener(this, (Item)guiObject);
			iStatus = USE_GUI_PRIMITIVE;
			return guiObject;
		} else if (aMode == USE_DIRECT_VIDEO) {
			if (aArg != null) {
				if (!(aArg instanceof javax.microedition.lcdui.Canvas)) {
					throw new java.lang.IllegalArgumentException(
							"For USE_DIRECT_VIDEO mode argument should be of type Canvas");
				}
			} else {
				throw new java.lang.IllegalArgumentException(
						"For USE_DIRECT_VIDEO mode argument should not be null");
			}
			iControl = com.nokia.mj.impl.nokialcdui.LCDUIInvoker
					.getEswtControl(aArg);
			System.out.println(DEBUG_STRING
					+ "This is USE_DIRECT_VIDEO mode, control is " + iControl);
			iStatus = USE_DIRECT_VIDEO;
		} else {
			throw new java.lang.IllegalArgumentException(
					"Mode not supported or invalid, "
							+ "valid modes are USE_DIRECT_VIDEO and USE_GUI_PRIMITIVE");
		}
		System.out.println(DEBUG_STRING + "-");
		return null;
	}
	/**
	 * 
	 * @return
	 */
	private Object initLCDUI(){
		final String DEBUG_STR="VideoControl::initLCDUI()";
		System.out.println(DEBUG_STR+"+");
		iVideoItem = new VideoItem(((AnimationPlayer)iPlayer).getImageDimension());
        iPlayer.addPlayerListener(iVideoItem);
        iStatus = USE_GUI_PRIMITIVE;
        System.out.println(DEBUG_STR+"-");
        return iVideoItem;
	}

	/**
	    * Initializes USE_GUI_PRIMITIVE mode when null parameter is given to
	    * initDisplayMode method.
	    * UI toolkit gets selected when application uses UI toolkit
	    * first time. After this selection null parameter must be
	    * interpreted as the selected UI toolkit. initDisplayMode call
	    * with null parameter before the selection must cause
	    * IllegalArgumentException if there are several toolkits.
	    * @return GUI object
	    */
	private Object initNullMode() {
		String DEBUG_STR = "VideoControl::initNullMode()";
	
		String toolkit = null;
	
		Object guiObject = null;

		// If lcdui was selected init it even if there might be several
		// toolkits. This is done to support existing applications.
		if ((LCDUI_PACKAGE + DISPLAY).equals(toolkit)) {
			System.out.println(DEBUG_STR + "going to use initLCDUI function ");
			guiObject = initLCDUI();
		} else {
			try {
				System.out.println(DEBUG_STR + "Seems it is ESWT toolkit ");
				// Several UI toolkits are supported if there are eSWT classes
				// and eSWT direct content component
				// Trying to load eSWT Listener interface or eSWT GUI factory
				// does not cause any initialization to eSWT.
				Class.forName(ESWT_PACKAGE + LISTENER);
				Class.forName(ESWT_PACKAGE + ESWT_CONTROL
						+ GUI_FACTORY_CLASS_NAME);

				// check if eSWT was selected
				if ((ESWT_PACKAGE + DISPLAY).equals(toolkit))
				{
					System.out
							.println(DEBUG_STR
									+ "It is eswtPackage going to call initDynamicDisplay ");
					guiObject = initDynamicDisplayMode(ESWT_PACKAGE
							+ ESWT_CONTROL + GUI_FACTORY_CLASS_NAME);
				} else {
					// If no toolkit is registered and if lcdui library exists
					// select it
					try {
						Class.forName(LCDUI_PACKAGE + DISPLAY);

						guiObject = initLCDUI();
					} catch (ClassNotFoundException cnfe) {
						// If there are several toolkits and none is selected
						// IllegalArgumentException must be thrown
						throw new IllegalArgumentException(
								"UI toolkit is not available or found.");
					}
				}
			} catch (ClassNotFoundException cnfe) {
				System.out.println(DEBUG_STR
						+ "Exception caought, going to call initLCDUI funtion");
				// Only lcdui is supported
				guiObject = initLCDUI();
			}
		}
		return guiObject;
	}
	    
	    /**
	     * 
	     * @param aMode class name of the component 
	     * @return
	     */
	//MobileShell iMobileShell=null;
	ProxyControl control=null;
	private Object initDynamicDisplayMode(Object aMode) {
		final String DEBUG_STR="VideoControl::initDynamicDisplayMode()";
		//MMAGUIFactory guiFactory = null;
		try {
//			String className = ((String) aMode).toLowerCase();
			// there is no more any Factory class 
//					+ GUI_FACTORY_CLASS_NAME;
			System.out.println(DEBUG_STR+"Class name is "+aMode);
			// Following line make sure that class name provided in aMode is proper
			Class guiClass = Class.forName((String)aMode);
			iDisplay.syncExec(new Runnable() {
				public void run() {
					control=new ProxyControl();
				}
			});
			
			System.out.println(DEBUG_STR+"Class instantiated is "+control);
			//guiFactory = (MMAGUIFactory) guiClass.newInstance();
		} catch (ClassNotFoundException cnfe) {
			// if the class could not be found
			throw new IllegalArgumentException(
					"Mode not supported or invalid, "
							+ "valid modes are USE_DIRECT_VIDEO and USE_GUI_PRIMITIVE");
//		} catch (IllegalAccessException iae) {
//			// if the class or initializer is not accessible
//			throw new IllegalArgumentException("Mode: " + aMode + " caused "
//					+ iae);
//		} catch (InstantiationException ie) {
//			// if an application tries to instantiate an abstract class or an
//			// interface, or if the instantiation fails for some other reason
//			throw new IllegalArgumentException("Mode: " + aMode + " caused "
//					+[= ie);
		} catch (ClassCastException cce) {
			// Thrown to indicate that the code has attempted to cast an
			// object to a subclass of which it is not an instance.
			throw new IllegalArgumentException("Mode: " + aMode + " caused "
					+ cce);
		}catch(Exception e){
			System.out.println("Exception thrown while creating the control object"+e);
			e.printStackTrace();
		}
//		Object guiObject = guiFactory.initDisplayMode();
//		return guiObject;
		return control;
	}
	    
	/* (non-Javadoc)
	 * @see javax.microedition.media.control.VideoControl#setDisplayFullScreen(boolean)
	 */
	public void setDisplayFullScreen(final boolean aFullScreenMode)
			throws MediaException {
		if (iStatus == NOT_INITIALIZED)
        {
            throw new IllegalStateException(
            "VideoControl.initDisplayMode() not called yet");
        }
		if(aFullScreenMode){
			//Image size should be resized to maximum width, 
			//so that the aspect ratio should be maintained 
			
			((AnimationPlayer)iPlayer).updateImageData(360, 360);
			// when the size is set to full screen, set the location to 0,0
			// but what if user will exit from the full screen mode, 
			//we will have to revert back to the original position
			// Image should be drawn at the center of the screen
			setDisplayLocation(0, 140);
		}
		iDisplay.syncExec(new Runnable(){
			public void run(){
				MobileShell shell= (MobileShell)iControl.getShell();
				shell.setFullScreenMode(aFullScreenMode);
			}
		});
		
	}

	/* (non-Javadoc)
	 * @see javax.microedition.media.control.VideoControl#setDisplayLocation(int, int)
	 */
	public void setDisplayLocation(int aX, int aY) {
		final String DEBUG_STR="VideoControl::setDisplayLocation";
		System.out.println(DEBUG_STR+">>>>>>> Control is "+iControl);
		// in case of customItem, we are getting canvasExtension as control 
		// and in this case we need to ignore the setDisplayLocation call.
		// it is also possible that iControl may be null( it will be null until and unless
		// notifyControlAvailable function is not get called)
		if(iControl==null || (iControl instanceof org.eclipse.swt.internal.extension.CanvasExtension))
			return ;
		((AnimationPlayer)iPlayer).setDisplayLocation(aX, aY);
	}

	/* (non-Javadoc)
	 * @see javax.microedition.media.control.VideoControl#setDisplaySize(int, int)
	 */
	public void setDisplaySize(int aWidth, int aHeight) throws MediaException {
		final String DEBUG_STR="VideoControl::setDisplaySize(int aWidth, int aHeight)";
		System.out.println(DEBUG_STR+"width "+aWidth+" aHeight "+aHeight);
		if (iStatus == NOT_INITIALIZED)
        {
            throw new IllegalStateException(
            "VideoControl.initDisplayMode() not called yet");
        }
        if (aWidth <= 0 || aHeight <= 0)
        {
            throw new IllegalArgumentException(
            "Width and height must be positive");
        }
        ((AnimationPlayer) iPlayer).updateImageData(aWidth,aHeight);
        // Since the DisplaySize is being changed, we need to change the  
        // size of VideoItem as well 
        // if the videoItem is not null, it means currently we are playing animation on form 
        if(iVideoItem!=null)// means it is for customItem 
        	iVideoItem.setPreferredSize(aWidth, aHeight);
        // Notify to all player listener that video Size has been changed 
        ((AnimationPlayer) iPlayer).getiPlayerListenerImpl().postEvent(PlayerListener.SIZE_CHANGED, this);
        System.out.println(DEBUG_STR+"-");
	}

	/* (non-Javadoc)
	 * @see javax.microedition.media.control.VideoControl#setVisible(boolean)
	 */
	public void setVisible(final boolean aVisible) {
		if (iControl != null) {
			iDisplay.syncExec(new Runnable() {
				public void run() {
					iControl.setVisible(aVisible);
					System.out.println("VideoControl::setVisible()"+aVisible);
				}
			});
		}
	}
	/**
	 * 
	 * @return Display object retrieved from ESWT
	 */
	public Display getiDisplay() {
		return iDisplay;
	}
	
	/**
	 * 
	 * @return
	 */
	public Control getControl() {
		return iControl;
	}
	/**
	 * Function of ItemStateChangeListener 
	 * @param ctrl
	 * @param item
	 */
	public void notifyControlAvailable(Control ctrl,Item item){
		final String DEBUG_STR= "VideoControl::notifyControlAvailable(Control ctrl,Item item)";
		System.out.println(DEBUG_STR+"+");
		iControl=ctrl;
		System.out.println(DEBUG_STR+"Control is "+ctrl.hashCode()+ " Item is "+item);
		//TODO is it proper here to put the below line in try/catch?, remove if we can
		// otherwise it may deteriorate the performance, as in case of CustomItem on each 
		// repaint, eSWT control is getting destroyed, and reconstructed
		try {
			((AnimationPlayer)iPlayer).addPaintListener(iControl);
		} catch (MediaException e) {
			// TODO What to do here
			e.printStackTrace();
		}
		System.out.println(DEBUG_STR+"-");
	}
	/**
	 * Function of ItemStateChangeListener
	 * We don't need to do anything in this function
	 * @param item
	 */
	public void notifyControlDisposed(Item item){
		final String DEBUG_STR= "VideoControl::notifyControlDisposed(Item item)";
		System.out.println(DEBUG_STR+"+");
//		System.out.println(DEBUG_STR+"Item Disposed is "+item);
//		System.out.println(DEBUG_STR+"-");
	}

}
