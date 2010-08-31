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

import org.eclipse.ercp.swt.mobile.TextExtension;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.*;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.internal.extension.TextExtensionExtension;
/**
 * Responsible for correct layout of TextField in a Form.
 */
class TextFieldLayouter extends ItemLayouter
{

    /**
     * Key name for modify listener.
     */
    private static final String MODIFY_LISTENER = "modify";

    /**
     * Key name for key listener.
     */
    private static final String KEY_LISTENER = "key";

    /**
     * Key name for mouse listener.
     */
    private static final String MOUSE_LISTENER = "mouse";


    /**
     * Percentage of the whole screen.
     */
    private static final int TOTAL_PERCENTAGE = 100;


    // private static Control[] staticControls = new Control[6];

    private static boolean isCorrectText;

    /**
     * Constructor.
     *
     * @param aFormLayouter FormLayouter used for layouting.
     */
    TextFieldLayouter(FormLayouter aFormLayouter)
    {
        super(aFormLayouter);
    }

    /**
     * Get static eSWT control (ConstraintText or TextExtension).
     *
     * @param constraint
     */
    static Control eswtGetStaticTextControl(int constraint)
    {
        Control ret = null;

        /*
        int maskedConstraint = constraint & TextField.CONSTRAINT_MASK;

        if (staticControls[maskedConstraint] == null) {
            staticControls[maskedConstraint] = TextWrapper.eswtConstructText(
                    eswtGetStaticShell(), SWT.MULTI | SWT.WRAP, constraint);
            ret = staticControls[maskedConstraint];
        }
        */

        if (constraint == TextField.NUMERIC) {
            constraint = TextExtensionExtension.NUMERIC;
        }
        else if (constraint == TextField.DECIMAL) {
            constraint = TextExtensionExtension.DECIMAL;
        }
        else if (constraint == TextField.PHONENUMBER) {
            constraint = TextExtensionExtension.PHONENUMBER;
        }
        else
        {
            // TODO: eSWT support required - text validation on EMAIL and URL constraints
            // default
            constraint = 0;
        }

        return new TextExtensionExtension(eswtGetStaticShell(), SWT.MULTI | SWT.WRAP,constraint);
    }

