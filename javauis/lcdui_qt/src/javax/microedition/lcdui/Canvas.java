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

import java.util.Vector;
import java.util.Timer;
import java.util.TimerTask;
import javax.microedition.lcdui.EventDispatcher.LCDUIEvent;
import org.eclipse.ercp.swt.mobile.MobileShell;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.*;
import org.eclipse.swt.widgets.*;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.graphics.*;
import org.eclipse.swt.internal.extension.CompositeExtension;

/**
 * The abstract <code>Canvas</code> class is designed to handle low-level
 * graphical operations as well as low-level key and pointer events. Canvas is
 * derived from <code>Displayable</code> so the application can switch between
 * different <code>Canvases</code> together with other
 * <code>Displayables</code>.
 * <p>
 * <b>Canvas</b> the abstract method <code>paint(Graphics g)</code> designed
 * to be implemented by the developer.
 */
public abstract class Canvas extends Displayable {

    /**
     *  Constant for <code>UP</code> game action.
     */
    public static final int UP = 1;

    /**
     * Constant for <code>DOWN</code> game action.
     */
    public static final int DOWN = 6;

    /**
     * Constant for <code>LEFT</code> game action.
     */
    public static final int LEFT = 2;

    /**
     * Constant for <code>RIGHT</code> game action.
     */
    public static final int RIGHT = 5;

    /**
     * Constant for <code>FIRE</code> game action.
     */
    public static final int FIRE = 8;

    /**
     * Constant for general "<code>A</code>" game action.
     */
    public static final int GAME_A = 9;

    /**
     * Constant for general "<code>B</code>" game action.
     */
    public static final int GAME_B = 10;

    /**
     * Constant for general "<code>C</code>" game action.
     */
    public static final int GAME_C = 11;

    /**
     * Constant for general "<code>D</code>" game action.
     */
    public static final int GAME_D = 12;

    /**
     * Key code for <code>0</code> key.
     */
    public static final int KEY_NUM0 = 48;

    /**
     * Key code for <code>1</code> key.
     */
    public static final int KEY_NUM1 = 49;

    /**
     * Key code for <code>2</code> key.
     */
    public static final int KEY_NUM2 = 50;

    /**
     * Key code for <code>3</code> key.
     */
    public static final int KEY_NUM3 = 51;

    /**
     * Key code for <code>4</code> key.
     */
    public static final int KEY_NUM4 = 52;

    /**
     * Key code for <code>5</code> key.
     */
    public static final int KEY_NUM5 = 53;

    /**
     * Key code for <code>6</code> key.
     */
    public static final int KEY_NUM6 = 54;

    /**
     * Key code for <code>7</code> key.
     */
    public static final int KEY_NUM7 = 55;

    /**
     * Key code for <code>8</code> key.
     */
    public static final int KEY_NUM8 = 56;

    /**
     * Key code for <code>9</code> key.
     */
    public static final int KEY_NUM9 = 57;

    /**
     * Key code for <code>*</code> key.
     */
    public static final int KEY_STAR = 42;

    /**
     * Key code for <code>#</code> key.
     */
    public static final int KEY_POUND = 35;

    // Listeners for various events.
    private org.eclipse.swt.events.PaintListener paintListener =
        new CanvasShellPaintListener();

    private CanvasShellMouseListener mouseListener =
        new CanvasShellMouseListener();

    // Canvas Graphics object passed to paint(Graphics g).
    // Graphics has its own back-buffer for double buffering.
    private Graphics canvasGraphics;

    //On Screen Keypad
    //private Composite keypadComposite;
    private CanvasKeypad onScreenkeypad;
    private int oskHeight;

    // Vector of flags that a certain key was pressed but was not released.
    // Used to implement keyRepeated since eSWT does not support
    // key repeat events.
    private Vector keysPressed;

    // Canvas modes
    private static final int MODE_UNINITIALIZED = 0;
    private static final int MODE_NORMAL = 1;
    private static final int MODE_BUFFER_FLUSH = 2;
    private static final int MODE_GAME_BUFFER_FLUSH = 3;
    private int canvasMode = MODE_UNINITIALIZED;

    private boolean suppressGameKeys;
    private boolean suppressDragEvent;
    private boolean cleanupNeeded;
    private Object cleanupLock;
    private boolean noBackground;
    private int gameKeyState;

