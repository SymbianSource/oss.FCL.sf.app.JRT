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

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Layout;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.ScrollBar;

/**
 * Composite control which is able to scroll text with a vertical scroll bar.
 */
class ScrolledTextComposite extends Composite
{

    private Label iESWTTextLabel;
    private ScrollBar vBar;
    private boolean resizing;
    private boolean iTextScrolling;

    /**
     * Constructor.
     */
    public ScrolledTextComposite(Composite parent, ScrollBar vertBar)
    {
        super(parent, 0);
        vBar = vertBar;
        vBar.setVisible(false);
        vBar.setMaximum(0);
        vBar.setThumb(0);
        vBar.setSelection(0);

        vBar.addListener(SWT.Selection, new Listener()
        {
            public void handleEvent(Event e)
            {
                vScroll();
            }
        });

        addListener(SWT.Resize, new Listener()
        {
            public void handleEvent(Event e)
            {
                resize();
            }
        });

        iESWTTextLabel = new Label(this, SWT.WRAP);
        iESWTTextLabel.setLocation(0, 0);
        layout();

        iESWTTextLabel.addListener(SWT.Resize, new Listener()
        {
            public void handleEvent(Event e)
            {
                if(e.type == SWT.Resize)
                {
                    resize();
                }
            }
        });
    }

    public void setText(String aText)
    {
        iESWTTextLabel.setText(aText);
        iESWTTextLabel.setSize(calculateTextSize());
    }

    private Point calculateTextSize()
    {
        iTextScrolling = false;
        vBar.setVisible(false);
        Point nextSize = iESWTTextLabel.computeSize(getClientArea().width,
                         SWT.DEFAULT);
        if(nextSize.y > getClientArea().height)
        {
            iTextScrolling = true;
            vBar.setVisible(true);
            nextSize = iESWTTextLabel.computeSize(getClientArea().width,
                                                  SWT.DEFAULT);
        }
        return nextSize;
    }

    /**
     * Return if the text is scrolling. The vBar.isVisible() is not good,
     * because it depends on parent's visibility.
     */
    public boolean isTextScrolling()
    {
        return iTextScrolling;
    }

    public void setLayout(Layout layout)
    {
        // do not allow a layout to be set
    }

    public void layout(boolean changed)
    {
        iESWTTextLabel.setSize(calculateTextSize());

        Rectangle contentRect = iESWTTextLabel.getBounds();
        Rectangle hostRect = getClientArea();
        vBar.setMaximum(contentRect.height);
        vBar.setThumb(Math.min(contentRect.height, hostRect.height));
        int vPage = contentRect.height - hostRect.height;
        int vSelection = vBar.getSelection();
        if(vSelection >= vPage)
        {
            if(vPage <= 0)
            {
                vSelection = 0;
                vBar.setSelection(0);
            }
            contentRect.y = -vSelection;
        }
        iESWTTextLabel.setLocation(contentRect.x, contentRect.y);
    }

    public Point computeSize(int wHint, int hHint, boolean changed)
    {
        Point size = iESWTTextLabel.computeSize(wHint, hHint, changed);
        Rectangle trim = computeTrim(0, 0, size.x, size.y);
        return new Point(trim.width, trim.height);
    }

    public Point getOrigin()
    {
        Point location = iESWTTextLabel.getLocation();
        return new Point(-location.x, -location.y);
    }

    public void setOrigin(Point origin)
    {
        setOrigin(origin.x, origin.y);
    }

    public void setOrigin(int x, int y)
    {
        if(iESWTTextLabel != null)
        {
            vBar.setSelection(y);
            iESWTTextLabel.setLocation(0, -vBar.getSelection());
        }
    }

    private void resize()
    {
        if(!resizing)
        {
            resizing = true;
            layout();
            resizing = false;
        }
    }

    private void vScroll()
    {
        Point location = iESWTTextLabel.getLocation();
        int vSelection = vBar.getSelection();
        iESWTTextLabel.setLocation(location.x, -vSelection);
    }
}
