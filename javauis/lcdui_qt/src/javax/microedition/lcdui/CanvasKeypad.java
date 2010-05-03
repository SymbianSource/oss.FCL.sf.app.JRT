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



final class CanvasKeypad {

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
    
    private BaseCSSEngine cssEngine;

     /**
             * keypad and game keys composites.
             */
    private Composite keypadComposite;
    private Composite gameKeysComposite;
    private Canvas canvas;
    
     /**
             * Listener for receiving mouse events on the keypad buttons.
             */
    private CanvasKeypadMouseListener mouseListener = new CanvasKeypadMouseListener();
    
     /**
             * Canvas keypad buttons.
             */
    private Button up;
    private Button down;
    private Button left;
    private Button right;
    private Button center;
    private Button skright;
    private Button skleft;
    private Button gameA;
    private Button gameB;
    private Button gameC;
    private Button gameD;
    
    private int keyPressed;
        
    private Timer timer = new Timer();
    private CanvasKeypadTimerTask timerTask;
    
	/**
             * Constructs a canvas kepad based on the JAD attribute
             *
             * @param canvas - canvas for which the keypad is required.
             * @param canvasComp - Composite corresponding to the canvas.
             * @param mode- the jad attribute as a string.
             */
	CanvasKeypad(Canvas canvas, Composite canvasComp, String mode) {
		
		this.canvas = canvas;
        cssEngine = new BaseCSSEngine(Display.getCurrent());
        
        //Set form layout for the shell conatining the canvas.
        canvas.getShell().setLayout(new FormLayout());
        
        //Create the Navigation keypad 
        createNavKeypad();
        
        //check if game keys are needed
        if( mode.equalsIgnoreCase("gameactions") ) {
            //create the game key pad
            createGamePad();
        }
        		
		        
        //Layout the game keys, navigation keys and the canvas on the shell using form layout
        
        FormData canvasData = new FormData();
        canvasData.right = new FormAttachment(100);
        canvasData.left = new FormAttachment(0);
        canvasData.top = new FormAttachment(0);
        canvasData.bottom = new FormAttachment(keypadComposite);
        canvasComp.setLayoutData(canvasData);
        
        FormData navKeyData = new FormData();
        navKeyData.right = new FormAttachment(100);
        navKeyData.left = new FormAttachment(0);
        if(gameKeysComposite != null) {
            //game keypad was created above, so layout the game keys.
            navKeyData.bottom = new FormAttachment(gameKeysComposite);
        } else {
            //game keys are not needed
            navKeyData.bottom = new FormAttachment(100);
        }
        keypadComposite.setLayoutData(navKeyData);
        
                
        if(gameKeysComposite != null) {
            //layout the game keys
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
    private void createNavKeypad() {
        
        //Create the navigation key composite
        
        keypadComposite = new Composite(canvas.getShell(), SWT.NONE);
        
        //Create the buttons of the navigation keypad
        
        up = new Button(keypadComposite, SWT.PUSH);
        up.addMouseListener(mouseListener);

		left = new Button(keypadComposite, SWT.PUSH);
        left.addMouseListener(mouseListener);
        
		center = new Button(keypadComposite, SWT.PUSH);
        center.addMouseListener(mouseListener);
        
		right = new Button(keypadComposite, SWT.PUSH);
        right.addMouseListener(mouseListener);
        
		down = new Button(keypadComposite, SWT.PUSH);
        down.addMouseListener(mouseListener);
        
		//up.setStyleSheet(
        cssEngine.applyCSS(up,"QPushButton" +
                "{" +
				"background-color:blue;" +
				"border-style: outset;" +
				"border-width: 2px; " +
				"border-radius: 5px;" +
				"border-top-right-radius: 27px;"+
				"border-top-left-radius: 27px;"+
				"border-color: beige;" +
				"font: bold 14px;" +
				"min-width: 3em;" +
				"min-height: 2em;" +
				"}" +
				"QPushButton:pressed { background-color: rgb(224, 0, 0);" +
				" border-style: inset;" +
				"}");
		
		cssEngine.applyCSS(down,"QPushButton" +
                "{" +
				"background-color:blue;" +
				"border-style: outset;" +
				"border-width: 2px; " +
				"border-radius: 5px;" +
				"border-bottom-right-radius: 27px;"+
				"border-bottom-left-radius: 27px;"+
				"border-color: beige;" +
				"font: bold 14px;" +
				"min-width: 3em;" +
				"min-height: 2em;" +
				"}" +
				"QPushButton:pressed { background-color: rgb(224, 0, 0);" +
				" border-style: inset;" +
				"}");

		cssEngine.applyCSS(left,"QPushButton" +
                "{" +
				"background-color:blue;" +
				"border-style: outset;" +
				"border-width: 2px; " +
				"border-radius: 5px;" +
				"border-top-left-radius: 27px;"+
				"border-bottom-left-radius: 27px;"+
				"border-color: beige;" +
				"font: bold 14px;" +
				"min-height: 3em;" +
				"min-width: 2em;" +
				"}" +
				"QPushButton:pressed { background-color: rgb(224, 0, 0);" +
				" border-style: inset;" +
				"}");

		cssEngine.applyCSS(right,"QPushButton" +
                "{" +
				"background-color:blue;" +
				"border-style: outset;" +
				"border-width: 2px; " +
				"border-radius: 5px;" +
				"border-top-right-radius: 27px;"+
				"border-bottom-right-radius: 27px;"+
				"border-color: beige;" +
				"font: bold 14px;" +
				"min-height: 3em;" +
				"min-width: 2em;" +
				"}" +
				"QPushButton:pressed { background-color: rgb(224, 0, 0);" +
				" border-style: inset;" +
				"}");
		
		cssEngine.applyCSS(center, "QPushButton" +
                "{" +
				"background-color:blue;" +
				"border-style: outset;" +
				"border-width: 2px; " +
				"border-radius: 27px;" +
				"border-color: beige;" +
				"font: bold 14px;" +
				"min-height: 3em;" +
				"min-width: 3em;" +
				"}" +
				"QPushButton:pressed { background-color: rgb(224, 0, 0);" +
				" border-style: inset;" +
				"}");

		up.setText("^");
		center.setText("*");
		left.setText("<");
		right.setText(">");
		down.setText("v");
		
        
        //layout the navigation keys
        keypadComposite.setLayout(new FormLayout());
        
        		
		FormData upFormData = new FormData();
		upFormData.top=new FormAttachment(0,10);
		upFormData.left=new FormAttachment(40);
		up.setLayoutData(upFormData);
		
		FormData centerFormData = new FormData();
		centerFormData.top=new FormAttachment(up,0,SWT.CENTER|SWT.TRAIL);
		centerFormData.left=new FormAttachment(40);
		center.setLayoutData(centerFormData);
		
		FormData downFormData = new FormData();
		downFormData.top = new FormAttachment(center,0,SWT.TRAIL);
		downFormData.left=new FormAttachment(left);
        down.setLayoutData(downFormData);
		
		FormData rightFormData = new FormData();
		rightFormData.top=new FormAttachment(up,0,SWT.TRAIL);
		rightFormData.left=new FormAttachment(center);
		right.setLayoutData(rightFormData);
		
		FormData leftFormData = new FormData();
		leftFormData.top= new FormAttachment(up);
		leftFormData.right = new FormAttachment(center);
		left.setLayoutData(leftFormData);
		
		keypadComposite.layout();
    }
    
    
    /**
             * creates the game keypad - the game action keys A, B, C and D.
             */
    private void createGamePad() {
        //create the game pad composite
        gameKeysComposite = new Composite(canvas.getShell(), SWT.NONE);
        gameKeysComposite.setLayout(new FormLayout());
        
        //Add the game buttons        
        gameA = new Button(gameKeysComposite, SWT.PUSH);
        gameA.addMouseListener(mouseListener);
        
		gameB = new Button(gameKeysComposite, SWT.PUSH);
        gameB.addMouseListener(mouseListener);
        
		gameC = new Button(gameKeysComposite, SWT.PUSH);
        gameC.addMouseListener(mouseListener);
        
		gameD = new Button(gameKeysComposite, SWT.PUSH);
        gameD.addMouseListener(mouseListener);
        
        //set the style sheet
        String gameKeysStylesheet = "QPushButton" +
                "{" +
				"background-color:blue;" +
				"border-style: outset;" +
				"border-width: 0px; " +
				"border-color: beige;" +
				"font: bold 14px;" +
				"min-height: 2em;" +
				"min-width: 5.5em;" +
				"}" +
				"QPushButton:pressed { background-color: rgb(224, 0, 0);" +
				" border-style: inset;" +
				"}";
                
		cssEngine.applyCSS(gameA,gameKeysStylesheet);
		cssEngine.applyCSS(gameB,gameKeysStylesheet);
		cssEngine.applyCSS(gameC,gameKeysStylesheet);
		cssEngine.applyCSS(gameD,gameKeysStylesheet);
                
        gameA.setText("A");
		gameB.setText("B");
		gameC.setText("C");
		gameD.setText("D");
        		
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
    void handleOrientationChange(int mode) {
        //change the layout as per orientation mode
    }
    
    
    /**
             * Adds LSk and RSK keys to navigation keypad if the canvas is set to full screen mode.
             * @param mode- Indicates if the canvas is in fullscreen mode or not. True if fullscreen, False otherwise.
             */
    void setFullScreenMode(boolean mode) {
    
        //add the soft keys if the canvas is set to full screen mode/remove otherwise
        if(mode == true) {
        
            if(keypadComposite != null) {
        
                skright = new Button(keypadComposite, SWT.PUSH);
                skright.addMouseListener(mouseListener);
            
                skleft = new Button(keypadComposite, SWT.PUSH);
                skleft.addMouseListener(mouseListener);
            
                //Set the style sheet
        		cssEngine.applyCSS(skright,"QPushButton" +
                        "{" +
        				"background-color:blue;" +
        				"border-style: outset;" +
        				"border-width: 1px; " +
        				"border-bottom-left-radius: 22px;"+
        				"border-color: beige;" +
        				"font: bold 14px;" +
        				"min-width: 3em;" +
        				"min-height: 2em;" +
        				"}" +
        				"QPushButton:pressed { background-color: rgb(224, 0, 0);" +
        				" border-style: inset;" +
        				"}");

        		cssEngine.applyCSS(skleft,"QPushButton" +
                        "{" +
        				"background-color:blue;" +
        				"border-style: outset;" +
        				"border-width: 1px; " +
        				"border-bottom-right-radius: 22px;"+
        				"border-color: beige;" +
        				"font: bold 14px;" +
        				"min-width: 3em;" +
        				"min-height: 2em;" +
        				"}" +
        				"QPushButton:pressed { background-color: rgb(224, 0, 0);" +
        				" border-style: inset;" +
        				"}");
                        
                skright.setText("RSK");
                skleft.setText("LSK");
                        
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
        else {
            //LSK and RSK buttons are not needed, remove the buttons from navigation keypad
            if(skright != null)
                skright.dispose();
            if(skleft != null)
                skleft.dispose();
        }
        
    }

    Button getLeftSoftKey()
    {
        return skleft;
    }
    
    Button getRightSoftKey()
    {
        return skright;
    }

    /**
             * returns the height of the canvas keypad in pixels.
             * @return Height of the canvas keypad in pixels.
             */
    int getHeight() {
    
        int navKeyPadHeight = 0;
        int gameKeyPadHeight = 0;
        
        if(keypadComposite != null) {
            //calculate the navigation keypad height
            navKeyPadHeight = keypadComposite.computeSize(SWT.DEFAULT, SWT.DEFAULT).y;
        }
        
        if(gameKeysComposite != null) {
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
    private int getKeyCode(Widget w) {
    
        int keyCode = INVALID_CODE;
        
        // Find out which osk button was pressed
        if(w == up) {
            keyCode = UP;
        } else if(w == down) {
            keyCode = DOWN;
        } else if(w == left) {
            keyCode = LEFT;
        } else if(w == right) {
            keyCode = RIGHT;
        } else if(w == center) {
            keyCode = FIRE;
        } else if(w == skleft) {
            keyCode = SLK;
        } else if(w == skright) {
            keyCode = SRK;
        } else if(w == gameA) {
            keyCode = GAME_A;
        } else if(w == gameB) {
            keyCode = GAME_B;
        } else if(w == gameC) {
            keyCode = GAME_C;
        } else if(w == gameD) {
            keyCode = GAME_D;
        } else {
            keyCode = INVALID_CODE;;
        }
        
        return keyCode;
    }
            
      
    /**
             * Mouse listener.
             */
    class CanvasKeypadMouseListener implements MouseListener {

        public void mouseDown(MouseEvent e) {
                      
            canvas.doKeyPressed(getKeyCode(e.widget));
            
            keyPressed = getKeyCode(e.widget);          
            
            // Create and Schedule the long keypress timer
            timerTask = new CanvasKeypadTimerTask();
            timer.schedule(timerTask, KEYREPEAT_INTERVAL);
        }

        public void mouseUp(MouseEvent e) {
                       
            if( timerTask != null ) {
                //cancel the long keypress timer if it is running
                timerTask.cancel();
                timerTask = null;
            }
            canvas.doKeyReleased(getKeyCode(e.widget));
        }

        public void mouseMove(MouseEvent e) {
        
            if( timerTask != null ) {
                timerTask.cancel();
                timerTask = null;
            }
  
            canvas.doKeyPressed(getKeyCode(e.widget));
            
            keyPressed = getKeyCode(e.widget);
            
            // Create and Schedule Timer
            timerTask = new CanvasKeypadTimerTask();
            timer.schedule(timerTask, KEYREPEAT_INTERVAL);
        	  
        }

        public void mouseDoubleClick(MouseEvent event) {
            //do nothing
        }

        
    }
    
    class CanvasKeypadTimerTask extends TimerTask {

        public void run() {
        	
            ESWTUIThreadRunner.syncExec(new Runnable() {
            public void run() {
                canvas.doKeyPressed(keyPressed);
            }
            });
            
            
        }
    }
    
    

}
