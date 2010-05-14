/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


// PACKAGE
package com.nokia.microedition.volumekeys;

// IMPORTS
import com.nokia.microedition.media.NativeError;
import java.lang.OutOfMemoryError;
//import com.nokia.microedition.volumekeys.LCDUIForegroundListener;

import org.eclipse.swt.*;
import org.eclipse.swt.widgets.*;
import javax.microedition.lcdui.*;
import org.eclipse.swt.widgets.Display;


// for xm-radio fix
//import com.symbian.midp.runtime.ToolkitInvoker;

// CLASS DESCRIPTION
/**
 * Provides information about the state of the MIDlet
 */
public class ForegroundListener implements Listener
{
    //private final LCDUIForegroundListener iLCDUIForegroundListener;
    private final int iForegroundHandle;
    private final int iEventSourceHandle;
     // Added for MMAPI UI 3.x
    private boolean flagForground = true;
   // private MMAPILCDUIInvokerImpl obj;

    /*
     * Constructor. Creates Foreground listener
     * The listener is added to Event source native side
     */
    public ForegroundListener(int aEventSourceHandle)
    {
        iEventSourceHandle = aEventSourceHandle;
		 // Added for MMAPI UI 3.x
        // obj = new MMAPILCDUIInvokerImpl();
        // obj.AddObservertoDisplay(this);
         System.out.println("inside ForgroundLIstener constructor before _initialize");
         iForegroundHandle = _initialize(aEventSourceHandle, this/*, iToolkitHandle*/);
         System.out.println("inside ForgroundLIstener constructor after _initialize");
         if (iForegroundHandle < NativeError.KErrNone)
         {
            throw new OutOfMemoryError("Foreground initialization failed.");
         }



    }


public void init()
{
final ForegroundListener listener = this;
System.out.println("inside init()....");

try{
	 final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
	 System.out.println("inside init()....1");


	 			 disp.syncExec(new Runnable() {
	 			             public void run() {
	 							org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
	 			               disp.addFilter(SWT.Activate,listener);
	 			               disp.addFilter(SWT.Deactivate,listener);
	 			             }
		 });
}
catch(Exception e)
{
	System.out.println("inside init()....exception is " + e.toString());
}


}


    //  Added for MMAPI UI 3.x

	    public boolean isForeground()
	    {
	   		return flagForground;
	    }

		/*
	     * From org.eclipse.swt.widget.Listener interface
	     * Handling all the event
	     * Used to filter out the event which can be used for Foreground/Background notification
	     */
	    public void handleEvent(Event event)
	    {
			System.out.println("ForegroundListener :handle event is called");
			if(event.type == SWT.Activate)
			{
				System.out.println("handle event got a event: not in background");
				flagForground = true;
			//	update();

			}
			else if(event.type == SWT.Deactivate)
			{
				System.out.println("handle event got a event: in background");
				flagForground = false;
			// update();
			}
	}

  /*
  * Called from handleEvent callback function to notify native about the midlet foreground status
  */
    public synchronized void update()
    {
		boolean isFg = isForeground();
		System.out.println("ForegroundListener.java inside update() : before calling native function ");
       _setForeground(iEventSourceHandle, iForegroundHandle, isFg);
       System.out.println("ForegroundListener.java inside update() : after calling native function ");
    }

    private native int _initialize(int aEventSourceHandle,
                                   Object aForegroundListener/*,
                                   int atoolkitHandle*/); // xm-radio fix

   private native void _setForeground(int aEventSourceHandle,
                                       int aForegroundHandle,
                                       boolean aForeground);

}

// End of File
