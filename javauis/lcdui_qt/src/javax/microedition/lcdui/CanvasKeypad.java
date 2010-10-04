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

import java.util.Timer;
import java.util.TimerTask;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.*;
import org.eclipse.swt.widgets.*;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.graphics.*;
import org.eclipse.swt.internal.extension.ImageUtil;
import org.eclipse.swt.internal.qt.*;

import org.eclipse.swt.layout.FormAttachment;
import org.eclipse.swt.layout.FormData;
import org.eclipse.swt.layout.FormLayout;

/**
 * The CanvasKeypad class is designed to generate the on screen keypad and
 * handle graphical operations as well as low-level pointer events. CanvasKeypad
 * maps the pointer events to key events that are needed by the Canvas.
 * 
 */
final class CanvasKeypad implements MouseListener, MouseMoveListener
{
    /**
     * Constant for <code>UP</code> game action.
     */
    private static final int UP = -1;

    /**
     * Constant for <code>DOWN</code> game action.
     */
    private static final int DOWN = -2;

    /**
     * Constant for <code>LEFT</code> game action.
     */
    private static final int LEFT = -3;

    /**
     * Constant for <code>RIGHT</code> game action.
     */
    private static final int RIGHT = -4;

    /**
     * Constant for <code>FIRE</code> game action.
     */
    private static final int FIRE = -5;

    /**
     * Constant for <code>SLK</code> game action.
     */
    private static final int SLK = -6;

    /**
     * Constant for <code>SRK</code> game action.
     */
    private static final int SRK = -7;

    /**
     * Constant for general "<code>A</code>" game action.
     */
    private static final int GAME_A = 55;

    /**
     * Constant for general "<code>B</code>" game action.
     */
    private static final int GAME_B = 57;

    /**
     * Constant for general "<code>C</code>" game action.
     */
    private static final int GAME_C = 42;

    /**
     * Constant for general "<code>D</code>" game action.
     */
    private static final int GAME_D = 35;

    /**
     * Constant for general invalid key code.
     */
    private static final int INVALID_CODE = -99;

    /**
     * Constant for long keypress timeout.
     */
    private static final int KEYREPEAT_INTERVAL = 500;

    /**
     * Alpha value of a transparent pixel.
     */
    private static final int TRANSPARENT = 0;

    /**
     * Keypad and game keys composites.
     */
    private Composite keypadComposite;
    private Composite gameKeysComposite;
    private Canvas canvas;

    /**
     * CSS engine instance
     */
    private BaseCSSEngine cssEngine;

    /**
     * Canvas keypad buttons.
     */
    private Label center;
    private Label up;
    private Label down;
    private Label left;
    private Label right;

    private org.eclipse.swt.graphics.Image center_normal;
    private org.eclipse.swt.graphics.Image center_pressed;
    private org.eclipse.swt.graphics.Image left_normal;
    private org.eclipse.swt.graphics.Image left_pressed;
    private org.eclipse.swt.graphics.Image right_normal;
    private org.eclipse.swt.graphics.Image right_pressed;
    private org.eclipse.swt.graphics.Image up_normal;
    private org.eclipse.swt.graphics.Image up_pressed;
    private org.eclipse.swt.graphics.Image down_normal;
    private org.eclipse.swt.graphics.Image down_pressed;

    private ImageData center_data;
    private ImageData left_data;
    private ImageData right_data;
    private ImageData up_data;
    private ImageData down_data;

    private ImageData pressedKeyData;

    private Button skright;
    private Button skleft;
    private Button gameA;
    private Button gameB;
    private Button gameC;
    private Button gameD;

    private Display display;

    private int keyPressed;
    private Label rockerKeyPressed;

    /**
     * Timer for handling long key press and mouse move events.
     */
    private Timer timer = new Timer();
    private CanvasKeypadTimerTask timerTask;

    /**
     * Path of the osk graphics
     */
    private static final String RESOURCE_PATH = "c:/data/images/";

    /**
     * Offset on the composite in pixels where the osk rocker keys are layed
     * out.
     */
    private int image_offset;

