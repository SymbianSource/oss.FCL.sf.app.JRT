/*******************************************************************************
 * Copyright (c) 2004, 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Yu You (Nokia Corp.)- initial API specification
 *     Nokia Corporation - S60 implementation
 *     Nokia Corporation - QT implementation
 *******************************************************************************/
package org.eclipse.ercp.swt.mobile;

import org.eclipse.swt.*;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.graphics.*;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.PackageProxy;
import org.eclipse.swt.internal.qt.QObjectDeleteWrapper;
import org.eclipse.swt.internal.qt.WidgetConstant;
import org.eclipse.swt.internal.qt.WidgetState;
import org.eclipse.swt.widgets.*;

import java.lang.Math;

/**
 *
 * A CaptionedControl is used to display a label (caption) in front of a
 * control. An optional trailing text can be used after the control, for
 * example, to indicate units of measurement.
 * <p>
 * The highlighting is implementation dependent. Typically, a focused
 * CaptionedControl will highlight all its elements, i.e. the leading label, the
 * control, and optional trailing label, when it gets focus. The positioning
 * order for the captions is determined by the <code>SWT.LEFT_TO_RIGHT</code>
 * and <code>SWT.RIGHT_TO_LEFT</code> styles hints.
 * </p>
 *
 * <p>
 * CaptionedControl does not support nested CaptionedControls. An exception will
 * be thrown when an instance of CaptionedControl is given as the constructor's
 * argument. To change the control contained by the CaptionedControl, a new
 * control can be created using the CaptionedControl as its parent control. The
 * old control must be disposed by the application manually. Only one control
 * can be active and visible in a CaptionedControl. Which control is displayed
 * when multiple controls have been added is implementation-dependent.
 * </p>
 * <p>
 * <em>Example code:</em>
 * </p>
 * <p>
 * <code><pre>
 * CaptionedControl control = new CaptionedControl(shell, SWT.NONE);
 * ConstraintedText currency = new ConstraintedText(control, SWT.NONE,
 *      ConstraintedText.NUMERIC);
 * control.setText(&quot;Velocity&quot;);
 * control.getTrailingText(&quot;cm&quot;);
 * </pre></code>
 * </p>
 *
 * <p>
 * <dl>
 * <dt><b>Styles: </b></dt>
 * <dd>SWT.LEFT_TO_RIGHT (default)</dd>
 * <dd>SWT.RIGHT_TO_LEFT</dd>
 * <dt><b>Events: </b></dt>
 * <dd>(none)</dd>
 * </dl>
 * <p>
 * Note: Even this class is a subclass of Composite, it does not make sense to
 * set a layout on it.
 * </p>
 * <p>
 * IMPORTANT: This class is not intended to be subclassed.
 * </p>
 *
 */
public final class CaptionedControl extends Composite {
    boolean initPhase;
    private int focusSignalProxy;
    private MouseListener mouseListener;
    private boolean monitorMouseEvent;

static final class CaptionedControlPackageProxy extends PackageProxy {
    public void createHandle(int index) {
        ((CaptionedControl)w).createHandle(index);
    }
    public void addControl(Control control) {
        ((CaptionedControl)w).addControl(control);
    }
    public void qt_signal_qapplication_focusChanged(int old, int now) {
        ((CaptionedControl)w).qt_signal_qapplication_focusChanged(old, now);
    }

