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

import org.eclipse.ercp.swt.mobile.MobileShell;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.*;
import org.eclipse.swt.widgets.*;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.graphics.*;
import org.eclipse.swt.internal.qt.*;
import com.nokia.mj.impl.rt.support.ApplicationInfo;

import org.eclipse.swt.layout.FormAttachment;
import org.eclipse.swt.layout.FormData;
import org.eclipse.swt.layout.FormLayout;

import javax.microedition.lcdui.EventDispatcher.LCDUIEvent;


/**
 * The CanvasKeypad class is designed to generate the on screen keypad
 * and handle graphical operations as well as low-level pointer events.
 * CanvasKeypad maps the pointer events to key events that are needed by the Canvas.
 *
 */
final class CanvasKeypad implements MouseListener, MouseMoveListener
{

    /**
             *  Constant for <code>UP</code> game action.
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
             * alpha value of a transparent pixel.
             */
    private static final int TRANSPARENT = 0;

    /**
             * keypad and game keys composites.
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
             * offset on the composite in pixels where the osk rocker keys are layed out.
             */
    private int image_offset;


    /**
             * Constructs a canvas kepad based on the JAD attribute
             *
             * @param canvas - canvas for which the keypad is required.
             * @param mode- the jad attribute as a string.
             */
    CanvasKeypad(Canvas canvas, String mode)
    {

        this.canvas = canvas;
        display = ESWTUIThreadRunner.getInstance().getDisplay();

        cssEngine = new BaseCSSEngine(display);


        //load the images
        center_normal = new org.eclipse.swt.graphics.Image(display,RESOURCE_PATH + "qtg_fr_rocker_normal_c.svg");
        center_pressed = new org.eclipse.swt.graphics.Image(display,RESOURCE_PATH + "qtg_fr_rocker_pressed_c.svg");
        left_normal = new org.eclipse.swt.graphics.Image(display,RESOURCE_PATH + "qtg_fr_rocker_normal_l.svg");
        left_pressed = new org.eclipse.swt.graphics.Image(display,RESOURCE_PATH + "qtg_fr_rocker_pressed_l.svg");
        right_normal = new org.eclipse.swt.graphics.Image(display,RESOURCE_PATH + "qtg_fr_rocker_normal_r.svg");
        right_pressed = new org.eclipse.swt.graphics.Image(display,RESOURCE_PATH + "qtg_fr_rocker_pressed_r.svg");
        up_normal = new org.eclipse.swt.graphics.Image(display,RESOURCE_PATH + "qtg_fr_rocker_normal_t.svg");
        up_pressed = new org.eclipse.swt.graphics.Image(display,RESOURCE_PATH + "qtg_fr_rocker_pressed_t.svg");
        down_normal = new org.eclipse.swt.graphics.Image(display,RESOURCE_PATH + "qtg_fr_rocker_normal_b.svg");
        down_pressed = new org.eclipse.swt.graphics.Image(display,RESOURCE_PATH + "qtg_fr_rocker_pressed_b.svg");

        image_offset = (display.getBounds().width/2) - (center_normal.getBounds().width/2);

        //Load image data required to determine the osk button pressed on mouse events
        center_data = center_normal.getImageData();
        left_data = left_normal.getImageData();
        right_data = right_normal.getImageData();
        up_data = up_normal.getImageData();
        down_data = down_normal.getImageData();


        //Set form layout for the shell containing the canvas.
        canvas.getShell().setLayout(new FormLayout());

        //Create the Navigation keypad
        createNavKeypad();

        //check if game keys are needed
        if(mode.equalsIgnoreCase("gameactions"))
        {
            //create the game key pad
            createGamePad();
        }


        //Layout the game keys, navigation keys and the canvas composites on the shell using form layout

        FormData canvasData = new FormData();
        canvasData.right = new FormAttachment(100);
        canvasData.left = new FormAttachment(0);
        canvasData.top = new FormAttachment(0);
        canvasData.bottom = new FormAttachment(keypadComposite);
        canvas.getContentComp().setLayoutData(canvasData);

        FormData navKeyData = new FormData();
        navKeyData.right = new FormAttachment(100);
        navKeyData.left = new FormAttachment(0);

        if(gameKeysComposite != null)
        {
            //game keypad was created above, so layout the game keys.
            navKeyData.bottom = new FormAttachment(gameKeysComposite);
        }
        else
        {
            //game keys are not needed
            navKeyData.bottom = new FormAttachment(100);
        }

        keypadComposite.setLayoutData(navKeyData);


        if(gameKeysComposite != null)
        {
            //game keys were created, layout the game keys
            FormData gameKeyData = new FormData();
            gameKeyData.right = new FormAttachment(100);
            gameKeyData.left = new FormAttachment(0);
            gameKeyData.bottom = new FormAttachment(100);
            gameKeysComposite.setLayoutData(gameKeyData);
        }

        //layout the shell
        canvas.getShell().layout();

    }