    /**
     * Constructs a canvas kepad based on the JAD attribute
     * 
     * @param canvas
     *            - canvas for which the keypad is required.
     * @param mode
     *            - the jad attribute as a string.
     */
    CanvasKeypad(Canvas canvas, String mode)
    {

        this.canvas = canvas;
        display = ESWTUIThreadRunner.getInstance().getDisplay();

        cssEngine = new BaseCSSEngine(display);

        // Load the images
        center_normal = ImageUtil.createImageFromTheme(display, "qtg_fr_rocker_normal_c");
        center_pressed = ImageUtil.createImageFromTheme(display, "qtg_fr_rocker_pressed_c");
        left_normal = ImageUtil.createImageFromTheme(display, "qtg_fr_rocker_normal_l");
        left_pressed = ImageUtil.createImageFromTheme(display, "qtg_fr_rocker_pressed_l");
        right_normal = ImageUtil.createImageFromTheme(display, "qtg_fr_rocker_normal_r");
        right_pressed = ImageUtil.createImageFromTheme(display, "qtg_fr_rocker_pressed_r");
        up_normal = ImageUtil.createImageFromTheme(display, "qtg_fr_rocker_normal_t");
        up_pressed = ImageUtil.createImageFromTheme(display, "qtg_fr_rocker_pressed_t");
        down_normal = ImageUtil.createImageFromTheme(display, "qtg_fr_rocker_normal_b");
        down_pressed = ImageUtil.createImageFromTheme(display, "qtg_fr_rocker_pressed_b");

        image_offset = (display.getBounds().width / 2) - (center_normal.getBounds().width / 2);

        // Load image data required to determine the osk button pressed on mouse events
        center_data = center_normal.getImageData();
        left_data = left_normal.getImageData();
        right_data = right_normal.getImageData();
        up_data = up_normal.getImageData();
        down_data = down_normal.getImageData();

        // Set form layout for the shell containing the canvas.
        canvas.getShell().setLayout(new FormLayout());

        // Create the Navigation keypad
        createNavKeypad();

        // Check if game keys are needed
        if (mode.equalsIgnoreCase("gameactions"))
        {
            createGamePad();
        }

        // Layout the game keys, navigation keys and the canvas composites on
        // the shell using form layout
        FormData canvasData = new FormData();
        canvasData.right = new FormAttachment(100);
        canvasData.left = new FormAttachment(0);
        canvasData.top = new FormAttachment(0);
        canvasData.bottom = new FormAttachment(keypadComposite);
        canvas.getContentComp().setLayoutData(canvasData);

        FormData navKeyData = new FormData();
        navKeyData.right = new FormAttachment(100);
        navKeyData.left = new FormAttachment(0);

        if (gameKeysComposite != null)
        {
            // Game keypad was created above, so layout the game keys.
            navKeyData.bottom = new FormAttachment(gameKeysComposite);
        }
        else
        {
            // Game keys are not needed
            navKeyData.bottom = new FormAttachment(100);
        }

        keypadComposite.setLayoutData(navKeyData);

        if (gameKeysComposite != null)
        {
            // Game keys were created, layout the game keys
            FormData gameKeyData = new FormData();
            gameKeyData.right = new FormAttachment(100);
            gameKeyData.left = new FormAttachment(0);
            gameKeyData.bottom = new FormAttachment(100);
            gameKeysComposite.setLayoutData(gameKeyData);
        }

        canvas.getShell().layout();
    }

    /**
     * creates the navigation keypad - the directional keys and selctio key.
     */
    private void createNavKeypad()
    {
        // Create the navigation key composite
        keypadComposite = new Composite(canvas.getShell(), SWT.NONE);

        center = new Label(keypadComposite, SWT.NONE);
        left = new Label(keypadComposite, SWT.NONE);
        right = new Label(keypadComposite, SWT.NONE);
        up = new Label(keypadComposite, SWT.NONE);
        down = new Label(keypadComposite, SWT.NONE);

        center.setImage(center_normal);
        center.pack();
        left.setImage(left_normal);
        left.pack();
        right.setImage(right_normal);
        right.pack();
        up.setImage(up_normal);
        up.pack();
        down.setImage(down_normal);
        down.pack();

        // layout the navigation keys
        keypadComposite.setLayout(new FormLayout());

        FormData navkeysFormData = new FormData();
        navkeysFormData.top = new FormAttachment(0);
        navkeysFormData.left = new FormAttachment(0, image_offset);
        center.setLayoutData(navkeysFormData);
        left.setLayoutData(navkeysFormData);
        right.setLayoutData(navkeysFormData);
        up.setLayoutData(navkeysFormData);
        down.setLayoutData(navkeysFormData);

        keypadComposite.pack(); // will also layout

        // The down Label will catch all mouse down events as it is covering
        // all other rocker Labels, with the assumption that they all have 
        // the same bounds!
        down.addMouseListener(this);
        down.addMouseMoveListener(this);
        
        // We want to catch all drag events occurring on top of the rocker 
        // including those starting outside. 
        keypadComposite.addMouseListener(this);
        keypadComposite.addMouseMoveListener(this);
    }

