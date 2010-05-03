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

/**
 * Responsible for implementing interaction in DefaultFormLayoutPolicy.
 */
class DefaultFormInteraction extends DefaultFormLayoutPolicy {

    private static final int NO_DIRECTION = -1;

    private Item currentSelectedItem;

    private int direction = NO_DIRECTION;

    private boolean mousePressed;

    private LayoutObject currentlyUnderMouse;

    /**
     * Constructor.
     *
     * @param form where DFLP is applied.
     */
    DefaultFormInteraction(Form form) {
        super(form);
    }

    final void handleShowCurrentEvent() {
        super.handleShowCurrentEvent();
        eswtApplyCurrentFocus();
    }

    final void handleHideCurrentEvent() {
        super.handleHideCurrentEvent();
        direction = NO_DIRECTION;
    }

    /* (non-Javadoc)
     * @see DefaultFormLayoutPolicy#eswtLayoutForm(int)
     */
    final void eswtLayoutForm(int startIndex) {
        super.eswtLayoutForm(startIndex);

        // clear invalid selected item
        eswtCheckCurrentSelectedItem();

        if (currentSelectedItem != null
                && (currentSelectedItem.isFocusable())) {
            eswtApplyCurrentFocus();
        }
        else {
            // If there's no item currently selected try to find first
            // focusable item and set it current (if found):
            Item found = eswtGetNextFocusableItem(
                    getItem(startIndex - 1), SWT.ARROW_RIGHT);
            if (found != null) {
                eswtSetCurrentSelectedItem(found, NO_DIRECTION);
            }
            else {
                eswtApplyCurrentFocus();
            }
        }
    }

    /* (non-Javadoc)
     * @see DefaultFormLayoutPolicy#eswtSetCurrentItem(Item)
     */
    boolean eswtSetCurrentItem(Item item) {
        boolean ret = super.eswtSetCurrentItem(item);
        if (ret && item != null && item.isFocusable()) {
            eswtSetCurrentSelectedItem(item, NO_DIRECTION);
            Logger.info("eswtSetCurrentItem" + item);
        }
        return ret;
    }

    /**
     * DefaultFormInteraction handler for key events.<br>
     * The DefaultFormInteraction is responsible to react to key presses
     * accordingly. The implementation moves focus and/or scrolls the form when
     * needed. The method is called by the Form.
     *
     * @param keyCode eSWT key code.
     * @param keyType eSWT key type.
     */
    final void handleKeyEvent(int keyCode, int keyType) {
        Logger.method(this, "handleKeyEvent", currentSelectedItem,
                String.valueOf(keyCode), String.valueOf(keyType));

        boolean isDirectionalKey = isDirectionKey(keyCode);
        if (keyType == SWT.KeyDown && isDirectionalKey) {
            eswtCheckCurrentSelectedItem();
        }

        if (currentSelectedItem != null) {
            if (getLayouter(currentSelectedItem).eswtOfferKeyEvent(
                    currentSelectedItem, keyCode, keyType)) {
                // if the key has been consumed
                return;
            }
        }

        // scrolling/focus traverse only happens on directional key's down event
        if (keyType == SWT.KeyDown && isDirectionalKey) {
            // try to find next focusable item
            Item next = eswtGetNextFocusableItem(currentSelectedItem, keyCode);

            // if no visible & focusable item was found to transfer focus
            if (next == currentSelectedItem) {
                // try to scroll a bit
                eswtSetScrollingPosition(getNextScrollingPosition(keyCode),
                        true);
                // find next focusable after scrolling
                next = eswtGetNextFocusableItem(currentSelectedItem, keyCode);
            }

            if (next != currentSelectedItem) {
                //textfield always have to be fully visible when focused.
                if (next instanceof TextField) {
                    eswtScrollToItem(next);
                }
                eswtSetCurrentSelectedItem(next, keyCode);
            }
        }
    }

    /**
     * Returns if the parameter is a eSWT directional key code.
     *
     * @param keyCode key code
     */
    private boolean isDirectionKey(int keyCode) {
        return (keyCode == SWT.ARROW_DOWN || keyCode == SWT.ARROW_UP
             || keyCode == SWT.ARROW_LEFT || keyCode == SWT.ARROW_RIGHT);
    }

