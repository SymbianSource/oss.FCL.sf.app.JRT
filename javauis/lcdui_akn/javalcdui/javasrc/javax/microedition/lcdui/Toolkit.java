/*
* Copyright (c) 1999 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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


package javax.microedition.lcdui;

import java.util.Hashtable;
import java.util.Vector;
import java.util.Enumeration;
import java.io.IOException;
import java.io.InputStream;

import com.nokia.mj.impl.rt.legacy.LegacyRtPort;
import com.nokia.mj.impl.rt.legacy.LegacySupport;
import com.nokia.mj.impl.rt.legacy.MIDEventServer;
import com.nokia.mj.impl.rt.legacy.NativeError;
import com.nokia.mj.impl.rt.legacy.ToolkitObserver;

import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.rt.support.ShutdownListener;
import com.nokia.mj.impl.utils.Logger;

final class Toolkit
{
    static
    {
        com.nokia.mj.impl.rt.support.Jvm.loadSystemLibrary("javalcdui");
        DirectGraphicsInvoker.createInvoker();
        FreeSizeFontInvoker.createInvoker();
        LcduiPropertyInvoker.createInvoker();
        CanvasGraphicsItemPainterInvoker.createInvoker();
    }

    private Finalizer mFinalizer = new Finalizer()
    {
        public void finalizeImpl()
        {
            doFinalize();
        }
    };
    //
    private static final String SERVER_NAME = "javax.microedition.lcdui";
    private static final String DEFAULT_MIDLET_NAME = "MIDlet ";
    private static final int    DEFAULT_MIDLET_UID  = 0x101F9515;

    // NGA specific change.
    // Java side doesn't support native (c++) style macro flags and
    // that is why Toolkit & ToolkitInvoker have been harnessed to substitutes
    // NGA flag & its index
    static final int FLAG_NONE = 0;
    static final int FLAG_NGA_INDEX = 0;
    static final int FLAG_NGA = 1 << FLAG_NGA_INDEX;
    // Defines total flag count. Must be keep up to date.
    private static final int FLAG_COUNT = 1;

    private static final String RESOURCE_OR_PLUGIN_NOT_FOUND =
        "Cannot find lcdui dll, or lcdgr dll or lcdui resource file";

    //
    private static int iServerCount;

    private ToolkitInvoker  iInvoker;
    private Object iDestroySync = new Object();
    private static final int HANDLE_UNINITIALIZED = -0xfffffe;
    private int     iHandle = HANDLE_UNINITIALIZED;
    private Device  iDevice;
    private Display iDisplay;
    private Image   iIcon;
    private MIDEventServer iEventServer;
    private Hashtable iFontCache;
    Buffer  iBuffer;
    private Object iCallbackLock;

    private Vector iObservers;
    private boolean iStartingInBackground = false;
    protected Vector tmpDisplayables;     //temporary Vector of created Displayables
    protected boolean activated = false;  //events are dispatched only if Toolkit is active, see Toolkit.start()

    // NGA specific change
    private int iFlags = FLAG_NONE;
    
    //
    // Event source : must keep in sync with TSourceType in lcdui.h
    //
    private static final int ITEM                           = 3;
    private static final int DISPLAYABLE                    = 4;
    private static final int MIDLET                         = 5;
    private static final int SERIAL                         = 6; // deprecated
    private static final int CANVAS_GRAPHICS_ITEM_PAINTER   = 7;

    //
    // Event type : must keep in sync with TEventType in lcdui.h
    //
    static final int EVENT_PAINT = 10;
    static final int EVENT_KEY_PRESSED      = 11;
    static final int EVENT_KEY_RELEASED     = 12;
    static final int EVENT_KEY_REPEATED     = 13;
    static final int EVENT_POINTER_PRESSED  = 14;
    static final int EVENT_POINTER_RELEASED = 15;
    static final int EVENT_POINTER_DRAGGED  = 16;
    static final int EVENT_SIZE_CHANGED     = 17;

    static final int EVENT_EXIT_REQUESTED = 18;
    static final int EVENT_FOREGROUND = 19;
    static final int EVENT_BACKGROUND = 20;
    static final int EVENT_PAUSE_REQUESTED   = 21;
    static final int EVENT_START_REQUESTED   = 22;
    static final int EVENT_DESTROY_REQUESTED = 23;
    static final int EVENT_COMMAND     = 24;
    static final int EVENT_VISIBLE     = 25;
    static final int EVENT_TRAVERSE    = 26;
    static final int EVENT_SET_CURRENT = 29;
    static final int EVENT_SERIAL      = 30;

    static final int EVET_CANVAS_GRAPHICS_ITEM_REPAINT = 31;

    // IMPLICIT EVENT TYPES
    static final int EVENT_DISMISS = 0;         // ALERT
    static final int EVENT_ITEM_CHANGED = 0;    // ITEM(S)
    static final int EVENT_SELECT  = 0;         // LIST

    //
    // Op codes for syncing to screen
    // These must be kept in sync with MMIDCanvas::TDrawOp
    //
    private static final int SYNC      = 0;
    private static final int SYNC_RECT = 1;

    // NGA specific change.
    // Op code indicating M3G content start
    private static final int M3G_CONTENT_START = 2;
    
    Toolkit(ToolkitInvoker aInvoker)
    {
        iInvoker     = aInvoker;
        tmpDisplayables = new Vector();

        iStartingInBackground = iInvoker.iStartInBackGround;

        ApplicationUtils.getInstance().addShutdownListener(new ShutdownListener()
        {
            public final void shuttingDown()
            {
                dispose();
            }
        });

    }

    /**
     * Initializes the Toolkit, if it is uninitialized.
     */
    private void ensureInitialized()
    {
        if (iEventServer == null)
        {
            initialize();
        }
    }

    /**
     * Initializes the Toolkit.
     */
    synchronized private void initialize()
    {
        if (iEventServer != null)
        {
            return;
        }
        /**
         * get UI server instance number
         */
        final String serverNumber = getServerNumber();

        /**
         * create mangled server name
         */
        final String serverName = SERVER_NAME.concat(serverNumber);

        ApplicationInfo appInfo = ApplicationInfo.getInstance();
        String appName = appInfo.getLocalizedName();
        if (null == appName)
        {
            appName = DEFAULT_MIDLET_NAME.concat(serverNumber);
        }

        int appUid = LegacyRtPort.getMidletUid();
        if (appUid == 0)
        {
            appUid = DEFAULT_MIDLET_UID;
        }

        String appHome = appInfo.getRootPath();
        if (appHome.endsWith("\\"))
        {
            appHome = appHome.substring(0, appHome.length() - 1);
        }

        /**
         * Create native event server + thread. This will run until shutdown
         * in Toolkit.dispose().
         */
        iEventServer  = new UiEventServer(serverName, appUid);
        iCallbackLock = new Object();

        final Hashtable properties = iInvoker.iMidletArgs;
        final String[]  attributes = new String [properties.size() * 2];
        int count = -1;
        for (final Enumeration keys = properties.keys(); keys.hasMoreElements();)
        {
            final String key = (String)keys.nextElement();
            attributes[++count] = key;
            attributes[++count] = (String)properties.get(key);
        }

        final int server = iEventServer.getHandle();

        // NGA specific change.
        // Array that will be filled with native flag macros
        int[] flags = new int[Toolkit.FLAG_COUNT];
        iHandle = _create(server, appName, appUid, appHome, attributes, flags);
        switch (iHandle)
        {
        case NativeError.KErrNotFound:
        case NativeError.KErrPathNotFound:
            throw new RuntimeException(RESOURCE_OR_PLUGIN_NOT_FOUND);
        default:
            checkHandle(iHandle); // leaves if error
            break;
        }
        // NGA specific change.
        // Update flags
        if (flags[Toolkit.FLAG_NGA_INDEX] > 0)
        {
            this.iFlags |= Toolkit.FLAG_NGA;
        }

        iDevice    = new Device(this);
        iBuffer    = new Buffer(this);
        iFontCache = new Hashtable();

    }

    InputStream getResourceAsStream(String aResource)
    throws
                IOException
    {
        return getClass().getResourceAsStream(aResource);
    }

    synchronized void start(Display aDisplay)
    {
        ensureInitialized();
        iDisplay = aDisplay;
        NativeError.check(_activate(iHandle));
        activated = true;
    }

    private static synchronized String getServerNumber()
    {
        return Integer.toString(++iServerCount);
    }

    /**
     * NGA specific change.
     * Check if flags are set.
     * @param flags Flags indexes
     * @return True if flags are on.
     * @since S60 9.2
     */
    boolean checkFlags(int flags)
    {
        return ((iFlags & flags) > 0);
    }

    public boolean isClosed()
    {
        return ((iHandle != HANDLE_UNINITIALIZED) &&
                (iHandle <= 0));
    }

    public int getHandle()
    {
        ensureInitialized();
        if (isClosed())
        {
            throw new RuntimeException("Toolkit closed");
        }
        return iHandle;
    }

    static Toolkit getToolkit()
    {
        ToolkitInvoker invoker = (ToolkitInvoker)ToolkitInvoker.getToolkitInvoker();
        return invoker.getCurrentToolkit();
    }

    final Device getDevice()
    {
        ensureInitialized();
        return iDevice;
    }

    final Display getDisplay()
    {
        ensureInitialized();
        return iDisplay;
    }

    final Object getCallbackLock()
    {
        ensureInitialized();
        return iCallbackLock;
    }

    final Hashtable getFontCache()
    {
        ensureInitialized();
        return iFontCache;
    }

    final synchronized void addObserver(ToolkitObserver aObserver)
    {
        if (aObserver == null)
        {
            return;
        }

        if (iObservers == null)
        {
            iObservers = new Vector(2);
        }

        if (!iObservers.contains(aObserver))
        {
            iObservers.addElement(aObserver);
        }
    }

    final synchronized void removeObserver(ToolkitObserver aObserver)
    {
        if ((iObservers != null) && (aObserver != null))
        {
            iObservers.removeElement(aObserver);
        }
    }

    /**
     * If toolkit is already disposed, then native object has already been deleted
     * aHandle can be null if exception thrown in the object's constructor (where the handle
     * is typically assigned
     */
    final synchronized void disposeObject(int aHandle)
    {
        ensureInitialized();

        if (aHandle == 0)
        {
            Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                       "disposeObject(" + aHandle + ") < ignoring NULL handle");
            return;
        }

        if (iHandle <= 0)
        {
            Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                       "disposeObject(" + aHandle + ") < : Toolkit already closed!");
            return;
        }

        //
        // Lock buffer for duration of dispose.
        //
        synchronized (iBuffer)
        {
            iBuffer.sync();
            _disposeObject(iHandle, aHandle);
        }

    }

    private void doFinalize()
    {
        if (mFinalizer != null)
        {
            registeredFinalize();
            mFinalizer = null;
        }
    }

    void registeredFinalize()
    {
        dispose();
    }

    final synchronized void dispose()
    {
        if ((iHandle == HANDLE_UNINITIALIZED) || (iHandle > 0))
        {
            //
            // Notify observers that toolkit is being destroyed.
            //
            if (null != iObservers)
            {
                final int lastIndex = iObservers.size() -1;
                for (int ii=lastIndex; ii>=0; ii--)
                {
                    final ToolkitObserver observer = (ToolkitObserver)iObservers.elementAt(ii);
                    try
                    {
                        observer.destroyNotify();
                    }
                    catch (Exception ex)
                    {
                        Logger.ELOG(Logger.EJavaUI,
                                    "exception thrown in destroyNotify: ", ex);
                        // close must complete otherwise could get panic, so ignore
                        // any exception thrown by observers.
                    }
                }
            }

            iInvoker.removeToolkit();

            if (null != iDisplay)
            {
                iDisplay.dispose();
                iDisplay = null;
            }

            //
            // Close the graphics buffer if present.
            //
            if (null != iBuffer)
            {
                synchronized (iBuffer)
                {
                    iBuffer.close();
                }
                iBuffer = null;
            }

            if (iHandle > 0)
            {
                _dispose(iHandle);
                iHandle = 0;
            }

            //
            // Shutdown the ui event thread.
            //
            if (iEventServer != null)
            {
                iEventServer.shutdown();
            }
        }
    }

    /* deprecated */
    private void handleAsyncEvent(Object aSource, int aResult)
    {
        ((AsyncCall)aSource).complete(aResult);
    }

    private boolean resetThread()
    {
        if (!isClosed())
        {
            return true;
        }
        return false;
    }

    private void handleItemEvent(Item aSource, int aEvent, int aParam0, int aParam1, int aParam2)
    {
        if (resetThread())
        {
            aSource.handleEvent(aEvent, aParam0, aParam1, aParam2);
        }
    }

    private void handleDisplayableEvent(Displayable aSource, int aEvent, int aParam0, int aParam1, int aParam2)
    {
        synchronized (iDestroySync)
        {
            //S60 Modification ==>
            try
            {
                if (resetThread())
                {
                    aSource.handleEvent(aEvent, aParam0, aParam1);
                }
            }
            catch (Throwable t)
            {
                String className = t.getClass().getName();
                int index = className.lastIndexOf('.');
                if (index != -1)
                {
                    className = className.substring(index + 1, className.length());
                }
                String errStr= "Exception in handleDisplayableEvent. Reason: "+className+
                               "\n  event is : "+aEvent+
                               "\n  source is : "+aSource+"\n  p0 is     : "+aParam0+
                               "\n  p1 is     : "+aParam1+"\n  p2 is     : "+aParam2;
                Logger.ELOG(Logger.EJavaUI,errStr, t);

                //Removing Displayable if is exception thrown
                if (tmpDisplayables.contains(this))
                {
                    tmpDisplayables.removeElement(this);
                }
            }
        }
    }

    private void handleDisplayEvent(Toolkit aToolkit, int aEvent, int aParam0, int aParam1, int aParam2)
    {
        synchronized (iDestroySync)
        {
            if (resetThread())
            {
                switch (aEvent)
                {
                case EVENT_EXIT_REQUESTED:
                    ApplicationUtils.getInstance().notifyExitCmd();
                    break;
                case EVENT_DESTROY_REQUESTED:
                    ApplicationUtils.getInstance().notifyExitCmd();
                    break;
                case EVENT_PAUSE_REQUESTED:
                    ApplicationUtils.getInstance().pauseApplication();
                    break;
                case EVENT_START_REQUESTED:
                    ApplicationUtils.getInstance().resumeApplication();
                    break;
                case EVENT_FOREGROUND:
                    iDisplay.handleForeground(true);
                    break;
                case EVENT_BACKGROUND:
                    iDisplay.handleForeground(false);
                    break;
                case EVENT_SET_CURRENT:
                    iDisplay.switchCurrent();
                    break;
                case EVENT_SERIAL:
                    iDisplay.processSerialEvents();
                    break;
                default:
                    throw new IllegalArgumentException();
                }
            }
        }
    }

    private void handleCanvasGraphicsItemPainterEvent(CanvasGraphicsItemPainter aSource, int aEvent, int aParam0, int aParam1, int aParam2)
    {
        if (resetThread())
        {
            aSource.handleEvent(aEvent, aParam0, aParam1);
        }
    }

    synchronized void setForeground(boolean aForeground)
    {
        if (iStartingInBackground)
        {
            NativeError.check(_setForeground(getHandle(), false));
            iStartingInBackground = false;
        }
        else
        {
            NativeError.check(_setForeground(getHandle(), aForeground));
        }
    }

    /**
     * blocks until all drawing complete
     */
    void sync()
    {
        ensureInitialized();
        synchronized (iBuffer)
        {
            iBuffer.sync();
        }
    }

    /*
     * NGA specific change.
     * Adds m3g content start op code to buffer,
     * if Graphics's target is Canvas.
     * The buffer is synchronized in any case.
     * @param aGraphics Graphics instance
     * @see ToolkitInvoker#toolkitSync(Object, Object)
     * @since S60 9.2
     */
    void sync(Object aObject)
    {
        ensureInitialized();
        Graphics graphics = (Graphics)aObject;
        if (checkFlags(FLAG_NGA) &&
                graphics != null &&
                graphics.iTarget instanceof Canvas)
        {
            Canvas canvas = (Canvas)graphics.iTarget;
            canvas.setM3GContent(true);
            synchronized (iBuffer)
            {
                iBuffer.write(canvas.getContentHandle(), M3G_CONTENT_START);
                iBuffer.sync();
            }
        }
        else
        {
            sync();
        }
    }

    /**
     * Write buffer command which when processed will draw to screen.
     * Used by Canvas and CustomItem.
     */
    void sync(int aDrawable)
    {
        ensureInitialized();
        synchronized (iBuffer)
        {
            iBuffer.write(aDrawable, SYNC);
            iBuffer.sync();
        }
    }

    /**
     * Write buffer command which when processed will draw to screen.
     * Used by Canvas and CustomItem.
     */
    void sync(int aDrawable, int aX, int aY, int aW, int aH)
    {
        ensureInitialized();
        synchronized (iBuffer)
        {
            final int x2=aX+aW;
            final int y2=aY+aH;
            iBuffer.write(aDrawable, SYNC_RECT, aX, aY, x2, y2);
            iBuffer.sync();
        }
    }

    /**
     JSR 135 Support
     */
    final int getEventServerHandle()
    {
        ensureInitialized();
        return iEventServer.getHandle();
    }

    void postEvent(int aEvent)
    {
        NativeError.check(_postEvent(getHandle(), 0, MIDLET, aEvent));
    }

    void postEvent(Item aItem, int aEvent)
    {
        NativeError.check(_postEvent(getHandle(), aItem.getHandle(), ITEM, aEvent));
    }

    void postEvent(Displayable aDisplayable, int aEvent)
    {
        NativeError.check(_postEvent(getHandle(), aDisplayable.getContainerHandle(), DISPLAYABLE, aEvent));
    }

    void postEvent(CanvasGraphicsItemPainter aCanvasGraphicsItem, int aEvent)
    {
        NativeError.check(_postEvent(getHandle(), aCanvasGraphicsItem.getHandle(), CANVAS_GRAPHICS_ITEM_PAINTER, aEvent));
    }

    static int checkHandle(int aHandle)
    {
        if (NativeError.check(aHandle) == 0)
        {
            throw new OutOfMemoryError();
        }
        return aHandle;
    }

    /**
     * NGA specific change.
     * Create native peer on server thread reference by aContext.
     * @param aFlags The array that is filled according to native
     *        side's macro definitions
     */
    native int _create(
        int aContext,String aName, int aUid, String aHomeDir, String[] aAttributes, int[] aFlags);

    /**
     * Open toolkit - enable events.
     */
    native int  _activate(int aToolkit);

    /**
     * Delete toolkit - invalidates handle so no remaining threads must be
     * holding the toolkit handle.
     */
    native void _dispose(int aToolkit);

    /**
     * Deletion native UI object with handle aObject
     */
    native void _disposeObject(int aToolkit,int aObject);

    native int  _postEvent(int aToolkit, int aSource, int aType, int aEvent);
    native int  _setCurrent(int aToolkit,int aDisplayable);
    native int  _setForeground(int aToolkit, boolean aForeground);
}