    /**
     * creates the game keypad - the game action keys A, B, C and D.
     */
    private void createGamePad()
    {
        // create the game pad composite
        gameKeysComposite = new Composite(canvas.getShell(), SWT.NONE);
        gameKeysComposite.setLayout(new FormLayout());

        // Add the game buttons
        gameA = new Button(gameKeysComposite, SWT.PUSH);
        gameA.addMouseListener(this);

        gameB = new Button(gameKeysComposite, SWT.PUSH);
        gameB.addMouseListener(this);

        gameC = new Button(gameKeysComposite, SWT.PUSH);
        gameC.addMouseListener(this);

        gameD = new Button(gameKeysComposite, SWT.PUSH);
        gameD.addMouseListener(this);

        // Set the game key style sheets
        cssEngine.applyCSS(gameA, "QPushButton" + "{" + "background-color:white;"
            + "border-style: outset;" + "border-width: 1px; " + "border-color: black;"
            + "border-radius: 2px;" + "font: bold 14px;" + "min-height: 2em;" + "min-width: 5.1em;"
            + "}"
            + "QPushButton:pressed { border-style: inset;" + "}");
        cssEngine.applyCSS(gameB, "QPushButton" + "{" + "background-color:white;"
            + "border-style: outset;" + "border-width: 1px; " + "border-color: black;"
            + "border-radius: 2px;" + "font: bold 14px;" + "min-height: 2em;" + "min-width: 5.1em;"
            + "}"
            + "QPushButton:pressed { border-style: inset;" + "}");
        cssEngine.applyCSS(gameC, "QPushButton" + "{" + "background-color:white;"
            + "border-style: outset;" + "border-width: 1px; " + "border-color: black;"
            + "border-radius: 2px;" + "font: bold 14px;" + "min-height: 2em;" + "min-width: 5.1em;"
            + "}"
            + "QPushButton:pressed { border-style: inset;" + "}");
        cssEngine.applyCSS(gameD, "QPushButton" + "{" + "background-color:white;"
            + "border-style: outset;" + "border-width: 1px; " + "border-color: black;"
            + "border-radius: 2px;" + "font: bold 14px;" + "min-height: 2em;" + "min-width: 5.1em;"
            + "}"
            + "QPushButton:pressed { border-style: inset;" + "}");
            
        gameA.setText("A");
		    gameB.setText("B");
		    gameC.setText("C");
		    gameD.setText("D");

        // layout the game Keys
        FormData gameAFormData = new FormData();
        gameAFormData.bottom = new FormAttachment(100, 0);
        gameAFormData.left = new FormAttachment(0, 0);
        gameA.setLayoutData(gameAFormData);

        FormData gameBFormData = new FormData();
        gameBFormData.bottom = new FormAttachment(100, 0);
        gameBFormData.left = new FormAttachment(gameA, 0);
        gameB.setLayoutData(gameBFormData);

        FormData gameCFormData = new FormData();
        gameCFormData.bottom = new FormAttachment(100, 0);
        gameCFormData.left = new FormAttachment(gameB, 0);
        gameC.setLayoutData(gameCFormData);

        FormData gameDFormData = new FormData();
        gameDFormData.bottom = new FormAttachment(100, 0);
        gameDFormData.left = new FormAttachment(gameC, 0);
        gameD.setLayoutData(gameDFormData);

        gameKeysComposite.layout();
    }

    /**
     * Handles orientation changes.
     */
    void handleOrientationChange(int mode)
    {
        // change the layout as per orientation mode
    }

    /**
     * Sets the currect Canvas for Key events.
     * 
     * @param canvas
     *            - Current Canvas to which the key events needs to be
     *            delivered.
     */
    void setCurrentCanvas(Canvas canvas)
    {
        this.canvas = canvas;
    }

