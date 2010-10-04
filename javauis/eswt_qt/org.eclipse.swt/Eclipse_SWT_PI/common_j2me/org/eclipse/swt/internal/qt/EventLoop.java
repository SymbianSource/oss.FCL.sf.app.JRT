/*******************************************************************************
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

package org.eclipse.swt.internal.qt;

import org.eclipse.swt.internal.qt.midp.UIThreadLauncher;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Internal_PackageSupport;

import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.ShutdownListener;

/**
 * The internal event loop which will run until the application requests the UI 
 * thread. Then UI thread is handed over to the application which will continue
 * running the loop as it pleases. If the application exits cleanly the UI 
 * thread will return back to here and the event loop can continue to run
 * until the runtime signals shutdown. 
 * 
 * This class is the owner of the internal Display instance. 
 */
public final class EventLoop {

    /*
     * The internal Display instance. Owned by this class. 
     */
    private static Display internalDisplay;

    /*
     * Event loop can run here before the application's loop and after the UI
     * thread exits from the application's loop. These states are identifying
     * which of the two loops is currently running. 
     */
    private static final int PRE_APPLICATION = 0;
    private static final int POST_APPLICATION = 1;
    
    /*
     * Boolean used to detect when application has requested the UI thread and
     * the hand-over must begin. 
     */
    private static volatile boolean applicationRequestedUIThread;

    /*
     * Boolean used to detect when the runtime wants to bring the UI down and
     * the event loop must come down also. 
     */
    private static volatile boolean shuttingDownReceived;

    /*
     * Boolean used to store if starting has been attempted already. 
     */
    private static boolean started;
    
    /*
     * Runnable that gets called in the UI thread when starting up. Named 
     * class declared to be able to make it static. 
     */
    private static final class LoopRunner implements Runnable {
        public void run() {
            EventLoop.run();
        }       
    }
    
    /*
     * Not to be instantiated. 
     */
    private EventLoop() {
    }
    
    /**
     * Returns the internal Display owned by the EventLoop class. Caller must 
     * not dispose it. 
     * @return The internal Display instance
     * @throws RuntimeException
     */
    public static Display getInternalDisplay() {
        synchronized(EventLoop.class) {
            ensureStartedOrThrow();
            return internalDisplay;
        }
    }

    /*
     * Starts the event loop. If this returns without throwing then the 
     * internal Display instance has been successfully created and can be 
     * asked with getInternalDisplay().
     * 
     * Any calls after the first successful call are ignored. Any calls after
     * the first failed call will not retry but will throw.
     */
    private static void ensureStartedOrThrow() {
        // This is called synchronized with the Display creation that takes 
        // place in the UI thread. 
        
        if(!started) {
            started = true;
            
            // Start the UI thread
            if(!UIThreadLauncher.startInUIThread(new LoopRunner())) {
                // Failed to create the UI thread, release the starting thread
                EventLoop.class.notify();
            }
            
            // Wait until the Display gets created in the UI thread
            try {
                EventLoop.class.wait();
            } catch(InterruptedException e) {
                // Nothing to do
            }
        }
        
        // Check Display creation status
        if(internalDisplay == null) {
            throw new RuntimeException("Failed to start");
        }
    }
    
    /*
     * Start listening for the application requesting the UI thread via the
     * eSWT's UIThreadSupport API. 
     */
    private static void listenApplicationUIThreadRequest() {
        UIThreadHandOverManager.setApplicationUIListener(new ApplicationUIListener() {
            /*
             * Can get called in any thread but the UI thread. 
             */
            public void applicationUIThreadRequest() {
                // Synchronized with the Display creation and destruction
                synchronized(EventLoop.class) {
                    applicationRequestedUIThread = true;
                    // It's ok to skip wake if Display has not been created yet
                    if(internalDisplay != null && !internalDisplay.isDisposed()) {
                        internalDisplay.wake();
                    }
                }
            }
        });
    }
    
