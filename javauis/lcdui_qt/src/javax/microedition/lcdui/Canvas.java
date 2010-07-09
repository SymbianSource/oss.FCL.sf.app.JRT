/*
* Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
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
import java.util.Vector;
import java.util.Timer;
import java.util.TimerTask;
import javax.microedition.lcdui.game.GameCanvas;
import javax.microedition.lcdui.EventDispatcher.LCDUIEvent;
import org.eclipse.ercp.swt.mobile.MobileShell;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.*;
import org.eclipse.swt.widgets.*;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.graphics.*;
import org.eclipse.swt.internal.extension.CompositeExtension;
import org.eclipse.swt.internal.qt.graphics.WindowSurface;
import org.eclipse.swt.internal.extension.MobileShellExtension;
import org.eclipse.swt.internal.qt.SymbianWindowVisibilityListener;


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
public abstract class Canvas extends Displayable
{

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


    private static final int GAME_CANVAS = 1;
    private static final int NO_BACKGROUND = 1 << 1;
    private static final int FULLSCREEN_MODE = 1 << 2;

    private static final int DISABLE_TAPDETECTION = 1 << 3;
    private static final int SUPPRESS_GAMEKEYS = 1 << 4;
    private static final int SUPPRESS_DRAGEVENT = 1 << 5;
    private static final int CLEANUP_NEEDED = 1 << 6;
    private static final int REPAINT_PENDING = 1 << 7;
    private static final int SELECTIONKEY_COMPATIBILITY = 1 << 8;

    private static final int CURRENTLY_VISIBLE = 1 << 9;


    // Listeners for various events.
    private org.eclipse.swt.events.PaintListener paintListener =
        new CanvasShellPaintListener();

    private CanvasShellMouseListener mouseListener =
        new CanvasShellMouseListener();

    private CanvasShellVisibilityListener shellVisibilityListener =
        new CanvasShellVisibilityListener();

    // Canvas Graphics object passed to paint(Graphics g)
    private Graphics canvasGraphics;

    // Graphics object for transferring return values
    // from UI thread
    private Graphics tempGraphics;

    // Graphics command buffer for this instance
    Buffer graphicsBuffer;

    //On Screen Keypad
    //private Composite keypadComposite;
    private CanvasKeypad onScreenkeypad;
    private static CanvasKeypad sharedKeypad;
    private int oskHeight;

    // Vector of flags that a certain key was pressed but was not released.
    // Used to implement keyRepeated since eSWT does not support
    // key repeat events.
    private Vector keysPressed;
    private int gameKeyState;

    private static int objectCount;
    private static Shell sharedShell;
    private Shell mShell;
    private Composite canvasComp;
    private Label tickerLabel;

    private int mode;
    private Object modeLock;
    private Object cleanupLock;
    private Object repaintLock;
    private Object flushLock;

    private Timer timer = new Timer();
    private CanvasTimerTask timerTask;

    private static final int DEFAULT_TIMEOUT = 500;
    private static final int DEFAULT_TWIPS = 200;

    private int pointerDownX;
    private int pointerDownY;
    private int twips;
    private int timeout;

    private int repaintX1;
    private int repaintY1;
    private int repaintX2;
    private int repaintY2;


    /**
     * Constructs <code>Canvas</code> object.
     */
    public Canvas()
    {
        super(null);
        synchronized(this)
        {
            objectCount++;
        }

        modeLock = new Object();
        repaintLock = new Object();
        cleanupLock = new Object();
        flushLock = new Object();
        setMode(GAME_CANVAS, this instanceof GameCanvas);
        construct();
        keysPressed = new Vector();
    }

    /**
     * Disposes this instance
     * Called when finalizer is destroying this instance.
     */
    void dispose()
    {
        ESWTUIThreadRunner.update(getClass().getName(), -1);
        ESWTUIThreadRunner.safeSyncExec(new Runnable()
        {
            public void run()
            {
                if(graphicsBuffer != null)
                {
                    graphicsBuffer.dispose();
                    graphicsBuffer = null;
                }

                synchronized(this)
                {
                    objectCount--;

                    if((objectCount == 0) || isMode(GAME_CANVAS))
                    {
                        mShell.dispose();
                        sharedShell = null;
                        sharedKeypad = null;
                    }
                    else
                    {
                        Ticker ticker = getTicker();
                        if (ticker != null)
                        {
                            ticker.removeLabel(tickerLabel);
                        }
                        if(tickerLabel != null)
                        {
                            tickerLabel.dispose();
                        }

                        canvasComp.dispose();
                    }
                }
            }
        });
    }

    /* (non-Javadoc)
     * @see Displayable#eswtConstructShell(int)
     */
    Shell eswtConstructShell(int style)
    {
        if(isMode(GAME_CANVAS))
        {
            mShell = super.eswtConstructShell(style);
        }
        else
        {
            if(sharedShell == null)
            {
                sharedShell = super.eswtConstructShell(style);
            }
            mShell = sharedShell;
        }

        // Give the Shell the maximized size already before it becomes visible
        // so that it will return the correct size.
        mShell.setBounds(org.eclipse.swt.widgets.Display.getCurrent().getClientArea());

        // Make the Shell maximized. On Symbian it's automatically maximized
        // so this has no effect but on other platforms explicit maximizing
        // might be needed.
        mShell.setMaximized(true);

        return mShell;
    }

    /**
     * Sets ticker. If ticker is added already to other canvas(es),
     * it continues running from position where it was. Otherwise
     * it will start running from beginning when this method returns.
     *
     * @param newTicker New ticker. If null, current ticker is removed.
     */
    public void setTicker(Ticker newTicker)
    {
        super.setTicker(newTicker);

        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                tickerLabel.setVisible(isMode(CURRENTLY_VISIBLE));
            }
        });
    }

    /**
     * Creates singleton Label instance used by Ticker.
     * Creates tickerLabel on shell and sets the visibility of the same.
     */
    Label getTickerLabel()
    {
        tickerLabel = super.getTickerLabel();

        if (!isMode(CURRENTLY_VISIBLE))
        {
            ESWTUIThreadRunner.syncExec(new Runnable()
            {
                public void run()
                {
                    tickerLabel.setVisible(false);
                }
            });
        }

        return tickerLabel;
    }

    /* (non-Javadoc)
     * @see Displayable#eswtConstructContent(int)
     */
    Composite eswtConstructContent(int style)
    {
        // Get JAD attribute
        setMode(NO_BACKGROUND, !JadAttributeUtil.isValue(JadAttributeUtil.ATTRIB_NOKIA_UI_ENHANCEMENT,
                                                JadAttributeUtil.VALUE_CANVAS_HAS_BACKGROUND));
        if(isMode(NO_BACKGROUND))
        {
            style |= SWT.NO_BACKGROUND;
        }

        // Get JAD attribute for S60 Selection Key Compatibility
        setMode(SELECTIONKEY_COMPATIBILITY, JadAttributeUtil.isValue(JadAttributeUtil.ATTRIB_NOKIA_MIDLET_S60_SELECTION_KEY_COMPATIBILITY,
                                    JadAttributeUtil.VALUE_TRUE));

        // Get JAD attribute for MIDlet Tap Detection
        String tapAttr = JadAttributeUtil.getValue(JadAttributeUtil.ATTRIB_NOKIA_MIDLET_TAP_DETECTION_OPTIONS);
        if(tapAttr != null)
        {
            if(tapAttr.indexOf(',') == -1)
            {
                setDefaultTapValues();
            }
            else
            {
                String twipString = tapAttr.substring(0, tapAttr.indexOf(',')).trim();
                String timeoutString = tapAttr.substring(tapAttr.indexOf(',') + 1, tapAttr.length()).trim();

                try
                {
                    twips = Integer.parseInt(twipString);
                    timeout = Integer.parseInt(timeoutString);

                    // Check for Negative Values
                    if((twips < 0) && (timeout < 0))
                    {
                        setDefaultTapValues();
                    }

                    if((twips == 0)  && (timeout == 0))
                    {
                        setMode(DISABLE_TAPDETECTION, true);
                    }

                    // if any one of the value is zero, set defaults
                    if(!((twips != 0) && (timeout != 0)))
                    {
                        setDefaultTapValues();
                    }
                }
                catch(NumberFormatException e)
                {
                    // Alpha Numeric Values of Timeouts and Timeout
                    setDefaultTapValues();
                }
            }
        }
        else
        {
            setDefaultTapValues();
        }

        canvasComp = super.eswtConstructContent(style);
        canvasComp.setVisible(false);

        createOnScreenKeypad();
        return canvasComp;
    }

    void eswtInitGraphics() {
        // create graphics buffer
       graphicsBuffer = Buffer.createInstance(this, canvasComp);
    }

    /**
     * Creates OSK(OnScreenKeypad), shared Keypad will be created for Canvas,
     * seperate OSK will be created for each GameCanvas.
     */
    CanvasKeypad createOnScreenKeypad()
    {
        // Read the on screen keypad settings from the jad attribute
        String oskAttr = JadAttributeUtil
                         .getValue(JadAttributeUtil.ATTRIB_NOKIA_MIDLET_ON_SCREEN_KEYPAD);
        if(oskAttr != null
                && (!oskAttr.equalsIgnoreCase(JadAttributeUtil.VALUE_NO))
                && ((oskAttr
                     .equalsIgnoreCase(JadAttributeUtil.VALUE_GAMEACTIONS)) || (oskAttr
                             .equalsIgnoreCase(JadAttributeUtil.VALUE_NAVIGATIONKEYS))))
        {
            // On screen keypad is required, On devices without keyboard it can
            // be either navigation keys or navigation and game keys

            if(isMode(GAME_CANVAS))
            {
                onScreenkeypad = new CanvasKeypad(this, oskAttr);
                return onScreenkeypad;
            }

            if(sharedKeypad == null)
            {
                sharedKeypad = new CanvasKeypad(this, oskAttr);
            }
            onScreenkeypad = sharedKeypad;
            return onScreenkeypad;
        }

        return null;
    }

    Rectangle eswtLayoutShellContent()
    {
        Rectangle shellArea = mShell.getClientArea();
        int oskHeight = (onScreenkeypad != null ? onScreenkeypad.getHeight() : 0);
        int tickerHeight = (tickerLabel != null ? tickerLabel.getBounds().height : 0);

        canvasComp.setBounds(0, tickerHeight,
                              shellArea.width, shellArea.height - tickerHeight - oskHeight);

        canvasComp.setFocus();
        return canvasComp.getClientArea();
    }

    /* (non-Javadoc)
     * @see Displayable#eswtHandleShowCurrentEvent()
     */
    void eswtHandleShowCurrentEvent()
    {
        setMode(CURRENTLY_VISIBLE, true);
        eswtSetTitle();
        ((MobileShell) mShell).setFullScreenMode(isMode(FULLSCREEN_MODE));
        if(onScreenkeypad != null)
        {
            if(!isMode(GAME_CANVAS))
            {
                onScreenkeypad.setCurrentCanvas(this);
            }
            onScreenkeypad.setFullScreenMode(isMode(FULLSCREEN_MODE));
        }
        canvasComp.setVisible(true);
        if(tickerLabel != null)
        {
            tickerLabel.setVisible(!isMode(FULLSCREEN_MODE));
        }
        addCommands();
        super.eswtHandleShowCurrentEvent();
        getContentComp().addPaintListener(paintListener);
        getContentComp().addMouseListener(mouseListener);
        getContentComp().addMouseMoveListener(mouseListener);
        ((MobileShellExtension)getShell()).addSymbianWindowVisibilityListener(shellVisibilityListener);
    }

    /* (non-Javadoc)
     * @see Displayable#eswtHandleHideCurrentEvent()
     */
    void eswtHandleHideCurrentEvent()
    {
        setMode(CURRENTLY_VISIBLE, false);
        canvasComp.setVisible(false);
        if(tickerLabel != null)
        {
            tickerLabel.setVisible(false);
        }
        removeCommands();
        super.eswtHandleHideCurrentEvent();
        getContentComp().removePaintListener(paintListener);
        getContentComp().removeMouseListener(mouseListener);
        getContentComp().removeMouseMoveListener(mouseListener);
        ((MobileShellExtension)getShell()).removeSymbianWindowVisibilityListener(shellVisibilityListener);
    }

    /**
     * eSWT callback to add a Command.
     */
    void eswtAddCommand(Command cmd) {
        if (isMode(CURRENTLY_VISIBLE)) {
            cmd.eswtAddESWTCommand(mShell, false);
        }
        if (eswtIsShown()) {
            cmd.eswtAddCommandSelectionListener(mShell, getCommandListener());
        }
    }

    /**
     * Adds the commands to this Canvas.
     * Adds all the commands of displayable to the shell.
     */
    void addCommands()
    {
        Command cmd = null;
        for (Enumeration e = getCommands().elements(); e.hasMoreElements();)
        {
            cmd = (Command) e.nextElement();
            final Command finalCommand = cmd;
            finalCommand.eswtAddESWTCommand(mShell, false);
        }
    }

    /**
     * Removes the commands from this Canvas.
     * Removes all the commands of displayable from the shell.
     */
    void removeCommands()
    {
        Command cmd = null;
        for (Enumeration e = getCommands().elements(); e.hasMoreElements();)
        {
            cmd = (Command) e.nextElement();
            final Command finalCommand = cmd;
            finalCommand.eswtRemoveESWTCommand(mShell);
        }
    }

    /**
     * Issues the request to repaint the whole Canvas.
     */
    public void repaint()
    {
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
    public void repaint(int x, int y, int width, int height)
    {
        // Paint callback event is posted without any invalid area info.
        // Invalid area info is kept in the member variables. Only one event
        // per Canvas is added to the queue. If there are more repaint() calls
        // before the already posted event has been served those are merged
        // to the invalid area in the member variables without posting a new
        // event.
        synchronized(repaintLock)
        {
            if(invalidate(x, y, width, height))
            {
                // Note that repaintPending doesn't mean that there's a repaint
                // event in the queue. It means that the invalid area is going
                // to be repainted and there's no need to add a new event.
                // It's possible that the repaint event has already been
                // removed from the queue but repaintPending is still true. In
                // that case it's currently being processed and we can still
                // add to the invalid area.
                if(!isMode(REPAINT_PENDING))
                {
                    EventDispatcher eventDispatcher = EventDispatcher.instance();
                    LCDUIEvent event = eventDispatcher.newEvent(
                                           LCDUIEvent.CANVAS_PAINT_MIDLET_REQUEST, this);
                    event.widget = getContentComp();
                    eventDispatcher.postEvent(event);
                    setMode(REPAINT_PENDING, true);
                }
            }
        }
    }

    /**
     * Switches the Canvas between full screen and non-full screen modes.
     *
     * @param mode - true switches the Canvas to the full-screen mode.
     */
    public void setFullScreenMode(boolean aMode)
    {
        setMode(FULLSCREEN_MODE, aMode);
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                if(tickerLabel != null)
                {
                    if(isMode(FULLSCREEN_MODE))
                    {
                        tickerLabel.setBounds(Integer.MIN_VALUE, 0, 0, 0);
                    }
                    else
                    {
                        tickerLabel.pack();
                        tickerLabel.setLocation(Integer.MIN_VALUE, 0);
                    }

                    if(isMode(CURRENTLY_VISIBLE))
                    {
                        tickerLabel.setVisible(!isMode(FULLSCREEN_MODE));
                    }
                }

                if(isMode(CURRENTLY_VISIBLE))
                {
                    ((MobileShell)mShell).setFullScreenMode(isMode(FULLSCREEN_MODE));
                    //set the CanvasKeypad to the required mode
                    if(onScreenkeypad != null)
                    {
                        onScreenkeypad.setFullScreenMode(isMode(FULLSCREEN_MODE));
                    }
                }
            }
        });
    }

    /**
     * Processes all the issued paint requests.
     */
    public final void serviceRepaints()
    {
        EventDispatcher.instance().serviceRepaints(this);
    }

    /**
     * Checks if the Canvas supports pointer dragging events.
     *
     * @return true if the Canvas supports pointer dragging, false otherwise.
     */
    public boolean hasPointerMotionEvents()
    {
        return true;
    }

    /**
     * Checks if the Canvas supports pointer events.
     *
     * @return true if the Canvas supports pointer events, false otherwise.
     */
    public boolean hasPointerEvents()
    {
        return true;
    }

    /**
     * Is the Canvas double buffered.
     *
     * @return true if the Canvas is double buffered, false otherwise.
     */
    public boolean isDoubleBuffered()
    {
        return true;
    }

    /**
     * Queries if the Canvas supports key repeat events.
     *
     * @return true if Canvas supports key repeat events, false otherwise.
     */
    public boolean hasRepeatEvents()
    {
        return true;
    }

    /**
     * Returns game action for a specified key code.
     *
     * @param keyCode Key code to map to game action.
     * @return Game action for a a specified key code or
     *         IllegalArgumentException.
     */
    public int getGameAction(int keyCode)
    {
        return KeyTable.getGameAction(keyCode);
    }

    /**
     * Returns the key code specific for a certain game action.
     *
     * @param gameAction - game action to be mapped to the key code.
     * @return Key code that is mapped to the specified game action.
     */
    public int getKeyCode(int gameAction)
    {
        return KeyTable.getKeyCode(gameAction);
    }

    /**
     * Returns the key name specific for a certain key code.
     *
     * @param keyCode - key name to get the name of.
     * @return String that contains textual name of the key specified by the key
     *         code.
     */
    public String getKeyName(int keyCode)
    {
        return KeyTable.getKeyName(keyCode);
    }
    /**
     * Gets height.
     *
     * @return Height of the Displayable in pixels.
     */
    public int getHeight()
    {
        if(onScreenkeypad != null)
        {
            ESWTUIThreadRunner.syncExec(new Runnable()
            {
                public void run()
                {
                    oskHeight = onScreenkeypad.getHeight();
                }
            });
        }
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
    protected void keyPressed(int keyCode)
    {
    }

    /**
     * Callback to signal a key release.
     *
     * @param keyCode - key code of the key released.
     */
    protected void keyReleased(int keyCode)
    {
    }

    /**
     * Callback to signal a key repeat. Happens when the key is pressed down for
     * a long time.
     *
     * @param keyCode - key code of the key repeated.
     */
    protected void keyRepeated(int keyCode)
    {
    }

    /**
     * Callback to signal pointer press event.
     *
     * @param x - X-coordinate of the tap point.
     * @param y - Y-coordinate of the tap point.
     */
    protected void pointerPressed(int x, int y)
    {
    }

    /**
     * Callback to signal pointer release event.
     *
     * @param x - X-coordinate of the pointer release point.
     * @param y - Y-coordinate of the pointer release point.
     */
    protected void pointerReleased(int x, int y)
    {
    }

    /**
     * Callback to signal pointer drag event.
     *
     * @param x - X-coordinate of the pointer drag point.
     * @param y - Y-coordinate of the pointer drag point.
     */
    protected void pointerDragged(int x, int y)
    {
    }

    /**
     * Callback similar to the one in <code>Displayable</code>. Additional
     * case for <code>sizeChanged</code> to be called in <code>Canvas</code>
     * is switching between full-screen mode by calling
     * <code>setFullScreenMode()</code>.
     */
    protected void sizeChanged(int w, int h)
    {
    }

    /**
     * showNotify() is the callback called before the Canvas is shown on the
     * screen.
     */
    protected void showNotify()
    {
    }

    /**
     * hideNotify() is the callback called after the Canvas is removed from the
     * screen.
     */
    protected void hideNotify()
    {
    }

    /**
     * Init GameCanvas frame buffer.
     *
     * @param supressKeys suppress game keys
     * @return frame buffer
     */
    final void initGameCanvas(boolean suppressKeys)
    {
        setMode(SUPPRESS_GAMEKEYS, suppressKeys);
    }

    /**
     * Gets composite that contains Canvas content.
     *
     * @return Composite.
     */
    Composite getContentComp()
    {
        return canvasComp;
    }

    /**
     * Get game canvas frame buffer graphics.
     */
    final Graphics getGameBufferGraphics()
    {
        tempGraphics = null;
        ESWTUIThreadRunner.safeSyncExec(new Runnable()
        {
            public void run()
            {
                tempGraphics =  graphicsBuffer.getGraphics();
            }
        });
        return tempGraphics;
    }

    CanvasKeypad getCanvasKeypad()
    {
        return onScreenkeypad;
    }

    boolean IsFullScreenMode()
    {
        return isMode(FULLSCREEN_MODE);
    }

    /**
     * Return game key states for GameCanvas.
     *
     * @return game key states.
     */
    final int getGameKeyStates()
    {
        int ret = gameKeyState;
        gameKeyState = 0;
        return ret;
    }

    void renderGraphics(final Graphics g)
    {
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
                         final int height)
    {
        // This is serialized with the
        // paint callback processing
        synchronized(flushLock)
        {
            synchronized(graphicsBuffer)
            {
                 ESWTUIThreadRunner.safeSyncExec(new Runnable()
                {
                    public void run()
                    {
                        graphicsBuffer.sync();
                        graphicsBuffer.blitToDisplay(null, getContentComp());
                    }
                });
            }
        }
    }

    /**
     * Called by ShellListener when shell gets activated.
     */
    void handleShellActivatedEvent()
    {
        super.handleShellActivatedEvent();

        // reset the game key state
        gameKeyState = 0;

        synchronized(cleanupLock)
        {
            setMode(CLEANUP_NEEDED, true);
        }

        LCDUIEvent event = EventDispatcher.instance().newEvent(LCDUIEvent.CANVAS_SHOWNOTIFY, this);
        EventDispatcher.instance().postEvent(event);
    }

    /**
     * Called by ShellListener when shell gets de-activated.
     */
    void handleShellDeActivatedEvent()
    {
        super.handleShellDeActivatedEvent();
        LCDUIEvent event = EventDispatcher.instance().newEvent(LCDUIEvent.CANVAS_HIDENOTIFY, this);
        EventDispatcher.instance().postEvent(event);
    }

    /*
     * UI thread calls
     */
    void eswtHandleResizeEvent(int width, int height)
    {
        super.eswtHandleResizeEvent(width, height);
        // update new bounds to graphicsBuffer
        // this call must not be synchronized as we
        // cannot use locking in UI thread
        graphicsBuffer.setControlBounds(getContentComp());
        synchronized(cleanupLock)
        {
            setMode(CLEANUP_NEEDED, true);
        }
    }

    /*
     * UI thread calls
     */
    void eswtHandleEvent(Event e)
    {
        super.eswtHandleEvent(e);
        if(e.type == SWT.KeyDown)
        {
            doKeyPressed(e.keyCode);
        }
        else if(e.type == SWT.KeyUp)
        {
            doKeyReleased(e.keyCode);
        }
    }

    private void setMode(final int aMode, boolean value)
    {
        synchronized(modeLock)
        {
            if(value)
            {
                mode |= aMode;
            }
            else
            {
                mode &= ~aMode;
            }
        }
    }

    private boolean isMode(final int aMode)
    {
        return ((mode & aMode) != 0);
    }

    /*
     * UI thread calls. Paint listener of the eSWT widget.
     */
    class CanvasShellPaintListener implements PaintListener
    {
        public void paintControl(PaintEvent pe)
        {
            // Check if we got here from buffer flush
            if(graphicsBuffer.isPaintingActive())
            {
                graphicsBuffer.blitToDisplay(pe.gc.getGCData().internalGc, null);
            }
            else
            {
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
            }
        }
    }

    /*
     * Dispatcher thread or the serviceRepaints()-thread calls.
     */
    final void doCallback(LCDUIEvent event)
    {
        switch(event.type)
        {
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
    private final void doPaintCallback(final LCDUIEvent event)
    {
        synchronized(flushLock)
        {
            // It's possible that this Canvas is sent to background
            // right after the visibility is checked here, however
            // it is okay as in such case we just do one extra paint
            // callback. The visibility change cannot be synchronized with
            // this method, since it would expose implementation to deadlock
            if(!isMode(CURRENTLY_VISIBLE))
            {
                return;
            }

            // Decide the area going to be painted by the callback.
            final int redrawNowX;
            final int redrawNowY;
            final int redrawNowW;
            final int redrawNowH;
            // Before this thread obtains the repaintLock any repaint() calls
            // will still be adding to the invalid area that is going to be
            // painted by this callback.
            synchronized(repaintLock)
            {
                if(event.type == LCDUIEvent.CANVAS_PAINT_NATIVE_REQUEST)
                {
                    // Merge with possibly existing repaint() requests
                    invalidate(event.x, event.y, event.width, event.height);
                }
                else
                {
                    // Need to add a new event to the queue in subsequent repaint()
                    // calls.
                    setMode(REPAINT_PENDING, false);
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
                if(!((redrawNowW > 0) && (redrawNowH > 0)))
                {
                    return;
                }
            }

            // Create instance of Graphics if not created yet
            if(canvasGraphics == null)
            {
                canvasGraphics = graphicsBuffer.getGraphics();
                canvasGraphics.setSyncStrategy(Graphics.SYNC_LEAVE_SURFACE_SESSION_OPEN);
            }

            // Clean the background if dirty, buffer the operations.
            synchronized(cleanupLock)
            {
                if(isMode(CLEANUP_NEEDED) && isMode(NO_BACKGROUND))
                {
                    // UI thread can change the contentArea object reference at
                    // any time. Store the object reference locally to ensure it
                    // points to the same rectangle all the time.
                    Rectangle contentArea = getContentArea();

                    canvasGraphics.setClip(contentArea.x, contentArea.y,
                                           contentArea.width, contentArea.height);
                    canvasGraphics.cleanBackground(contentArea);
                    setMode(CLEANUP_NEEDED, false);
                }
            }

            // Clip must define the invalid area
            canvasGraphics.reset();
            canvasGraphics.setClip(redrawNowX, redrawNowY, redrawNowW, redrawNowH);

            // The callback
            paint(canvasGraphics);

            // Blit frame to display
            synchronized(graphicsBuffer)
            {
                ESWTUIThreadRunner.safeSyncExec(new Runnable()
                {
                     public void run()
                    {
                        if(event.widget.isDisposed())
                        {
                            return;
                        }
                        graphicsBuffer.sync();
                        graphicsBuffer.blitToDisplay(null, event.widget);
                    }
                });
            }
        }
    }

    /*
     * UI thread calls to flush the command buffer of a graphics context.
     */
    private final void doBufferFlush(PaintEvent event, Graphics graphics)
    {
        //  event.gc.getGCData().internalGc.render(graphics.getCommandBuffer());
    }

    /*
     * UI thread calls.
     */
    void doKeyPressed(int keyCode)
    {
        Logger.method(this, "doKeyPressed", String.valueOf(keyCode));
        boolean sendCallback = false;

        if(!(updateGameKeyState(keyCode, true) && isMode(SUPPRESS_GAMEKEYS)))
        {
            if(isMode(SELECTIONKEY_COMPATIBILITY) && (keyCode == -5))
            {
                if(isMode(FULLSCREEN_MODE))
                {
                    if(!((getNumCommands() > 0) && hasCommandListener()))
                    {
                        sendCallback = true;
                    }
                    else
                    {
                        sendCallback = false;
                    }
                }
                else
                {
                    sendCallback = true;
                }
            }
            else if((!isMode(SELECTIONKEY_COMPATIBILITY)) && (keyCode == -5))
            {
                sendCallback = false;
            }
            else
            {
                sendCallback = true;
            }
        }
        else
        {
            sendCallback = false;
        }

        if(sendCallback == true)
        {
            LCDUIEvent event;

            if(keysPressed.contains(new Integer(keyCode)))
            {
                event = EventDispatcher.instance().newEvent(LCDUIEvent.CANVAS_KEYREPEATED, this);
            }
            else
            {
                event = EventDispatcher.instance().newEvent(LCDUIEvent.CANVAS_KEYPRESSED, this);
            }
            event.keyCode = keyCode;
            EventDispatcher.instance().postEvent(event);

        }
    }

    /*
     * UI thread calls.
     */
    void doKeyReleased(int keyCode)
    {
        Logger.method(this, "doKeyReleased", String.valueOf(keyCode));
        boolean sendCallback = false;
        if(!(updateGameKeyState(keyCode, true) && isMode(SUPPRESS_GAMEKEYS)))
        {
            if(isMode(SELECTIONKEY_COMPATIBILITY) && (keyCode == -5))
            {
                if(isMode(FULLSCREEN_MODE))
                {
                    if(!((getNumCommands() > 0) && hasCommandListener()))
                    {
                        sendCallback = true;
                    }
                    else
                    {
                        sendCallback = false;
                    }
                }
                else
                {
                    sendCallback = true;
                }
            }
            else if((!isMode(SELECTIONKEY_COMPATIBILITY)) && (keyCode == -5))
            {
                sendCallback = false;
            }
            else
            {
                sendCallback = true;
            }
        }
        else
        {
            sendCallback = false;
        }

        if(sendCallback == true)
        {

            LCDUIEvent event = EventDispatcher.instance().newEvent(LCDUIEvent.CANVAS_KEYRELEASED, this);
            event.keyCode = keyCode;
            EventDispatcher.instance().postEvent(event);

        }
    }

    /**
     * Updates game key states and returns if the key was a game key.
     */
    private boolean updateGameKeyState(int keyCode, boolean addKeyState)
    {
        // Ignore key repeat events
        if(ESWTUIThreadRunner.getKeyRepeatCount() > 1)
        {
            return true;
        }
        try
        {
            int gameAction = KeyTable.getGameAction(keyCode);
            if(addKeyState)
            {
                // set bitfield
                gameKeyState |= (1 << gameAction);
            }
            return true;
        }
        catch(IllegalArgumentException iae)
        {
            return false;
        }
    }

    private void setDefaultTapValues()
    {
        twips = DEFAULT_TWIPS;
        timeout = DEFAULT_TIMEOUT;
    }

    class CanvasShellMouseListener implements
        org.eclipse.swt.events.MouseListener,
        org.eclipse.swt.events.MouseMoveListener
    {

        public void mouseDoubleClick(MouseEvent arg0)
        {
        }

        public void mouseDown(MouseEvent event)
        {
            LCDUIEvent e = EventDispatcher.instance().newEvent(LCDUIEvent.CANVAS_POINTERPRESSED,
                           javax.microedition.lcdui.Canvas.this);
            e.x = event.x;
            e.y = event.y;
            EventDispatcher.instance().postEvent(e);

            if(!isMode(DISABLE_TAPDETECTION))
            {
                // Supress Drag events
                setMode(SUPPRESS_DRAGEVENT, true);

                pointerDownX = event.x;
                pointerDownY = event.y;

                // Create and Schedule Timer
                timerTask = new CanvasTimerTask();
                timer.schedule(timerTask, timeout);
            }
        }

        public void mouseUp(MouseEvent event)
        {
            int pointerUpX = event.x;
            int pointerUpY = event.y;

            if(!isMode(DISABLE_TAPDETECTION))
            {
                if(timerTask != null)
                {
                    timerTask.cancel();
                    timerTask = null;
                }

                // If Timer not expired and Mouseup is withing rectangle assign
                // PointercDown to Pinter Up
                if(isMode(SUPPRESS_DRAGEVENT) && checkWithinRect(event.x, event.y))
                {
                    pointerUpX = pointerDownX;
                    pointerUpY = pointerDownY;
                    setMode(SUPPRESS_DRAGEVENT, false);
                }
            }

            LCDUIEvent e = EventDispatcher.instance().newEvent(LCDUIEvent.CANVAS_POINTERRELEASED,
                           javax.microedition.lcdui.Canvas.this);
            e.x = pointerUpX;
            e.y = pointerUpY;
            EventDispatcher.instance().postEvent(e);
        }

        public void mouseMove(MouseEvent event)
        {
            // Check for timeout expiration and if PointerUp falls outside the rectangle
            if(isMode(DISABLE_TAPDETECTION) || (!isMode(SUPPRESS_DRAGEVENT)) || !checkWithinRect(event.x, event.y))
            {
                LCDUIEvent e = EventDispatcher.instance().newEvent(LCDUIEvent.CANVAS_POINTERDRAGGED,
                               javax.microedition.lcdui.Canvas.this);
                e.x = event.x;
                e.y = event.y;
                EventDispatcher.instance().postEvent(e);
            }
        }

        boolean checkWithinRect(int x, int y)
        {
            // Get pixel per inch
            Point P = Display.getCurrent().getDPI();

            float xPxielwidth  = (twips * P.x) / 1440;
            float yPixelHeight = (twips * P.y) / 1440;

            int RightX = pointerDownX + (int) xPxielwidth;

            // If the rectange width falls outside the canvas area
            if(RightX > getWidth())
            {
                RightX = getWidth();
            }

            int LeftX = pointerDownX - (int) xPxielwidth;

            // If the rectange width falls outside the canvas area
            if(LeftX < 0)
                LeftX = 0;

            int TopY = pointerDownY - (int) yPixelHeight;

            // If the rectange height falls outside the canvas area
            if(TopY < 0)
                TopY = 0;

            int DownY = pointerDownY + (int) yPixelHeight;

            // If the rectange heightfalls outside the canvas area.
            if(DownY > getHeight())
                DownY = getHeight();

            // Find the PointerUp is within rectange
            if((x >= LeftX) && (x <= RightX))
            {
                if((y >= TopY) && (y <= DownY))
                {
                    return true;
                }
            }

            return false;
        }
    }

    private boolean invalidate(int x, int y, int width, int height)
    {
        // Regularize bounds
        final int x1 = x;
        final int y1 = y;
        final int x2 = x + width;
        final int y2 = y + height;

        // Union the current and new damaged rects
        final boolean valid = ((repaintX2 - repaintX1) <= 0) && ((repaintY2 - repaintY1) <= 0);
        if(!valid)
        {
            repaintX1 = Math.min(repaintX1, x1);
            repaintY1 = Math.min(repaintY1, y1);
            repaintX2 = Math.max(repaintX2, x2);
            repaintY2 = Math.max(repaintY2, y2);
        }
        else
        {
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

    class CanvasTimerTask extends TimerTask
    {

        public void run()
        {
            setMode(SUPPRESS_DRAGEVENT, false);
        }
    }

    class CanvasShellVisibilityListener implements SymbianWindowVisibilityListener
    {
        public void handleSymbianWindowVisibilityChange(Widget widget, boolean visible) {
            if (javax.microedition.lcdui.Canvas.this.getShell() == widget)
            {
                graphicsBuffer.getWindowSurface().handleSymbianWindowVisibilityChange(visible);
            }
        }
    }
}
