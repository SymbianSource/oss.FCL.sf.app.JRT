/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 * A listener for receiving notification of content changes that have been
 * invoked on <code>TextEditor</code> objects. When a content change happens,
 * the application is notified by calling the relevant methods on the
 * <code>TextEditorChangeListener</code> that had been set on the
 * <code>TextEditor</code> with a call to
 * {@link TextEditor#setTextEditorChangeListener setTextEditorChangeListener()}.
 */

public interface TextEditorChangeListener {

    /**
     * Constant indicating the caret movement direction up.
     */
    public static final int CARET_UP = 1;

    /**
     * Constant indicating the caret movement direction up.
     */
    public static final int CARET_RIGHT = 2;

    /**
     * Constant indicating the caret movement direction up.
     */
    public static final int CARET_DOWN = 3;

    /**
     * Constant indicating the caret movement direction up.
     */
    public static final int CARET_LEFT = 4;

    /**
     * Called by the implementation to indicate that a range of the content in
     * the TextEditor has been changed either by user, prgrammatically or via
     * external front end processors for advanced input methods. The parameters
     * indicate the range of the new modified text. The length parameter is
     * always > 0.
     * 
     * @param index the index of the first character that is selected.
     * @param length the length of selection as number of characters.
     */
    public void textChanged(int index, int length);

    /**
     * Called by the implementation to indicate that the caret has been moved.
     * The parameters indicate the new caret index and the movement direction in
     * relation the previous caret position.
     * 
     * @param newIndex the character index before which the caret is placed
     *            after the movement
     * @param movementDirection the caret movement direction; must be one of
     *            CARET_UP, CARET_RIGHT, CARET_DOWN, or CARET_LEFT
     */
    public void caretMoved(int newIndex, int movementDirection);

    /**
     * Called by the implementation to indicate that a range of the content in
     * the TextEditor has been selected. The selection have been set with
     * programmatically or by user interaction. The parameters indicate the
     * range of the current selection.
     * 
     * @param index the index of the first character that is selected.
     * @param length the length of selection as number of characters.
     */
    public void contentSelected(int index, int length);

}