    private Graphics gameBufferGraphics;

    // lock created by graphics object for serializing
    // flushing against graphics buffer writing
    private Object flushLock;

    private Timer timer = new Timer();
    private CanvasTimerTask timerTask;

    private static final int DEFAULT_TIMEOUT = 500;
    private static final int DEFAULT_TWIPS = 200;

    private int pointerDownX;
    private int pointerDownY;
    private int twips;
    private int timeout;
    private boolean disableTapDetection;

	private boolean repaintPending;
	private int repaintX1;
	private int repaintY1;
	private int repaintX2;
	private int repaintY2;
	private Object repaintLock;

	private boolean selectionKeyCompatibility;
	private boolean finalMode;
    /**
     * Constructs <code>Canvas</code> object.
     */
    public Canvas() {
        super(null);
        repaintLock = new Object();
        cleanupLock = new Object();
        construct();
        canvasGraphics = new Graphics();
        canvasMode = MODE_UNINITIALIZED;
        keysPressed = new Vector();
    }

    /* (non-Javadoc)
     * @see Displayable#eswtConstructShell(int)
     */
    Shell eswtConstructShell(int style) {
        return super.eswtConstructShell(style /*| SWT.RESIZE*/);
    }

    /* (non-Javadoc)
     * @see Displayable#eswtConstructContent(int)
     */
    Composite eswtConstructContent(int style) {
        // Get JAD attribute
        noBackground = JadAttributeUtil.isValue(JadAttributeUtil.ATTRIB_NOKIA_UI_ENHANCEMENT,
    			JadAttributeUtil.VALUE_CANVAS_HAS_BACKGROUND);
        if (noBackground){
        	 style |= SWT.NO_BACKGROUND;
        }

        // Get JAD attribute for S60 Selection Key Compatibility
        selectionKeyCompatibility = JadAttributeUtil.isValue(JadAttributeUtil.ATTRIB_NOKIA_MIDLET_S60_SELECTION_KEY_COMPATIBILITY,
    			JadAttributeUtil.VALUE_TRUE);

        // Get JAD attribute for MIDlet Tap Detection
        String tapAttr = JadAttributeUtil.getValue(JadAttributeUtil.ATTRIB_NOKIA_MIDLET_TAP_DETECTION_OPTIONS);
        if(tapAttr != null) {
          if(tapAttr.indexOf(',') == -1) {
						setDefaultTapValues();
          }
          else {
              String twipString = tapAttr.substring(0, tapAttr.indexOf(',')).trim();
              String timeoutString = tapAttr.substring(tapAttr.indexOf(',') + 1, tapAttr.length()).trim();

              try {
          	     twips = Integer.parseInt(twipString);
          	     timeout = Integer.parseInt(timeoutString);

                 // Check for Negative Values
          	     if( (twips < 0) && (timeout < 0) ) {
          		       setDefaultTapValues();
                 }

                 if( (twips == 0)  && (timeout == 0) ) {
                     disableTapDetection = true;
                 }

                 // if any one of the value is zero, set defaults
                 if( !((twips != 0) && (timeout != 0)) ) {
                     setDefaultTapValues();
                 }
              }
              catch (NumberFormatException e) {
              	 // Alpha Numeric Values of Timeouts and Timeout
					       setDefaultTapValues();
              }
          }
        }
        else
        {
        	 setDefaultTapValues();
        }

        Composite canvasComp = super.eswtConstructContent(style);

		// Read the on screen keypad settings from the jad attribute
		String oskAttr = JadAttributeUtil
				.getValue(JadAttributeUtil.ATTRIB_NOKIA_MIDLET_ON_SCREEN_KEYPAD);
		if (oskAttr != null
				&& (!oskAttr.equalsIgnoreCase(JadAttributeUtil.VALUE_NO))
				&& ((oskAttr
						.equalsIgnoreCase(JadAttributeUtil.VALUE_GAMEACTIONS)) || (oskAttr
						.equalsIgnoreCase(JadAttributeUtil.VALUE_NAVIGATIONKEYS)))) {

			// On screen keypad is required, On devices without keyboard it can
			// be either navigation keys or navigation and game keys
			onScreenkeypad = new CanvasKeypad(this, canvasComp, oskAttr);
		}


        return canvasComp;
    }

