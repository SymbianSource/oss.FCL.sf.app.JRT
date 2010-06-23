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
package org.eclipse.swt.widgets;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ControlListener;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.events.TraverseListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;

/**
 * A Proxy Class for eSWT control.
 *
 * Since re-parenting is not supported, thats why this class were needed.
 * When user will call the setParent function of Control(means this class), this class will create
 * the actual Control, for which it will be acting like a proxy. All further call to any function of
 * this class will be delegated to the Control object, created in the setParent method.
 *
 * @author d35kumar
 */
public class ProxyControl extends Control
{

    /**
     *  this is actual Control(Shell), for which this class is acting like proxy
     */
    Control iControl;

    /**
     * Dimesion of this shell needs to be set
     */
    Point iDimension;

    /**
     * @param aDimension Dimension to be set for the control class
     */
    public ProxyControl(Point aDimension)
    {
        iDimension=aDimension;
    }

    /**
     * Creates the actual control
     * @return true if it returns properly,
     *  or false if any exception is thrown during the execution of this function
     */
    public boolean setParent(Composite aParent)
    {
        if (aParent == null)
        {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
            return false;
        }
        if (aParent.isDisposed())
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            return false;
        }
//        if (this.parent == aParent)
//        {
//            return true;
//        }
        iControl= new Shell((Shell)aParent);
        ((Shell)iControl).open();
        iControl.setSize(iDimension);
        return true;
    }

    /**
     * Passes the call to iControl, if it is not null, otherwise return silently
     */
    public void addControlListener(ControlListener aListener)
    {
        if (iControl!=null)
        {
            iControl.addControlListener(aListener);
        }
    }
    /**
     *
     */
    public void addFocusListener(FocusListener listener)
    {
        if (iControl!=null)iControl.addFocusListener(listener);
    }
    /**
     *
     */
    public void addKeyListener(KeyListener listener)
    {
        if (iControl!=null)iControl.addKeyListener(listener);
    }
    public void addMouseListener(MouseListener listener)
    {
        if (iControl!=null)iControl.addMouseListener(listener);
    }
    public void addMouseMoveListener(MouseMoveListener listener)
    {
        if (iControl!=null)iControl.addMouseMoveListener(listener);
    }
    /**
     *
     */
    public void addPaintListener(PaintListener aListener)
    {
        if (iControl!=null)iControl.addPaintListener(aListener);

    }

    public void addTraverseListener(TraverseListener listener)
    {
        if (iControl!=null)iControl.addTraverseListener(listener);
    }
    public Point computeSize(int wHint,
                             int hHint)
    {
        Point point=null;
        if (iControl!=null)point=iControl.computeSize(wHint, hHint);
        return point;
    }

    public Point computeSize(int wHint,
                             int hHint,
                             boolean changed)
    {
        Point point=null;
        if (iControl!=null)point=iControl.computeSize(wHint, hHint,changed);
        return point;
    }

    public Color getBackground()
    {
        Color color=null;
        if (iControl!=null)color=iControl.getBackground();
        return color;
    }
    public int getBorderWidth()
    {
        if (iControl!=null)return iControl.getBorderWidth();
        else return 0;
    }
    public Rectangle getBounds()
    {
        if (iControl!=null) return iControl.getBounds();
        else return null;
    }
    public boolean getEnabled()
    {
        if (iControl!=null) return iControl.getEnabled();
        else return false;
    }
    public Font getFont()
    {
        if (iControl!=null)return iControl.getFont();
        else return null;
    }
    public Color getForeground()
    {
        if (iControl!=null) return iControl.getForeground();
        else return null;
    }

    public java.lang.Object getLayoutData()
    {
        if (iControl!=null) return iControl.getLayoutData();
        else return null;
    }

    public Point getLocation()
    {
        if (iControl!=null) return iControl.getLocation();
        else return null;
    }

    public Menu getMenu()
    {
        if (iControl!=null) return iControl.getMenu();
        else return null;
    }
    public Composite getParent()
    {
        if (iControl!=null) return iControl.getParent();
        else return null;
    }
    public Shell getShell()
    {
        if (iControl!=null) return (Shell)iControl;
        else return null;
    }
    public Point getSize()
    {
        if (iControl!=null) return iControl.getSize();
        else return null;
    }

    public java.lang.String getToolTipText()
    {
        if (iControl!=null) return iControl.getToolTipText();
        else return null;
    }
    public boolean getVisible()
    {
        if (iControl!=null)  return iControl.getVisible();
        else return false;
    }
    public boolean isEnabled()
    {
        if (iControl!=null) return iControl.isEnabled();
        else return false;
    }

    public boolean isFocusControl()
    {
        if (iControl!=null) return iControl.isFocusControl();
        else return false;
    }
    public boolean isReparentable()
    {
        if (iControl!=null) return iControl.isReparentable();
        else return false;
    }

    public boolean isVisible()
    {
        if (iControl!=null)return iControl.isVisible();
        else return false;
    }
    public void moveAbove(Control control)
    {
        if (iControl!=null) iControl.moveAbove(control);
    }
    public void moveBelow(Control control)
    {
        if (iControl!=null) iControl.moveBelow(control);
    }
    public void pack()
    {
        if (iControl!=null) iControl.pack();
    }
    public void pack(boolean changed)
    {
        if (iControl!=null) iControl.pack(changed);
    }
    public void redraw()
    {
        if (iControl!=null) iControl.redraw();
    }
    public void redraw(int x,
                       int y,
                       int width,
                       int height,
                       boolean all)
    {
        if (iControl!=null) iControl.redraw(x,y,width,height, all);
    }
    public void removeControlListener(ControlListener listener)
    {
        if (iControl!=null) iControl.removeControlListener(listener);
    }
    public void removeFocusListener(FocusListener listener)
    {
        if (iControl!=null) iControl.removeFocusListener(listener);
    }
    public void removeMouseListener(MouseListener listener)
    {
        if (iControl!=null) iControl.removeMouseListener(listener);
    }
    public void removeMouseMoveListener(MouseMoveListener listener)
    {
        if (iControl!=null) iControl.removeMouseMoveListener(listener);
    }
    public void removePaintListener(PaintListener listener)
    {
        if (iControl!=null)iControl.removePaintListener(listener);
    }
    public void removeTraverseListener(TraverseListener listener)
    {
        if (iControl!=null)iControl.removeTraverseListener(listener);
    }
    public void setBounds(int x,
                          int y,
                          int width,
                          int height)
    {
        if (iControl!=null)iControl.setBounds(x, y, width, height);
    }
    public void setBounds(Rectangle rect)
    {
        if (iControl!=null)iControl.setBounds(rect);
    }
    public void setCapture(boolean capture)
    {
        if (iControl!=null)iControl.setCapture(capture);
    }
    public void setEnabled(boolean enabled)
    {
        if (iControl!=null)iControl.setEnabled(enabled);
    }
    public boolean setFocus()
    {
        if (iControl!=null) return iControl.setFocus();
        else return false;
    }
    public void setFont(Font font)
    {
        if (iControl!=null)iControl.setFont(font);
    }
    public void setForeground(Color color)
    {
        if (iControl!=null)iControl.setForeground(color);
    }
    public void setLayoutData(java.lang.Object layoutData)
    {
        if (iControl!=null)iControl.setLayoutData(layoutData);
    }
    public void setLocation(int x,
                            int y)
    {
        if (iControl!=null)iControl.setLocation(x, y);
    }

    public void setLocation(Point location)
    {
        if (iControl!=null)iControl.setLocation(location);
    }
    public void setMenu(Menu menu)
    {
        if (iControl!=null)iControl.setMenu(menu);
    }
    public void setRedraw(boolean redraw)
    {
        if (iControl!=null)iControl.setRedraw(redraw);
    }
    public void setSize(int width,
                        int height)
    {
        if (iControl!=null) iControl.setSize(width, height);
    }

    public void setSize(Point size)
    {
        if (iControl!=null) iControl.setSize(size);
    }
    public void setToolTipText(java.lang.String string)
    {
        if (iControl!=null)iControl.setToolTipText(string);
    }
    public void setVisible(boolean visible)
    {
        if (iControl!=null)iControl.setVisible(visible);
    }
    public Point toControl(int x,
                           int y)
    {
        if (iControl!=null)return iControl.toControl(x, y);
        else return null;
    }
    public Point toControl(Point point)
    {
        if (iControl!=null) return iControl.toControl(point);
        else return null;
    }
    public Point toDisplay(int x,
                           int y)
    {
        if (iControl!=null)return iControl.toDisplay(x,y);
        else return null;
    }
    public Point toDisplay(Point point)
    {
        if (iControl!=null)return iControl.toDisplay(point);
        else return null;
    }
    public boolean traverse(int traversal)
    {
        if (iControl!=null)return iControl.traverse(traversal);
        else return false;
    }
    public void update()
    {
        if (iControl!=null)iControl.update();
    }

    public void checkSubclass()
    {
        // do nothing,sub-classing allowed
    }

    protected void internal_createHandle()
    {
        // TODO Auto-generated method stub

    }
    /**
     * Used in toString function
     * @return class name with full path
     */
    String getNameText()
    {
        return "org.eclipse.swt.widgets.ProxyControl";
    }
    public String toString()
    {
        return "org.eclipse.swt.widgets.ProxyControl";
    }

}
