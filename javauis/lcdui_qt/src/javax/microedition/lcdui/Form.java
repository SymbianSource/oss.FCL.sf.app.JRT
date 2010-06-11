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

import java.util.*;

import javax.microedition.lcdui.EventDispatcher.LCDUIEvent;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.widgets.*;

/**
 * This class represents LCDUI Form.
 */
public class Form extends Screen
{

    /**
     * Abstract layouting policy.
     */
    private FormLayoutPolicy layoutPolicy;

    /**
     * Item vector.
     */
    private Vector items = new Vector();

    /**
     * Item state listener.
     */
    private ItemStateListener itemStateListener;

    private int startIndex;

    private FormTimerTask formTimerTask;

    private Timer layoutTimer = new Timer();

    // private FormMouseListener mouseListener = new FormMouseListener();
    private FormScrollBarListener fsbl = new FormScrollBarListener();

    /**
     * ScrolledComposite represents Form
     */
    private ScrolledComposite formComposite;

    /**
     * Constructor.
     *
     * @param title the title string
     */
    public Form(String title)
    {
        super(title);
        construct();
        layoutPolicy = new DefaultFormInteraction(this);
    }

    /**
     * Constructor.
     *
     * @param title the title string
     * @param formItems array of items on the formComposite
     */
    public Form(String title, Item[] formItems)
    {
        this(title);
        if(formItems != null)
        {
            for(int i = 0; i < formItems.length; i++)
            {
                this.append(formItems[i]);
            }
        }
    }

    /* (non-Javadoc)
     * @see javax.microedition.lcdui.Displayable#eswtConstructContent(int)
     */
    Composite eswtConstructContent(int style)
    {
        Composite comp = super.eswtConstructContent(SWT.VERTICAL);
        ScrollBar vBar = comp.getVerticalBar();
        if(vBar != null)
        {
            vBar.setVisible(true);
            vBar.setEnabled(true);
        }
        formComposite = new ScrolledComposite(comp, SWT.NONE);
        return comp;
    }

    /* (non-Javadoc)
     * @see Displayable#eswtHandleShowCurrentEvent()
     */
    void eswtHandleShowCurrentEvent()
    {
        super.eswtHandleShowCurrentEvent();
        ScrollBar vBar = getContentComp().getVerticalBar();
        if(vBar != null)
        {
            vBar.addSelectionListener(fsbl);
        }
        layoutPolicy.handleShowCurrentEvent();
    }

    /* (non-Javadoc)
     * @see Displayable#eswtHandleHideCurrentEvent()
     */
    void eswtHandleHideCurrentEvent()
    {
        super.eswtHandleHideCurrentEvent();

        ScrollBar vBar = getContentComp().getVerticalBar();
        if(vBar != null)
        {
            vBar.removeSelectionListener(fsbl);
        }
        layoutPolicy.handleHideCurrentEvent();
    }

    /* (non-Javadoc)
     * @see Displayable#eswtHandleResizeEvent(int, int)
     */
    void eswtHandleResizeEvent(int width, int height)
    {
        super.eswtHandleResizeEvent(width, height);
        if(formComposite != null)
        {
            formComposite.setRedraw(false);
            formComposite.setOrigin(0, 0, true);
            formComposite.setSize(width, height);
            formComposite.setRedraw(true);
            layoutPolicy.handleResizeEvent(width, height);
            resetLayoutTimer(0);
        }
    }

    /**
     * Handle key and traverse events.
     *
     * @param e eSWT event
     */
    void eswtHandleEvent(Event e)
    {
        super.eswtHandleEvent(e);
        if(e.type == SWT.Traverse)
        {
            e.doit = false;
        }
        else
        {
            layoutPolicy.handleKeyEvent(e.keyCode, e.type);
        }
    }

    /**
     * Append Item to a Form.
     *
     * @param item Item to append to a Form.
     * @return index of Item in Form
     */
    public int append(Item item)
    {
        if(item == null)
        {
            throw new NullPointerException(
                MsgRepository.FORM_EXCEPTION_ITEM_NULL_POINTER);
        }
        if(item.getParent() != null)
        {
            throw new IllegalStateException(
                MsgRepository.FORM_EXCEPTION_ITEM_OWNED_BY_CONTAINER);
        }
        int returnValue = -1;
        synchronized(layoutPolicy)
        {
            item.setParent(this);
            items.addElement(item);
            returnValue = items.indexOf(item);
            resetLayoutTimer(returnValue);
        }
        return returnValue;
    }

    /**
     * Append StringItem to a Form.
     *
     * @param text - text for StringItem.
     * @return index of StringITem in a Form.
     */
    public int append(String text)
    {
        if(text == null)
        {
            throw new NullPointerException(
                MsgRepository.FORM_EXCEPTION_STRING_NULL_POINTER);
        }
        return append(new StringItem(null, text, Item.LAYOUT_DEFAULT));
    }