    /**
     * DefaultFormInteraction handler for pointer events.<br>
     * The method is called by the Form.
     *
     * @param x coordinate relative to scrolledComposite
     * @param y coordinate relative to scrolledComposite
     * @param type event type: SWT.MouseDown, SWT.MouseMove, SWT.MouseUp
     */
    final void handlePointerEvent(int x, int y, int type) {
        Logger.method(this, "handlePointerEvent", String.valueOf(x),
                String.valueOf(y), String.valueOf(type));

        // TODO: change when DirectUI style arrives.
        /*
        Item item;
        if (type == SWT.MouseMove) {
            if (currentlyUnderMouse == null
                    || !currentlyUnderMouse.contains(x, y)) {
                if (currentlyUnderMouse != null) {
                    //currentlyUnderMouse.getControl().setCapture(false);
                }
                item = eswtFindItemUnderMouse(x, y);
                if (item != null && item != currentSelectedItem
                        && item.isFocusable()) {
                    setCurrentItem(item);
                    item.internalSetFocused(true);
                    eswtSetCurrentSelectedItem(item);
                    //following method causes all mouse events delivered to it

                    currentlyUnderMouse.getControl().setCapture(true);
                    Logger.warning("seting capture to:" + item);
                }
            }
            int currentVPosition = getVPosition();
            boolean isMouseDirectionUp = false;
            boolean doScrolling = false;
            int localY = y;

            if (y <= currentVPosition) {
                localY = Math.max(0, y);
                eswtSetScrollingPosition(localY, true);
                isMouseDirectionUp = true;
                doScrolling = true;
            }
            else if (y > (currentVPosition + getFormHeight())) {
                //check for maxVPosition
                if (y > (eswtGetMaxVPosition() + getFormHeight())) {
                    localY = eswtGetMaxVPosition() + getFormHeight();
                }
                else {
                    localY = y;
                }
                currentVPosition = localY - getFormHeight();
                eswtSetScrollingPosition(currentVPosition, true);

                isMouseDirectionUp = false;
                doScrolling = true;
            }
            if (mousePressed && doScrolling) {
                resetEventTimer(isMouseDirectionUp, localY);
            }
        }
        else
        if (type == SWT.MouseDown) {
            mousePressed = true;
            item = eswtFindItemUnderMouse(x, y);
            if (item != null && item != currentSelectedItem
                    && item.isFocusable() && getForm().getShell() ==
                        getForm().getShell().getDisplay().getActiveShell()) {
                //eswtScrollToItem(item);
                //following method causes all mouse events delivered to it

                //currentlyUnderMouse.getControl().setCapture(true);
            }
        }
        else if (type == SWT.MouseUp) {
            mousePressed = false;
            if (currentlyUnderMouse != null) {
                //currentlyUnderMouse.getControl().setCapture(false);
            }
        }*/
    }

    /**
     * Find item at the specified point.
     *
     * @param x coordinate.
     * @param y coordinate.
     * @return Item.
     */
    Item eswtFindItemUnderMouse(int x, int y) {
        Row itemRow;
        for (int i = 0; i < getRowCount(); i++) {
            itemRow = getRow(i);
            if (itemRow.getYShift() <= y && y <= itemRow.getBottomPosition()) {
                LayoutObject lo;
                for (int j = 0; j < itemRow.size(); j++) {
                    lo = itemRow.getLayoutObject(j);
                    if (lo.contains(x, y)) {
                        Logger.info("Item under mouse: "
                                + lo.getOwningItem());
                        currentlyUnderMouse = lo;
                        return lo.getOwningItem();
                    }
                }
                break;
            }
        }
        return null;
    }

    /* (non-Javadoc)
     * @see DefaultFormLayoutPolicy#eswtHandleVisibilityChanges()
     */
    protected void eswtHandleVisibilityChanges() {
        super.eswtHandleVisibilityChanges();
        eswtCheckCurrentSelectedItem();
    }

