/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Nokia Corporation - initial implementation 
 *******************************************************************************/
package org.eclipse.swt.snippets;

import javax.microedition.midlet.MIDlet;

import org.eclipse.swt.widgets.Display;

/**
 * <p>
 * This class provides a basis for implementing MIDlets that use eSWT UI. 
 * You can write your own MIDlet easily by deriving from this class and
 * overriding either runMain() or runUI().
 * </p><p>
 * A MIDlet that overrides runMain() needs to create the Display and implement
 * the event loop from scratch similarly to a main() application. 
 * </p><p>
 * If you don't want to implement the event loop yourself you can choose to
 * override runUI(). When runUI() is called the Display has already been 
 * created and the event loop will be automatically started once runUI() is 
 * allowed to return. The implementation of runUI() only needs to initialize 
 * the desired UI components. 
 * </p><p>
 * An example overriding runMain():
 * </p><code><pre>
 * public class MyMIDlet extends SWTMIDlet {
 *     public void runMain() {
 *         Display display = new Display();
 *         Shell shell = new Shell(display);
 *         shell.setLayout(new FillLayout());
 *         Command exitCommand = new Command(shell, Command.EXIT, 0);
 *         exitCommand.setText("Exit");
 *         exitCommand.addSelectionListener(
 *             new SelectionListener() {
 *                 public void widgetDefaultSelected(SelectionEvent e) {
 *             }
 *             public void widgetSelected(SelectionEvent e) {
 *                 exit();
 *             }
 *         });
 *         Label label = new Label(shell, SWT.HORIZONTAL);
 *         label.setText("Label here");
 *         shell.open();
 *         while(!shell.isDisposed()) {
 *             if(!display.readAndDispatch())
 *                display.sleep();
 *           }
 *         display.dispose();
 *     }
 * }
 * </pre></code><p>
 * An example overriding runUI():
 * </p><code><pre>
 * public class MyMIDlet extends SWTMIDlet {
 *     public void runUI(Display display) {
 *         Shell shell = new Shell(display);
 *         Command exitCommand = new Command(shell, Command.EXIT, 0);
 *         exitCommand.setText("Exit");
 *         exitCommand.addSelectionListener(
 *             new SelectionListener() {
 *                 public void widgetDefaultSelected(SelectionEvent e) {
 *             }
 *             public void widgetSelected(SelectionEvent e) {
 *                 exit();
 *             }
 *         });
 *         shell.setLayout(new FillLayout());
 *         Label label = new Label(shell, SWT.HORIZONTAL);
 *         label.setText("Label here");
 *         shell.open();
 *     }
 * }
 * </pre></code>
 * @see #runMain()
 * @see #runUI(Display)
 */