    /**
     * Adds LSk and RSK keys to navigation keypad if the canvas is set to full
     * screen mode.
     * 
     * @param mode
     *            - Indicates if the canvas is in fullscreen mode or not. True
     *            if fullscreen, False otherwise.
     */
    void setFullScreenMode(boolean mode)
    {

        // add the soft keys if the canvas is set to full screen mode/remove
        // otherwise
        if (mode == true)
        {

            if (keypadComposite != null)
            {

                Color black = display.getSystemColor(SWT.COLOR_BLACK);
                keypadComposite.setBackground(black);

                skright = new Button(keypadComposite, SWT.PUSH);
                skright.addMouseListener(this);

                skleft = new Button(keypadComposite, SWT.PUSH);
                skleft.addMouseListener(this);

                // Set the style sheet
                cssEngine.applyCSS(skright, "QPushButton" + "{" + "background-color:white;"
                    + "border-style: outset;" + "border-width: 1px; "
                    + "border-bottom-left-radius: 22px;" + "border-color: black;"
                    + "font: bold 14px;" + "min-width: 3em;" + "min-height: 2em;" + "}"
                    + "QPushButton:pressed { border-style: inset;" + "}");

                cssEngine.applyCSS(skleft, "QPushButton" + "{" + "background-color:white;"
                    + "border-style: outset;" + "border-width: 1px; "
                    + "border-bottom-right-radius: 22px;" + "border-color: black;"
                    + "font: bold 14px;" + "min-width: 3em;" + "min-height: 2em;" + "}"
                    + "QPushButton:pressed { border-style: inset;" + "}");
                    
                skleft.setText("LSK");
                skright.setText("RSK");

                FormData sklFormData = new FormData();
                sklFormData.top = new FormAttachment(0, 0);
                sklFormData.left = new FormAttachment(0, 0);
                skleft.setLayoutData(sklFormData);

                FormData skrFormData = new FormData();
                skrFormData.top = new FormAttachment(0, 0);
                skrFormData.right = new FormAttachment(100, 0);
                skright.setLayoutData(skrFormData);

                keypadComposite.layout();
            }
        }
        else
        {
            // LSK and RSK buttons are not needed, remove the buttons from
            // navigation keypad
            if (skright != null)
                skright.dispose();
            if (skleft != null)
                skleft.dispose();

            keypadComposite.setBackground(null);
        }

    }

    /**
     * returns the height of the canvas keypad in pixels.
     * 
     * @return Height of the canvas keypad in pixels.
     */
    int getHeight()
    {

        int navKeyPadHeight = 0;
        int gameKeyPadHeight = 0;

        if (keypadComposite != null)
        {
            // calculate the navigation keypad height
            navKeyPadHeight = keypadComposite.computeSize(SWT.DEFAULT, SWT.DEFAULT).y;
        }

        if (gameKeysComposite != null)
        {
            // calculate the game keypad height
            gameKeyPadHeight = gameKeysComposite.computeSize(SWT.DEFAULT, SWT.DEFAULT).y;
        }

        // return the total height
        return (navKeyPadHeight + gameKeyPadHeight);
    }

    /**
     * returns the keycode corresponding to the canvas keypad button.
     * 
     * @return keycode.
     */
    private int getKeyCode(Widget w)
    {
        int keyCode = INVALID_CODE;

        // Find out which osk button was pressed
        if (w == up)
        {
            keyCode = UP;
        }
        else if (w == down)
        {
            keyCode = DOWN;
        }
        else if (w == left)
        {
            keyCode = LEFT;
        }
        else if (w == right)
        {
            keyCode = RIGHT;
        }
        else if (w == center)
        {
            keyCode = FIRE;
        }
        else if (w == skleft)
        {
            keyCode = SLK;
        }
        else if (w == skright)
        {
            keyCode = SRK;
        }
        else if (w == gameA)
        {
            keyCode = GAME_A;
        }
        else if (w == gameB)
        {
            keyCode = GAME_B;
        }
        else if (w == gameC)
        {
            keyCode = GAME_C;
        }
        else if (w == gameD)
        {
            keyCode = GAME_D;
        }
        else
        {
            keyCode = INVALID_CODE;
        }

        return keyCode;
    }

    /**
     * Mouse listeners.
     */
    public void mouseDown(MouseEvent e)
    {
        handleMouseDown(e);
    }

    public void mouseUp(MouseEvent e)
    {
        handleMouseUp(e);
    }

    public void mouseMove(MouseEvent e)
    {
        handleMouseMove(e);
    }

    public void mouseDoubleClick(MouseEvent event)
    {
        // Do nothing
    }

    /**
     * Handles the mouse down event
     * Note that e.x and e.y are relative to the widget that was pressed.
     */
    private void handleMouseDown(MouseEvent e)
    {
        if (e.widget == down) // highest in the z-order
        {
            keyPressed = handleRockerPressed(e);
        }
        else
        {
            keyPressed = getKeyCode(e.widget);
        }

        if (keyPressed != INVALID_CODE)
        {
            canvas.doKeyPressed(keyPressed);

            timerTask = new CanvasKeypadTimerTask();
            timer.schedule(timerTask, KEYREPEAT_INTERVAL);
        }
    }