    /* (non-Javadoc)
     * @see Displayable#eswtHandleShowCurrentEvent()
     */
    void eswtHandleShowCurrentEvent() {
        super.eswtHandleShowCurrentEvent();
        getContentComp().addPaintListener(paintListener);
        getContentComp().addMouseListener(mouseListener);
        getContentComp().addMouseMoveListener(mouseListener);
    }

    /* (non-Javadoc)
     * @see Displayable#eswtHandleHideCurrentEvent()
     */
    void eswtHandleHideCurrentEvent() {
        super.eswtHandleHideCurrentEvent();
        getContentComp().removePaintListener(paintListener);
        getContentComp().removeMouseListener(mouseListener);
        getContentComp().removeMouseMoveListener(mouseListener);
    }

    /**
     * Issues the request to repaint the whole Canvas.
     */
    public void repaint() {
        repaint(0, 0, getWidth(), getHeight());
    }

    /**
     * Issues the request to repaint the specified area of the Canvas. The
     * request is processed asynchronously.
     *
     * @param x - left bound of the rectangle to redraw.
     * @param y - top bound of the rectangle to redraw.
     * @param width - width of the rectangle to redraw.
     * @param height - height of the rectangle to redraw.
     */
    public void repaint(int x, int y, int width, int height) {
    	// Paint callback event is posted without any invalid area info.
		// Invalid area info is kept in the member variables. Only one event
    	// per Canvas is added to the queue. If there are more repaint() calls
    	// before the already posted event has been served those are merged
		// to the invalid area in the member variables without posting a new
    	// event.
    	synchronized(repaintLock) {
			if (invalidate(x, y, width, height)) {
				// Note that repaintPending doesn't mean that there's a repaint
				// event in the queue. It means that the invalid area is going
				// to be repainted and there's no need to add a new event.
				// It's possible that the repaint event has already been
				// removed from the queue but repaintPending is still true. In
				// that case it's currently being processed and we can still
				// add to the invalid area.
				if (!repaintPending) {
					EventDispatcher eventDispatcher = EventDispatcher.instance();
					LCDUIEvent event = eventDispatcher.newEvent(
							LCDUIEvent.CANVAS_PAINT_MIDLET_REQUEST, this);
					event.widget = getContentComp();
                    eventDispatcher.postEvent(event);
					repaintPending = true;
				}
			}
    	}
    }

    /**
     * Switches the Canvas between full screen and non-full screen modes.
     *
     * @param mode - true switches the Canvas to the full-screen mode.
     */
    public void setFullScreenMode(boolean mode) {
        finalMode = mode;
        ESWTUIThreadRunner.syncExec(new Runnable() {
            public void run() {
                ((MobileShell) getShell()).setFullScreenMode(finalMode);
                //set the CanvasKeypad to the required mode
                if(onScreenkeypad != null) {
                    onScreenkeypad.setFullScreenMode(finalMode);
                }
            }
        });
    }

    /**
     * Processes all the issued paint requests.
     */
    public final void serviceRepaints() {
    	EventDispatcher.instance().serviceRepaints(this);
    }

    /**
     * Checks if the Canvas supports pointer dragging events.
     *
     * @return true if the Canvas supports pointer dragging, false otherwise.
     */
    public boolean hasPointerMotionEvents() {
        return true;
    }

    /**
     * Checks if the Canvas supports pointer events.
     *
     * @return true if the Canvas supports pointer events, false otherwise.
     */
    public boolean hasPointerEvents() {
        return true;
    }

    /**
     * Is the Canvas double buffered.
     *
     * @return true if the Canvas is double buffered, false otherwise.
     */
    public boolean isDoubleBuffered() {
        return true;
    }

    /**
     * Queries if the Canvas supports key repeat events.
     *
     * @return true if Canvas supports key repeat events, false otherwise.
     */
    public boolean hasRepeatEvents() {
        return true;
    }

    /**
     * Returns game action for a specified key code.
     *
     * @param keyCode Key code to map to game action.
     * @return Game action for a a specified key code or
     *         IllegalArgumentException.
     */
    public int getGameAction(int keyCode) {
        return KeyTable.getGameAction(keyCode);
    }