    /**
     * Append ImageItem to a Form.
     *
     * @param img - Image for ImageItem.
     * @return index of ImageItem in a Form.
     */
    public int append(Image img)
    {
        if(img == null)
        {
            throw new NullPointerException(
                MsgRepository.FORM_EXCEPTION_IMAGE_NULL_POINTER);
        }
        return append(new ImageItem(null, img, Item.LAYOUT_DEFAULT, null));
    }

    /**
     * Insert item into Form before Item in specified position.
     *
     * @param position index where to insert.
     * @param item item insert.
     * @throws IndexOutOfBoundsException if position is incorrect.
     */
    public void insert(int position, Item item)
    {
        if(item == null)
        {
            throw new NullPointerException(
                MsgRepository.FORM_EXCEPTION_ITEM_NULL_POINTER);
        }
        if(item.getParent() != null)
        {
            throw new IllegalStateException(
                MsgRepository.FORM_EXCEPTION_ITEM_OWNED_BY_CONTAINER);
        }
        if ((position < 0) || (position > items.size() )) 
        {
            throw new IndexOutOfBoundsException(
                MsgRepository.FORM_EXCEPTION_INVALID_ITEM_INDEX);
        }
        synchronized(layoutPolicy)
        {
            item.setParent(this);
            items.insertElementAt(item, position);
            resetLayoutTimer(items.indexOf(item));
        }
    }

    /**
     * Delete item with index itemNumber from the formComposite.
     *
     * @param position - index of Item to delete from Form.
     * @throws IndexOutOfBoundsException if position is incorrect.
     */
    public void delete(int position)
    {
        if((position < 0) || (position > (items.size() - 1)))
        {
            throw new IndexOutOfBoundsException(
                MsgRepository.FORM_EXCEPTION_INVALID_ITEM_INDEX);
        }
        synchronized(layoutPolicy)
        {
            ((Item) items.elementAt(position)).setParent(null);
            items.removeElementAt(position);
            resetLayoutTimer(position - 1);
        }
    }

    /**
     * Delete all items from the formComposite.
     */
    public void deleteAll()
    {
        synchronized(layoutPolicy)
        {
            for(int i = 0; i < items.size(); i++)
            {
                ((Item) items.elementAt(i)).setParent(null);
            }
            items.removeAllElements();
            resetLayoutTimer(0);
        }
    }

    /**
     * Set the item to the formComposite, replacing the item on newItem index.
     *
     * @param position index of Item to replace.
     * @param newItem new item to set.
     * @throws IndexOutOfBoundsException if position is incorrect.
     */
    public void set(int position, Item newItem)
    {
        if(newItem == null)
        {
            throw new NullPointerException(
                MsgRepository.FORM_EXCEPTION_ITEM_NULL_POINTER);
        }
        if(newItem.getParent() != null)
        {
            throw new IllegalStateException(
                MsgRepository.FORM_EXCEPTION_ITEM_OWNED_BY_CONTAINER);
        }
        if((position < 0) || (position > (items.size() - 1)))
        {
            throw new IndexOutOfBoundsException(
                MsgRepository.FORM_EXCEPTION_INVALID_ITEM_INDEX);
        }
        synchronized(layoutPolicy)
        {
            newItem.setParent(this);
            // clear reference to a form for "old" item.
            get(position).setParent(null);
            items.setElementAt(newItem, position);
            resetLayoutTimer(position);
        }
    }

    /**
     * Return Item with specified index in the vector.
     *
     * @param position index of item.
     * @return Item - specified by position
     * @throws IndexOutOfBoundsException if position is incorrect.
     */
    public Item get(int position)
    {
        Item returnItem = null;
        synchronized(layoutPolicy)
        {
            try
            {
                returnItem = (Item) items.elementAt(position);
            }
            catch(ArrayIndexOutOfBoundsException e)
            {
                throw new IndexOutOfBoundsException(e.getMessage());
            }
        }
        return returnItem;
    }

    /**
     * Get number of items in the formComposite.
     *
     * @return number of Items in Form.
     */
    public int size()
    {
        return items.size();
    }

    /**
     * Set ItemStateListener.
     *
     * @param itemStateListener New ItemStateListener.
     */
    public void setItemStateListener(ItemStateListener itemStateListener)
    {
        this.itemStateListener = itemStateListener;
    }

    /**
     * Scrolls form so that specified item is visible and if possible transfers
     * focus to it.
     *
     * @param item The Item which should be set visible.
     */
    void setCurrentItem(Item item)
    {
        layoutPolicy.setCurrentItem(item);
    }