    /**
     * Handles the mouse up event
     * Note that e.x and e.y are relative to the widget that was pressed.
     */
    private void handleMouseUp(MouseEvent e)
    {
        int keyReleased;

        if (timerTask != null)
        {
            timerTask.cancel();
            timerTask = null;
        }

        if (e.widget == down || e.widget == keypadComposite)
        {
            keyReleased = handleRockerReleased(e);
        }
        else
        {
            keyReleased = getKeyCode(e.widget);
        }

        if (keyReleased != INVALID_CODE)
        {
            canvas.doKeyReleased(keyReleased);
        }
    }

    /**
     * Handles the mouse move event
     * Note that e.x and e.y are relative to the widget that was pressed.
     */
    private void handleMouseMove(MouseEvent e)
    {
        if (timerTask != null)
        {
            timerTask.cancel();
            timerTask = null;
        }
        
        if (e.widget == down || e.widget == keypadComposite)
        {
            handleRockerMoved(e);    
        }
        else
        {
            keyPressed = getKeyCode(e.widget);
            canvas.doKeyPressed(keyPressed);
            timerTask = new CanvasKeypadTimerTask();
            timer.schedule(timerTask, KEYREPEAT_INTERVAL);
        }
    }

    /**
     * Handles the rocker key press
     */
    private int handleRockerPressed(MouseEvent e)
    {
        keyPressed = INVALID_CODE;
        
        Point rockerSize = center.getSize();
        if (e.x >= 0 && e.x <= rockerSize.x && e.y >= 0 && e.y <= rockerSize.y)
        {
            if (center_data.getAlpha(e.x, e.y) != TRANSPARENT)
            {
                keyPressed = FIRE;
                rockerKeyPressed = center;
                center.setImage(center_pressed);
            }
            else if (left_data.getAlpha(e.x, e.y) != TRANSPARENT)
            {
                keyPressed = LEFT;
                rockerKeyPressed = left;
                left.setImage(left_pressed);
            }
            else if (right_data.getAlpha(e.x, e.y) != TRANSPARENT)
            {
                keyPressed = RIGHT;
                rockerKeyPressed = right;
                right.setImage(right_pressed);
            }
            else if (up_data.getAlpha(e.x, e.y) != TRANSPARENT)
            {
                keyPressed = UP;
                rockerKeyPressed = up;
                up.setImage(up_pressed);
            }
            else if (down_data.getAlpha(e.x, e.y) != TRANSPARENT)
            {
                keyPressed = DOWN;
                rockerKeyPressed = down;
                down.setImage(down_pressed);
            }
            else
            {
                keyPressed = INVALID_CODE;
            }
        }

        return keyPressed;
    }

    /**
     * Handles the mouse released on the rocker keys
     */
    private int handleRockerReleased(MouseEvent e)
    {
        int keyReleased = INVALID_CODE;

        Point rockerSize = center.getSize();
        Point rockerLoc = center.getLocation();
        
        if (e.widget == keypadComposite)
        {
            // Drag started outside rocker
            e.x -= rockerLoc.x;
            e.y -= rockerLoc.y;
        }

        if (e.x >= 0 && e.x <= rockerSize.x && e.y >= 0 && e.y <= rockerSize.y)
        {
            if ((rockerKeyPressed == center)
                && (center_data.getAlpha(e.x, e.y) != TRANSPARENT))
            {
                keyReleased = FIRE;
                center.setImage(center_normal);
            }
            else if ((rockerKeyPressed == left)
                && (left_data.getAlpha(e.x, e.y) != TRANSPARENT))
            {
                keyReleased = LEFT;
                left.setImage(left_normal);
            }
            else if ((rockerKeyPressed == right)
                && (right_data.getAlpha(e.x, e.y) != TRANSPARENT))
            {
                keyReleased = RIGHT;
                right.setImage(right_normal);
            }
            else if ((rockerKeyPressed == up)
                && (up_data.getAlpha(e.x, e.y) != TRANSPARENT))
            {
                keyReleased = UP;
                up.setImage(up_normal);
            }
            else if ((rockerKeyPressed == down)
                && (down_data.getAlpha(e.x, e.y) != TRANSPARENT))
            {
                keyReleased = DOWN;
                down.setImage(down_normal);
            }
            else
            {
                keyReleased = INVALID_CODE;
            }
        }

        return keyReleased;
    }

