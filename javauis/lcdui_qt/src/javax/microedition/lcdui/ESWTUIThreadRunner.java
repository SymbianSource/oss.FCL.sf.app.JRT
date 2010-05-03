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
package javax.microedition.lcdui;

import java.util.Enumeration;
import java.util.Hashtable;

import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.ercp.swt.midp.UIThreadSupport;
import org.eclipse.swt.widgets.*;
import org.eclipse.swt.widgets.Display;

import com.nokia.mj.impl.rt.support.*;

/**
 * Singleton class which is responsible to run the eSWT UI thread. This class is
 * also access-point to eSWT's Display-class.<br>
 * <br>
 * UI thread is started in static initialization block. If another UI Thread is
 * created before that the run()-method will fail and this class won't try to
 * run UI thread anymore. Instead, existing UI-thread will be used to run LCDUI.
 * <br>
 * <br>
 * Note that the method getDisplay() will still work normally.<br>
 * <br>
 * Also note that if the other thread which is running eSWT stops executing then
 * this class stops working too.
 */
final class ESWTUIThreadRunner implements Listener, ShutdownListener, Runnable {

    private static final int NONE = 1;
    private static final int CREATING = 2;
    private static final int RUNNING = 4;
    private static final int EXITING = 8;
    private static final int ALL = NONE | CREATING | RUNNING | EXITING;

    private static ESWTUIThreadRunner instance;
    private static int lastKeyScancode;
    private static int lastKeyModifier;
    private static int keyRepeatCount;

    static DisposeStorage ds;
    private static Hashtable finalizers = new Hashtable();

    private Object lock = new Object();

    private Display display;
    private int state = NONE;

    static {
        // Create LCDUIInvoker for MMAPI to access underlying eSWT widgets
        LCDUIInvokerImpl.createInvoker();

        // Create dispose storage to clean up newly created Fonts and Images.
        ds = new DisposeStorage();

        // Starting thread here makes sure that eSWT services
        // are always available when they are needed.

        // uiThread = new Thread(getInstance(), UI_THREAD_NAME);
        // uiThread.start();
    }

    /**
     * Private Constructor. Singleton.
     */
    private ESWTUIThreadRunner() {
        Logger.info("Starting up");

        // TODO: check if the startUI throws RuntimeException on already
        // existing UI thread

        // launch UI thread - TODO: also check return value/catch possible exceptions - what to do ?
        UIThreadSupport.startInUIThread(this);

        // subscribe to shutdown events
        ApplicationUtils.getInstance().addShutdownListener(this);
    }

    /**
     * Gets singleton instance of this class.
     *
     * @return Instance of this class.
     */
    public static ESWTUIThreadRunner getInstance() {
        synchronized (ESWTUIThreadRunner.class) {
        	if (instance == null) {
        		instance = new ESWTUIThreadRunner();
        	}
        }
        return instance;
    }

    public static boolean isDisposed() {
        return getInstance().getDisplay().isDisposed();
    }

    /**
     * Synchronized execution of runnable.
     *
     * @param runnable a runnable
     */
    public static void syncExec(Runnable runnable) {
        getInstance().getDisplay().syncExec(runnable);
    }

    /**
     * Synchronized execution of runnable with a harness.<br>
     * In case the runnable throws a RuntimeException, the harness extracts it
     * and throws that instead of the SWTException.
     *
     * @param runnable a Runnable
     */
    public static void safeSyncExec(Runnable runnable) {
        try {
            getInstance().getDisplay().syncExec(runnable);
        }
        catch (SWTException swte) {
            if (swte.getCause() != null && swte.getCause() instanceof RuntimeException) {
                // if the cause is RuntimeException, throw it
                throw (RuntimeException) swte.getCause();
            }
            else {
                // throw forward as a RuntimeException
                throw new RuntimeException(swte.getMessage());
            }
        }
    }

    /**
     * Return last key's scancode.
     */
    static int getLastKeyScancode() {
        return lastKeyScancode;
    }

    /**
     * Return last key's modifier.
     */
    static int getLastKeyModifier() {
        return lastKeyModifier;
    }

    static int getKeyRepeatCount() {
    	return keyRepeatCount;
    }

    /**
     * Gets eSWT Display.
     *
     * @return eSWT Display.
     */
    public Display getDisplay() {
        // wait until display is created or re-created
        while (isState(NONE | CREATING)) {
            doWait();
        }
        return display;
    }

    private static void doWait() {
        try {
            Thread.sleep(1);
        }
        catch (InterruptedException ex) {
            // ignore
        }
    }

    private void onStartup() {
        if (isState(CREATING)) {
            display = Display.getDefault();
            display.addListener(SWT.Close, this);
            display.addFilter(SWT.KeyDown, this);
            display.addFilter(SWT.KeyUp, this);
            display.addFilter(SWT.Traverse, this);
            if (display != null) {
                // We called Display.getDefault() first so we created the
                // Display which means that we are running the UI-thread
                // of this Display
                changeState(RUNNING);
            }
            else {
                // We don't own the Display - we cannot run the UI-thread
                changeState(EXITING);
            }
        }
    }

