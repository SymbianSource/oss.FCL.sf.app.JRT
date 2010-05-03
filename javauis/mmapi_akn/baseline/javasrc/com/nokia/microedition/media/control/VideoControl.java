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
* Description:  VideoControl is used to control the display of video.
*
*/

package com.nokia.microedition.media.control;

import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
import javax.microedition.media.MediaException;
import javax.microedition.media.Player;
import javax.microedition.midlet.MIDlet;
import com.nokia.mj.impl.rt.legacy.NativeError;
import com.nokia.mj.impl.rt.legacy.LegacyRtPort;
import com.nokia.mj.impl.rt.legacy.Security;
import com.nokia.mj.impl.rt.legacy.ToolkitInvoker;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.utils.Logger;

public class VideoControl
        extends ControlImpl
        implements javax.microedition.media.control.VideoControl
{
    private static final String GUI_OBJECT_CLASS_NAME =
        "javax.microedition.lcdui.Item";
    private static final int NOT_INITIALIZED = -1;
    private static final int ERROR_ARGUMENT = -6; // KErrArgument

    // use contans to avoid making native methods
    // which has same prototypes

    // getter constants for native side
    private static final int PROPERTY_DISPLAY_WIDTH = 1;
    private static final int PROPERTY_DISPLAY_HEIGHT = 2;
    private static final int PROPERTY_DISPLAY_X = 3;
    private static final int PROPERTY_DISPLAY_Y = 4;
    private static final int PROPERTY_SOURCE_WIDTH = 5;
    private static final int PROPERTY_SOURCE_HEIGHT = 6;

    // setter constants for native side
    private static final int SET_DISPLAY_SIZE = 20;
    private static final int SET_DISPLAY_LOCATION = 21;
    private static final int SET_DISPLAY_VISIBLE_TRUE = 22;
    private static final int SET_DISPLAY_VISIBLE_FALSE = 23;
    private static final int SET_DISPLAY_FULLSCREEN_TRUE = 24;
    private static final int SET_DISPLAY_FULLSCREEN_FALSE = 25;

    // MMA API defines return values like 'return value is undefined'
    // then UNDEFINED_RETURN_VALUE is returned
    private static final int UNDEFINED_RETURN_VALUE = 0;

    // String to be passed to Security.ensurePermission method
    // when getting snapshot.
    private static final String PERMISSION =
        "javax.microedition.media.control.VideoControl.getSnapshot";
    private static final String PERM_ARGS[] = { "" };

    // class name used with dynamic display mode initialization
    private static String GUI_FACTORY_CLASS_NAME = ".Factory";

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

    protected int iStatus = NOT_INITIALIZED;

    // Needed for getting native handle from lcdui components
    ToolkitInvoker iToolkitInvoker;

    private VideoItem iVideoItem;
    private Canvas iVideoCanvas;
    private Display iDisplay = null;

    // error code used asynchronous native calls
    private int iError;

    // used in asynchronous getSnapshot method
    private byte[] iImageData;
    private int iToolkitHandle = 0;

    private Finalizer mFinalizer = new Finalizer()
    {
        public void finalizeImpl()
        {
            doFinalize();
        }
    };

    public VideoControl()
    {
        iToolkitInvoker = ToolkitInvoker.getToolkitInvoker();
    }

    private void doFinalize()
    {
        if (mFinalizer != null)
        {
            registeredFinalize();
            mFinalizer = null;
        }
    }

    final void registeredFinalize()
    {
        if (iVideoItem != null)
        {
            iPlayer.removePlayerListener(iVideoItem);
            iVideoItem.iNativeHandle = 0;
        }
    }

    public void setHandles(Player aPlayer, int aEventSource, int aControlHandle)
    {
        iPlayer = aPlayer;
        iEventSource = aEventSource;
        iControlHandle = aControlHandle;
        if (!_isESWT())
        {
            // LCDUI midlet running
            // for mobitv fix
            try
            {
                iToolkitHandle = iToolkitInvoker.toolkitGetHandle(iToolkitInvoker.getToolkit());
            }
            catch (Exception e)
            {
                //iToolKitHandle = 0;
            }
        }
        else
        {
            // eSWT midlet running
            // Foreground status of the eSWT midlet is always set to 1 so midlet recieves volume key events even in background.
            // This is a minor problem to be fixed in future
            iToolkitHandle = 0;
        }
        int error = _construct(iControlHandle,
                               aEventSource,
                               iToolkitHandle);

        NativeError.check(error);
    }

    /**
     * from Interface VideoControl
     *
     */
    public void setDisplaySize(int aWidth, int aHeight) throws MediaException
    {
        checkState();
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
        int ret = setDisplayProperty(aWidth, aHeight, SET_DISPLAY_SIZE);
        if (ret < 0)
        {
            throw new MediaException("setDisplaySize() failed, SymbianOS error: "
                                     + ret);
        }
        if (iStatus == USE_GUI_PRIMITIVE &&
                iVideoItem != null)  // with dynamic display mode item can be null
        {
            iVideoItem.privateInvalidate();
        }
    }

    /**
     * from Interface VideoControl
     *
     */
    public void setDisplayFullScreen(boolean aFullScreenMode) throws MediaException
    {
        checkState();
        if (iStatus == NOT_INITIALIZED)
        {
            throw new IllegalStateException(
                "VideoControl.initDisplayMode() not called yet");
        }
        int ret;
        if (aFullScreenMode)
        {
            ret = setDisplayProperty(SET_DISPLAY_FULLSCREEN_TRUE);
        }
        else
        {
            ret = setDisplayProperty(SET_DISPLAY_FULLSCREEN_FALSE);
        }

        if (ret < 0)
        {
            throw new MediaException();
        }
    }

    /**
     * from Interface VideoControl
     *
     */
    public void setDisplayLocation(int aX, int aY)
    {
        checkState();
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

        // cannot fail -> ignore return value
        setDisplayProperty(aX, aY, SET_DISPLAY_LOCATION);
    }

    /**
     * from Interface VideoControl
     *
     */
    public void setVisible(boolean aVisible)
    {
        checkState();
        if (iStatus == NOT_INITIALIZED)
        {
            throw new IllegalStateException(
                "VideoControl.initDisplayMode() not called yet");
        }
        if (aVisible)
        {
            // Update the foreground or background state of the midlet before
            // setting the visibility of the control
            updateForeground();
            // cannot fail -> ignore return value
            setDisplayProperty(SET_DISPLAY_VISIBLE_TRUE);
        }
        else
        {
            // cannot fail -> ignore return value
            setDisplayProperty(SET_DISPLAY_VISIBLE_FALSE);
        }
    }

    /**
     * from Interface VideoControl
     *
     */
    public int getDisplayWidth()
    {
        checkState();
        if (iStatus == NOT_INITIALIZED)
        {
            throw new IllegalStateException(
                "VideoControl.initDisplayMode() not called yet");
        }
        int width = getControlProperty(PROPERTY_DISPLAY_WIDTH);
        return width;
    }

    /**
     * from Interface VideoControl
     *
     */
    public int getDisplayHeight()
    {
        checkState();
        if (iStatus == NOT_INITIALIZED)
        {
            throw new IllegalStateException(
                "VideoControl.initDisplayMode() not called yet");
        }
        int height = getControlProperty(PROPERTY_DISPLAY_HEIGHT);
        return height;
    }

    /**
     * from Interface VideoControl
     *
     */
    public int getDisplayX()
    {
        checkState();
        if (iStatus == NOT_INITIALIZED)
        {
            return UNDEFINED_RETURN_VALUE;
        }
        int x = getControlProperty(PROPERTY_DISPLAY_X);
        return x;
    }

    /**
     * from Interface VideoControl
     *
     */
    public int getDisplayY()
    {
        checkState();
        if (iStatus == NOT_INITIALIZED)
        {
            return UNDEFINED_RETURN_VALUE;
        }
        int y = getControlProperty(PROPERTY_DISPLAY_Y);
        return y;
    }

    /**
     * from Interface VideoControl
     *
     */
    public int getSourceWidth()
    {
        checkState();
        int width = getControlProperty(PROPERTY_SOURCE_WIDTH);
        if (width <= 0)
        {
            width = 1;
        }
        return width;
    }

    /**
     * from Interface VideoControl
     *
     */
    public int getSourceHeight()
    {
        checkState();
        int height = getControlProperty(PROPERTY_SOURCE_HEIGHT);
        if (height <= 0)
        {
            height = 1;
        }
        return height;
    }

    /**
     * from Interface VideoControl
     *
     */
    synchronized public byte[] getSnapshot(String aImageType) throws MediaException
    {
        checkState();
        if (iStatus == NOT_INITIALIZED)
        {
            throw new IllegalStateException(
                "VideoControl.initDisplayMode() not called yet");
        }

        // starts native side. Native will release wait lock if
        // _getSnapshot doesn't leave.
        int error = _getSnapshot(iControlHandle,
                                 iEventSource, aImageType);
        if (error == ERROR_ARGUMENT)
        {
            throw new MediaException("Unsupported image type: " + aImageType);
        }
        else if (error < 0)
        {
            throw new MediaException("getSnapshot() failed, Symbian OS error: " + error);
        }

        synchronized (this)
        {
            try
            {
                // wait native side
                wait();
            }
            catch (Exception  e)
            {
                throw new MediaException("" + e);
            }
        }
        if (iError < 0)
        {
            throw new MediaException("Symbian OS error: " + iError);
        }

        // Check the permission here, so 'the moment' is not lost?
        Security.ensurePermission(PERMISSION, PERMISSION, PERM_ARGS);

        return iImageData;
    }

    /**
     * from Interface VideoControl
     *
     */
    public Object initDisplayMode(int aMode, Object aArg)
    {
        checkState();
        if (iStatus != NOT_INITIALIZED)
        {
            // IllegalStateException - Thrown if initDisplayMode is
            // called again after it has previously been called successfully.
            throw new IllegalStateException(
                "initDisplayMode() already called successfully");
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
                }
            }

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
            iVideoCanvas = (Canvas)aArg;

            initNativeDisplay(iVideoCanvas,
                              iToolkitInvoker.getDisplayableHandle(
                                  (javax.microedition.lcdui.Displayable)aArg));

            iStatus = USE_DIRECT_VIDEO;

            try
            {
                this.setVisible(false);
            }

            catch (IllegalStateException ex) { }

            return null;
        }
        else
        {
            // java.lang.IllegalArgumentException - Thrown if the mode is invalid.
            throw new java.lang.IllegalArgumentException(
                "Mode not supported or invalid, " +
                "valid modes are USE_DIRECT_VIDEO and USE_GUI_PRIMITIVE");
        }
    }

    /**
     * Initializes native display.
     *
     */
    private int initNativeDisplay(Object aGuiObject, int aComponentNativeHandle)
    {
        int handle = _initDisplayMode(iControlHandle,
                                      iEventSource,
                                      aComponentNativeHandle,
                                      aGuiObject);
        NativeError.check(handle);
        return handle;
    }

    /**
     * Gets control property from native side.
     * @param aPropertyType One of the types defined in this class
     *
     */
    private int getControlProperty(int aPropertyType)
    {
        return _getControlProperty(iControlHandle,
                                   iEventSource,
                                   aPropertyType);
    }

    /**
     * Sets two properties to native side.
     *
     * @param aParamA Specific parameter A for property type.
     * @param aParamB Specific parameter B for property type.
     * @param aPropertyType One of the types defined in this class
     */
    private int setDisplayProperty(int aParamA, int aParamB, int aPropertyType)
    {
        return _setDisplayProperty(iControlHandle,
                                   iEventSource,
                                   aParamA,
                                   aParamB,
                                   aPropertyType);
    }

    /**
     * Sets one property to native side.
     *
     * @param aPropertyType One of the types defined in this class
     */
    private int setDisplayProperty(int aPropertyType)
    {
        return _setDisplayProperty(iControlHandle,
                                   iEventSource,
                                   0,
                                   0,
                                   aPropertyType);
    }

    /**
     * Called from native side when snapshot is ready
     *
     */
    private void snapshotReady(int aError, byte[] aImageData)
    {
        iImageData = aImageData;
        iError = aError;

        synchronized (this)
        {
            notify();
        }
    }

    /**
     * This method will try to use dynamic class loading to instantiate GUI
     * object for given mode.
     * @param aMode parameter for display mode
     * @return GUI object
     */
    private Object initDynamicDisplayMode(Object aMode)
    {
        MMAGUIFactory guiFactory = null;
        try
        {
            String className = ((String)aMode).toLowerCase() +
                               GUI_FACTORY_CLASS_NAME;
            Class guiClass = Class.forName(className);
            guiFactory = (MMAGUIFactory)guiClass.newInstance();
        }
        catch (ClassNotFoundException cnfe)
        {
            // if the class could not be found
            throw new IllegalArgumentException(
                "Mode not supported or invalid, " +
                "valid modes are USE_DIRECT_VIDEO and USE_GUI_PRIMITIVE");
        }
        catch (IllegalAccessException iae)
        {
            // if the class or initializer is not accessible
            throw new IllegalArgumentException("Mode: " + aMode +
                                               " caused " + iae);
        }
        catch (InstantiationException ie)
        {
            // if an application tries to instantiate an abstract class or an
            // interface, or if the instantiation fails for some other reason
            throw new IllegalArgumentException("Mode: " + aMode +
                                               " caused " + ie);
        }
        catch (ClassCastException cce)
        {
            // Thrown to indicate that the code has attempted to cast an
            // object to a subclass of which it is not an instance.
            throw new IllegalArgumentException("Mode: " + aMode +
                                               " caused " + cce);
        }

        Object guiObject = guiFactory.initDisplayMode();
        // initNativeDisplay return handle MMMADirectContainer, 0 parameter
        // indicates that dynamic display will be used
        guiFactory.setContentHandle(initNativeDisplay(guiObject, 0));
        return guiObject;
    }

    /**
     * Initializes LCDUI display mode.
     * @return LCDUI Item
     */
    private Object initLCDUI()
    {
        iVideoItem = new VideoItem(iEventSource);
        iPlayer.addPlayerListener(iVideoItem);
        int handle = initNativeDisplay(iVideoItem,
                                       iToolkitInvoker.itemGetHandle(iVideoItem));

        iVideoItem.setNativeHandle(handle);
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
        // selected toolkit or null if not selected
        String toolkit = LegacyRtPort.getRegisteredToolkit();

        Object guiObject = null;

        // If lcdui was selected init it even if there might be several
        // toolkits. This is done to support existing applications.
        if ((LCDUI_PACKAGE + DISPLAY).equals(toolkit))
        {
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
                Class.forName(ESWT_PACKAGE +
                              ESWT_CONTROL +
                              GUI_FACTORY_CLASS_NAME);

                // check if eSWT was selected
                if ((ESWT_PACKAGE + DISPLAY).equals(toolkit))

                {
                    guiObject = initDynamicDisplayMode(
                                    ESWT_PACKAGE +
                                    ESWT_CONTROL +
                                    GUI_FACTORY_CLASS_NAME);
                }
                else
                {
                    // If no toolkit is registered and if lcdui library exists select it
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
                // Only lcdui is supported
                guiObject = initLCDUI();
            }
        }
        return guiObject;
    }
    private int updateForeground()
    {

        int visible = 1;

        // Check whether display is initialized
        checkState();
        if (iStatus == NOT_INITIALIZED)
        {
            return visible;
        }


        MIDlet midlet = LegacyRtPort.getMidlet();
        //Displayable displayable;
        if (midlet == null)
        {
            return visible;
        }
        else
        {
            Displayable displayable = Display.getDisplay(midlet).getCurrent();

            if (displayable != null && displayable.isShown())
            {
                Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,
                           "VideoControl.updateForeground isShown() = 1");
                // visible
            }
            else
            {
                Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,
                           "VideoControl.updateForeground isShown() = 0");
                // not visible
                visible = 0;
            }

            // Set the foreground state
            _setForeground(iControlHandle,
                           iEventSource,
                           visible);
        }

        return visible;
    }

    private native int _construct(int aControlHandle,
                                  int aEventSourceHandle,
                                  int aToolkitHandle);

    private native int _getControlProperty(int aControlHandle,
                                           int aEventSourceHandle,
                                           int aPropertyType);

    private native int _setDisplayProperty(int aControlHandle,
                                           int aEventSourceHandle,
                                           int aParamA,
                                           int aParamB,
                                           int aPropertyType);

    private native int _getSnapshot(int aControlHandle,
                                    int aEventSourceHandle,
                                    String aProperties);

    private native int _initDisplayMode(int aControlHandle,
                                        int aEventSourceHandle,
                                        int aToolkitHandle,
                                        Object aJavaDisplayObject);
    private native int _setForeground(int aControlHandle,
                                      int aEventSourceHandle,
                                      int aIsForeground);

    private native boolean _isESWT();
}