    public void removeControl(Control control) {
        ((CaptionedControl)w).removeControl(control);
    }
}

class CaptionedControlLayout extends Layout {

int checkHint(int hint, int spacing) {

    // Adjust a hint by spacing, check that result is not negative
    if (hint == SWT.DEFAULT) {
        return hint;
    }

    hint -= spacing;

    if (hint < 0) {
        hint = 0;
    }

    return hint;
}


protected Point computeSize(Composite composite, int hint, int hint2, boolean flushCache) {
    Point res = new Point(0, 0);
    int wHint = checkHint(hint, 2 * margin);
    int hHint = checkHint(hint2, 2 * margin);

    CaptionedControlLayoutData data = getData(wHint, hHint);
    if ((getStyle() & SWT.HORIZONTAL) != 0) {
        if (hint == SWT.DEFAULT) {
            res.x = data.row1.x;
        }
        else {
            res.x = data.row1.x - data.child.x + hint;
        }
        if (hint2 == SWT.DEFAULT) {
            res.y = data.row1.y;
        }
        else {
            res.y = hint2;
        }
    }
    else {
        if (hint == SWT.DEFAULT) {
            res.x = Math.max(data.row1.x, data.row2.x);
        }
        else {
            res.x = data.trail.x + Math.max( data.row1.x, hint);
        }
        if (hint2 == SWT.DEFAULT) {
            res.y = data.row1.y + data.row2.y;
        }
        else {
            res.y = data.row1.y + Math.max(hint2, data.trail.y);
        }
    }

    // Since the margins are big enough, adding the border is pointless.
    res.x += 2 * margin;
    res.y += 2 * margin;

    return res;
}

protected void layout(Composite composite, boolean flushCache) {

    Point size = composite.getSize();
    if (size.x <= 0 || size.y <= 0) {
        return;
    }

    CaptionedControlLayoutData data = getData(size.x, size.y);

    int x = margin;
    int y = margin;
    int w = size.x - 2 * margin;
    int h = 0;

    if ((getStyle() & SWT.HORIZONTAL) != 0) {
        h = size.y - 2 * margin;
    }
    else {
        h = data.row1.y;
    }

    if (h < 0) h = 0;

    imageLabel.setBounds(x, y, data.img.x, h);
    x += data.img.x;

    titleLabel.setBounds(x, y, data.title.x, h);
    x += data.title.x;

    int trailW = data.trail.x;

    if ((getStyle() & SWT.VERTICAL) != 0) {
        y += h;
        h = size.y - data.row1.y - 2 * margin;
        if (h < 0) h = 0;
        x = margin;

        if (child == null) {
            trailW = data.row1.x - 2 * margin;
        }
    }

    int childW = 0;
    if (child != null) {
        if ((getStyle() & SWT.HORIZONTAL) != 0) {
            childW = w - data.img.x - data.title.x - data.trail.x;
        }
        else {
            childW = w - data.trail.x;
        }
        if (childW < 0) {
            childW = 0;
        }

        child.setBounds(x, y, childW, h);
        x += childW;
    }

    trailLabel.setBounds(x, y, trailW, h);
}

protected CaptionedControlLayoutData getData() {
    return getData(SWT.DEFAULT, SWT.DEFAULT);
}

protected CaptionedControlLayoutData getData(int wHint, int hHint) {
    CaptionedControlLayoutData res = new CaptionedControlLayoutData();

    int extra = spacing * 2;

    wHint = checkHint(wHint, extra);
    hHint = checkHint(hHint, extra);

    if ((getStyle() & SWT.HORIZONTAL) != 0) {
        if (imageLabel.getImage() != null) {
            res.img = imageLabel.computeSize(SWT.DEFAULT, SWT.DEFAULT);
        }
        String caption = titleLabel.getText();
        if (caption != null && caption.length() > 0) {
            res.title = titleLabel.computeSize(SWT.DEFAULT, SWT.DEFAULT);
        }
        String trail = trailLabel.getText();
        if (trail != null && trail.length() > 0) {
            res.trail = trailLabel.computeSize(SWT.DEFAULT, SWT.DEFAULT);
        }

    } else {
        if (imageLabel.getImage() != null) {
            res.img = imageLabel.computeSize(SWT.DEFAULT, SWT.DEFAULT);
        }
        String caption = titleLabel.getText();
        if (caption != null && caption.length() > 0) {

            if (wHint != SWT.DEFAULT) {
                int hint = wHint - res.img.x;
                if (hint < 0) {
                    hint = 0;
                }
                res.title = titleLabel.computeSize(hint, SWT.DEFAULT);
            } else {
                res.title = titleLabel.computeSize(SWT.DEFAULT, SWT.DEFAULT);
            }
        }

        String trail = trailLabel.getText();
        if (trail != null && trail.length() > 0) {
            res.trail = trailLabel.computeSize(SWT.DEFAULT, SWT.DEFAULT);
        }


    }

    if (res.img.x > 0 && res.img.y > 0) {
        res.img.x += extra;
        res.img.y += extra;
    }
    if (res.title.x > 0 && res.title.y > 0) {
        res.title.x += extra;
        res.title.y += extra;
    }
    if (res.trail.x > 0 && res.trail.y > 0) {
        res.trail.x += extra;
        res.trail.y += extra;
    }

    if ((getStyle() & SWT.HORIZONTAL) != 0) {
        if (child != null) {
            res.child = child.computeSize(SWT.DEFAULT, SWT.DEFAULT);

            if (wHint != SWT.DEFAULT) {
                res.child.x = wHint - res.img.x - res.title.x - res.trail.x;
                if (res.child.x < 0) {
                    res.child.x = 0;
                }
            }
        }
    } else {
        if (child != null) {
            if (wHint != SWT.DEFAULT) {
                res.child = child.computeSize(wHint-res.trail.x, SWT.DEFAULT);
            } else {
                res.child = child.computeSize(SWT.DEFAULT, SWT.DEFAULT);
            }

            if (hHint != SWT.DEFAULT) {
                res.child.y = hHint - res.row1.y;
            }
        }
    }

    if ((getStyle() & SWT.HORIZONTAL) != 0) {
        res.row1.x = res.img.x + res.title.x + res.child.x + res.trail.x;
        res.row1.y = Math.max(res.row1.y, res.img.y);
        res.row1.y = Math.max(res.row1.y, res.title.y);
        res.row1.y = Math.max(res.row1.y, res.trail.y);
        res.row1.y = Math.max(res.row1.y, res.child.y);
    }
    else {
        // If the first row would expand to be bigger than the contained
        // control, adjust title caption width.
        if ((child != null) && (res.img.x + res.title.x > res.child.x + res.trail.x)) {
            int width = res.child.x + res.trail.x - res.img.x;

            if (width < 0) {
                width = 0;
            }

            Point newTitle = titleLabel.computeSize(width, SWT.DEFAULT);

            // Adjust title size only if the adjustment causes line count to
            // change (i.e. title height increases)
            if (newTitle.y > res.title.y) {
                res.title = newTitle;
            }
        }

        res.row1.x = res.img.x + res.title.x;
        res.row1.y = Math.max(res.row1.y, res.img.y);
        res.row1.y = Math.max(res.row1.y, res.title.y);
        res.row2.x = res.child.x + res.trail.x;
        res.row2.y = Math.max(res.row2.y, res.trail.y);
        res.row2.y = Math.max(res.row2.y, res.child.y);
    }

    return res;
}
}

class CaptionedControlLayoutData {
    Point img = new Point(0, 0);
    Point title = new Point(0, 0);
    Point trail = new Point(0, 0);
    Point child = new Point(0, 0);
    Point row1 = new Point(0, 0);
    Point row2 = new Point(0, 0);
}

private Label imageLabel;
private Label titleLabel;
private Label trailLabel;
private Control child;
private final int margin = 5;
private final int spacing = 1;
private CaptionedControlLayout layout;

/**
 * Constructs a new instance of this class given its parent and a style
 * value describing its behavior and appearance.
 * <p>
 * The style value is either one of the style constants defined in class
 * <code>SWT</code> which is applicable to instances of this class, or must
 * be built by <em>bitwise OR</em> 'ing together (that is, using the
 * <code>int</code> "|" operator) two or more of those <code>SWT</code>
 * style constants. The class description lists the style constants that are
 * applicable to the class. Style bits are also inherited from superclasses.
 * </p>
 *
 * @param parent
 *            a widget which will be the parent of the new instance (cannot
 *            be null)
 * @param style
 *            the style of widget to construct
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the parent</li>
 *                <li>ERROR_INVALID_SUBCLASS - if this class is not an
 *                allowed subclass</li>
 *                </ul>
 *
 * @see SWT#LEFT_TO_RIGHT
 * @see SWT#RIGHT_TO_LEFT
 */
public CaptionedControl (Composite parent, int style) {
    super(parent, Internal_PackageSupport.checkBits(style, SWT.HORIZONTAL, SWT.VERTICAL, 0, 0, 0, 0), 0,
            new CaptionedControlPackageProxy(), false);
    if (parent instanceof CaptionedControl) {
        SWT.error(SWT.ERROR_INVALID_PARENT);
    }
    Internal_PackageSupport.setScrollAreaHandle(this, 0);
    initPhase = true;
    imageLabel = new Label(this, SWT.CENTER);
    if ((style & SWT.VERTICAL) != 0) {
        titleLabel = new Label(this, SWT.LEFT | SWT.WRAP);
    } else {
        titleLabel = new Label(this, SWT.CENTER);
    }
    trailLabel = new Label(this, SWT.CENTER);
    initPhase = false;



    layout = new CaptionedControlLayout();
    super.setLayout(layout);
    layout();
}


void addControl(Control control) {
    if (initPhase) {
        return;
    }

    int policy = OS.QWidget_focusPolicy(Internal_PackageSupport
            .handle(titleLabel));
    if (control instanceof Composite) {
        // let labels to have click focus when contained control is a
        // Composite
        if ((policy & OS.QT_FOCUSPOLICY_CLICKFOCUS) == 0) {
            policy |= OS.QT_FOCUSPOLICY_CLICKFOCUS;
            OS.QWidget_setFocusPolicy(Internal_PackageSupport
                    .handle(imageLabel), policy);
            OS.QWidget_setFocusPolicy(Internal_PackageSupport
                    .topHandle(titleLabel), policy);
            OS.QWidget_setFocusPolicy(Internal_PackageSupport
                    .topHandle(trailLabel), policy);
        }

        // forward focus to focused control if there is
        // otherwise, clean up focus proxy
        Control focusedControl = focusedDescendant(control);
        if (focusedControl != null) {

            OS.QWidget_setFocusProxy(Internal_PackageSupport
                    .topHandle(imageLabel), Internal_PackageSupport
                    .topHandle(focusedControl));
            OS.QWidget_setFocusProxy(Internal_PackageSupport
                    .topHandle(titleLabel), Internal_PackageSupport
                    .topHandle(focusedControl));
            OS.QWidget_setFocusProxy(Internal_PackageSupport
                    .topHandle(trailLabel), Internal_PackageSupport
                    .topHandle(focusedControl));
        } else {
            OS.QWidget_setFocusProxy(Internal_PackageSupport
                    .topHandle(imageLabel), Internal_PackageSupport
                    .topHandle(control));
            OS.QWidget_setFocusProxy(Internal_PackageSupport
                    .topHandle(titleLabel), Internal_PackageSupport
                    .topHandle(control));
            OS.QWidget_setFocusProxy(Internal_PackageSupport
                    .topHandle(trailLabel), Internal_PackageSupport
                    .topHandle(control));
        }
        if (focusSignalProxy == 0) {
            // starting to monitor focus change if it has not be started yet
            // from now on, focus changed event handler will update focus
            // proxy for the contained composite
            hookFocusChangedEvent();
        }
        // start to monitor mouse up event because composite can have button
        // or text editor
        // which needs sending selection event or launching virtual keypad
        // for it when tapping on a internal label
        enableMouseEventListening();
    } else {
        if (focusSignalProxy > 0) {
            // delete focus proxy object and stop monitoring focus change
            // event if contained control is no longer a composite
            QObjectDeleteWrapper.deleteSafely(focusSignalProxy);
            focusSignalProxy = 0;
        }

        // update labels focus policy according to the focus policy of
        // contained control
        int controlClickFocusPolicy = OS
                .QWidget_focusPolicy(Internal_PackageSupport
                        .handle(control))
                & OS.QT_FOCUSPOLICY_CLICKFOCUS;
        if (controlClickFocusPolicy != (policy & OS.QT_FOCUSPOLICY_CLICKFOCUS)) {
            policy |= controlClickFocusPolicy;
            OS.QWidget_setFocusPolicy(Internal_PackageSupport
                    .handle(imageLabel), policy);
            OS.QWidget_setFocusPolicy(Internal_PackageSupport
                    .topHandle(titleLabel), policy);
            OS.QWidget_setFocusPolicy(Internal_PackageSupport
                    .topHandle(trailLabel), policy);
        }

        // update focus proxies of internal labels according to contained
        // control focus policy
        if (controlClickFocusPolicy != 0) {

            OS.QWidget_setFocusProxy(Internal_PackageSupport
                    .topHandle(imageLabel), Internal_PackageSupport
                    .topHandle(control));
            OS.QWidget_setFocusProxy(Internal_PackageSupport
                    .topHandle(titleLabel), Internal_PackageSupport
                    .topHandle(control));
            OS.QWidget_setFocusProxy(Internal_PackageSupport
                    .topHandle(trailLabel), Internal_PackageSupport
                    .topHandle(control));
        } else {
            OS.QWidget_setFocusProxy(Internal_PackageSupport
                    .topHandle(imageLabel), 0);
            OS.QWidget_setFocusProxy(Internal_PackageSupport
                    .topHandle(titleLabel), 0);
            OS.QWidget_setFocusProxy(Internal_PackageSupport
                    .topHandle(trailLabel), 0);
        }
        // start to monitor mouse up event if contained control is button or
        // editor
        if (control instanceof Button
                || (OS.windowServer == OS.WS_SYMBIAN_S60
                        && OS.QApplication_autoSipEnabled() && OS
                        .QWidget_testAttribute(Internal_PackageSupport
                                .topHandle(control),
                                OS.QT_WA_INPUTMETHODENABLED))) {
            enableMouseEventListening();
        } else {
            disableMouseEventListening();
        }
    }
    child = control;
}

public Point computeSize(int wHint, int hHint, boolean changed) {
    checkWidget();
    if (changed) {
        OS.QWidget_updateGeometry(topHandle());
    }
    if (layout != null) {
        return layout.computeSize(this, wHint, hHint, changed);
    }
    else {
        return new Point(WidgetConstant.DEFAULT_WIDTH, WidgetConstant.DEFAULT_HEIGHT);
    }
}

public Rectangle computeTrim(int x, int y, int width, int height) {
    checkWidget();

    Rectangle res = new Rectangle(0, 0, 0, 0);

    if (layout != null) {
        CaptionedControlLayoutData data = layout.getData();

        if ((getStyle() & SWT.HORIZONTAL) != 0) {
            res.width = data.img.x + data.title.x + data.trail.x + width;
            res.height = height;
            res.x = x - data.img.x - data.title.x - margin;
            res.y = y - margin;
        }
        else {
            res.width = data.trail.x + width;
            res.height = data.row1.y + height;
            res.x = x - margin;
            res.y = y - data.row1.y - margin;
        }
    }
    else {
        res.width = width;
        res.height = height;
        res.x = x;
        res.y = y;
    }
    res.width += 2 * margin;
    res.height += 2 * margin;

    return res;
}

void createHandle (int index) {
    int handle = OS.QCaptionedWidget_new();
    Internal_PackageSupport.setFrameHandle(this, handle);
    Internal_PackageSupport.setHandle(this, handle);
    Internal_PackageSupport.setTopHandle(this, handle);
    int policy = OS.QWidget_focusPolicy(handle) & ~OS.QT_FOCUSPOLICY_CLICKFOCUS;
    policy |= OS.QT_FOCUSPOLICY_TABFOCUS;
    OS.QWidget_setFocusPolicy(handle, policy);
    Internal_PackageSupport.setState(this, Internal_PackageSupport.state(this) | WidgetState.HANDLE);
    Internal_PackageSupport.setState(this, Internal_PackageSupport.state(this) & ~WidgetState.CANVAS);
}

private void disableMouseEventListening(){
    if(monitorMouseEvent){
        removeMouseListener(mouseListener);
        monitorMouseEvent = false;
        mouseListener = null;
    }
}

private void enableMouseEventListening()
{
    if (mouseListener == null) {
        mouseListener = new MouseListener() {
            public void mouseDoubleClick(MouseEvent e) {
            }

            public void mouseDown(MouseEvent e) {
            }

            // tapping on a internal label should have the same effect as
            // tapping on contained control
            public void mouseUp(MouseEvent e) {
                if (child != null && e.button == 1) {
                    if (child instanceof Button) {
                        // tapping on a internal label should also generate
                        // a selection event
                        if (child.getEnabled()) {
                            Internal_PackageSupport.sendEvent(child,
                                    SWT.Selection);
                            int style = child.getStyle();
                            if ((style & SWT.CHECK) != 0
                                    || (style & SWT.RADIO) != 0) {
                                Button button = (Button) child;
                                button.setSelection(!button.getSelection());
                            }
                        }
                    } else if (OS.windowServer == OS.WS_SYMBIAN_S60
                            && OS.QApplication_autoSipEnabled()) {
                        // tapping on a internal label should also launch
                        // virtual keypad
                        Control focusedControl = focusedDescendant(child);
                        if (focusedControl != null) {
                            int focusedControlTopHandle = Internal_PackageSupport
                                    .topHandle(focusedControl);
                            if (OS.QWidget_testAttribute(
                                    focusedControlTopHandle,
                                    OS.QT_WA_INPUTMETHODENABLED)) {
                                int eventHandle = OS
                                        .QEvent_new(OS.QEVENT_REQUESTSOFTWAREINPUTPANEL);
                                // this line deletes the event created by
                                // above line
                                OS.QCoreApplication_sendEvent(OS
                                        .QCoreApplication_instance(),
                                        focusedControlTopHandle,
                                        eventHandle); // launch virtual
                                // keypad
                            }
                        }

                    }
                }
            }
        };
    }
    if (!monitorMouseEvent) {
        addMouseListener(mouseListener);
        monitorMouseEvent = true;
    }
}


private Control focusedDescendant(Control control) {
    if (control == null)
        return null;
    Control focusedControl = getDisplay().getFocusControl();
    if (focusedControl == null)
        return null;

    if (isAncestorDescendantRelation(control, focusedControl)) {
        return focusedControl;
    }

    return null;
}

public Control[] getChildren() {
    checkWidget();
    Control[] children = super.getChildren();
    Control[] res = new Control[children.length - 3];
    for (int i = 0, j = 0; i < children.length; i++) {
        if (children[i] != imageLabel
            && children[i] != titleLabel
            && children[i] != trailLabel ) {
            res[j] = children[i];
            j++;
        }
    }
    return res;
}

public Rectangle getClientArea() {
    checkWidget();

    Rectangle res = new Rectangle(0, 0, 0, 0);

    Point size = getSize();
    size.x -= 2 * margin;
    size.y -= 2 * margin;

    if (layout != null) {

        CaptionedControlLayoutData data = layout.getData(size.x, size.y);
        if ((getStyle() & SWT.HORIZONTAL) != 0) {
            res.width = size.x - data.img.x - data.title.x - data.trail.x;
            res.height = size.y;
            res.x = data.img.x + data.title.x + margin;
            res.y = margin;
        }
        else {
            res.width = size.x - data.trail.x;
            res.height = size.y - data.row1.y;
            res.x = margin;
            res.y = data.row1.y + margin;
        }
    }
    else {
        res.width = size.x;
        res.height = size.y;
    }

    if (res.width < 0) res.width = 0;
    if (res.height < 0) res.height = 0;

    return res;
}

/**
 * Returns the CaptionedControl's icon image, or null if it has never been
 * set.
 *
 * @return the icon image or null.
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 * @see #setImage(Image)
 */
public Image getImage () {
    checkWidget();
    return imageLabel.getImage();
}

/**
 * Gets the caption text, which will be an empty string if it has never been
 * set.
 *
 * @return The label text.
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the parent</li>
 *                </ul>
 *
 * @see #setText(java.lang.String)
 */
public String getText () {
    checkWidget();
    return titleLabel.getText();
}

/**
 * Gets the trailing text, which will be an empty string if it has never
 * been set.
 *
 * @return The trailing text.
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the parent</li>
 *                </ul>
 *
 * @see #setTrailingText(java.lang.String)
 */
public String getTrailingText () {
    checkWidget();
    return trailLabel.getText();
}

public void pack(boolean changed) {
        layout(changed);
        super.pack(changed);
}

void qt_signal_qapplication_focusChanged(int old, int now) {
    Control focusedControl = now == 0 ? null
            : (Control) Internal_PackageSupport.getDWidget(this
                    .getDisplay(), now);
    Control lostFocusControl = old == 0 ? null
            : (Control) Internal_PackageSupport.getDWidget(this
                    .getDisplay(), old);
    if (focusedControl != null
            && isAncestorDescendantRelation(child, focusedControl)) {
        OS.QWidget_setFocusProxy(topHandle(), Internal_PackageSupport
                .topHandle(focusedControl));
        OS.QWidget_setFocusProxy(Internal_PackageSupport
                .topHandle(imageLabel), Internal_PackageSupport
                .topHandle(focusedControl));
        OS.QWidget_setFocusProxy(Internal_PackageSupport
                .topHandle(titleLabel), Internal_PackageSupport
                .topHandle(focusedControl));
        OS.QWidget_setFocusProxy(Internal_PackageSupport
                .topHandle(trailLabel), Internal_PackageSupport
                .topHandle(focusedControl));
        return;
    }

    if (lostFocusControl != null
            && isAncestorDescendantRelation(child, lostFocusControl)) {
        OS.QWidget_setFocusProxy(topHandle(), Internal_PackageSupport
                .topHandle(child));
        OS.QWidget_setFocusProxy(Internal_PackageSupport
                .topHandle(imageLabel), Internal_PackageSupport
                .topHandle(child));
        OS.QWidget_setFocusProxy(Internal_PackageSupport
                .topHandle(titleLabel), Internal_PackageSupport
                .topHandle(child));
        OS.QWidget_setFocusProxy(Internal_PackageSupport
                .topHandle(trailLabel), Internal_PackageSupport
                .topHandle(child));
    }
}

void removeControl (Control control) {
    if (control != null && control == child) {
        child = null;
    }
}

/**
 * Sets the image as an icon to the CaptionedControl. The icon can co-exist
 * with caption text. The icon position is platform-dependent.
 *
 * <p>
 * The parameter can be null indicating that no image should be displayed.
 * The implementation will adjust the image size to make it best fit the
 * CaptionedControl.
 * </p>
 *
 * @param image
 *            the image to display on the receiver
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_INVALID_ARGUMENT - if the image has been
 *                disposed</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 */
public void setImage (Image image) {
    checkWidget();
    if (image != null && image.isDisposed()) {
        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    }
    imageLabel.setImage(image);
    layout();
}

public void setLayout (Layout layout) {
    // Even though this class is a subclass of Composite, setting a layout
    // on it is not supported.
    checkWidget();
}

/**
 * Sets the caption label
 *
 * @param string
 *            the new caption label
 *
 * @throws java.lang.IllegalArgumentException
 *             <code>ERROR_NULL_ARGUMENT</code> if the text is null
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the parent</li>
 *                </ul>
 * @see #getText
 */
public void setText (java.lang.String string) {
    checkWidget();
    if (string == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    titleLabel.setText(string);
    layout();
}

/**
 * Sets the trailing label
 *
 * @param string
 *            the new trailing label
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the parent</li>
 *                </ul>
 *
 * @see #getTrailingText
 */
public void setTrailingText (java.lang.String string) {
    checkWidget();
    if (string == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    trailLabel.setText(string);
    layout();
}

private final int handle() {
    return Internal_PackageSupport.handle(this);
}


void hookFocusChangedEvent() {
    focusSignalProxy = OS.SignalHandler_new(topHandle(),
            OS.QSIGNAL_QAPPLICATION_FOCUSCHANGED_TO_WIDDGET);
    OS.QObject_connectOrThrow(OS.QCoreApplication_instance(),
            "focusChanged(QWidget*, QWidget*)", focusSignalProxy,
            "widgetSignal(QWidget*, QWidget*)", OS.QT_AUTOCONNECTION);
}

boolean isAncestorDescendantRelation(Control ancestor, Control descendant) {
    Control control = descendant;
    while (control != null && control != ancestor) {
        control = control.getParent();
    }
    return (control != null);
}

private final int topHandle() {
    return Internal_PackageSupport.topHandle(this);
}
}
