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
package com.nokia.test.java.eswt.control;

import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.ControlListener;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;

import org.eclipse.swt.widgets.Display;

import com.nokia.mid.ui.TactileFeedback;


public class CustomControl extends Composite implements PaintListener, ControlListener, FocusListener, DisposeListener
{


    private Image image;
    private GC gc;
    private TactileFeedback tf;
    private boolean draw_focus;

    /** */
    protected Display display = null;

    public CustomControl(Composite parent)
    {
        super(parent,SWT.NO_BACKGROUND);

        this.display = super.getDisplay();
        this.draw_focus = true;
        addPaintListener(this);
        addControlListener(this);
        addFocusListener(this);
        addDisposeListener(this);
        tf = new TactileFeedback();
    }

    /**
     * paintlistener implementation
     */
    public void paintControl(PaintEvent event)
    {
        //if (this.isEnabled())
        if (image == null || image.isDisposed())
        {
            image = new Image(Display.getCurrent(), getSize().x, getSize().y);
            gc = new GC(image);

        }
        if (draw_focus)
        {
            gc.drawFocus(image.getBounds().x,image.getBounds().y,image.getBounds().width,image.getBounds().height);
        }

        gc.setLineStyle(SWT.LINE_SOLID);
        gc.setLineWidth(5);

        gc.setBackground(this.display.getSystemColor(SWT.COLOR_YELLOW));
        gc.fillRectangle(image.getBounds().x,image.getBounds().y,image.getBounds().width,image.getBounds().height);
        //draw  Feedback areas
        gc.setForeground(this.display.getSystemColor(SWT.COLOR_RED));
        gc.drawRectangle(image.getBounds().x,image.getBounds().y,(image.getBounds().width/2)+20,image.getBounds().height);
        gc.setForeground(this.display.getSystemColor(SWT.COLOR_BLUE));
        gc.drawRectangle((image.getBounds().width/2),image.getBounds().y,image.getBounds().width,image.getBounds().height);


        event.gc.drawImage(image,0,0);

    }

    public void registerTactileFeedback()
    {

        tf.registerFeedbackArea(this,0,5,0,(this.getSize().x/2)+20,this.getSize().y,TactileFeedback.FEEDBACK_STYLE_BASIC);
        tf.registerFeedbackArea(this,1,(this.getSize().x/2),0,this.getSize().x-5,this.getSize().y,TactileFeedback.FEEDBACK_STYLE_SENSITIVE);

    }

    public void unregisterArea(int id)
    {
        tf.unregisterFeedbackArea(this,id);

    }
    public void removeFeedback()
    {
        tf.removeFeedbackForComponent(this);
    }
    public void switchAreas()
    {
        tf.moveFeedbackAreaToFirstPriority(this,0);
    }

    public void destroy()
    {
        if (image != null && !image.isDisposed())
        {
            image.dispose();
            gc.dispose();
        }
    }
    public void setVisible(boolean visible)
    {
        super.setVisible(visible);

        if (visible)
        {
            this.registerTactileFeedback();
        }
        else
        {
            this.removeFeedback();
        }
    }
    public void setEnabled(boolean enable)
    {
        super.setEnabled(enable);
        if (enable)
        {
            this.registerTactileFeedback();
        }
        else
        {
            this.removeFeedback();
        }
    }

    public void widgetDisposed(DisposeEvent e)
    {
        removeFeedback();
        destroy();
    }
    public void controlResized(ControlEvent e)
    {
        destroy();
        this.registerTactileFeedback();
        this.update();
    }
    public void controlMoved(ControlEvent e)
    {
        this.registerTactileFeedback();
    }
    public void focusGained(FocusEvent e)
    {
        this.draw_focus = true;
        tf.directFeedback(TactileFeedback.FEEDBACK_STYLE_BASIC);
    }
    public void focusLost(FocusEvent e)
    {
        this.draw_focus = false;
        tf.directFeedback(TactileFeedback.FEEDBACK_STYLE_SENSITIVE);
    }
}