    /**
     * Returns the key code specific for a certain game action.
     *
     * @param gameAction - game action to be mapped to the key code.
     * @return Key code that is mapped to the specified game action.
     */
    public int getKeyCode(int gameAction) {
        return KeyTable.getKeyCode(gameAction);
    }

    /**
     * Returns the key name specific for a certain key code.
     *
     * @param keyCode - key name to get the name of.
     * @return String that contains textual name of the key specified by the key
     *         code.
     */
    public String getKeyName(int keyCode) {
        return KeyTable.getKeyName(keyCode);
    }
    /**
     * Gets height.
     *
     * @return Height of the Displayable in pixels.
     */
    public int getHeight() {
        ESWTUIThreadRunner.syncExec(new Runnable() {
            public void run() {
                if( onScreenkeypad != null ) {
                    oskHeight = onScreenkeypad.getHeight();
                }
            }
        });
        return (super.getHeight() - oskHeight);

    }


    /**
     * Callback to be implemented by the application to render the
     * <code>Canvas</code>. The clip region of <code>Graphics</code> object
     * specifies the area that needs to be updated. All the region has to be
     * updated in <code>paint()</code> method.
     * <p>
     * Application should not take into account the source where the
     * <code>paint()</code> request came from.
     * <p>
     * Any interaction with Graphics object outside of <code>paint()</code>
     * method is undefined.
     * <p>
     * The application should never call <code>paint()</code> explicitly, it is
     * called by the framework.
     *
     * @param g - the <code>Graphics</code> object to be used for graphical
     *            operations on the <code>Canvas.</code>
     */
    protected abstract void paint(Graphics g);

    /**
     * Callback to signal a key press.
     *
     * @param keyCode - key code of the key pressed.
     */
    protected void keyPressed(int keyCode) {
    }

    /**
     * Callback to signal a key release.
     *
     * @param keyCode - key code of the key released.
     */
    protected void keyReleased(int keyCode) {
    }

    /**
     * Callback to signal a key repeat. Happens when the key is pressed down for
     * a long time.
     *
     * @param keyCode - key code of the key repeated.
     */
    protected void keyRepeated(int keyCode) {
    }

    /**
     * Callback to signal pointer press event.
     *
     * @param x - X-coordinate of the tap point.
     * @param y - Y-coordinate of the tap point.
     */
    protected void pointerPressed(int x, int y) {
    }

    /**
     * Callback to signal pointer release event.
     *
     * @param x - X-coordinate of the pointer release point.
     * @param y - Y-coordinate of the pointer release point.
     */
    protected void pointerReleased(int x, int y) {
    }

    /**
     * Callback to signal pointer drag event.
     *
     * @param x - X-coordinate of the pointer drag point.
     * @param y - Y-coordinate of the pointer drag point.
     */
    protected void pointerDragged(int x, int y) {
    }

    /**
     * Callback similar to the one in <code>Displayable</code>. Additional
     * case for <code>sizeChanged</code> to be called in <code>Canvas</code>
     * is switching between full-screen mode by calling
     * <code>setFullScreenMode()</code>.
     */
    protected void sizeChanged(int w, int h) {
    }

    /**
     * showNotify() is the callback called before the Canvas is shown on the
     * screen.
     */
    protected void showNotify() {
    }

    /**
     * hideNotify() is the callback called after the Canvas is removed from the
     * screen.
     */
    protected void hideNotify() {
    }

    /**
     * Init GameCanvas frame buffer.
     *
     * @param supressKeys suppress game keys
     * @return frame buffer
     */
    final void initGameCanvas(boolean suppressKeys) {
        this.suppressGameKeys = suppressKeys;
    }

    /**
     * Update game buffer graphics.
     */
    final void eswtUpdateGameBufferGraphics() {
        if (gameBufferGraphics == null) {
            gameBufferGraphics = new Graphics();
            gameBufferGraphics.initBuffered(this, 0, 0, getWidth(), getHeight());
            flushLock = gameBufferGraphics.getLock();
        }
    }