    /*
     * Start listening for the runtime's shutting down message. 
     */
    private static void listenRuntimeShutdownRequest() {
        ApplicationUtils.getInstance().addShutdownListener(
            /*
             * Can get called in any thread but the UI thread. 
             */
            new ShutdownListener() {
                public void shuttingDown() {
                    // Synchronized with the Display creation and destruction
                    synchronized(EventLoop.class) {
                        shuttingDownReceived = true;
                        // It's ok to skip wake if Display has not been created yet
                        if(internalDisplay != null && !internalDisplay.isDisposed()) {
                            internalDisplay.wake();
                        }       
                    }
                }
            });
    }
    
    /*
     * Creates the internal Display and runs the event loop. Called in the UI 
     * thread. 
     */
    private static void run() {
        try {
            // Add the appropriate listeners. 
            listenRuntimeShutdownRequest();
            listenApplicationUIThreadRequest();
            
            // Create the internal Display. 
            createInternalDisplay();
    
            // Run event loop until the application wants to take over or the 
            // runtime signals shutdown. In the latter case exit from the UI 
            // thread immediately. 
            loop(PRE_APPLICATION);
            if(shuttingDownReceived) {
                return;
            }
            
            // Hand over the UI thread to the application. The application is
            // responsible of the event loop until it allows the UI thread to
            // return back here. 
            UIThreadHandOverManager.runApplicationUI();
            
            // Application allowed its event loop to exit. Continue 
            // dispatching the events until the runtime wants to shut down. 
            loop(POST_APPLICATION);
            
        } finally {
            destroy();
        }
    }
    
    /* 
     * Creates the Display. Synchronized with the starting thread that 
     * waits until the Display has been created. 
     */
    private static void createInternalDisplay() {
        synchronized(EventLoop.class) {
            try {
                // Will throw if fails
                internalDisplay = Internal_PackageSupport.internalInstance();
            } finally {
                // Release the starting thread
                EventLoop.class.notify();
            }
        }
    }
    
    /*
     * Frees all the owned resources when shutting down. Ideally this would be
     * executed only after all the application's threads have been finalized. 
     * If the application's threads are still running it's possible that 
     * someone would try to access the internal Display after this. This would 
     * cause an exception with "device is disposed".
     */
    private static void destroy() {
        // Synchronized with the shutting down message and the application UI 
        // thread request. 
        synchronized(EventLoop.class) {
            if(internalDisplay != null) {
                // If this throws then Display.dispose() was called on the 
                // internal Display by someone who doesn't own it. 
                if(internalDisplay.isDisposed()) {
                    throw new RuntimeException(
                            "Internal Display has been disposed by someone who " +
                            "doesn't own it");
                }
                
                // If the application's Display has been disposed then 
                // disposing also the internal Display here would destroy the 
                // QApplication and all the UI resources tied to it. Disposing 
                // is not done but the resources are allowed to leak on 
                // purpose:
                //
                // In MIDP many of the related APIs are thread-safe and are 
                // exclusively using garbage collection for memory management. 
                // Disposing the UI resources of API objects that still may be 
                // referenced would create a deviation from the pattern and 
                // that would need special handling all around the API 
                // implementations. That handling is not attempted but instead 
                // the internal Display instance is allowed to live until the 
                // process terminates. 
            }
        }
    }
    
    /*
     * The event loop that can run before and after the application's loop. 
     * State parameter tells which one of the two this is. The exit 
     * condition depends on the state. 
     */
    private static void loop(final int state) {
        while(!loopExitCondition(state)) {
            if(!internalDisplay.readAndDispatch()) {
                internalDisplay.sleep();
            }
        }
    }
    
    /*
     * Checks if the loop exit condition is met. The condition is different
     * depending on if the loop is running before or after the application's
     * loop.
     */
    private static boolean loopExitCondition(final int state) {
        switch(state) {
        case PRE_APPLICATION:
            return shuttingDownReceived || applicationRequestedUIThread;
        case POST_APPLICATION:
            return shuttingDownReceived;
        default:
            return true;
        }
    }
}