    /**
             * creates the navigation keypad - the directional keys and selctio key.
             */
    private void createNavKeypad()
    {

        //Create the navigation key composite

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

        //layout the navigation keys
        keypadComposite.setLayout(new FormLayout());


        FormData navkeysFormData = new FormData();
        navkeysFormData.top=new FormAttachment(0);
        navkeysFormData.left = new FormAttachment(0,image_offset);
        center.setLayoutData(navkeysFormData);
        left.setLayoutData(navkeysFormData);
        right.setLayoutData(navkeysFormData);
        up.setLayoutData(navkeysFormData);
        down.setLayoutData(navkeysFormData);

        keypadComposite.pack();

        keypadComposite.layout();

        keypadComposite.addMouseListener(this);
        keypadComposite.addMouseMoveListener(this);

    }


    /**
             * creates the game keypad - the game action keys A, B, C and D.
             */
    private void createGamePad()
    {
        //create the game pad composite
        gameKeysComposite = new Composite(canvas.getShell(), SWT.NONE);
        gameKeysComposite.setLayout(new FormLayout());

        //Add the game buttons
        gameA = new Button(gameKeysComposite, SWT.PUSH);
        gameA.addMouseListener(this);

        gameB = new Button(gameKeysComposite, SWT.PUSH);
        gameB.addMouseListener(this);

        gameC = new Button(gameKeysComposite, SWT.PUSH);
        gameC.addMouseListener(this);

        gameD = new Button(gameKeysComposite, SWT.PUSH);
        gameD.addMouseListener(this);

        //Set the game key style sheets
        cssEngine.applyCSS(gameA,"QPushButton" +
                           "{" +
                           "background-color:white;" +
                           "border-style: outset;" +
                           "border-width: 1px; " +
                           "border-color: black;" +
                           "border-radius: 2px;" +
                           "font: bold 14px;" +
                           "min-height: 2em;" +
                           "min-width: 5.1em;" +
                           "image: url(" + RESOURCE_PATH + "qtg_mono_game_a.svg);" +
                           "}" +
                           "QPushButton:pressed { image: url(" + RESOURCE_PATH + "qtg_mono_game_a.svg);" +
                           " border-style: inset;" +
                           "}");
        cssEngine.applyCSS(gameB,"QPushButton" +
                           "{" +
                           "background-color:white;" +
                           "border-style: outset;" +
                           "border-width: 1px; " +
                           "border-color: black;" +
                           "border-radius: 2px;" +
                           "font: bold 14px;" +
                           "min-height: 2em;" +
                           "min-width: 5.1em;" +
                           "image: url(" + RESOURCE_PATH + "qtg_mono_game_b.svg);" +
                           "}" +
                           "QPushButton:pressed { image: url(" + RESOURCE_PATH + "qtg_mono_game_b.svg);" +
                           " border-style: inset;" +
                           "}");
        cssEngine.applyCSS(gameC,"QPushButton" +
                           "{" +
                           "background-color:white;" +
                           "border-style: outset;" +
                           "border-width: 1px; " +
                           "border-color: black;" +
                           "border-radius: 2px;" +
                           "font: bold 14px;" +
                           "min-height: 2em;" +
                           "min-width: 5.1em;" +
                           "image: url(" + RESOURCE_PATH + "qtg_mono_game_c.svg);" +
                           "}" +
                           "QPushButton:pressed { image: url(" + RESOURCE_PATH + "qtg_mono_game_c.svg);" +
                           " border-style: inset;" +
                           "}");
        cssEngine.applyCSS(gameD,"QPushButton" +
                           "{" +
                           "background-color:white;" +
                           "border-style: outset;" +
                           "border-width: 1px; " +
                           "border-color: black;" +
                           "border-radius: 2px;" +
                           "font: bold 14px;" +
                           "min-height: 2em;" +
                           "min-width: 5.1em;" +
                           "image: url(" + RESOURCE_PATH + "qtg_mono_game_d.svg);" +
                           "}" +
                           "QPushButton:pressed { image: url(" + RESOURCE_PATH + "qtg_mono_game_d.svg);" +
                           " border-style: inset;" +
                           "}");


        //layout the game Keys
        FormData gameAFormData = new FormData();
        gameAFormData.bottom=new FormAttachment(100,0);
        gameAFormData.left=new FormAttachment(0,0);
        gameA.setLayoutData(gameAFormData);

        FormData gameBFormData = new FormData();
        gameBFormData.bottom=new FormAttachment(100,0);
        gameBFormData.left=new FormAttachment(gameA,0);
        gameB.setLayoutData(gameBFormData);

        FormData gameCFormData = new FormData();
        gameCFormData.bottom=new FormAttachment(100,0);
        gameCFormData.left=new FormAttachment(gameB,0);
        gameC.setLayoutData(gameCFormData);

        FormData gameDFormData = new FormData();
        gameDFormData.bottom=new FormAttachment(100,0);
        gameDFormData.left=new FormAttachment(gameC,0);
        gameD.setLayoutData(gameDFormData);

        gameKeysComposite.layout();
    }