    /**
     * Gets next (or nearest) focusable item.
     *
     * @param fromItem Item where to start to search the next focusable item.
     * @param dir Search direction, one of the arrow key constants defined
     *      in class SWT.
     *
     * @return Nearest focusable item or null if no item found.
     */
    final Item eswtGetNextFocusableItem(Item fromItem, int dir) {
        Item nextItem = fromItem;

        switch (dir) {
            case SWT.ARROW_RIGHT: {
                LayoutObject obj = getLastLayoutObjectOfItem(fromItem);
                while ((obj = getNextLayoutObjectOfItem(obj, null)) != null) {
                    Item owner = obj.getOwningItem();
                    if (owner != null && owner != fromItem
                            && owner.isFocusable()
                            && isPartiallyVisible(obj, Config.DFI_VISIBILITY_PERCENT)) {
                        nextItem = owner;
                        break;
                    }
                }
                break;
            }

            case SWT.ARROW_LEFT: {
                LayoutObject obj = getFirstLayoutObjectOfItem(fromItem);
                while ((obj = getPrevLayoutObjectOfItem(obj, null)) != null) {
                    Item owner = obj.getOwningItem();
                    if (owner != null && owner != fromItem
                            && owner.isFocusable()
                            && isPartiallyVisible(obj, Config.DFI_VISIBILITY_PERCENT)) {
                        nextItem = owner;
                        break;
                    }
                }
                break;
            }

            case SWT.ARROW_DOWN: {
                int minDist = Integer.MAX_VALUE;
                LayoutObject start = getLastLayoutObjectOfItem(fromItem);
                LayoutObject obj = start;
                while ((obj = getNextLayoutObjectOfItem(obj, null)) != null) {
                    Item owner = obj.getOwningItem();
                    if (owner != null && owner != fromItem
                            && owner.isFocusable() && obj.isBelow(start)
                            && isPartiallyVisible(obj, Config.DFI_VISIBILITY_PERCENT)) {
                        int dist = obj.distanceTo(start);
                        if (dist < minDist) {
                            minDist = dist;
                            nextItem = owner;
                        }
                    }
                }
                break;
            }

            case SWT.ARROW_UP: {
                int minDist = Integer.MAX_VALUE;
                LayoutObject start = getFirstLayoutObjectOfItem(fromItem);
                LayoutObject obj = start;
                while ((obj = getPrevLayoutObjectOfItem(obj, null)) != null) {
                    Item owner = obj.getOwningItem();
                    if (owner != null && owner != fromItem
                            && owner.isFocusable() && obj.isAbove(start)
                            && isPartiallyVisible(obj, Config.DFI_VISIBILITY_PERCENT)) {
                        int dist = obj.distanceTo(start);
                        if (dist < minDist) {
                            minDist = dist;
                            nextItem = owner;
                        }
                    }
                }
                break;
            }

            default:
        }

        return nextItem;
    }

    /**
     * Check if the currentSelectedItem is valid and visible. If not then it
     * sets it to null.
     */
    final void eswtCheckCurrentSelectedItem() {
        if (currentSelectedItem != null) {
            if (currentSelectedItem.getParent() != getForm()
                    || !currentSelectedItem.isVisible()) {
                // we need to find another
                Logger.method(this, "eswtCheckCurrentSelectedItem");
                eswtSetCurrentSelectedItem(null, NO_DIRECTION);
            }
        }
    }

    /**
     * Sets currentSelectedItem and sets focus to it.<br>
     * If one of form's items is already selected when this method is called,
     * removes focus from old item and then moves focus to new one.
     *
     * @param item Item to set as current selected. If null, nothing happens.
     * @param dir Direction which is delivered to layouter.
     */
    void eswtSetCurrentSelectedItem(Item item, int dir) {
        if (currentSelectedItem != item) {
            Logger.info(this + "::SelectedItem: "
                    + currentSelectedItem + " --(" + dir + ")--> " + item);

            // Save direction
            direction = dir;
            // Remove focus from currentSelectedItem and notify its Layouter.
            if (currentSelectedItem != null) {
                getLayouter(currentSelectedItem).eswtFocusLost(
                        currentSelectedItem);
            }

            // Set new currentSelectedItem, must be focusable or null
            currentSelectedItem = item;

            // Set focus to currentSelectedItem and notify its Layouter.
            if (currentSelectedItem != null) {
                getLayouter(currentSelectedItem).eswtFocusGained(
                        currentSelectedItem, dir);
            }

            // Apply eSWT focus to currentSelectedItem's control
            eswtApplyCurrentFocus();
        }
    }

    /**
     * Sets currentSelectedItem and sets focus to it.<br>
     * If one of form's items is already selected when this method is called,
     * removes focus from old item and then moves focus to new one.
     *
     * @param item Item to set as current selected. If null, nothing happens.
     * @param dir Direction which is delivered to layouter.
     */
    void eswtSetCurrentSelectedItem(Item item) {
        if (currentSelectedItem != item) {
            Logger.info(this + "::SelectedItem: "
                    + currentSelectedItem + " ---> " + item);

            // Remove focus from currentSelectedItem and notify its Layouter.
            if (currentSelectedItem != null) {
                getLayouter(currentSelectedItem).eswtFocusLost(
                        currentSelectedItem);
            }

            // Set new currentSelectedItem, must be focusable or null
            currentSelectedItem = item;

            // Set focus to currentSelectedItem and notify its Layouter.
            if (currentSelectedItem != null) {
                getLayouter(currentSelectedItem).eswtFocusGained(
                        currentSelectedItem, NO_DIRECTION);
            }

            // Apply eSWT focus to currentSelectedItem's control
            //eswtApplyCurrentFocus();
        }
    }

