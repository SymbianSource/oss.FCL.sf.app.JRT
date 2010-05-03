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

abstract class FormLayoutPolicy {

    /**
     * Layouts the Form.
     *
     * @param startItem item from which to start Layout;
     */
    abstract void layoutForm(int startItem);

    /**
     * Set focus to an item.
     */
    abstract void setCurrentItem(Item item);

    /**
     * Called when Form becomes current Displayable.
     */
    abstract void handleShowCurrentEvent();

    /**
     * Called when Form stops being current Displayable.
     */
    abstract void handleHideCurrentEvent();

    /**
     * Called when Form is being resized.
     *
     * @param width new width
     * @param height new height
     */
    abstract void handleResizeEvent(int width, int height);

    /**
     * Handle key event.
     *
     * @param keyCode key
     * @param keyType type
     */
    abstract void handleKeyEvent(int keyCode, int keyType);

    /**
     * Handle pointer event.
     *
     * @param x coordinate relative to scrolledComposite
     * @param y coordinate relative to scrolledComposite
     * @param type event type: SWT.MouseDown, SWT.MouseMove, SWT.MouseUp
     */
    abstract void handlePointerEvent(int x, int y, int type);

    /**
     * Set scrolling of form composite.
     */
    abstract void updateScrolling(int value, boolean keyNavi);

    /**
     * Get the layouted conrol of an item.
     *
     * @param item item
     * @return layouted control
     */
    abstract org.eclipse.swt.widgets.Control getItemControl(Item item);

    /**
     * Gets layouter that can layout the specified item.
     *
     * @param item Item to be layouted.
     * @return ItemLayouter or null if no Layouter found.
     */
    abstract ItemLayouter getLayouter(Item item);    
}
