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
* Description: 
*
*/

package javax.microedition.lcdui;

import org.eclipse.swt.widgets.Widget;


/*
 * Event dispatcher singleton class. Takes care of dispatching the UI callbacks
 * to the MIDlet in a dedicated dispatcher thread. Owns and encapsulates an 
 * event queue of callback events. 
 */
final class EventDispatcher {

	private static EventDispatcher singleton;
	private static EventQueue eventQueue;
	private static Thread dispatcherThread;
	private static boolean terminated;
	private static Object wakeLock;
	private static Object callbackLock;
	private static boolean pendingWake;
	private static Runnable terminatedNotification;
	private final static String logName = "LCDUI event dispatcher: ";

	/*
	 * LCDUI-internal event class for callback events. 
	 */
	static final class LCDUIEvent {
		/*
		 * High bits of the event event type identifier are used to group the
		 * events that are handled the same way in the Event Dispatcher.
		 */
		static final int CANVASBIT = 0x80000000;
		static final int CUSTOMITEMBIT = 0x40000000;
		static final int DISPLAYABLEBIT = 0x20000000;
		static final int EVENTTYPEMASK = CANVASBIT|CUSTOMITEMBIT|DISPLAYABLEBIT;
		
		/*
		 * Event type identifiers. 
		 */
		static final int CANVAS_HIDENOTIFY = 1|CANVASBIT;
		static final int CANVAS_KEYPRESSED = 2|CANVASBIT;
		static final int CANVAS_KEYREPEATED = 3|CANVASBIT;
		static final int CANVAS_KEYRELEASED = 4|CANVASBIT;
		static final int CANVAS_PAINT_NATIVE_REQUEST = 5|CANVASBIT;
		static final int CANVAS_PAINT_MIDLET_REQUEST = 6|CANVASBIT;
		static final int CANVAS_POINTERDRAGGED = 7|CANVASBIT;
		static final int CANVAS_POINTERPRESSED = 8|CANVASBIT;
		static final int CANVAS_POINTERRELEASED = 9|CANVASBIT;
		static final int CANVAS_SHOWNOTIFY = 10|CANVASBIT;
		static final int DISPLAYABLE_SIZECHANGED = 11|DISPLAYABLEBIT;
		static final int DISPLAYABLE_COMMANDACTION = 12|DISPLAYABLEBIT;
		static final int CUSTOMITEM_HIDENOTIFY = 13|CUSTOMITEMBIT;
		static final int CUSTOMITEM_KEYPRESSED = 14|CUSTOMITEMBIT;
		static final int CUSTOMITEM_KEYREPEATED = 15|CUSTOMITEMBIT;
		static final int CUSTOMITEM_KEYRELEASED = 16|CUSTOMITEMBIT;
		static final int CUSTOMITEM_PAINT_NATIVE_REQUEST = 17|CUSTOMITEMBIT;
		static final int CUSTOMITEM_PAINT_MIDLET_REQUEST = 18|CUSTOMITEMBIT;
		static final int CUSTOMITEM_POINTERDRAGGED = 19|CUSTOMITEMBIT;
		static final int CUSTOMITEM_POINTERPRESSED = 20|CUSTOMITEMBIT;
		static final int CUSTOMITEM_POINTERRELEASED = 21|CUSTOMITEMBIT;
		static final int CUSTOMITEM_SHOWNOTIFY = 22|CUSTOMITEMBIT;
		static final int CUSTOMITEM_SIZECHANGED = 23|CUSTOMITEMBIT;
		static final int CUSTOMITEM_TRAVERSE = 24|CUSTOMITEMBIT;
		static final int CUSTOMITEM_TRAVERSEOUT = 25|CUSTOMITEMBIT;
		static final int ITEM_COMMANDACTION = 26;
		static final int ITEMSTATELISTENER_ITEMSTATECHANGED = 27;
		static final int RUNNABLE_CALLSERIALLY = 28;
		
