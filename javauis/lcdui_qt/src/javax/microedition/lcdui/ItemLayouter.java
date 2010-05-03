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

import org.eclipse.ercp.swt.mobile.CaptionedControl;
import org.eclipse.ercp.swt.mobile.MobileShell;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.*;

/**
 * Abstract base class for Item layouters.
 */
abstract class ItemLayouter {

    /**
     * Key name for paint listener.
     */
    private static final String FOCUS_LISTENER = "itemfocus";

    protected static final String MIN_TEXT = "...";

    protected DefaultFormInteraction dfi;

    protected Composite formComposite;

    private static Label staticLabel;
    private static CaptionedControl staticCC;
    private static MobileShell staticShell;

    private static Point captionedTrim;

    protected static int formWidth;
    protected static int formHeigh;

    /**
     * Gets static singleton off-screen Shell which can be used by item
     * layouters when creating temporary Controls.
     *
     * @return Static Shell. Never null.
     */
    static MobileShell eswtGetStaticShell() {
        if (staticShell == null) {
            staticShell = new MobileShell(ESWTUIThreadRunner.getInstance()
                    .getDisplay(), SWT.SYSTEM_MODAL | SWT.VERTICAL);
            staticShell.getVerticalBar().setVisible(true);
            formWidth = staticShell.getClientArea().width;
            formHeigh = staticShell.getClientArea().height;
        }
        return staticShell;
    }

    /**
     * Gets static singleton off-screen Label control.
     */
    private static Label eswtGetStaticLabel() {
        if (staticLabel == null) {
            staticLabel = new Label(eswtGetStaticShell(), SWT.NONE);
        }
        return staticLabel;
    }

    /**
     * Gets static singleton off-screen Captioned control.
     */
    private static CaptionedControl eswtGetStaticCC() {
        if (staticCC == null) {
            staticCC = new CaptionedControl(eswtGetStaticShell(), SWT.VERTICAL);
        }
        return staticCC;
    }

    /**
     * The static "layouting" shell's size is updated.
     */
    static void eswtUpdateStaticShellSize(int width, int height) {
        if (staticShell != null) {
            staticShell.setBounds(staticShell.computeTrim(0, 0, width, height));
            formWidth = width;
            formHeigh = height;
        }
    }

    /**
     * Constructor.
     *
     * @param dflp - DefaultFormLayoutPolicy used for layouting.
     */
    ItemLayouter(DefaultFormLayoutPolicy dflp) {
        this.dfi = (DefaultFormInteraction) dflp;
        formComposite = dflp.getForm().getFormComposite();
        ESWTUIThreadRunner.syncExec(new Runnable() {
            public void run() {
                ItemLayouter.eswtGetStaticShell();
            }
        });
    }

    /**
     * Label alignment directive.
     */
    int eswtGetLabelAlignmentDirective() {
        return dfi.getLanguageSpecificLayoutDirective();
    }

    /**
     * Layout Item in a row.
     *
     * @param row current Row
     * @param item Item to layout
     */
    void eswtLayoutItem(Row row, Item item) {
        LayoutObject lo = new LayoutObject(item, eswtGetCaptionedControl(item));
        dfi.eswtAddNewLayoutObject(lo);
    }

    /**
     * Wraps this item's control in the necessary composites.<br>
     * Based on the item, the result of this method can be:
     * <li> specific Control
     * <li> labeled CaptionedControl (containing the specific Control) <br>
     * <br>
     * The method will set the size of these using the eswtCaptionedResize
     * method.
     *
     * @param item Item to be layouted
     */
    final Control eswtGetCaptionedControl(Item item) {
        if (item.hasLabel()) {
            CaptionedControl captioned = new CaptionedControl(formComposite, SWT.VERTICAL);
            captioned.setText(item.getLabel());
            eswtGetControl(captioned, item);
            eswtCaptionedResize(item, captioned, item.getLayoutWidth(), item.getLayoutHeight());
            return captioned;
        }
        else {
            Control ret = eswtGetControl(formComposite, item);
            eswtCaptionedResize(item, ret, item.getLayoutWidth(), item.getLayoutHeight());
            return ret;
        }
    }