    /**
     * Get game canvas frame buffer graphics.
     */
    final Graphics getGameBufferGraphics() {
        ESWTUIThreadRunner.safeSyncExec(new Runnable() {
            public void run() {
                eswtUpdateGameBufferGraphics();
            }
        });
        return gameBufferGraphics;
    }
    
    CanvasKeypad getCanvasKeypad()
    {
    	return onScreenkeypad;
    }

    boolean IsFullScreenMode()
    {
    	return finalMode;
    }

    /**
     * Return game key states for GameCanvas.
     *
     * @return game key states.
     */
    final int getGameKeyStates() {
        int ret = gameKeyState;
        gameKeyState = 0;
        return ret;
    }

    void renderGraphics(final Graphics g) {
		// Implementation missing. GameCanvas.paint() should by default render
		// the the off-screen buffer at (0,0). Rendering of the buffer must be
		// subject to the clip region and origin translation of the Graphics
		// object.
    }

    /**
     * Flushes the frameBuffer to the display.
     *
     * @param x
     * @param y
     * @param width
     * @param height
     */
    void flushGameBuffer(final int x, final int y, final int width,
            final int height) {
    	synchronized( flushLock ) {
    		ESWTUIThreadRunner.safeSyncExec(new Runnable() {
    			public void run() {
    				canvasMode = MODE_GAME_BUFFER_FLUSH;
    				((CompositeExtension)getContentComp()).redrawNow(x, y, width, height);
    		    	gameBufferGraphics.resetCommandBuffer();
    		    	canvasMode = MODE_NORMAL;
    			}
    		});
    	}
    }

    /**
     * Called by ShellListener when shell gets activated.
     */
    void handleShellActivatedEvent() {
        super.handleShellActivatedEvent();

        // reset the game key state
        gameKeyState = 0;
        canvasMode = MODE_NORMAL;
		synchronized(cleanupLock) {
			cleanupNeeded = true;
		}

        LCDUIEvent event = EventDispatcher.instance().newEvent(LCDUIEvent.CANVAS_SHOWNOTIFY, this);
        EventDispatcher.instance().postEvent(event);
    }

    /**
     * Called by ShellListener when shell gets de-activated.
     */
    void handleShellDeActivatedEvent() {
        super.handleShellDeActivatedEvent();
        LCDUIEvent event = EventDispatcher.instance().newEvent(LCDUIEvent.CANVAS_HIDENOTIFY, this);
        EventDispatcher.instance().postEvent(event);
    }

    /*
     * UI thread calls
     */
    void eswtHandleResizeEvent(int width, int height) {
        super.eswtHandleResizeEvent(width, height);
		canvasMode = MODE_NORMAL;
		synchronized(cleanupLock) {
			cleanupNeeded = true;
		}
    }

    /*
     * UI thread calls
     */
    void eswtHandleEvent(Event e) {
        super.eswtHandleEvent(e);

        if (e.type == SWT.KeyDown) {
             doKeyPressed(e.keyCode);
        }
        else if (e.type == SWT.KeyUp) {
             doKeyReleased(e.keyCode);
        }

    }

    /*
     * UI thread calls. Paint listener of the eSWT widget.
     */
	class CanvasShellPaintListener implements PaintListener {

		public void paintControl(PaintEvent pe) {
			switch (canvasMode) {
			case MODE_BUFFER_FLUSH:
				// Paint event initiated by us to paint the Canvas.
				doBufferFlush(pe, canvasGraphics);
				break;
			case MODE_GAME_BUFFER_FLUSH:
				// Paint event initiated by us to paint the GameCanvas.
				doBufferFlush(pe, gameBufferGraphics);
				break;
			case MODE_NORMAL:
				// Native toolkit is requesting an update of an area that has
				// become invalid. Can't do anything here because the contents
				// need to be queried from the MIDlet in another thread by
				// a paint callback. For this a paint callback event is posted.
				// For a moment the native toolkit thinks that the area has
				// been validated when in truth it will be painted later after
				// the paint callback has been executed.
				EventDispatcher eventDispatcher = EventDispatcher.instance();
				LCDUIEvent event = eventDispatcher.newEvent(
						LCDUIEvent.CANVAS_PAINT_NATIVE_REQUEST,
						javax.microedition.lcdui.Canvas.this);
				event.x = pe.x;
				event.y = pe.y;
				event.width = pe.width;
				event.height = pe.height;
				event.widget = pe.widget;
                eventDispatcher.postEvent(event);
				break;
			}
		}
	}