		/*
		 * Event parameters. 
		 */
		int type;
		int x;
		int y;
		int width;
		int height;
		int keyCode;
		Runnable runnable;
		Command command;
		CommandListener commandListener;
		ItemCommandListener itemCommandListener;
		Item item;

		/*
		 * Handler object. The object that the LCDUIEvent will be passed to 
		 * when the event is dispatched. Based on the event type it's known 
		 * what type of an object it must be. The handler object will 
		 * implement how to do the actual callback. 
		 */
		Object handlerObject;
		
		/*
		 * The associated eSWT widget. Used to store the eSWT widget which 
		 * the original eSWT event was for. E.g. in Form the CustomItem's 
		 * widget may change while the event is in the queue. 
		 */
		Widget widget;
		
		/*
		 * Used by EventQueue. 
		 */
		LCDUIEvent next;

		private LCDUIEvent() {	
		}
	}
	
	/*
	 * The event loop executed in a dedicated dispatcher thread. Events are 
	 * popped from the queue and dispatched oldest first until there are no 
	 * more events. Then the thread sleeps until new events are posted or 
	 * termination is requested. Any exceptions thrown from the event handlers
	 * are let through to the runtime which should lead into termination of
	 * all threads in the process. 
	 */
	private static class EventLoop implements Runnable {
		private boolean noException;
		public void run() {
			try {
				Logger.info(logName + "Dispatcher thread started");
				while(true) {
					LCDUIEvent event;
					do {
						synchronized(wakeLock) {
							if(terminated) {
								cleanup();
								noException = true;
								return;
							}
							event = eventQueue.pop();
							pendingWake = false;
						}
						
						if(event != null) {
							synchronized(callbackLock) {
								// No callbacks are sent if already terminating
								if(!terminated) {
									handleEvent(event);
								}
							}
						}
					} while(event != null);
					
					// No more events 
					waitForWake();
				}
			} finally {
				if(noException) {
					Logger.info(logName + "Dispatcher thread exiting normally");
				} else {
					Logger.error(logName + "Dispatcher thread exiting abnormally");					
				}
				if(terminatedNotification != null) {
					terminatedNotification.run();
					terminatedNotification = null;
				}
			}
		}
	}

	private EventDispatcher() {
		wakeLock = new Object();
		callbackLock = new Object();
		eventQueue = new EventQueue();
		dispatcherThread = new Thread(new EventLoop(), this.toString());
		dispatcherThread.start();
	}

	/*
	 * Cleanup that is done when closing down in a controlled way. 
	 */
	private static void cleanup() {
		eventQueue = null;
		dispatcherThread = null;
	}

	/*
	 * Synchronized to the EventDispatcher class to synchronize with multiple
	 * clients requesting the instance concurrently.
	 */
	static synchronized EventDispatcher instance() {
		if(singleton == null) {
			singleton = new EventDispatcher();
		}
		return singleton;
	}
	
	/*
	 * LCDUIEvent factory. 
	 */
	LCDUIEvent newEvent(int type, Object handlerObject) {
		LCDUIEvent event = new LCDUIEvent();
		event.type = type;
		event.handlerObject = handlerObject;
		return event;
	}

	/*
	 * UI thread and the application threads call to add an event to the queue.
	 * Synchronized to the EventDispatcher instance to synchronize with queries about
	 * the queue content.
	 */
	synchronized void postEvent(LCDUIEvent event) {
		// New events ignored after terminate() has been called. 
		if(terminated) {
			return;
		}
		eventQueue.push(event);
		wake();
	}
	
	/*
	 * Asynchronously terminates the event dispatcher. If it's currently 
	 * executing a callback it will finish it but not execute any callbacks 
	 * after it. Events possibly remaining in the queue are discarded. The 
	 * method is synchronized to the EventDispatcher instance to synchronize 
	 * with queue content manipulation and query operations.
	 * 
	 * @param runnable Called when dispatcher is out of the final callback and 
	 * is going to die. 
	 */
	synchronized void terminate(Runnable runnable) {
		synchronized(wakeLock) {
			wake();
			terminatedNotification = runnable;
			terminated = true;
		}
	}
		