    /**
     * This abstract method creates the eSWT control.
     *
     * @param parent where to create
     * @param item on which it is based
     * @return Control
     */
    abstract Control eswtGetControl(Composite parent, Item item);

    /**
     * Update size of an LayoutObject.
     */
    final void eswtResizeObject(LayoutObject lo) {
        Item item = lo.getOwningItem();
        eswtCaptionedResize(item, lo.getControl(), item.getLayoutWidth(), item.getLayoutHeight());
        lo.eswtUpdateSize();
    }

    /**
     * Set the size of a LayoutObject.
     *
     * @param lo LayoutObject
     * @param width
     * @param height
     */
    final void eswtResizeObject(LayoutObject lo, int width, int height) {
        eswtCaptionedResize(lo.getOwningItem(), lo.getControl(), width, height);
        lo.eswtUpdateSize();
    }

    final void eswtCaptionedResize(Item item, Control control, int width, int height) {
        if (control instanceof CaptionedControl) {
            CaptionedControl cc = (CaptionedControl) control;
            cc.setSize(width, height);
            Rectangle ccArea = cc.getClientArea();
            eswtResizeControl(item, eswtFindSpecificControl(item, control),
                    ccArea.width, ccArea.height);
        }
        else {
            eswtResizeControl(item, eswtFindSpecificControl(item, control),
                    width, height);
        }
    }

    /**
     * Should be implemented in sub-classes where resizing might happen.
     *
     * @param item Item
     * @param control layouted Control
     * @param width item width.
     * @param height item height
     */
    void eswtResizeControl(Item item, Control control, int width, int height) {
        control.setSize(width, height);
    }

    /**
     * Add listeners to layouted control.
     *
     * @param item Item
     * @param lo LayoutObject
     */
    void eswtAddListeners(Item item, LayoutObject lo) {
        lo.eswtAddSelectionListenerForCommands();
        Control specific = eswtFindSpecificControl(item, lo.getControl());
        if (specific != null) {
            eswtAddSpecificListeners(item, specific);
        }
        else {
            Logger.warning(this + "::eswtAddListeners didnt find control for " + item);
        }
    }

    /**
     * Add listeners to Layouter specific control.
     *
     * @param item Item
     * @param control specific Control
     */
    void eswtAddSpecificListeners(Item item, Control control) {
        if (item.isFocusable()) {
            ItemFocusListener ifl = new ItemFocusListener(item);
            control.addFocusListener(ifl);
            control.setData(FOCUS_LISTENER, ifl);
        }
    }

    /**
     * Remove listeners from layouted control.
     *
     * @param item Item
     * @param lo LayoutObject
     */
    void eswtRemoveListeners(Item item, LayoutObject lo) {
        lo.eswtRemoveSelectionListenerForCommands();
        Control specific = eswtFindSpecificControl(item, lo.getControl());
        if (specific != null) {
            eswtRemoveSpecificListeners(item, specific);
        }
        else {
            Logger.warning(this + "::eswtRemoveListeners didnt find control for " + item);
        }
    }

    /**
     * Remove listeners from Layouter specific control.
     *
     * @param item Item
     * @param control specific Control
     */
    void eswtRemoveSpecificListeners(Item item, Control control) {
        if (item.isFocusable()) {
            ItemFocusListener ifl = (ItemFocusListener) control
                    .getData(FOCUS_LISTENER);
            if (ifl != null) {
                control.removeFocusListener(ifl);
                control.setData(FOCUS_LISTENER, null);
            }
        }
    }

    /**
     * Update control of an Item.
     *
     * @param item Item to update
     * @param reason reason of update
     * @param param optional parameter
     */
    final void updateItem(final Item item, final Control control,
            final int reason, final Object param) {
        ESWTUIThreadRunner.syncExec(new Runnable() {
            public void run() {
                Control specific = eswtFindSpecificControl(item, control);
                if (specific != null) {
                    if (!specific.isDisposed()) {
                        eswtUpdateItem(item, specific, reason, param);
                    }
                    else {
                        Logger.warning(ItemLayouter.this
                                + "::updateItem found a disposed widget for " + item);
                    }
                }
                else {
                    Logger.warning(ItemLayouter.this
                            + "::updateItem didnt find control for " + item);
                }
            }
        });
    }