    /**
     * Handles the mouse move on the rocker keys
     */
    private void handleRockerMoved(MouseEvent e)
    {
        keyPressed = INVALID_CODE;
        Point size = center.getSize();
        Point rockerLoc = center.getLocation();
        
        if (e.widget == keypadComposite)
        {
            // Drag started outside rocker
            e.x -= rockerLoc.x;
            e.y -= rockerLoc.y;
        }

        if (e.x >= 0 && e.x <= size.x && e.y >= 0 && e.y <= size.y)
        {
            if ((center_data.getAlpha(e.x, e.y) != TRANSPARENT)
                && (rockerKeyPressed != center))
            {
                // Mouse moved to center rocker key from another rocker key,
                // release the other(source) rocker key
                releaseRockerKey(rockerKeyPressed);
            }
            else if ((left_data.getAlpha(e.x, e.y) != TRANSPARENT)
                && (rockerKeyPressed != left))
            {
                // Mouse moved to left rocker key from another rocker key,
                // release the other(source) rocker key
                // and press the left key
                releaseRockerKey(rockerKeyPressed);
                keyPressed = LEFT;
                left.setImage(left_pressed);
                rockerKeyPressed = left;
                canvas.doKeyPressed(keyPressed);
                pressedKeyData = left_data;
            }
            else if ((right_data.getAlpha(e.x, e.y) != TRANSPARENT)
                && (rockerKeyPressed != right))
            {
                // Mouse moved to right rocker key from another rocker key,
                // release the other(source) rocker key
                // and press the right key
                releaseRockerKey(rockerKeyPressed);
                keyPressed = RIGHT;
                right.setImage(right_pressed);
                rockerKeyPressed = right;
                canvas.doKeyPressed(keyPressed);
                pressedKeyData = right_data;
            }
            else if ((up_data.getAlpha(e.x, e.y) != TRANSPARENT)
                && (rockerKeyPressed != up))
            {
                // Mouse moved to up rocker key from another rocker key, release
                // the other(source) rocker key
                // and press the up key
                releaseRockerKey(rockerKeyPressed);
                keyPressed = UP;
                up.setImage(up_pressed);
                canvas.doKeyPressed(keyPressed);
                rockerKeyPressed = up;
                pressedKeyData = up_data;
            }
            else if ((down_data.getAlpha(e.x, e.y) != TRANSPARENT)
                && (rockerKeyPressed != down))
            {
                // Mouse moved to down rocker key from another rocker key,
                // release the other(source) rocker key
                // and press the down key
                releaseRockerKey(rockerKeyPressed);
                keyPressed = DOWN;
                down.setImage(down_pressed);
                rockerKeyPressed = down;
                canvas.doKeyPressed(keyPressed);
                pressedKeyData = down_data;
            }
            else
            {
                if ((pressedKeyData != null)
                    && (pressedKeyData.getAlpha(e.x, e.y) == TRANSPARENT))
                {
                    // Mouse moved to out of all rocker keys from another rocker
                    // key, release the other(source) rocker key
                    releaseRockerKey(rockerKeyPressed);
                    pressedKeyData = null;
                    rockerKeyPressed = null;

                }
            }
        }
        else
        {
            // Mouse moved to out of all rocker keys, release the other(source)
            // rocker key
            releaseRockerKey(rockerKeyPressed);
        }
    }

    /**
     * Releases a rocker key
     */
    private void releaseRockerKey(Label rockerKeyPressed)
    {
        if (rockerKeyPressed == center)
        {
            center.setImage(center_normal);
            canvas.doKeyReleased(FIRE);
        }
        else if (rockerKeyPressed == left)
        {
            left.setImage(left_normal);
            canvas.doKeyReleased(LEFT);
        }
        else if (rockerKeyPressed == right)
        {
            right.setImage(right_normal);
            canvas.doKeyReleased(RIGHT);
        }
        else if (rockerKeyPressed == up)
        {
            up.setImage(up_normal);
            canvas.doKeyReleased(UP);
        }
        else if (rockerKeyPressed == down)
        {
            down.setImage(down_normal);
            canvas.doKeyReleased(DOWN);
        }
    }

    /**
     * Canvas Keypad timer task
     */
    class CanvasKeypadTimerTask extends TimerTask
    {
        public void run()
        {
            ESWTUIThreadRunner.syncExec(new Runnable()
            {
                public void run()
                {
                    canvas.doKeyPressed(keyPressed);
                }
            });
        }
    }
}