    /**
     * Notify item state listener about state change.
     *
     * @param item which triggered this event.
     */
    void notifyItemStateChanged(final Item item)
    {
        if(item != null && item.getParent() == this)
        {
            if(itemStateListener != null)
            {
                ESWTUIThreadRunner.getInstance().getDisplay().asyncExec(
                    new Runnable()
                {
                    public void run()
                    {
                        itemStateListener.itemStateChanged(item);
                    }
                });
            }
        }
    }

    /**
     * Update item state in form.
     *
     * @param item
     * @param updateReason
     * @param param additional parameter
     */
    void updateItemState(Item item, int updateReason, Object param)
    {
        if(item != null && item.getParent() == this)
        {
            if((updateReason & Item.UPDATE_WIDTH_CHANGED) != 0)
            {
                synchronized(layoutPolicy)
                {
                    resetLayoutTimer(items.indexOf(item));
                }
            }
            else
            {
                if(layoutPolicy instanceof DefaultFormInteraction)
                {
                    if(layoutTimer != null)
                    {
                        ((DefaultFormInteraction) layoutPolicy)
                        .updateItemState(item, updateReason, param);
                    }
                }
            }
        }
    }

    /**
     * Get Vector of items.
     *
     * @return Vector of items
     */
    Vector getItems()
    {
        return items;
    }

    /**
     * @return ScrolledComposite which represents Form
     */
    ScrolledComposite getFormComposite()
    {
        return formComposite;
    }


    /**
     * Returns form layout policy.
     * @return Reference to layout policy.
     *
     */
    FormLayoutPolicy getLayoutPolicy()
    {
        return layoutPolicy;
    }

    // =========================================================================
    // TODO: remove this

    private boolean layoutLR = true;

    public Form(boolean leftRightLanguage, String title)
    {
        super(title);
        construct();
        layoutLR = leftRightLanguage;
        layoutPolicy = new DefaultFormInteraction(this);
    }

    // =========================================================================

    /**
     * Returns if the language Left-to-Right.
     */
    boolean getLeftRightLanguage()
    {
        return layoutLR;
    }

    /**
     * Reset timer for do layout with a given start index.
     */
    private void resetLayoutTimer(int newStartIndex)
    {
        if(layoutTimer != null)
        {
            if(formTimerTask != null)
            {
                formTimerTask.cancel();
                formTimerTask = null;
            }
            // schedule new timer
            startIndex = Math.max(0, Math.min(newStartIndex, startIndex));
            // layoutPolicy.layoutForm(startIndex);
            formTimerTask = new FormTimerTask(startIndex);
            layoutTimer.schedule(formTimerTask, Config.FORM_LAYOUT_TIMER_DELAY);
        }
    }

    /**
     * Form Timer task. Triggers the formComposite to Layout.
     */
    class FormTimerTask extends TimerTask
    {

        private int index;

        FormTimerTask(int newIndex)
        {
            index = newIndex;
        }

        public void run()
        {
            Logger.method(Form.this, "layout");
            layoutPolicy.layoutForm(index);
            startIndex = items.size();
        }

    }


    /**
     * Form mouse listener.<Br>
     * TODO: change when DirectUI style arrives
     */
    /*class FormMouseListener implements Listener {
        public void handleEvent(Event e) {
            if (e.widget instanceof Control) {
                // translate this control's coordinates to display coordinates
                Point p = ((Control) e.widget).toDisplay(new Point(e.x, e.y));
                // translate display coordinates to composite coordinates
                p = formComposite.toControl(p);
                layoutPolicy.handlePointerEvent(p.x, p.y, e.type);
            }
        }
    }*/

    /**
     * Scrollbar selection listener.
     */
    class FormScrollBarListener implements SelectionListener
    {

        public void widgetDefaultSelected(SelectionEvent se)
        {
        }

        public void widgetSelected(SelectionEvent se)
        {
            ScrollBar sb = (ScrollBar) se.widget;
            layoutPolicy.updateScrolling(sb.getSelection(), false);
        }
    }

    /**
     * Dispose Form.
     */
    void dispose()
    {
        super.dispose();
        layoutTimer.cancel();
        layoutTimer = null;
        deleteAll();
        ((DefaultFormInteraction) layoutPolicy).dispose();
        layoutPolicy = null;
    }

    /*
     * Dispatcher thread calls.
     */
    void doCallback(LCDUIEvent event)
    {
        if((event.type & LCDUIEvent.CUSTOMITEMBIT) != 0)
        {
            // Synchronize with Form content modification operations.
            synchronized(layoutPolicy)
            {
                // If the eSWT widget has been explicitly disposed while the event
                // was in the queue then ignore the event.
                if(event.widget != null && event.widget.isDisposed())
                {
                    return;
                }
                // Find out if the Item is still in this Form. If not then
                // ignore the event.
                if(!items.contains(event.item))
                {
                    return;
                }
                // Perform the callback. Then allow Form modification again.
                event.item.doCallback(event);
            }
        }
        else
        {
            super.doCallback(event);
        }
    }
}