    /**
     * This abstract method updates the eSWT control.
     *
     * @param item Item to update
     * @param control layouted eSWT control
     * @param reason reason of update
     * @param param optional parameter
     */
    abstract void eswtUpdateItem(Item item, Control control, int reason,
            Object param);

    /**
     * Finds the Layouter specific eSWT control in the eSWT Composite tree.
     *
     * @param item Item
     * @param control eSWT control
     * @return a specific control or null if not found
     */
    final Control eswtFindSpecificControl(Item item, Control control) {
        Control ret = null;
        if (eswtIsSpecificControl(item, control)) {
            ret = control;
        }
        else if (control != null && control instanceof Composite) {
            Control[] children = ((Composite) control).getChildren();
            for (int i = 0; i < children.length; i++) {
                Control result = eswtFindSpecificControl(item, children[i]);
                if (result != null) {
                    ret = result;
                    break;
                }
            }
        }
        return ret;
    }

    /**
     * Returns the unlocked preferred size needed to display an Item.
     * Subclasses may overwrite this. By default returns (0,0).
     *
     * @param item Item.
     * @return Preferred area needed to display Item. x is width
     *      and y is height.
     */
    static Point calculatePreferredBounds(Item item) {
        return new Point(0, 0);
    }

    /**
     * Returns if this eSWT control is Layouter specific.
     *
     * @param item Item
     * @param control eSWT control
     * @return true if this is Layouter specific
     */
    abstract boolean eswtIsSpecificControl(Item item, Control control);

    /**
     * Gets the first control of an Item.
     *
     * @param item Item
     * @return layouted Control
     */
    Control eswtGetFirstControl(Item item) {
        LayoutObject lo = dfi.getFirstLayoutObjectOfItem(item);
        if (lo != null) {
            return lo.getControl();
        }
        return null;
    }

    /**
     * Gets the first layouter specific control of an Item.
     *
     * @param item Item
     * @return layouted specific Control
     */
    Control eswtGetFirstSpecificControl(Item item) {
        LayoutObject lo = dfi.getFirstLayoutObjectOfItem(item);
        if (lo != null) {
            Control control = lo.getControl();
            if (control != null) {
                return eswtFindSpecificControl(item, control);
            }
        }
        return null;
    }

    /**
     * Offers a key event to be consumed by the control.<br>
     * If the key is not consumed (default) then it's used for scrolling.
     *
     * @param item Item
     * @param key eSWT key code
     * @param type eSWT key type
     * @return if the key was consumed or not
     */
    boolean eswtOfferKeyEvent(Item item, int key, int type) {
        if (type == SWT.KeyDown) {
            return eswtOfferKeyPressed(item, key);
        }
        else if (type == SWT.KeyUp) {
            return eswtOfferKeyReleased(item, key);
        }
        else {
            return eswtOfferKeyRepeated(item, key);
        }
    }

    boolean eswtOfferKeyPressed(Item item, int key) {
        // Do not consume these by default
        return false;
    }

    boolean eswtOfferKeyRepeated(Item item, int key) {
        // Do not consume these by default
        return false;
    }

    boolean eswtOfferKeyReleased(Item item, int key) {
        // Do not consume these by default
        return false;
    }

    /**
     * Processing for item when it gets focus.
     *
     * @param item
     * @param dir
     */
    void eswtFocusGained(Item item, int dir) {
        Logger.method(item, "focusGained", String.valueOf(dir));
        item.internalSetFocused(true);
    }

    /**
     * Processing for item when it looses focus.
     *
     * @param item item which looses focus.
     */
    void eswtFocusLost(Item item) {
        Logger.method(item, "focusLost");
        item.internalSetFocused(false);
    }

    final void eswtHandleVisibilityChange(Item item, boolean visible) {
        if (item.isVisible() != visible) {
            item.internalSetVisible(visible);
            if (visible) {
                eswtHandleShow(item);
            }
            else {
                eswtHandleHide(item);
            }
        }
    }

    /**
     * Special processing of Item when it becomes visible.
     *
     * @param item which becomes visible.
     */
    void eswtHandleShow(Item item) {
        // Implementation not needed. Subclasses may override.
    }