	/*
	 * Dispatcher thread or the serviceRepaints()-thread calls.
	 */
	final void doCallback(LCDUIEvent event) {
		switch(event.type) {
		case LCDUIEvent.CANVAS_PAINT_MIDLET_REQUEST: // fall through
		case LCDUIEvent.CANVAS_PAINT_NATIVE_REQUEST:
			doPaintCallback(event);
			break;
		case LCDUIEvent.CANVAS_HIDENOTIFY:
			hideNotify();
			break;
		case LCDUIEvent.CANVAS_KEYPRESSED:
            keyPressed(event.keyCode);
			break;
		case LCDUIEvent.CANVAS_KEYREPEATED:
			keyRepeated(event.keyCode);
			break;
		case LCDUIEvent.CANVAS_KEYRELEASED:
			keyReleased(event.keyCode);
			break;
		case LCDUIEvent.CANVAS_POINTERDRAGGED:
			pointerDragged(event.x, event.y);
			break;
		case LCDUIEvent.CANVAS_POINTERPRESSED:
			pointerPressed(event.x, event.y);
			break;
		case LCDUIEvent.CANVAS_POINTERRELEASED:
			pointerReleased(event.x, event.y);
			break;
		case LCDUIEvent.CANVAS_SHOWNOTIFY:
			showNotify();
			break;
		default:
			super.doCallback(event);
			break;
		}
	}

    /*
     * Dispatcher thread or the serviceRepaints()-thread calls.
     */
	private final void doPaintCallback(final LCDUIEvent event) {
		// Decide the area going to be painted by the callback.
		final int redrawNowX;
		final int redrawNowY;
		final int redrawNowW;
		final int redrawNowH;
		// Before this thread obtains the repaintLock any repaint() calls
		// will still be adding to the invalid area that is going to be
		// painted by this callback.
		synchronized(repaintLock) {
			if(event.type == LCDUIEvent.CANVAS_PAINT_NATIVE_REQUEST) {
				// Merge with possibly existing repaint() requests
                invalidate(event.x, event.y, event.width, event.height);
			} else {
				// Need to add a new event to the queue in subsequent repaint()
				// calls.
				repaintPending = false;
			}

			// Store the current area to be painted
			redrawNowX = repaintX1;
			redrawNowY = repaintY1;
			redrawNowW = repaintX2-repaintX1;
			redrawNowH = repaintY2-repaintY1;


			// After releasing the lock the repaint() calls will start with
			// new invalid area.
			repaintX1 = repaintX2 = repaintY1 = repaintY2 = 0;

			// Don't do the callback if there's nothing to paint
			if(!((redrawNowW > 0) && (redrawNowH > 0))) {
				return;
			}
		}

		// Prepare the GC's buffer if not done yet
		if (canvasGraphics.getCommandBuffer() == null) {
			canvasGraphics.initBuffered(this, event.x, event.y, event.width, event.height);
		}

		// Clean the background if dirty, buffer the operations.
		synchronized(cleanupLock) {
			if (cleanupNeeded && noBackground) {
				// UI thread can change the contentArea object reference at
				// any time. Store the object reference locally to ensure it
				// points to the same rectangle all the time.
				Rectangle contentArea = getContentArea();

				canvasGraphics.setClip(contentArea.x, contentArea.y,
						contentArea.width, contentArea.height);
				canvasGraphics.cleanBackground(contentArea);
				cleanupNeeded = false;
			}
		}

		// Clip must define the invalid area
		canvasGraphics.setClip(redrawNowX, redrawNowY, redrawNowW, redrawNowH);

		// The callback
        paint(canvasGraphics);

		// Wait until the UI thread is available. Then in the UI thread
		// synchronously send a paint event.
		ESWTUIThreadRunner.safeSyncExec(new Runnable() {
			public void run() {
				if(event.widget.isDisposed()) {
                    return;
                }

                canvasMode = MODE_BUFFER_FLUSH;

                ((CompositeExtension) event.widget)
                        .redrawNow(redrawNowX, redrawNowY, redrawNowW, redrawNowH);
                canvasGraphics.resetCommandBuffer();
                canvasMode = MODE_NORMAL;
            }

		});
	}

