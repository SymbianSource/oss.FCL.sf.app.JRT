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

import java.util.Vector;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.ControlListener;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;

import com.nokia.mid.ui.TactileFeedback;


public class ImageCanvas extends Composite implements ControlListener, DisposeListener, PaintListener, MouseListener, MouseMoveListener
{

    private Image image;
    private GC gc;
    private Point oldMouseXY;
    private boolean mouseDown = false;
    private Vector mouseEvents;
    private TactileFeedback tf;

    public ImageCanvas(Composite parent, int style)
    {
        super(parent, style);
        mouseEvents = new Vector();
        mouseEvents.ensureCapacity(100);
        tf = new TactileFeedback();
        addDisposeListener(this);
        addPaintListener(this);
        addMouseListener(this);
        addMouseMoveListener(this);
    }

    public void setImage(String imagePath)
    {
        destroy();
    }


    public void paintControl(PaintEvent e)
    {
        if (image == null || image.isDisposed())
        {
            image = new Image(Display.getCurrent(), getSize().x, getSize().y);
            gc = new GC(image);
            gc.setForeground(Display.getCurrent().getSystemColor(SWT.COLOR_BLUE));
        }

        for (int i = 0; i < mouseEvents.size(); ++i)
        {
            MouseMoveEvent mouse = (MouseMoveEvent)mouseEvents.elementAt(i);
            gc.drawLine(mouse.a.x, mouse.a.y, mouse.b.x, mouse.b.y);
        }
        mouseEvents.removeAllElements();

        e.gc.drawImage(image, 0, 0);
    }

    public void controlMoved(ControlEvent e)
    {
    }

    public void destroy()
    {
        if (image != null && !image.isDisposed())
        {
            image.dispose();
            gc.dispose();
        }
    }

    public void controlResized(ControlEvent e)
    {
        destroy();
    }

    public void widgetDisposed(DisposeEvent e)
    {
        destroy();
    }

    public void mouseDoubleClick(MouseEvent e)
    {
    }

    public void mouseDown(MouseEvent e)
    {
        mouseDown = true;
        tf.directFeedback(TactileFeedback.FEEDBACK_STYLE_BASIC);
    }

    public void mouseUp(MouseEvent e)
    {
        mouseDown = false;
        oldMouseXY = null;
    }

    private class MouseMoveEvent
    {
        public MouseMoveEvent(Point a, Point b)
        {
            this.a = a;
            this.b = b;
        }
        public Point a;
        public Point b;
    }

    public void mouseMove(MouseEvent e)
    {
        if (image == null || image.isDisposed() || !mouseDown)
        {
            return;
        }
        tf.directFeedback(TactileFeedback.FEEDBACK_STYLE_SENSITIVE);
        Point newMouseXY = new Point(e.x, e.y);

        if (oldMouseXY != null)
        {
            mouseEvents.addElement(new MouseMoveEvent(oldMouseXY, newMouseXY));
            if (mouseEvents.size() == 1)
            {
                redraw(0, 0, 20, 20, false);
            }
        }
        oldMouseXY = newMouseXY;
    }

}