    /**
     * Sets focus to currentSelectedItem's control if its partially visible.
     * Otherwise it sets dummy focus to form's composite.<br>
     * <br>
     * Note that this method applies focus only to eSWT control. Item focus
     * update and layouter notifications are handled in method
     * <code>eswtSetCurrentSelectedItem()</code>.<br>
     * If currentSelectedItem is null or form is not shown, this method has no
     * effect.
     */
    void eswtApplyCurrentFocus() {
        if (isFormCurrent()) {
            // if any of the Item's LayoutObjects is visible
            if (isItemPartiallyVisible(currentSelectedItem)) {
                Logger.method(this, "ApplyFocus", currentSelectedItem);
                eswtSetFocusToFirstControl(currentSelectedItem);
            }
            else {
                Logger.method(this, "ApplyFocus", "dummy");
                formComposite.forceFocus();
            }
        }
    }

    /**
     * If the Item is valid and it is layouted, then sets the Item's first
     * LayoutObject focused.
     *
     * @param item an item which first LayoutObject is set focused.
     */
    void eswtSetFocusToFirstControl(Item item) {
        if (item != null && item.isFocusable()) {
            LayoutObject lo = getFirstLayoutObjectOfItem(item);
            if (lo != null) {
                lo.getControl().forceFocus();
            }
        }
    }

    /**
     * Gets Current selected item.
     *
     * @return Current selected item. May also return null.
     */
    Item getCurrentSelectedItem() {
        return currentSelectedItem;
    }

    /**
     * Get the direction of scrolling.
     *
     * @return direction of scrolling.
     */
    int getDirection() {
        return direction;
    }

    /* (non-Javadoc)
     * @see DefaultFormLayoutPolicy#eswtResizeItemAndShift(Item)
     */
    int eswtResizeItemAndShift(Item item) {
        // save the state of the last row before resizing and Shifting.
        boolean itemWasVisible = isItemPartiallyVisible(item);

        int newVPosition = super.eswtResizeItemAndShift(item);

        if (item == currentSelectedItem) {
            if (itemWasVisible) {
                int itemTop = getItemTopPosition(item);
                int itemBottom = getItemBottomPosition(item);
                // currentSelectedItem has to be focused if it was focused
                // before resizing e.g TextField when it is resized by adding a
                // new row and it was in the bottom of the Screen.
                if (newVPosition <= itemTop
                        && (newVPosition + getFormHeight()) >= itemBottom) {
                    // do not change vPosition;
                }
                else if (newVPosition > itemTop) {
                    newVPosition = itemTop;
                }
                else if ((newVPosition + getFormHeight()) < itemBottom) {
                    newVPosition = itemBottom - getFormHeight();
                }
            }
        }
        return newVPosition;
    }

    private Timer eventTimer = new Timer();
    private EventGeneratorTask eventTask;

    /**
     * Reset timer for do layout with a given start index.
     */
    private void resetEventTimer(boolean directionUp, int y) {
        if (eventTimer != null) {
            if (eventTask != null) {
                eventTask.cancel();
                eventTask = null;
            }
            // schedule new timer
            eventTask = new EventGeneratorTask(directionUp, y);
            eventTimer.schedule(eventTask, Config.DFI_EVENT_TIMER_DELAY);
        }
    }

    /**
     * Form Timer task. Triggers the formComposite to Layout.
     */
    class EventGeneratorTask extends TimerTask {

        private boolean isUpDirection;
        private int localY;

        public EventGeneratorTask(boolean direction, int y) {
            isUpDirection = direction;
            localY = y;
            Logger.info("y is " + localY);
        }

        public void run() {
            if (isUpDirection) {
                localY -= Config.DFI_EVENT_MOVE_DELTA;
            }
            else {
                localY += Config.DFI_EVENT_MOVE_DELTA;
            }
            handlePointerEvent(0, localY, SWT.MouseMove);
        }
    }

}