    /*
     * UI thread calls to flush the command buffer of a graphics context.
     */
    private final void doBufferFlush(PaintEvent event, Graphics graphics) {
    	event.gc.getGCData().internalGc.render(graphics.getCommandBuffer());
    }

    /*
     * UI thread calls.
     */
    void doKeyPressed(int keyCode) {
        Logger.method(this, "doKeyPressed", String.valueOf(keyCode));
        boolean sendCallback = false;

        if (!(updateGameKeyState(keyCode, true) && suppressGameKeys)) {
            if( (selectionKeyCompatibility == true) && (keyCode == -5) ) {
                if(finalMode == true) {
                    if(!((getNumCommands() > 0) && hasCommandListener()) ) {
                        sendCallback = true;
                    }
                    else {
                        sendCallback = false;
                    }
                }
                else {
                    sendCallback = true;
                }
            }
            else if( (selectionKeyCompatibility == false) && (keyCode == -5) ) {
                sendCallback = false;
            }
            else {
                sendCallback = true;
            }
        }
        else {
            sendCallback = false;
        }

        if(sendCallback == true) {
            LCDUIEvent event;

            if (keysPressed.contains(new Integer(keyCode))) {
                event = EventDispatcher.instance().newEvent(LCDUIEvent.CANVAS_KEYREPEATED, this);
            }
            else {
                event = EventDispatcher.instance().newEvent(LCDUIEvent.CANVAS_KEYPRESSED, this);
            }
            event.keyCode = keyCode;
            EventDispatcher.instance().postEvent(event);

        }
    }


    /*
     * UI thread calls.
     */
    void doKeyReleased(int keyCode) {
        Logger.method(this, "doKeyReleased", String.valueOf(keyCode));
        boolean sendCallback = false;
        if (!(updateGameKeyState(keyCode, true) && suppressGameKeys)) {
            if( (selectionKeyCompatibility == true) && (keyCode == -5) ) {
                if(finalMode == true) {
                    if(!((getNumCommands() > 0) && hasCommandListener()) ) {
                        sendCallback = true;
                    }
                    else {
                        sendCallback = false;
                    }
                }
                else {
                    sendCallback = true;
                }
            }
            else if( (selectionKeyCompatibility == false) && (keyCode == -5) ) {
                sendCallback = false;
            }
            else {
                sendCallback = true;
            }
        }
        else {
            sendCallback = false;
        }

        if(sendCallback == true) {

            LCDUIEvent event = EventDispatcher.instance().newEvent(LCDUIEvent.CANVAS_KEYRELEASED, this);
            event.keyCode = keyCode;
            EventDispatcher.instance().postEvent(event);

        }
    }

    /**
     * Updates game key states and returns if the key was a game key.
     */
    private boolean updateGameKeyState(int keyCode, boolean addKeyState) {
    	// Ignore key repeat events
    	if (ESWTUIThreadRunner.getKeyRepeatCount() > 1) {
            return true;
    	}
        try {
            int gameAction = KeyTable.getGameAction(keyCode);
            if (addKeyState) {
                // set bitfield
                gameKeyState |= (1 << gameAction);
            }
            return true;
        }
        catch (IllegalArgumentException iae) {
            return false;
        }
    }

    private void setDefaultTapValues() {
				twips = DEFAULT_TWIPS;
				timeout = DEFAULT_TIMEOUT;
    }