    /**
             * Handles orientation changes.
             */
    void handleOrientationChange(int mode)
    {
        //change the layout as per orientation mode
    }


    /**
             * Sets the currect Canvas for Key events.
             * @param canvas - Current Canvas to which the key events needs to be delivered.
             */
    void setCurrentCanvas(Canvas canvas)
    {
        this.canvas = canvas;
    }


    /**
             * Adds LSk and RSK keys to navigation keypad if the canvas is set to full screen mode.
             * @param mode- Indicates if the canvas is in fullscreen mode or not. True if fullscreen, False otherwise.
             */
    void setFullScreenMode(boolean mode)
    {

        //add the soft keys if the canvas is set to full screen mode/remove otherwise
        if(mode == true)
        {

            if(keypadComposite != null)
            {

                Color black = display.getSystemColor(SWT.COLOR_BLACK);
                keypadComposite.setBackground(black);

                skright = new Button(keypadComposite, SWT.PUSH);
                skright.addMouseListener(this);

                skleft = new Button(keypadComposite, SWT.PUSH);
                skleft.addMouseListener(this);

                //Set the style sheet
                cssEngine.applyCSS(skright,"QPushButton" +
                                   "{" +
                                   "background-color:white;" +
                                   "border-style: outset;" +
                                   "border-width: 1px; " +
                                   "border-bottom-left-radius: 22px;"+
                                   "border-color: black;" +
                                   "font: bold 14px;" +
                                   "min-width: 3em;" +
                                   "min-height: 2em;" +
                                   "image: url(" + RESOURCE_PATH + "qtg_mono_rsk_horizontal.svg);" +
                                   "}" +
                                   "QPushButton:pressed { image: url(" + RESOURCE_PATH + "qtg_mono_rsk_horizontal.svg);" +
                                   " border-style: inset;" +
                                   "}");

                cssEngine.applyCSS(skleft,"QPushButton" +
                                   "{" +
                                   "background-color:white;" +
                                   "border-style: outset;" +
                                   "border-width: 1px; " +
                                   "border-bottom-right-radius: 22px;"+
                                   "border-color: black;" +
                                   "font: bold 14px;" +
                                   "min-width: 3em;" +
                                   "min-height: 2em;" +
                                   "image: url(" + RESOURCE_PATH + "qtg_mono_lsk_horizontal.svg);" +
                                   "}" +
                                   "QPushButton:pressed { image: url(" + RESOURCE_PATH + "qtg_mono_lsk_horizontal.svg);" +
                                   " border-style: inset;" +
                                   "}");


                FormData sklFormData = new FormData();
                sklFormData.top=new FormAttachment(0, 0);
                sklFormData.left=new FormAttachment(0, 0);
                skleft.setLayoutData(sklFormData);

                FormData skrFormData = new FormData();
                skrFormData.top=new FormAttachment(0, 0);
                skrFormData.right=new FormAttachment(100, 0);
                skright.setLayoutData(skrFormData);

                keypadComposite.layout();
            }
        }
        else
        {
            //LSK and RSK buttons are not needed, remove the buttons from navigation keypad
            if(skright != null)
                skright.dispose();
            if(skleft != null)
                skleft.dispose();

            keypadComposite.setBackground(null);
        }

    }