public abstract class SWTMIDlet extends MIDlet {

private Thread uiThread;
private boolean started;
private boolean exiting;

/**
 * <p>
 * The MIDlet should override either this method or runUI().
 * </p>
 * <p>
 * runMain() is called in the UI thread. A MIDlet that overrides it needs to
 * create the Display and implement the event loop similarly to a main()
 * application.
 * </p>
 * <p>
 * The default implementation creates the Display and then calls runUI() where
 * the application is expected to initialize the UI components. Finally it 
 * enters the event loop. 
 * </p>
 * 
 * @see#runUI
 */
protected void runMain() {
    // Create the Display.  
    final Display display = new Display();
    
    // Call runUI to construct the UI. 
    runUI(display);
    
    // Enter the event loop. 
    while (!display.isDisposed()) {
        if (!display.readAndDispatch()) {
            if (!display.isDisposed()) {
                display.sleep();
            }
        }
    } 
}

/**
 * <p>
 * The MIDlet should override either this method or runMain().
 * </p>
 * <p>
 * The Display has been created and the event loop is running in the calling
 * thread. The application should override this to create and initialize the
 * desired UI components and then let it return to allow the event loop to
 * dispatch events. 
 * </p>
 * <p>
 * The default implementation does nothing.
 * </p>
 * 
 * @see #runMain
 */
protected void runUI(Display display) {
}

/**
 * <p>
 * Exits the MIDlet in a controlled way. Can be called from any thread.
 * </p>
 * <p>
 * This implementation will make the event loop exit by disposing the Display.
 * Then it's waited until the UI thread has finished. 
 * </p>
 * <p>
 * Disposing the Display will dispose all the Widgets because they are in a
 * parent-child hierarchy with the Display as the root object. Resources such as
 * Images, Colors, GCs, and Fonts are not disposed by disposing the Display.
 * They should be managed elsewhere.
 * </p>
 */
public void exit() {
    synchronized (SWTMIDlet.class) {
        exiting = true;
        // Dispose the Display if it exists to make all event loops exit. 
        final Display display;
        if ((display = Display.findDisplay(uiThread)) != null) {
            display.syncExec(new Runnable() {
                public void run() {
                    display.dispose();
                }
            });
        }
        
        // If there's no Display then we don't have means to signal the UI
        // thread to exit. Destroy immediately. 
        if(display == null) {
            notifyDestroyed();
        }
        
        // Wait for the UI thread to finish cleaning up unless it's this thread
        // or it already exited.
        try {
            if (uiThread != null && !uiThread.equals(Thread.currentThread())) {
                // Release the lock and wait
                SWTMIDlet.class.wait();
            }
        } catch (InterruptedException e) {
        }
    }
}

/**
 * <p>
 * Abstract method from javax.microedition.midlet.MIDlet has to be overridden to
 * implement MIDlet startup. The application management software will call this
 * method to start the MIDlet. When the call is allowed to return without
 * throwing any exceptions the MIDlet has successfully transitioned to
 * <i>Active</i> state.
 * </p>
 * <p>
 * This implementation obtains a platform specific UI thread that is executed
 * concurrently with the calling thread and then calls runMain() in the UI
 * thread. The call to this method returns immediately. Subsequent invocations
 * after the first invocation do nothing.
 * </p>
 * 
 * @see MIDlet#startApp
 */
protected void startApp() {
    synchronized (SWTMIDlet.class) {
        if (!started) {
            started = true;
            startInUIThread(new Runnable() {
                public void run() {
                    try {
                        synchronized(SWTMIDlet.class) {
                            // If already exiting then quit here.
                            // Note that it's possible that exit() has been
                            // called but it didn't yet acquire the lock. 
                            if(exiting) {
                                return;
                            }
                            uiThread = Thread.currentThread();
                        }
                        
                        runMain();
                        
                    } finally {
                        synchronized(SWTMIDlet.class) {
                            // Notify the thread possibly waiting in destroyApp. 
                            SWTMIDlet.class.notifyAll();
                            uiThread = null;
                        
                            // If destroyApp() has not been called we need to
                            // destroy the MIDlet explicitly. 
                            notifyDestroyed();
                        }
                    }
                }
            });
        }
    }
}

/**
 * <p>
 * Abstract method from javax.microedition.midlet.MIDlet has to be overridden
 * to implement how to pause the MIDlet. 
 * </p><p>
 * This implementation does nothing. 
 * </p> 
 * @see MIDlet#pauseApp
 */
protected void pauseApp() {
}

/**
 * <p>
 * Abstract method from javax.microedition.midlet.MIDlet has to be overridden to
 * implement how to destroy the MIDlet. This method will be called by the
 * application management software when the system requests the application to
 * destroy itself. This can happen e.g. because the user has chosen to terminate
 * the application or if the system is running low on resources.
 * </p>
 * <p>
 * When the call to this method returns back to the application management
 * software the MIDlet has transitioned to <i>Destroyed</i> state.
 * </p>
 * 
 * @see MIDlet#destroyApp
 */
protected void destroyApp(boolean unconditional) {
    exit();
}

/*
 * Obtain the UI thread and call the Runnable's run() method in it. 
 * If the platform eSWT implementation doesn't provide the UI thread
 * support API for obtaining the UI thread for a MIDlet then try using
 * a random Java thread as the UI thread. 
 */
private void startInUIThread(Runnable runnable) {
    // Try if the UI thread support API for MIDlets is available. 
    Class clazz;
    try {
        clazz = Class.forName("org.eclipse.ercp.swt.midp.UIThreadSupport");
    } catch(Exception e) {
        clazz = null;
    }
    if(clazz != null) {
        // UIThreadSupport is available. However, we can't use it directly 
        // because we want this class to compile also without it. Let's use
        // it through a wrapper class. 
        UIThreadSupportWrapper.startInUIThread(runnable);
    } else {
        // UIThreadSupport is not available. Try launching the UI in 
        // a random Java thread. 
        uiThread = new Thread(runnable);
        uiThread.start();
    }
}

}