    /**
     * Check that text satisfies specified constraints.
     *
     * @param constraint TextField.NUMERIC etc.
     * @return true if text is correct for specified constraint.
     */
    static boolean checkText(final int constraint, final String text)
    {
        isCorrectText = true;

        try
        {   
         	  if(constraint == TextField.NUMERIC && !text.equals(""))
            {
                Integer.parseInt(text);
            }
            else if(constraint == TextField.DECIMAL && !text.equals(""))
            {
                Float.parseFloat(text);
            }
        }
        catch( NumberFormatException e )
        {   
        	  // Illegal text
            return false;
        }

        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                try
                {
                    TextWrapper.eswtSetContent(
                        eswtGetStaticTextControl(constraint), text);
                }
                catch(IllegalArgumentException e)
                {
                    isCorrectText = false;
                }
            }
        });
        return isCorrectText;
    }

    /**
     * eSWT specific calls to implement getControl.
     *
     * @param parent for the control.
     * @param item TextField item.
     */
    Control eswtGetControl(Composite parent, Item item)
    {
        TextField textfield = (TextField) item;

        Control te = TextWrapper.eswtConstructText(parent,
                     SWT.WRAP | SWT.MULTI, textfield.getConstraints());
        TextWrapper.eswtSetMaxSize(te, textfield.getMaxSize());
        TextWrapper.eswtSetContent(te, textfield.getString());
        TextWrapper.eswtSetSelection(te,
                                     textfield.getCaretPosition(), textfield.getCaretPosition());

        if(textfield.getInitialInputMode() != null)
        {
            eswtUpdateItem(textfield, te, TextField.UPDATE_INITIALINPUTMODE,
                           null);
        }
        return te;
    }

    /**
     * Returns true if this eSWT control is suitable to be used for updating.
     *
     * @param item Item.
     * @param control eSWT control.
     *
     * @return true if this control is suitable for update
     */
    boolean eswtIsSpecificControl(Item item, Control control)
    {
        return (control instanceof TextExtension);
    }

    /**
     * Updates the values of TextField.
     *
     * @param item Item.
     * @param control eSWT control.
     * @param reason reason to update.
     */
    void eswtUpdateItem(Item item, Control control, int reason, Object param)
    {
        TextField textfield = (TextField) item;
        if(reason == TextField.UPDATE_INITIALINPUTMODE)
        {
            TextWrapper.eswtSetInputMode(control,
                                         textfield.getInitialInputMode(),
                                         textfield.getConstraints());
        }
        else
        {
            TextWrapper.eswtSetContent(control, textfield.getString());
        }
    }

    /**
     * Update size of TextField.
     *
     * @param item TextField.
     * @param control Control which represents TextField.
     * @param width which control must occupy.
     */
    void eswtResizeControl(Item item, Control control, int width, int height)
    {
        super.eswtResizeControl(item, control, width, height);
        if(control instanceof TextExtension)
        {
            TextExtension te = (TextExtension) control;
            ((TextField) item).internalSetLinesCount(te.getLineCount());
        }
    }

    /**
     * Returns true if that key was consumed by TextField.
     *
     * @param item TextField.
     * @param key keyCode.
     */
    boolean eswtOfferKeyPressed(Item item, int key)
    {
        TextField tf = (TextField) item;
        if(item.hasLayout(Item.LAYOUT_SHRINK))
        {
            if((key == SWT.ARROW_LEFT
                    && tf.getCaretPosition() == 0)
                    || (key == SWT.ARROW_RIGHT
                        && tf.getCaretPosition() == tf.size()))
            {
                return false;
            }
        }
        if(((key == SWT.ARROW_UP)
                && (tf.getCaretPosition() == 0))
                || ((key == SWT.ARROW_DOWN)
                    && (tf.getCaretPosition() == tf.size())))
        {
            return false;
        }
        return true;
    }

    /**
     * Responsible for reacting on focusGained event, and according to direction
     * from which that event comes sets the caret of the TextField.
     *
     * @param item TextField.
     * @param dir direction from which focus came, in case if it was set with
     *            setCurrentItem() default direction is used (-1).
     */
    void eswtFocusGained(Item item, int dir)
    {
        super.eswtFocusGained(item, dir);
        TextField tf = (TextField) item;
        // direction = dir;
        resetCaretPosition(tf, dir);
        Control control = eswtGetFirstSpecificControl(item);
        TextWrapper.eswtSetSelection(control,
                                     tf.getCaretPosition(), tf.getCaretPosition());
    }

    /**
     * Returns the minimum area needed to display a TextField.
     *
     * @param textField TextField object
     * @return Minimum area needed to display TextField.
     */
    static Point calculateMinimumBounds(final TextField textField)
    {
        final Point minSize = new Point(0, 0);
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                TextExtension tempExt = (TextExtension) eswtGetStaticTextControl(TextField.ANY);
                tempExt.setText(ItemLayouter.MIN_TEXT);
                tempExt.pack();
                minSize.x = tempExt.getSize().x;
                minSize.y = tempExt.getSize().y + Config.TEXTFIELD_MARGIN;
                applyMinMargins(textField, minSize);
            }
        });
        return minSize;
    }

    /**
     * Returns the preferred area needed to display an Item.
     *
     * @param item Item.
     * @return Preferred area needed to display Item. x is width and y is
     *         height.
     */
    static Point calculatePreferredBounds(Item item)
    {
        final TextField textfield = (TextField) item;
        final Point prefSize = new Point(0, 0);
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                TextExtension te = (TextExtension) eswtGetStaticTextControl(TextField.ANY);
                te.setText(textfield.getString());

                int maxHeight = (formHeigh
                                 * Config.TEXTFIELD_MAX_SCREEN_PERCENTAGE / TOTAL_PERCENTAGE)
                                - Config.TEXTFIELD_MARGIN;
                textfield.internalSetMaxVisibleLines(maxHeight
                                                     / te.getLineHeight());

                prefSize.x = getMaximumItemWidth(textfield);
                prefSize.y = Config.TEXTFIELD_MARGIN + Math.min(
                                 te.computeSize(prefSize.x, SWT.DEFAULT).y, maxHeight);
                // prefSize.y = Math.min(calc.y, maxHeight) + MARGIN;
                applyPrefMargins(textfield, prefSize);
            }
        });
        return prefSize;
    }

    /**
     * Update caret position based on direction.
     *
     * @param textfield TextField for which to update caret position.
     * @param dir direction of scrolling.
     */
    private void resetCaretPosition(TextField textfield, int dir)
    {
        switch(dir)
        {
        case SWT.ARROW_DOWN:
            textfield.internalSetCaretPosition(0);
            break;
        case SWT.ARROW_RIGHT:
            textfield.internalSetCaretPosition(0);
            break;
        case SWT.ARROW_UP:
            textfield.internalSetCaretPosition(textfield.size());
            break;
        case SWT.ARROW_LEFT:
            textfield.internalSetCaretPosition(textfield.size());
            break;
        default:
            break;
        }
    }

    /**
     * Add TextField listeners when form is activated.
     *
     * @param item TextField.
     * @param control Control which represents TextField.
     */
    void eswtAddSpecificListeners(Item item, Control control)
    {
        super.eswtAddSpecificListeners(item, control);
        TextField textfield = (TextField) item;
        ModifyListener listener = new TextFieldModifyListener(textfield);
        TextWrapper.eswtAddModListener(control, listener);
        control.setData(MODIFY_LISTENER, listener);
        KeyListener listener2 = new TextFieldKeyListener(textfield);
        control.addKeyListener(listener2);
        control.setData(KEY_LISTENER, listener2);
        MouseListener listener4 = new AllMouseListener(textfield);
        control.addMouseListener(listener4);
        control.setData(MOUSE_LISTENER, listener4);
    }

    /**
     * Remove listeners from a TextField if the form goes to background.
     *
     * @param item TextField.
     * @param control Control which represents TextField.
     */
    void eswtRemoveSpecificListeners(Item item, Control control)
    {
        super.eswtRemoveSpecificListeners(item, control);
        ModifyListener l1 = (ModifyListener) control.getData(MODIFY_LISTENER);
        if(l1 != null)
        {
            TextWrapper.eswtRemoveModListener(control, l1);
            control.setData(MODIFY_LISTENER, null);
        }
        KeyListener l2 = (KeyListener) control.getData(KEY_LISTENER);
        if(l2 != null)
        {
            control.removeKeyListener(l2);
            control.setData(KEY_LISTENER, null);
        }
        MouseListener l4 = (MouseListener) control.getData(MOUSE_LISTENER);
        if(l4 != null)
        {
            control.removeMouseListener(l4);
            control.setData(MOUSE_LISTENER, null);
        }
    }

    /**
     * Class that receives ModifyEvents from TextExtension and updates values of
     * TextField.
     */
    class TextFieldModifyListener implements ModifyListener
    {

        private TextField textfield;

        TextFieldModifyListener(TextField textField)
        {
            this.textfield = textField;
        }

        private void handleLinesChange(TextExtension te)
        {
            int lines = te.getLineCount();
            int visibleLines = te.getSize().y / te.getLineHeight();
            if(lines != textfield.internalGetLinesCount())
            {
                textfield.internalSetLinesCount(lines);
                Control control = eswtGetFirstControl(textfield);
                if(control.getSize().y + te.getLineHeight()
                        + Config.TEXTFIELD_MARGIN <= formLayouter.getFormHeight())
                {
                    textfield.updateParent(Item.UPDATE_HEIGHT_CHANGED);
                }
                if(textfield.internalGetLinesCount() > lines)
                {
                    if((te.getTopIndex() + visibleLines) > lines)
                    {
                        te.setTopIndex(Math.max(0, lines - visibleLines));
                    }
                    if(visibleLines > lines)
                    {
                        textfield.updateParent(Item.UPDATE_HEIGHT_CHANGED);
                    }
                }

                te.setTopIndex(Math.max(te.getCaretLineNumber() + 1
                                        - textfield.internalGetMaxVisibleLines(), 0));
            }
        }

        public void modifyText(ModifyEvent modifyEvent)
        {
            Control te = (Control) modifyEvent.widget;
            if(textfield.internalSetString(TextWrapper.eswtGetContent(te)))
            {
                Logger.method(textfield, "modify", modifyEvent);
                textfield.internalSetCaretPosition(
                    TextWrapper.eswtGetCaretPosition(te));
                if(te instanceof TextExtension)
                {
                    handleLinesChange((TextExtension) te);
                }
                textfield.notifyStateChanged();
            }
        }
    }

    /**
     * Class that receives KeyEvents from TextExtension and updates
     * caret position for TextField.
     */
    class TextFieldKeyListener implements KeyListener
    {

        private TextField textfield;

        TextFieldKeyListener(TextField textField)
        {
            this.textfield = textField;
        }

        public void keyPressed(KeyEvent keyEvent)
        {
            Control te = (Control) keyEvent.widget;
            int caretPos = TextWrapper.eswtGetCaretPosition(te);
            int caretLine = TextWrapper.eswtGetCaretLine(te);

            if(keyEvent.keyCode == SWT.ARROW_UP && caretLine == 0)
            {
                caretPos = 0;
                TextWrapper.eswtSetSelection(te, caretPos, caretPos);
            }
            else if(keyEvent.keyCode == SWT.ARROW_DOWN
                    && (caretLine == (TextWrapper.eswtGetLineCount(te) - 1)))
            {
                caretPos = textfield.size();
                TextWrapper.eswtSetSelection(te, caretPos, caretPos);
            }

            textfield.internalSetCaretPosition(caretPos);
        }

        public void keyReleased(KeyEvent keyEvent)
        {
            // this is needed if focus was changed with touch.
            // so ne scrolling was done in DFI.
            if(!formLayouter.isItemFullyVisible(textfield))
            {
                formLayouter.eswtScrollToItem(textfield);
            }
            textfield.internalSetCaretPosition(
                TextWrapper.eswtGetCaretPosition((Control) keyEvent.widget));
        }

    }

    class AllMouseListener implements MouseListener, MouseMoveListener
    {

        private TextField textfield;
        private boolean isEnabled;

        AllMouseListener(TextField tf)
        {
            textfield = tf;
        }

        public void enable(boolean enabled)
        {
            isEnabled = enabled;
        }

        public void mouseUp(MouseEvent me)
        {
            if(isEnabled)
            {
                //
            }
        }

        public void mouseDown(MouseEvent me)
        {
            textfield.internalSetCaretPosition(
                TextWrapper.eswtGetCaretPosition((Control) me.widget));
        }

        public void mouseMove(MouseEvent me)
        {
        }

        public void mouseDoubleClick(MouseEvent me)
        {
        }

    }

}