	/*
	 * Canvas.serviceRepaints support is implemented in EventDispatcher so that
	 * it's possible to synchronize properly with termination and other 
	 * callbacks without exposing the event dispatcher's internal 
	 * implementation details to other classes. This method must be called 
	 * directly in the application thread calling Canvas.serviceRepaints(). It 
	 * can be either the dispatcher thread or any application thread. Never 
	 * the UI thread. 
	 */
	void serviceRepaints(Canvas canvas) {
		// Synchronizing to callbackLock guarantees that dispatcher thread is
		// not in a callback and is not going to make a callback until the lock
		// is freed from here. If this is the dispatcher thread then it already 
		// holds the lock. 
		synchronized(callbackLock) {
			// Synchronized to the EventDispatcher instance to synchronize with
			// terminate() calls. Lock is freed before the callback so that
			// it's possible to complete a terminate() call during the 
			// callback. Event loop is going to check after acquiring the
			// callbackLock if terminate() was called during this callback. 
			synchronized(this) {
				if(terminated) {
					return;
				}
			}
			// Canvas.paint() is called back directly in the thread that called
			// serviceRepaints(). Event is not really needed but is created so
			// that the same APIs can be used. 
            LCDUIEvent event = newEvent(LCDUIEvent.CANVAS_PAINT_MIDLET_REQUEST, canvas);
            event.widget = canvas.getContentComp();
    		canvas.doCallback(event);
		}
	}
	
	private static void wake() {
		synchronized(wakeLock) {
			wakeLock.notify();
			pendingWake = true;
		}
	}
	
	private static void waitForWake() {
		try {
			synchronized(wakeLock) {
				// If there has been a call to wake() then one more iteration
				// must be executed before entering wait(). 
				if(!pendingWake) {
					wakeLock.wait();
				}
			}
		} catch(InterruptedException interruptedException) {
		}
	}
	
	private static void handleEvent(LCDUIEvent event) {
		switch(event.type & LCDUIEvent.EVENTTYPEMASK) {
		case LCDUIEvent.CANVASBIT:
			handleCanvasEvent(event);
			break;
		case LCDUIEvent.CUSTOMITEMBIT:
			handleCustomItemEvent(event);
			break;
		case LCDUIEvent.DISPLAYABLEBIT:
			handleDisplayableEvent(event);
			break;
		default:
			handleOtherEvent(event);
			break;
		}
		// When returning from here all the references to the event have been 
		// lost and the objects referenced by the event can be gc'd. No need 
		// to set the fields to null. 
	}
	
	private static void handleCanvasEvent(LCDUIEvent event) {
		Canvas canvas = (Canvas)event.handlerObject;
		canvas.doCallback(event);
	}	
	
	private static void handleCustomItemEvent(LCDUIEvent event) {
		Form form = (Form)event.handlerObject;
		form.doCallback(event);
	}
	
	private static void handleDisplayableEvent(LCDUIEvent event) {
		Displayable displayable = (Displayable)event.handlerObject;
		displayable.doCallback(event);
	}
	
	private static void handleOtherEvent(LCDUIEvent event) {
		switch(event.type) {
		case LCDUIEvent.RUNNABLE_CALLSERIALLY:
			handleCallSerially(event);
			break;
		case LCDUIEvent.ITEM_COMMANDACTION:
			handleItemCommandListenerCommandAction(event);
			break;
		default:
			Logger.error(logName + "Unknown event type: " + event.type);
			break;
		}		
	}
	
	private static void handleCallSerially(LCDUIEvent event) {
		Display display = (Display)event.handlerObject;
		display.doCallback(event);
	}
		
	private static void handleItemCommandListenerCommandAction(LCDUIEvent event) {
		Item item = (Item)event.handlerObject;
		item.doCallback(event);
	}
}