    /**
             * returns the height of the canvas keypad in pixels.
             * @return Height of the canvas keypad in pixels.
             */
    int getHeight()
    {

        int navKeyPadHeight = 0;
        int gameKeyPadHeight = 0;

        if(keypadComposite != null)
        {
            //calculate the navigation keypad height
            navKeyPadHeight = keypadComposite.computeSize(SWT.DEFAULT, SWT.DEFAULT).y;
        }

        if(gameKeysComposite != null)
        {
            //calculate the game keypad height
            gameKeyPadHeight = gameKeysComposite.computeSize(SWT.DEFAULT, SWT.DEFAULT).y;
        }

        //return the total height
        return (navKeyPadHeight + gameKeyPadHeight);
    }

    /**
             * returns the keycode corresponding to the canvas keypad button.
             * @return keycode.
             */
    private int getKeyCode(Widget w)
    {

        int keyCode = INVALID_CODE;

        // Find out which osk button was pressed
        if(w == up)
        {
            keyCode = UP;
        }
        else if(w == down)
        {
            keyCode = DOWN;
        }
        else if(w == left)
        {
            keyCode = LEFT;
        }
        else if(w == right)
        {
            keyCode = RIGHT;
        }
        else if(w == center)
        {
            keyCode = FIRE;
        }
        else if(w == skleft)
        {
            keyCode = SLK;
        }
        else if(w == skright)
        {
            keyCode = SRK;
        }
        else if(w == gameA)
        {
            keyCode = GAME_A;
        }
        else if(w == gameB)
        {
            keyCode = GAME_B;
        }
        else if(w == gameC)
        {
            keyCode = GAME_C;
        }
        else if(w == gameD)
        {
            keyCode = GAME_D;
        }
        else
        {
            keyCode = INVALID_CODE;;
        }

        return keyCode;
    }


    /**
             * Mouse listeners.
             */

    public void mouseDown(MouseEvent e)
    {

        //handle the mouse down event
        handleMouseDown(e);

    }

    public void mouseUp(MouseEvent e)
    {

        //handle the mouse up event
        handleMouseUp(e);

    }

    public void mouseMove(MouseEvent e)
    {

        //handle the mouse move event
        handleMouseMove(e);

    }

    public void mouseDoubleClick(MouseEvent event)
    {
        //do nothing
    }





    /**
             * Handles the mouse down event
             *
             */
    private void handleMouseDown(MouseEvent e)
    {

        if(e.widget == keypadComposite)
        {
            //the mouse down was grabbed by the keypad composite, map the key code based on the on rocker images.
            keyPressed = handleRockerPressed(e);

        }
        else
        {
            //get the keycode of the game or soft keys based on the button that was clicked
            keyPressed = getKeyCode(e.widget);
        }

        if(keyPressed != INVALID_CODE)
        {

            //notify the canvas
            canvas.doKeyPressed(keyPressed);

            // Create and Schedule the long keypress timer
            timerTask = new CanvasKeypadTimerTask();
            timer.schedule(timerTask, KEYREPEAT_INTERVAL);
        }
    }

    /**
             * Handles the mouse up event
             *
             */
    private void handleMouseUp(MouseEvent e)
    {
        int keyReleased;

        if(timerTask != null)
        {
            //cancel the long keypress timer if it is running
            timerTask.cancel();
            timerTask = null;
        }

        if(e.widget == keypadComposite)
        {
            //the mouse up was grabbed by the keypad composite, map the key code based on the on rocker images.
            keyReleased = handleRockerReleased(e);

        }
        else
        {
            //get the keycode of the game or soft keys based on the button that was clicked
            keyReleased = getKeyCode(e.widget);
        }

        if(keyReleased != INVALID_CODE)
        {

            //notify the canvas
            canvas.doKeyReleased(keyReleased);
        }
    }