    private void onShutdown() {
        if (isState(EXITING)) {
            display.removeListener(SWT.Close, this);
            display.removeFilter(SWT.KeyDown, this);
            display.removeFilter(SWT.KeyUp, this);
            display.removeFilter(SWT.Traverse, this);
            ds.dispose();
            Font.disposeFonts();
            listClassStat();
            try {
                // NullPointerException is simply caught and ignored
                display.dispose();
            }
            catch (Exception ex) {
                Logger.exception("Display dispose", ex);
            }
        }
    }

    /* (non-Javadoc)
     * @see org.eclipse.swt.widgets.Listener#handleEvent(org.eclipse.swt.widgets.Event)
     */
    public void handleEvent(Event e) {
        switch (e.type) {
            case SWT.Close: {
                Logger.info("Close event");
                // Check if the No-Exit attribute is set
                 if (JadAttributeUtil.isValue(JadAttributeUtil.ATTRIB_NOKIA_MIDLET_NO_EXIT, JadAttributeUtil.VALUE_TRUE)) {
                    // don't exit
                    e.doit = false;
                }
                break;
            }
            case SWT.KeyDown:
            	if (e.keyCode == lastKeyScancode || lastKeyScancode == 0) {
            		keyRepeatCount++;
            	} else {
            		keyRepeatCount = 0;
            	}
            	handleKey(e);
            	break;
            case SWT.KeyUp: {
            	keyRepeatCount = 0;
            	handleKey(e);
            	// After KeyUp event has been handled, clear stored values
            	lastKeyScancode = 0;
            	lastKeyModifier = 0;
                break;
            }
            case SWT.Traverse: {
                javax.microedition.lcdui.Display.getDisplay().eswtHandleEvent(e);
            }
            default:
                break;
        }
    }

    /* (non-Javadoc)
     * @see com.nokia.mj.impl.rt.support.ShutdownListener#shuttingDown()
     */
    public void shuttingDown() {
        Logger.info("Shutting Down");
        EventDispatcher.instance().terminate(new Runnable() {
        	public void run() {
                changeState(EXITING);
        	}
        });
    }

    /**
     * Creates new eSWT Display and runs eSWT UI-thread.
     */
    public void run() {
        changeState(CREATING);
        onStartup();
        while (isState(ALL - EXITING)) {
            try {
                // Execute the eSWT event loop.
                while (isState(RUNNING)) {
                    if (!display.readAndDispatch()) {
                        display.sleep();
                    }
                }
            }
            catch (Exception ex) {
                if (ex instanceof SWTException) {
                    Throwable t = ((SWTException) ex).getCause();
                    if (t != null && t instanceof RuntimeException) {
                        // this might be an expected exception of safeSyncExec
                        Logger.warning("eSWT Thread Exception: " + ex);
                        // t.printStackTrace();
                    }
                }
                else {
                    Logger.error("eSWT Thread Exception: " + ex);
                    ex.printStackTrace();
                }
            }
            catch (Error er) {
                Logger.error("eSWT Thread Error" + er);
                er.printStackTrace();
            }
        }
        onShutdown();
        changeState(NONE);
    }

    private boolean isState(int mask) {
        synchronized (lock) {
            return ((state & mask) != 0);
        }
    }

    private void changeState(int newstate) {
        Logger.info(getStateString(state) + " --> " + getStateString(newstate));
        if (display != null) {
            try {
                display.wake();
            }
            catch (Exception e) {
                // ignore
            }
        }
        synchronized (lock) {
            state = newstate;
        }
    }

    private String getStateString(int aState) {
        switch (aState) {
            case NONE:
                return "NONE";
            case CREATING:
                return "CREATING";
            case RUNNING:
                return "RUNNING";
            case EXITING:
                return "EXITING";
            default:
                return "";
        }
    }

    private void handleKey(Event e) {
    	lastKeyScancode = e.keyCode;
    	/*
         * Left Shift  = 1280  = 0x00000500
         * Right Shift = 1536  = 0x00000600
         * Left Ctrl   = 160   = 0x000000A0
         * Right Ctrl  = 192   = 0x000000C0
         * Fn          = 12288 = 0x00003000
         * Chr         = 10240 = 0x00002800
         */

        lastKeyModifier = 0;
        int modifiers = e.stateMask & SWT.MODIFIER_MASK;
        // TODO: eSWT support required - howto map (left/right) SWT.SHIFT and SWT.CTRL
        if ((modifiers & SWT.SHIFT) != 0) {
            lastKeyModifier |= 0x00000500; //left shift
        }
        if ((modifiers & SWT.CTRL) != 0) {
            lastKeyModifier |= 0x000000A0; //left control
        }
        /*if ((modifiers & SWT.COMMAND) != 0) {
            lastKeyModifier |= 0x00003000; //function
        }*/
        javax.microedition.lcdui.Display.getDisplay().eswtHandleEvent(e);
    }

    static void update(String className, int delta) {
        if (finalizers.containsKey(className)) {
            delta += ((Integer) finalizers.get(className)).intValue();
        }
        finalizers.put(className, new Integer(delta));
    }

    static void listClassStat() {
        Logger.info("OpenLCDUI Unfinalized classes :");
        Enumeration e = finalizers.keys();
        while (e.hasMoreElements()) {
            String key = (String) e.nextElement();
            Logger.info(key + " : " + (finalizers.get(key)));
        }
        finalizers.clear();
    }


}
