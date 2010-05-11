package com.nokia.microedition.media.control;

import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.events.*;
import com.nokia.microedition.volumekeys.*;


public class MMAPIeSWTObserver
{
	//MMAPILCDUIInvokerImpl mmapiLcduiInvoker;

 mmacontrollistener cntrlsner ;
 mmashelllistener shellsner ;
 mmadisposelistener displsner;

MMACanvasDisplay display;


	public MMAPIeSWTObserver()
	{
		//mmapiLcduiInvoker = new MMAPILCDUIInvokerImpl();
	}

	public void setDisplayToObserver(MMACanvasDisplay display)
	{
		this.display = display;
		cntrlsner = new mmacontrollistener(display);
		shellsner = new mmashelllistener(display);
		displsner = new mmadisposelistener(display);
	}

	public void addControlListenerToControl(final Control eSWTControl)
	{

		System.out.println("MMAPIeSWTObserver.java :addControlListenerToControl +");
		//mmapiLcduiInvoker.AddControlListenerToControl(eSWTControl,(org.eclipse.swt.events.ControlListener)this);

	  try{
			 final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
			 disp.syncExec(new Runnable() {
			 			             public void run() {
			 							               eSWTControl.addControlListener(cntrlsner);
			 			             					}
				 });
		}
		catch(Exception e)
		{
			System.out.println("addControlListenerToControl....exception is " + e.toString());
		}

		System.out.println("MMAPIeSWTObserver.java :addControlListenerToControl -");

	}

	public void addShellListenerToControl(final Control eSWTControl)
	{
			System.out.println("MMAPIeSWTObserver.java :addShellListenerToControl +");
			try{
				 final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
				 disp.syncExec(new Runnable() {
										 public void run() {
														     eSWTControl.getShell().addShellListener(shellsner);
														   }
					 });
				}
				catch(Exception e)
				{
					System.out.println("addShellListenerToControl....exception is " + e.toString());
				}

			System.out.println("MMAPIeSWTObserver.java :addShellListenerToControl -");

	}


	public void addDisposeListenerToControl(final Control eSWTControl)
	{
			System.out.println("MMAPIeSWTObserver.java :addDisposeListenerToControl +");
			//mmapiLcduiInvoker.AddDisposeListenerToControl(eSWTControl,(org.eclipse.swt.events.DisposeListener)this);

			try{
				 final org.eclipse.swt.widgets.Display disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
				 disp.syncExec(new Runnable() {
										 public void run() {
															 eSWTControl.addDisposeListener(displsner);
														   }
					 });
				}
				catch(Exception e)
				{
					System.out.println("addShellListenerToControl....exception is " + e.toString());
				}
			System.out.println("MMAPIeSWTObserver.java :addDisposeListenerToControl -");

	}

	public void removeControlListenerToControl(Control eSWTControl)
	{
		System.out.println("MMAPIeSWTObserver.java :removeControlListenerToControl +");
		//mmapiLcduiInvoker.RemoveControlListenerToControl(eSWTControl,(org.eclipse.swt.events.ControlListener)this);
		System.out.println("MMAPIeSWTObserver.java :removeControlListenerToControl -");

	}
	public void removeShellListenerToControl(Control eSWTControl)
	{
			System.out.println("MMAPIeSWTObserver.java :removeShellListenerToControl +");
			//mmapiLcduiInvoker.RemoveShellListenerToControl(eSWTControl,(org.eclipse.swt.events.ShellListener)this);
			System.out.println("MMAPIeSWTObserver.java :removeShellListenerToControl -");

	}
	public void removeDisposeListenerToControl(Control eSWTControl)
	{
			System.out.println("MMAPIeSWTObserver.java :removeDisposeListenerToControl +");
			//mmapiLcduiInvoker.RemoveDisposeListenerToControl(eSWTControl,(org.eclipse.swt.events.DisposeListener)this);
			System.out.println("MMAPIeSWTObserver.java :removeDisposeListenerToControl -");

	}

}


// Control Listener

class mmacontrollistener implements ControlListener
{

	private MMACanvasDisplay display;
	mmacontrollistener(MMACanvasDisplay display)
	{
		this.display = display;

	}
	// from ControlListener
	public void controlMoved(ControlEvent e)
	{
		System.out.println("MMAPIeSWTObserver.java :controlMoved callback");
	}

	public void controlResized(ControlEvent e)
	{
		System.out.println("MMAPIeSWTObserver.java :controlResized callback");
	}
}

// ShellListener
class mmashelllistener implements ShellListener
{
    private MMACanvasDisplay display;
	mmashelllistener(MMACanvasDisplay display)
	{
		this.display = display;

	}
	// from ShellListener
	public void shellActivated(ShellEvent e)
	{
		System.out.println("MMAPIeSWTObserver.java :shellActivated callback");
		display.setContainerVisibilityToNative(true);
	}

	public void shellClosed(ShellEvent e)
	{
		System.out.println("MMAPIeSWTObserver.java :shellClosed callback");
	}

	public void shellDeactivated(ShellEvent e)
	{
		System.out.println("MMAPIeSWTObserver.java :shellDeactivated callback");
		display.setContainerVisibilityToNative(false);
	}

	public void shellDeiconified(ShellEvent e)
	{
		System.out.println("MMAPIeSWTObserver.java :shellDeiconified callback");
	}

	public void shellIconified(ShellEvent e)
	{
		System.out.println("MMAPIeSWTObserver.java :shellIconified callback");
	}


}

// DisposeListener

class mmadisposelistener implements DisposeListener
{
	private MMACanvasDisplay display;
  mmadisposelistener(MMACanvasDisplay display)
	{
	 this.display = display;
	}

	// from DisposeListener
	public void widgetDisposed(DisposeEvent e)
	{
		System.out.println("MMAPIeSWTObserver.java :widgetDisposed callback");
	}

}