    /**
             * Handles the mouse move event
             *
             */
    private void handleMouseMove(MouseEvent e)
    {

        if(timerTask != null)
        {
            //cancel the long keypress timer if it is running
            timerTask.cancel();
            timerTask = null;
        }

        if(e.widget == keypadComposite)
        {
            //the mouse move was grabbed by the keypad composite, handle the rocker key swipe
            handleRockerMoved(e);

        }
        else
        {
            //get the keycode of the game or soft keys based on the button that was clicked
            keyPressed = getKeyCode(e.widget);

            //notify the canvas
            canvas.doKeyPressed(keyPressed);

            // Create and Schedule Timer
            timerTask = new CanvasKeypadTimerTask();
            timer.schedule(timerTask, KEYREPEAT_INTERVAL);
        }


    }


    /**
             * Handles the rocker key press
             *
             */
    private int handleRockerPressed(MouseEvent e)
    {

        keyPressed = INVALID_CODE;

        Rectangle image_bounds = center.getBounds();

        // Find out which rocker button was pressed
        if(image_bounds.contains(e.x,e.y) == true)
        {
            //check each of the button images  to check which osk navkey button was pressed
            if(center_data.getAlpha(e.x-image_offset,e.y)!=TRANSPARENT)
            {
                //center button was pressed
                keyPressed = FIRE;
                rockerKeyPressed = center;
                center.setImage(center_pressed);
            }
            else if(left_data.getAlpha(e.x-image_offset,e.y)!=TRANSPARENT)
            {
                //left button was pressed
                keyPressed = LEFT;
                rockerKeyPressed = left;
                left.setImage(left_pressed);
            }
            else if(right_data.getAlpha(e.x-image_offset,e.y)!=TRANSPARENT)
            {
                //right button was pressed
                keyPressed = RIGHT;
                rockerKeyPressed = right;
                right.setImage(right_pressed);
            }
            else if(up_data.getAlpha(e.x-image_offset,e.y)!=TRANSPARENT)
            {
                //up button was pressed
                keyPressed = UP;
                rockerKeyPressed = up;
                up.setImage(up_pressed);
            }
            else if(down_data.getAlpha(e.x-image_offset,e.y)!=TRANSPARENT)
            {
                //down button was pressed
                keyPressed = DOWN;
                rockerKeyPressed = down;
                down.setImage(down_pressed);
            }
            else
            {
                keyPressed = INVALID_CODE;;
            }

        }

        return keyPressed;

    }


    /**
             * Handles the mouse released on the rocker keys
             *
             */
    private int handleRockerReleased(MouseEvent e)
    {

        int keyReleased = INVALID_CODE;

        Rectangle image_bounds = center.getBounds();

        // Find out which rocker button was released
        if(image_bounds.contains(e.x,e.y) == true)
        {
            //check each of the button images  to check which osk navkey button was released
            if((rockerKeyPressed == center) &&
                    (center_data.getAlpha(e.x-image_offset,e.y)!=TRANSPARENT))
            {
                //center button was released
                keyReleased = FIRE;
                center.setImage(center_normal);
            }
            else if((rockerKeyPressed == left) &&
                    (left_data.getAlpha(e.x-image_offset,e.y)!=TRANSPARENT))
            {
                //left button was released
                keyReleased = LEFT;
                left.setImage(left_normal);
            }
            else if((rockerKeyPressed == right) &&
                    (right_data.getAlpha(e.x-image_offset,e.y)!=TRANSPARENT))
            {
                //right button was released
                keyReleased = RIGHT;
                right.setImage(right_normal);
            }
            else if((rockerKeyPressed == up) &&
                    (up_data.getAlpha(e.x-image_offset,e.y)!=TRANSPARENT))
            {
                //up button was released
                keyReleased = UP;
                up.setImage(up_normal);
            }
            else if((rockerKeyPressed == down) &&
                    (down_data.getAlpha(e.x-image_offset,e.y)!=TRANSPARENT))
            {
                //down button was released
                keyReleased = DOWN;
                down.setImage(down_normal);
            }
            else
            {
                keyReleased = INVALID_CODE;;
            }

        }

        return keyReleased;

    }