    class CanvasShellMouseListener implements
            org.eclipse.swt.events.MouseListener,
            org.eclipse.swt.events.MouseMoveListener {

        public void mouseDoubleClick(MouseEvent arg0) {
        }

        public void mouseDown(MouseEvent event) {
            LCDUIEvent e = EventDispatcher.instance().newEvent(LCDUIEvent.CANVAS_POINTERPRESSED,
            		javax.microedition.lcdui.Canvas.this);
            e.x = event.x;
            e.y = event.y;
            EventDispatcher.instance().postEvent(e);

           if(!disableTapDetection) {
                // Supress Drag events
           	    suppressDragEvent = true;

                pointerDownX = event.x;
                pointerDownY = event.y;

                // Create and Schedule Timer
                timerTask = new CanvasTimerTask();
                timer.schedule(timerTask, timeout);
           }
        }

        public void mouseUp(MouseEvent event) {
            int pointerUpX = event.x;
            int pointerUpY = event.y;

            if(!disableTapDetection) {
                if (timerTask != null) {
                    timerTask.cancel();
                    timerTask = null;
                }

                // If Timer not expired and Mouseup is withing rectangle assign
                // PointercDown to Pinter Up
        	      if(suppressDragEvent && checkWithinRect(event.x, event.y)) {
        	  	      pointerUpX = pointerDownX;
        	  	      pointerUpY = pointerDownY;
       	  	  	    suppressDragEvent = false;
         	      }
           }

            LCDUIEvent e = EventDispatcher.instance().newEvent(LCDUIEvent.CANVAS_POINTERRELEASED,
            		javax.microedition.lcdui.Canvas.this);
            e.x = pointerUpX;
            e.y = pointerUpY;
            EventDispatcher.instance().postEvent(e);
        }

        public void mouseMove(MouseEvent event) {
        	  // Check for timeout expiration and if PointerUp falls outside the rectangle
         	  if( disableTapDetection || (!suppressDragEvent) || !checkWithinRect(event.x, event.y)) {
                LCDUIEvent e = EventDispatcher.instance().newEvent(LCDUIEvent.CANVAS_POINTERDRAGGED,
                		javax.microedition.lcdui.Canvas.this);
                e.x = event.x;
                e.y = event.y;
                EventDispatcher.instance().postEvent(e);
            }
        }

        boolean checkWithinRect(int x, int y) {
        	  // Get pixel per inch
        		Point P = Display.getCurrent().getDPI();

            float xPxielwidth  = (twips * P.x) / 1440;
            float yPixelHeight = (twips * P.y) / 1440;

            int RightX = pointerDownX + (int) xPxielwidth;

            // If the rectange width falls outside the canvas area
            if(RightX > getWidth()) {
               RightX = getWidth();
            }

            int LeftX = pointerDownX - (int) xPxielwidth;

            // If the rectange width falls outside the canvas area
            if(LeftX < 0)
              LeftX = 0;

            int TopY = pointerDownY - (int) yPixelHeight;

            // If the rectange height falls outside the canvas area
            if( TopY < 0 )
               TopY = 0;

            int DownY = pointerDownY + (int) yPixelHeight;

            // If the rectange heightfalls outside the canvas area.
            if( DownY > getHeight() )
              DownY = getHeight();

            // Find the PointerUp is within rectange
            if( (x >= LeftX ) && (x <= RightX) ) {
                if( (y >= TopY ) && (y <= DownY) ) {
                	return true;
                }
            }

    	  return false;
        }
    }

	private boolean invalidate(int x, int y, int width, int height) {
		// Regularize bounds
		final int x1 = x;
		final int y1 = y;
		final int x2 = x + width;
		final int y2 = y + height;

		// Union the current and new damaged rects
		final boolean valid = ((repaintX2 - repaintX1) <= 0) && ((repaintY2 - repaintY1) <= 0);
		if (!valid) {
			repaintX1 = Math.min(repaintX1, x1);
			repaintY1 = Math.min(repaintY1, y1);
			repaintX2 = Math.max(repaintX2, x2);
			repaintY2 = Math.max(repaintY2, y2);
		} else {
			repaintX1 = x1;
			repaintY1 = y1;
			repaintX2 = x2;
			repaintY2 = y2;
		}

		// UI thread can change the the contentArea object reference at
		// any time. Store the object reference locally to ensure it
		// points to the same rectangle all the time.
		Rectangle contentArea = getContentArea();
		if(contentArea == null) return valid;
		final int w = contentArea.width;
		final int h = contentArea.height;

		// Clip to bounds
		repaintX1 = repaintX1 > 0 ? repaintX1 : 0;
		repaintY1 = repaintY1 > 0 ? repaintY1 : 0;
		repaintX2 = repaintX2 < w ? repaintX2 : w;
		repaintY2 = repaintY2 < h ? repaintY2 : h;

		return valid;
	}

    class CanvasTimerTask extends TimerTask {

        public void run() {
        	   suppressDragEvent = false;
        }
    }
}