    /**
     * Special processing for item which becomes not visible due to scrolling.
     *
     * @param item which becomes hidden.
     */
    void eswtHandleHide(Item item) {
        // Implementation not needed. Subclasses may override.
    }

    static void applyMinMargins(Item item, Point size) {
        if (item.hasLabel()) {
            applyCaptionedTrim(MIN_TEXT, size);
        }
        size.x = Math.min(size.x, formWidth);
    }

    static void applyPrefMargins(Item item, Point size) {
        if (item.hasLabel()) {
            applyCaptionedTrim(item.getLabel(), size);
        }
        size.x = Math.min(size.x, formWidth);
    }


    static final void applyCaptionedTrim(final String text, Point size) {
        final Point localSize = size;
        ESWTUIThreadRunner.syncExec(new Runnable() {
            public void run() {
                CaptionedControl cc = eswtGetStaticCC();
                cc.setText(text);
                Rectangle rect = cc.computeTrim(0, 0, localSize.x, localSize.y);
                captionedTrim = new Point(rect.width, rect.height);
            }
        });
        size.x = captionedTrim.x;
        size.y = captionedTrim.y;
    }

    /**
     * Applies the label size to the unlocked preferred area needed to display
     * an Item.
     *
     * @param size
     *            Point where the Label's size is added.
     * @param item
     *            Item containing the label.
     */
    static Point getLabelSize(final String labelStr) {
        final Point size = new Point(0, 0);
        ESWTUIThreadRunner.syncExec(new Runnable() {
            public void run() {
                Label label = eswtGetStaticLabel();
                label.setText(labelStr);
                Point temp = label.computeSize(SWT.DEFAULT, SWT.DEFAULT);
                size.x = Math.min(temp.x, formWidth);
                size.y = temp.y;
            }
        });
        return size;
    }

    /**
     * Calculate X position based on horizontal layout.
     *
     * @param owningWidth owning area width
     * @param objectWidth object's width
     * @param horizontalLayout horizontal layout
     * @return x-position of object
     */
    static final int getXLocation(int owningWidth, int objectWidth,
            int horizontalLayout) {
        switch (horizontalLayout) {
            case Item.LAYOUT_RIGHT:
                return owningWidth - objectWidth;
            case Item.LAYOUT_CENTER:
                return (owningWidth - objectWidth) / 2;
            case Item.LAYOUT_LEFT:
            default:
                return 0;
        }
    }

    /**
     * Calculate Y position based on vertical layout.
     *
     * @param owningHeight owning area height
     * @param objectHeight object's height
     * @param verticalLayout vertical layout
     * @return y-position of object
     */
    static final int getYLocation(int owningHeight, int objectHeight,
            int verticalLayout) {
        switch (verticalLayout) {
            case Item.LAYOUT_VCENTER:
                return (owningHeight - objectHeight) / 2;
            case Item.LAYOUT_TOP:
                return 0;
            case Item.LAYOUT_BOTTOM:
                return owningHeight - objectHeight;
            default:
                return owningHeight - objectHeight;
        }
    }

    /**
     * Gets maximum width of an item. The maximum width is same for all items
     * and it's the width of form's content area.
     *
     * @param item Item which maximum width is returned. The width is same for
     *            all items but this parameter is useful because method could
     *            use item's parent to calculate the width.
     * @return Maximum width of an item.
     */
    static int getMaximumItemWidth(final Item item) {
        if (item != null && item.hasLabel()) {
            Point temp = new Point(0, 0);
            applyCaptionedTrim("", temp);
            return formWidth - temp.x;
        }
        return formWidth;
    }

    /**
     * Item focus Listener reacts on eSWT focusGained event.
     */
    class ItemFocusListener implements FocusListener {

        private Item item;

        ItemFocusListener(Item item) {
            this.item = item;
        }

        public void focusGained(FocusEvent focusEvent) {
            if (!item.isFocused()) {
                // Logger.method(item, "focusGained");
                dfi.eswtSetCurrentSelectedItem(item);
            }
        }

        public void focusLost(FocusEvent fe) {
            // Logger.method(item, "focusLost");
        }
    }

}