    /**
             * Handles the mouse move on the rocker keys
             *
             */
    private void handleRockerMoved(MouseEvent e)
    {

        keyPressed = INVALID_CODE;

        Rectangle image_bounds = center.getBounds();

        // Find out which rocker button was traversed to
        if(image_bounds.contains(e.x,e.y) == true)
        {
            //check each of the button images  to check which osk navkey button was traversed to
            if((center_data.getAlpha(e.x-image_offset,e.y)!=TRANSPARENT) &&
                    (rockerKeyPressed != center))
            {
                //Mouse moved to center rocker key from another rocker key, release the other(source) rocker key
                releaseRockerKey(rockerKeyPressed);
            }
            else if((left_data.getAlpha(e.x-image_offset,e.y)!=TRANSPARENT) &&
                    (rockerKeyPressed != left))
            {
                //Mouse moved to left rocker key from another rocker key, release the other(source) rocker key
                // and press the left key
                releaseRockerKey(rockerKeyPressed);
                keyPressed = LEFT;
                left.setImage(left_pressed);
                rockerKeyPressed = left;
                canvas.doKeyPressed(keyPressed);
                pressedKeyData = left_data;
            }
            else if((right_data.getAlpha(e.x-image_offset,e.y)!=TRANSPARENT) &&
                    (rockerKeyPressed != right))
            {
                //Mouse moved to right rocker key from another rocker key, release the other(source) rocker key
                // and press the right key
                releaseRockerKey(rockerKeyPressed);
                keyPressed = RIGHT;
                right.setImage(right_pressed);
                rockerKeyPressed = right;
                canvas.doKeyPressed(keyPressed);
                pressedKeyData = right_data;
            }
            else if((up_data.getAlpha(e.x-image_offset,e.y)!=TRANSPARENT) &&
                    (rockerKeyPressed != up))
            {
                //Mouse moved to up rocker key from another rocker key, release the other(source) rocker key
                // and press the up key
                releaseRockerKey(rockerKeyPressed);
                keyPressed = UP;
                up.setImage(up_pressed);
                canvas.doKeyPressed(keyPressed);
                rockerKeyPressed = up;
                pressedKeyData = up_data;
            }
            else if((down_data.getAlpha(e.x-image_offset,e.y)!=TRANSPARENT) &&
                    (rockerKeyPressed != down))
            {
                //Mouse moved to down rocker key from another rocker key, release the other(source) rocker key
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
                if((pressedKeyData != null) &&
                        (pressedKeyData.getAlpha(e.x-image_offset,e.y) == TRANSPARENT))
                {
                    //Mouse moved to out of all rocker keys from another rocker key, release the other(source) rocker key
                    releaseRockerKey(rockerKeyPressed);
                    pressedKeyData = null;
                    rockerKeyPressed = null;

                }
            }

        }
        else
        {
            //Mouse moved to out of all rocker keys, release the other(source) rocker key
            releaseRockerKey(rockerKeyPressed);
        }


    }



    /**
             * Releases a rocker key
             *
             */
    private void releaseRockerKey(Label rockerKeyPressed)
    {

        if(rockerKeyPressed == center)
        {
            center.setImage(center_normal);
            canvas.doKeyReleased(FIRE);
        }
        else if(rockerKeyPressed == left)
        {
            left.setImage(left_normal);
            canvas.doKeyReleased(LEFT);
        }
        else if(rockerKeyPressed == right)
        {
            right.setImage(right_normal);
            canvas.doKeyReleased(RIGHT);
        }
        else if(rockerKeyPressed == up)
        {
            up.setImage(up_normal);
            canvas.doKeyReleased(UP);
        }
        else if(rockerKeyPressed == down)
        {
            down.setImage(down_normal);
            canvas.doKeyReleased(DOWN);
        }

    }



    /**
             * Canvas Keypad timer task
             *
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
