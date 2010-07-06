/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: VideoControl
*
*/

package com.nokia.microedition.media.animation;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;

import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.Item;
import javax.microedition.media.MediaException;
import javax.microedition.media.Player;
import javax.microedition.media.PlayerListener;

import org.eclipse.ercp.swt.mobile.MobileShell;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.ProxyControl;
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
import com.nokia.mj.impl.utils.Logger;


public class VideoControl extends ControlImpl implements
        javax.microedition.media.control.VideoControl , ItemControlStateChangeListener
{

    //  Following variable has been taken from VideoControl class
    private static final String GUI_OBJECT_CLASS_NAME =
        "javax.microedition.lcdui.Item";
    // lcdui package used with UiToolkitRegister
    private static String LCDUI_PACKAGE =
        "javax.microedition.lcdui";
    // eswt package used with UiToolkitRegister
    private static String ESWT_PACKAGE = "org.eclipse.swt.widgets";
    // ToolkitRegister class name used with eSWT and LCDUI
    private static String DISPLAY = ".Display";

    // class name used to check if eswt is included
    private static String LISTENER = ".Listener";

    // class name used to check if eswt is included
    private static String ESWT_CONTROL = ".control";

    private static final int NOT_INITIALIZED = -1;
    protected int iStatus = NOT_INITIALIZED;
    private static final int UNDEFINED_RETURN_VALUE=0;
    // For integrating with eSWT API
    private Display iDisplay;
    // This is reference of eSWT Control, don't get confused with the Player Control
    private Control iControl;
    /**
    * When video display is set to full screen mode, old
    * video position & size is stored to this member. When
    * full screen mode is turned off, this member is used to
    * find out if display size has been changed during full
    * screen mode. This is needed to generate a SIZE_CHANGED
    * event.
    */
    private Point iOldDisplaySize;
    /**
     * A CustomItem which needs to be returned in case USE_GUI_PRIMITIVE and null to MIDlet
     * from initDisplayMode function.
     */
    // Global??? yes because we need to remove it from player listener, while finalizer will be called
    private VideoItem iVideoItem;

    ESWTinitializeListener iESWTinitializeListener;

    /**
     * Constructor of VideoControl
     * @param player
     */
    public VideoControl(Player player)
    {
        this.iPlayer=player;
        iESWTinitializeListener=(AnimationPlayer)iPlayer;
    }
    /**
     *
     */
    private Finalizer mFinalizer = new Finalizer()
    {
        public void finalizeImpl()
        {
            doFinalize();
        }
    };
    /**
     *
     */
    final void registeredFinalize()
    {
        if (iVideoItem != null)
        {
            iPlayer.removePlayerListener(iVideoItem);
        }
    }

    /**
     *
     */
    private void doFinalize()
    {
        if (mFinalizer != null)
        {
            registeredFinalize();
            mFinalizer = null;
        }
    }

    /**
     * Return the actual height of the current render video.
     * @return height of the display video
     * @throws java.lang.IllegalStateException - Thrown if initDisplayMode has not been called.
     */
    public int getDisplayHeight()
    {
        // this function can't be called, when the player is in closed state
        // or until initDisplayMode function is not called
        checkState();
        if (iStatus == NOT_INITIALIZED)
        {
            throw new IllegalStateException(
                "VideoControl.initDisplayMode() not called yet");
        }
        // Following function will always return Player's current height
        return ((AnimationPlayer) iPlayer).getImageDimension().x;
    }

    /**
     * Return the actual width of the current render video.
     * @return width of the display video
     * @throws java.lang.IllegalStateException - Thrown if initDisplayMode has not been called.
     */
    public int getDisplayWidth()
    {
        checkState();
        if (iStatus == NOT_INITIALIZED)
        {
            throw new IllegalStateException(
                "VideoControl.initDisplayMode() not called yet");
        }
        return ((AnimationPlayer) iPlayer).getImageDimension().x;
    }

    /**
     * Return the X-coordinate of the video with respect to the GUI object where the video is displayed.
     * The coordinate is specified in pixel values relative to the upper left hand corner of the GUI object.
     * The return value is undefined if initDisplayMode has not been called.
     * @return the X-coordinate of the video.
     */
    public int getDisplayX()
    {
        checkState();
        if (iStatus == NOT_INITIALIZED)
        {
            return UNDEFINED_RETURN_VALUE;
        }
        return ((AnimationPlayer) iPlayer).getiDisplayLocation().x;
    }

    /**
     * Return the Y-coordinate of the video with respective to the GUI object where the video is displayed.
     * The coordinate is specified in pixel values relative to the upper left hand corner of the GUI object.
     * The return value is undefined if initDisplayMode has not been called.
     * @return the Y-coordinate of the video.
     */
    public int getDisplayY()
    {
        checkState();
        if (iStatus == NOT_INITIALIZED)
        {
            return UNDEFINED_RETURN_VALUE;
        }
        return ((AnimationPlayer) iPlayer).getiDisplayLocation().y;
    }

    /**
     * Get a snapshot of the displayed content. Features and format of the captured image are specified by
     * imageType. Supported formats can be queried from System.getProperty with  video.snapshot.encodings
     * as the key. The first format in the supported list is the default capture format.
     * @param imageType - Format and resolution of the returned image.
     *  If null is given, the default capture format is used.
     *
     * @return image as a byte array in required format.
     * @throws java.lang.IllegalStateException - Thrown if initDisplayMode has not been called.
     * @throws MediaException - Thrown if the requested format is not supported or the Player does not support snapshots.
     * @throws java.lang.SecurityException - Thrown if the caller does not have the security permission to take the snapshot.
     */
    //TODO Implementation pending, need discussion with UI team
    public byte[] getSnapshot(String aImageType) throws MediaException
    {
        checkState();
        if (iStatus == NOT_INITIALIZED)
        {
            throw new IllegalStateException(
                "VideoControl.initDisplayMode() not called yet");
        }
        String supportedFormat=System.getProperty("video.snapshot.encodings");
        if (aImageType==null)
        {
            aImageType=supportedFormat.substring(0, supportedFormat.indexOf(' '));
        }
        else if (supportedFormat.indexOf(aImageType)==-1)
        {
            throw new MediaException("Unsupported image type: " + aImageType);
        }
        byte data[]=((AnimationPlayer)iPlayer).getCurrentFrame(aImageType);
        //TODO before returning the check permissions here
        // we are making late check here, so that moment should not be lost.

//        ApplicationUtils appUtils = ApplicationUtils.getInstance();
//        PlayerPermission per = new PlayerPermission("audio/video recording","snapshot");
//        appUtils.checkPermission(per);
        return data;
    }

    /**
     * Return the height of the source video. The height must be a positive number.
     * @return the height of the source video.
     */
    public int getSourceHeight()
    {
        checkState();
        return ((AnimationPlayer)iPlayer).getSourceDimension().y;
    }

    /**
     * Return the width of the source video. The width must be a positive number.
     * @return the width of the source video
     */
    public int getSourceWidth()
    {
        checkState();
        return ((AnimationPlayer)iPlayer).getSourceDimension().x;
    }

    /**
     * Initialize the mode on how the video is displayed.
     * This method must be called before video can be displayed.
     * Two modes are defined:
     * USE_GUI_PRIMITIVE (inherited from GUIControl)
     * USE_DIRECT_VIDEO
     * @see javax.microedition.media.control.VideoControl#initDisplayMode(int, java.lang.Object)
     */
    public Object initDisplayMode(int aMode, Object aArg)
    {
        final String DEBUG_STRING = "VideoControl::initDisplayMode(int, Object)";
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,DEBUG_STRING + "+");
        // To check if state is in not closed state
        checkState();
        if (iStatus != NOT_INITIALIZED)
        {
            // IllegalStateException - Thrown if initDisplayMode is
            // called again after it has previously been called successfully.
            throw new IllegalStateException(
                "initDisplayMode() already called successfully");
        }
        // if the aArg is equal to the name of eSWT Control class name
        // create the Display object in following, assuming Display
        // object is already created in MIDlet.
        if (aArg!=null && aArg.equals(Control.class.getName()))
        {
            // Following line will return the same Display object
            // Which is created in Midlet
            iDisplay =Display.getDefault();
            iESWTinitializeListener.notifyDisplayAvailable(iDisplay);
        }
        else
        {
            // Get the Display object of ESWT
            iDisplay = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
            iESWTinitializeListener.notifyDisplayAvailable(iDisplay);
        }
        if (aMode == USE_GUI_PRIMITIVE)
        {
            Object guiObject = null;
            if (aArg == null)
            {
                guiObject = initNullMode();
            }
            else
            {
                if (aArg.equals(GUI_OBJECT_CLASS_NAME))
                {
                    guiObject = initLCDUI();
                }
                else // try load dynamic display mode
                {
                    guiObject = initDynamicDisplayMode(aArg);
                    // Since it is eswtControl itself so no need of LCDUIInvoker here
                    // assign it in iControl and return immediately from here
                    iControl=(Control)guiObject;
                    iESWTinitializeListener.notifyControlAvailable(iControl);
                    // Now we will change the status, when MIDlet developer will call the setParent
                    iStatus = USE_GUI_PRIMITIVE;
                    return iControl;
                }
            }
            // this will return the control as null, because the item might not
            // have been appended in form
            // Solution to this is as:- Add the listener through LCDUIInvoker
            // and when item will be appended
            // by MIDlet developer, notifyControlAvailable function of
            // ItemStateChangeListener will be called
            iControl = com.nokia.mj.impl.nokialcdui.LCDUIInvoker
                       .getEswtControl(guiObject);
            // add this class as item state change listener
            com.nokia.mj.impl.nokialcdui.LCDUIInvoker
            .setItemControlStateChangeListener(this, (Item)guiObject);
            iStatus = USE_GUI_PRIMITIVE;
            return guiObject;
        }
        else if (aMode == USE_DIRECT_VIDEO)
        {
            if (aArg != null)
            {
                if (!(aArg instanceof javax.microedition.lcdui.Canvas))
                {
                    throw new java.lang.IllegalArgumentException(
                        "For USE_DIRECT_VIDEO mode argument should be of type Canvas");
                }
            }
            else
            {
                throw new java.lang.IllegalArgumentException(
                    "For USE_DIRECT_VIDEO mode argument should not be null");
            }
            iControl = com.nokia.mj.impl.nokialcdui.LCDUIInvoker
                       .getEswtControl(aArg);
            //in this case Control will be provided immediately
            iESWTinitializeListener.notifyControlAvailable(iControl);
            //If USE_DIRECT_VIDEO is set, the video by default is not
            //shown when the canvas is displayed until setVisible(true) is called
            //iIsControlVisible variable of AnimationPlayer class decides, whether
            //animation should be visible or not, and by default it is true;
            //So making it false here.
            ((AnimationPlayer)iPlayer).setiIsControlVisible(false);
            iStatus = USE_DIRECT_VIDEO;
        }
        else
        {
            throw new java.lang.IllegalArgumentException(
                "Mode not supported or invalid, "
                + "valid modes are USE_DIRECT_VIDEO and USE_GUI_PRIMITIVE");
        }
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,DEBUG_STRING + "-");
        return null;
    }
    /**
     * Creates the VideoItem object of size of image(gif)
     * @return VideoItem(a CustomItem ) object
     */
    private Object initLCDUI()
    {
        iVideoItem = new VideoItem(((AnimationPlayer)iPlayer).getImageDimension());
        iPlayer.addPlayerListener(iVideoItem);
        iStatus = USE_GUI_PRIMITIVE;
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
    private Object initNullMode()
    {
        String DEBUG_STR = "VideoControl::initNullMode()";
        // TODO remove this 'toolkit' variable
        String toolkit = null;
        Object guiObject = null;

        // If LCDUI was selected init it even if there might be several
        // toolkits. This is done to support existing applications.
        if ((LCDUI_PACKAGE + DISPLAY).equals(toolkit))
        {
            Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,DEBUG_STR + "going to use initLCDUI function ");
            guiObject = initLCDUI();
        }
        else
        {
            try
            {
                // Several UI toolkits are supported if there are eSWT classes
                // and eSWT direct content component
                // Trying to load eSWT Listener interface or eSWT GUI factory
                // does not cause any initialization to eSWT.
                Class.forName(ESWT_PACKAGE + LISTENER);
                // check if eSWT was selected
                if ((ESWT_PACKAGE + DISPLAY).equals(toolkit))
                {
                    Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,DEBUG_STR
                               + "It is eswtPackage going to call initDynamicDisplay ");
                    guiObject = initDynamicDisplayMode(ESWT_PACKAGE
                                                       + ESWT_CONTROL);

                }
                else
                {
                    // If no toolkit is registered and if LCDUI library exists
                    // select it
                    try
                    {
                        Class.forName(LCDUI_PACKAGE + DISPLAY);
                        guiObject = initLCDUI();
                    }
                    catch (ClassNotFoundException cnfe)
                    {
                        // If there are several toolkits and none is selected
                        // IllegalArgumentException must be thrown
                        throw new IllegalArgumentException(
                            "UI toolkit is not available or found.");
                    }
                }
            }
            catch (ClassNotFoundException cnfe)
            {
                Logger.LOG(Logger.EJavaMMAPI, Logger.EError,DEBUG_STR
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
    // TODO this ProxyControl object should not be used as member variable of VC.
    private ProxyControl control=null;
    private Object initDynamicDisplayMode(Object aMode)
    {
        try
        {
            // Following line make sure that class name provided in aMode is proper
            // TODO is it necessary to load the class name
            Class guiClass = Class.forName((String)aMode);
            iDisplay.syncExec(new Runnable()
            {
                public void run()
                {
                    control=new ProxyControl(((AnimationPlayer)iPlayer).getImageDimension());
                }
            });
        }
        catch (ClassNotFoundException cnfe)
        {
            // if the class could not be found
            throw new IllegalArgumentException(
                "Mode not supported or invalid, "
                + "valid modes are USE_DIRECT_VIDEO and USE_GUI_PRIMITIVE");
//      } catch (IllegalAccessException iae) {
//          // if the class or initializer is not accessible
//          throw new IllegalArgumentException("Mode: " + aMode + " caused "
//                  + iae);
//      } catch (InstantiationException ie) {
//          // if an application tries to instantiate an abstract class or an
//          // interface, or if the instantiation fails for some other reason
//          throw new IllegalArgumentException("Mode: " + aMode + " caused "
//                  +[= ie);
        }
        catch (ClassCastException cce)
        {
            // Thrown to indicate that the code has attempted to cast an
            // object to a subclass of which it is not an instance.
            throw new IllegalArgumentException("Mode: " + aMode + " caused "
                                               + cce);
        }
        // TODO remove this catch(Exception) block, it is added for testing purpose
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI, Logger.EError,"Exception thrown while creating the control object"+e);
            e.printStackTrace();
        }
        return control;
    }

    /* (non-Javadoc)
     * @see javax.microedition.media.control.VideoControl#setDisplayFullScreen(boolean)
     */
    public void setDisplayFullScreen(final boolean aFullScreenMode)
    throws MediaException
    {
        if (iStatus == NOT_INITIALIZED)
        {
            throw new IllegalStateException(
                "VideoControl.initDisplayMode() not called yet");
        }
        //if this is in case of form, return silently
        // No need to display the CustomItem in full screen mode
        //This is as per earlier NOKIA implementation,
        if (iVideoItem!=null)
        {
            return;
        }
        if (aFullScreenMode)
        {
            // Before going to full screen mode, we need to store the current display size;
            // so that when user will exit from the full screen,
            // it will return to it's previous size
            iOldDisplaySize = ((AnimationPlayer)iPlayer).getImageDimension();
            Rectangle displayDimension=calculateFullScreenDimension();
            ((AnimationPlayer)iPlayer).updateImageData(displayDimension.width, displayDimension.height);
            setDisplayLocation(displayDimension.x, displayDimension.y);
            ((AnimationPlayer)iPlayer).getiPlayerListenerImpl().postEvent(PlayerListener.SIZE_CHANGED, this);
        }
        else
        {
            // user may call setFullScreen(false), prior to setting it full screen
            // so making it sure here that iOldDisplaySize is not null
            if (iOldDisplaySize!=null)
            {
                ((AnimationPlayer)iPlayer).updateImageData(iOldDisplaySize);
                // Do we need to make it null?
                iOldDisplaySize=null;
                // post event to player Listener
                ((AnimationPlayer)iPlayer).getiPlayerListenerImpl().postEvent(PlayerListener.SIZE_CHANGED, this);
            }
        }
    }

    /* (non-Javadoc)
     * @see javax.microedition.media.control.VideoControl#setDisplayLocation(int, int)
     */
    public void setDisplayLocation(int aX, int aY)
    {
        if (iStatus == USE_GUI_PRIMITIVE)
        {
            // In USE_GUI_PRIMITIVE mode, this call will be ignored.
            return;
        }
        if (iStatus != USE_DIRECT_VIDEO)
        {
            // This method only works when the USE_DIRECT_VIDEO mode is set.
            throw new IllegalStateException();
        }
        // Need to ignore this call in case of USE_GUI_PRIMITIVE
        // in case of customItem, we are getting canvasExtension as control
        // and in this case we need to ignore the setDisplayLocation call.
        // it is also possible that iControl may be null( it will be null until and unless
        // notifyControlAvailable function is not get called)
        if (iControl==null)// || (iControl instanceof org.eclipse.swt.internal.extension.CanvasExtension))
            return ;
        ((AnimationPlayer)iPlayer).setDisplayLocation(aX, aY);
    }

    /**
     * (non-Javadoc)
     * @see javax.microedition.media.control.VideoControl#setDisplaySize(int, int)
     */
    // TODO we need to handle here the following scenario
    // if the player is in FULL_SCREEN mode, just change the display size in background(means do not exit from
    // the full screen mode and don't send event to player Listener the player size changed )
    // and changed side should take effect
    public void setDisplaySize(int aWidth, int aHeight) throws MediaException
    {
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
        // resize the all frames
        ((AnimationPlayer) iPlayer).updateImageData(aWidth, aHeight);
        // Since the DisplaySize is being changed, we need to change the
        // size of VideoItem as well
        // if the videoItem is not null, it means currently we are playing
        // animation on form
        if (iVideoItem != null)// means it is for customItem
            iVideoItem.setPreferredSize(aWidth, aHeight);
        // Notify to all player listener that video Size has been changed
        ((AnimationPlayer) iPlayer).getiPlayerListenerImpl().postEvent(
            PlayerListener.SIZE_CHANGED, this);
    }

    /**
     * Show or hide the video.
     * If USE_GUI_PRIMITIVE is set, the video by default is shown when the GUI primitive is displayed.
     * If USE_DIRECT_VIDEO is set, the video by default is not shown when the canvas is displayed until
     * setVisible(true) is called. If the canvas is removed from the screen, the video will not be displayed.
     *
     * @return visible - Show the video if true, hide it otherwise.
     * @throws java.lang.IllegalStateException - Thrown if initDisplayMode has not been called.
     */
    public void setVisible(final boolean aVisible)
    {
        if (iStatus == NOT_INITIALIZED)
        {
            throw new IllegalStateException(
                "VideoControl.initDisplayMode() not called yet");
        }
        if (iControl != null)
        {
            ((AnimationPlayer)iPlayer).setiIsControlVisible(aVisible);
            // even if the player is stopped, need to give the effect of
            // setVisible
            iDisplay.syncExec(new Runnable()
            {
                public void run()
                {
                    iControl.redraw();//setVisible(aVisible);
                }
            });
        }
    }
    /*******************************FOLLOWING FUNCTIONS ARE USED FOR INTERNAL PURPOSE***********************/
    /**
     *
     * @return Display object retrieved from ESWT
    //     */
//    Display getiDisplay()
//    {
//        return iDisplay;
//    }

    /**
     *
     * @return
     */
//    Control getControl()
//    {
//        return iControl;
//    }
    /**
     * Function of ItemStateChangeListener
     * Notified by LCDUI implementation
     * @param ctrl
     * @param item
     */
    public void notifyControlAvailable(Control ctrl,Item item)
    {
        final String DEBUG_STR= "VideoControl::notifyControlAvailable(Control ctrl,Item item)";
        iControl=ctrl;
        iESWTinitializeListener.notifyControlAvailable(iControl);
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo, DEBUG_STR+"Control is "+ctrl.hashCode()+ " Item is "+item);
    }
    /**
     * Function of ItemStateChangeListener
     * We don't need to do anything in this function
     *
     * @param item
     */
    public void notifyControlDisposed(Item item)
    {
        final String DEBUG_STR= "VideoControl::notifyControlDisposed(Item item)";
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo, DEBUG_STR+"Item Disposed is "+item);
    }

    /**
     * This function calculate the size and location of the image in case of full screen
     * So that Aspect ratio should be maintained
     * @return Rectangle, After calculating the size of the Video(image) in full screen mode
     */
    // It is not the proper way to declare the variable as global variable
    // only for the reason that it should be accessible in inner class.
    // Following variable(deviceDimension) declared shouldn't be the part of VideoControl object.
    // My suggestion:- Move it to some utility class
    // TODO let's discuss
    private Rectangle deviceDimension;
    private Rectangle calculateFullScreenDimension()
    {
        Point actualImageSize = ((AnimationPlayer)iPlayer).getImageDimension();
        // initialize the deviceDimension, in local variable
        iDisplay.syncExec(new Runnable()
        {
            public void run()
            {
                //deviceDimension=iDisplay.getBounds();
                deviceDimension=iDisplay.getClientArea();
            }
        });
        int deviceWidth=deviceDimension.width;
        int deviceHeight=deviceDimension.height;
        //Returning the following rectangle after alteration
        Rectangle rect= new Rectangle(0,0,deviceWidth,deviceHeight);

        // to handle the divide by zero
        if (actualImageSize.x==0 && actualImageSize.y==0)
            return rect;
        // Following logic has been copied from
        //earlier native implementation ( from cmmadisplay.cpp::ScaleToFullScreen)
        // Smaller dimension scale ratio will be scaled.
        // Changed to area calculation to avoid reals and dimension
        // with smaller area will be scaled.
        // remember that the argument received in this function is a point
        // and it's x and y coordinate represent the width and height of the image respectively
        int vDiff=(deviceWidth - actualImageSize.x)* actualImageSize.y;
        int hDiff= (deviceHeight - actualImageSize.y) * actualImageSize.x;
        // Check which side to scale to full screen size.
        // Width or Height will be full size.
        if (hDiff > vDiff)
        {
            rect.height = (deviceWidth * actualImageSize.y)/actualImageSize.x;
            rect.y= (deviceHeight -  rect.height)>>1;// divide by 2
        }
        else
        {
            rect.width = (deviceHeight * actualImageSize.x)/actualImageSize.y;
            rect.x= (deviceWidth - rect.width)>>1;
        }
        return rect;
    }

}
