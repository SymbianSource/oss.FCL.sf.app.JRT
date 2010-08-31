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

import java.util.Vector;
import java.lang.ref.WeakReference;

import javax.microedition.lcdui.EventDispatcher.LCDUIEvent;
import com.nokia.mj.impl.nokialcdui.ItemControlStateChangeListener;
import org.eclipse.swt.graphics.Point;

/**
 * Abstract class representing an item.<br>
 */
public abstract class Item
{

    /**
     * A layout directive.
     */
    public static final int LAYOUT_DEFAULT = 0;

    /**
     * A layout directive.
     */
    public static final int LAYOUT_LEFT = 1;

    /**
     * A layout directive.
     */
    public static final int LAYOUT_RIGHT = 2;

    /**
     * A layout directive.
     */
    public static final int LAYOUT_CENTER = 3;

    /**
     * A layout directive.
     */
    public static final int LAYOUT_TOP = 16;

    /**
     * A layout directive.
     */
    public static final int LAYOUT_BOTTOM = 32;

    /**
     * A layout directive.
     */
    public static final int LAYOUT_VCENTER = 48;

    /**
     * A layout directive.
     */
    public static final int LAYOUT_NEWLINE_BEFORE = 256;

    /**
     * A layout directive.
     */
    public static final int LAYOUT_NEWLINE_AFTER = 512;

    /**
     * A layout directive.
     */
    public static final int LAYOUT_SHRINK = 1024;

    /**
     * A layout directive.
     */
    public static final int LAYOUT_EXPAND = 2048;

    /**
     * A layout directive.
     */
    public static final int LAYOUT_VSHRINK = 4096;

    /**
     * A layout directive.
     */
    public static final int LAYOUT_VEXPAND = 8192;

    /**
     * A layout directive indicating that MIDP 2 layout rules are used.
     */
    public static final int LAYOUT_2 = 16384;

    /**
     * An appearance mode value.
     */
    public static final int PLAIN = 0;

    /**
     * An appearance mode value.
     */
    public static final int HYPERLINK = 1;

    /**
     * An appearance mode value.
     */
    public static final int BUTTON = 2;

    /**
     * Combination of all possible layout directives.
     */
    private static final int LAYOUT_BITMASK =
        LAYOUT_DEFAULT
        | LAYOUT_LEFT
        | LAYOUT_RIGHT
        | LAYOUT_CENTER
        | LAYOUT_TOP
        | LAYOUT_BOTTOM
        | LAYOUT_VCENTER
        | LAYOUT_NEWLINE_BEFORE
        | LAYOUT_NEWLINE_AFTER
        | LAYOUT_SHRINK
        | LAYOUT_EXPAND
        | LAYOUT_VSHRINK
        | LAYOUT_VEXPAND
        | LAYOUT_2;

    static final int LAYOUT_HORIZONTAL_MASK = LAYOUT_CENTER; // 15;

    static final int LAYOUT_VERTICAL_MASK = LAYOUT_VCENTER; // 48;

    /**
     * If Item is changed, reasons for Re-layouting.
     */
	static final int UPDATE_NONE = 0;

	static final int UPDATE_ADDCOMMAND = 1;
	static final int UPDATE_REMOVECOMMAND = 1 << 1;
	static final int UPDATE_DEFAULTCOMMAND = 1 << 2;
	static final int UPDATE_ITEMCOMMANDLISTENER = 1 << 3;
	static final int UPDATE_LABEL = 1 << 4;
	static final int UPDATE_LAYOUT = 1 << 5;
	static final int UPDATE_PREFERREDSIZE = 1 << 6;

    static final int UPDATE_HEIGHT_CHANGED = 1 << 7;
    static final int UPDATE_WIDTH_CHANGED = 1 << 8;
    static final int UPDATE_SIZE_CHANGED =
        UPDATE_HEIGHT_CHANGED | UPDATE_WIDTH_CHANGED;
    static final int UPDATE_SIZE_MASK = ~UPDATE_SIZE_CHANGED;

	static final int UPDATE_ITEM_MAX = 1 << 15;


    private String label;

    /**
     * Vector of commands added to this item that have eSWT Command associated
     * with them.
     */
    private Vector commands = new Vector();
    private ItemCommandListener itemCommandListener;
    private ItemControlStateChangeListener controlListener;

    private Command defaultCommand;
	private WeakReference wParent;

    private int layout;
    private int lockedPrefWidth = -1;
    private int lockedPrefHeight = -1;

    private Point calculatedMinSize;
    private Point calculatedPrefSize;

    private boolean focused;
    private boolean visible;

    /**
     * Sets the parent of this Item.
     * @param parent new Parent. If null, current parent is to be removed.
     */
    void setParent(Screen parent)
    {
    	if(parent != null)
    	{
    		wParent = new WeakReference(parent);
    	}
		else
		{
			clearParent();
		}
    }

    /**
     * Gets the Item's parent.
     *
     * @return the Item's parent or null if it has none.
     */
    Screen getParent()
    {
    	if(wParent != null)
    	{
    		return (Screen)wParent.get();
    	}
		else
		{
			return null;
		}
    }

    /**
     * Clears the Item's parent.
     *
     */
    void clearParent()
    {
    	if(wParent != null)
    	{
    		wParent.clear();
			wParent = null;
    	}
    }

    /**
     * Sets the label of the item.
     *
     * @param newLabel New label to be set.
     * @throws IllegalStateException If this item is contained within an alert.
     */
    public void setLabel(String newLabel)
    {
        if(isContainedInAlert())
        {
            throw new IllegalStateException(
                MsgRepository.ITEM_EXCEPTION_OWNED_BY_ALERT);
        }
    	if((newLabel == null) && (label == null))
		{
			return;
		}
        label = newLabel;
        updateParent(UPDATE_LABEL | UPDATE_SIZE_CHANGED);
    }

    /**
     * Gets the label of the item.
     *
     * @return The label of the item.
     */
    public String getLabel()
    {
        return label;
    }

    /**
     * Returns if this item has a valid label, not null and not empty.
     */
    boolean hasLabel()
    {
        return ((label != null) && (!label.equals("")));
    }

    /**
     * Gets item's current layout.
     *
     * @return A combination of layout directives currently in use.
     */
    public int getLayout()
    {
        return layout;
    }

    /**
     * Sets the layout of the item.
     *
     * @param newLayout a Combination of layout directives to be used.
     * @throws IllegalArgumentException if newLayout is not valid bitwise OR
     *             combination of layout directives spesified in this class.
     * @throws IllegalStateException If this Item is contained within an Alert.
     */
    public void setLayout(int newLayout)
    {
        if(isContainedInAlert())
        {
            throw new IllegalStateException(
                MsgRepository.ITEM_EXCEPTION_OWNED_BY_ALERT);
        }

        if(!isValidLayout(newLayout))
        {
            throw new IllegalArgumentException(
                MsgRepository.ITEM_EXCEPTION_INVALID_LAYOUT);
        }
        layout = newLayout;
        Logger.method(this, "setLayout", String.valueOf(layout));
        updateParent(UPDATE_LAYOUT | UPDATE_SIZE_CHANGED);
    }

    /**
     * Adds command to this item. If same command is already added to this item,
     * nothing happens.
     *
     * @param command A command to be added.
     * @throws NullPointerException if cmd is null.
     * @throws IllegalStateException If this Item is contained within an Alert.
     */
    public void addCommand(Command command)
    {
        if(isContainedInAlert())
        {
            throw new IllegalStateException(
                MsgRepository.ITEM_EXCEPTION_OWNED_BY_ALERT);
        }
        if(command == null)
        {
            throw new NullPointerException(
                MsgRepository.ITEM_EXCEPTION_NULL_COMMAND_ADDED);
        }
        if(!commands.contains(command))
        {
            commands.addElement(command);
            updateParent(UPDATE_ADDCOMMAND, command);
        }
    }

    /**
     * Removes command from the item. If command doesn't exists in this item,
     * nothing happens.
     *
     * @param command The command to be removed.
     */
    public void removeCommand(Command command)
    {
        // It is not specified what should happen when this method is
        // called with null-parameter !
        if(command != null && commands.contains(command))
        {
            // Remove command from commands-vector
            commands.removeElement(command);
			if(defaultCommand == command)
			{
            	defaultCommand = null;
			}
            updateParent(UPDATE_REMOVECOMMAND, command);
        }
    }


    /**
     * Sets Listener which will receive command events associated with this
     * item.
     *
     * @param newItemCmdListener A new listener. If null, already existing
     *            listener is removed.
     * @throws IllegalStateException If this Item is contained within an Alert.
     */
    public void setItemCommandListener(ItemCommandListener newItemCmdListener)
    {
        if(isContainedInAlert())
        {
            throw new IllegalStateException(
                MsgRepository.ITEM_EXCEPTION_OWNED_BY_ALERT);
        }
        itemCommandListener = newItemCmdListener;
    }

    /**
     * Gets the minimum width of this item.
     *
     * @return Minimum width.
     */
    public int getMinimumWidth()
    {
        return getCalculatedMinimumSize().x;
    }

    /**
     * Gets the minimum height of this item.
     *
     * @return Minimum height.
     */
    public int getMinimumHeight()
    {
        return getCalculatedMinimumSize().y;
    }

    private Point getCalculatedMinimumSize()
    {
        if(calculatedMinSize == null)
        {
            calculatedMinSize = calculateMinimumSize();
            // Logger.method(this, "calculateMinimumSize", calculatedMinSize);
        }
        return calculatedMinSize;
    }

    /**
     * Calculates minimum size of this item.
     *
     * @return Minimum size.
     */
    abstract Point calculateMinimumSize();

    /**
     * Gets the preferred width of this item.
     *
     * @return Preferred width.
     */
    public int getPreferredWidth()
    {
        if(lockedPrefWidth >= 0)
        {
            if(calculatedMinSize == null)
            {
                checkLockedSizes();
            }
            return lockedPrefWidth;
        }
        else
        {
            return getCalculatedPreferredSize().x;
        }
    }

    /**
     * Gets the preferred height of this item.
     *
     * @return Preferred height.
     */
    public int getPreferredHeight()
    {
        if(lockedPrefHeight >= 0)
        {
            if(calculatedMinSize == null)
            {
                checkLockedSizes();
            }
            return lockedPrefHeight;
        }
        else
        {
            return getCalculatedPreferredSize().y;
        }
    }

    private Point getCalculatedPreferredSize()
    {
        if(calculatedPrefSize == null)
        {
            calculatedPrefSize = calculatePreferredSize();
            // Logger.method(this, "calculatePreferredSize", calculatedPrefSize);
        }
        return calculatedPrefSize;
    }

    void checkLockedSizes()
    {
        if(lockedPrefWidth >= 0)
        {
            lockedPrefWidth = Math.min(
                                  Math.max(lockedPrefWidth, getMinimumWidth()),
                                  ItemLayouter.getMaximumItemWidth(null));
        }
        if(lockedPrefHeight >= 0)
        {
            lockedPrefHeight = Math.max(lockedPrefHeight, getMinimumHeight());
        }
    }

    /**
     * Invalidates this Item's calculated preferred size. Forces the
     * implementation to calculate it again.
     */
    void invalidateCachedSizes()
    {
        calculatedMinSize = null;
        calculatedPrefSize = null;
    }

    /**
     * Calculates preferred size of this item.
     *
     * @return Preferred size.
     */
    abstract Point calculatePreferredSize();

    /**
     * Sets the preferred size of this item. If new value is larger than -1 and
     * less than minimum value, the minimum value is used instead. If value is
     * larger than minimum value, the dimension is locked to provided value. It
     * is possible to unlock dimension by setting value to -1.
     *
     * @param w New preferred width.
     * @param h New preferred height.
     * @throws IllegalArgumentException if w or h is less than -1.
     * @throws IllegalStateException If this Item is contained within an Alert.
     */
    public synchronized void setPreferredSize(int w, int h)
    {
        if(isContainedInAlert())
        {
            throw new IllegalStateException(
                MsgRepository.ITEM_EXCEPTION_OWNED_BY_ALERT);
        }
        if((w < -1) || (h < -1))
        {
            throw new IllegalArgumentException(
                MsgRepository.ITEM_EXCEPTION_INVALID_DIMENSION);
        }

        lockedPrefWidth = w;
        lockedPrefHeight = h;
        checkLockedSizes();
        Logger.method(this, "setPrefSize",
                      String.valueOf(lockedPrefWidth),
                      String.valueOf(lockedPrefHeight));
        updateParent(UPDATE_PREFERREDSIZE | UPDATE_SIZE_CHANGED);
    }

    /**
     * Sets the default command of this item. This method doesn't remove
     * commands from the item, it just tells which command is the default one.
     *
     * @param cmd New default command. If cmd doesn't exists in this item yet it
     *            is added to it. Null means that this item wont have default
     *            command at all after this call.
     * @throws IllegalStateException If this Item is contained within an Alert.
     */
    public void setDefaultCommand(Command cmd)
    {
        if(isContainedInAlert())
        {
            throw new IllegalStateException(
                MsgRepository.ITEM_EXCEPTION_OWNED_BY_ALERT);
        }
        defaultCommand = cmd;
        if(cmd != null)
        {
            // It is safe to call addCommand() even if command already exists
            // because the addCommand() wont create duplicates:
            addCommand(cmd);
        }

        // Form need to be updated if this item is focused:
        updateParent(UPDATE_SIZE_CHANGED);
    }

    /**
     * Makes this item's containing form to notify item's ItemStateListener.
     * This method must be called only when item's state has actually changed
     * and when that change has occured because of user.
     *
     * If ItemStateListener is not set to the Form where this Item belongs,
     * nothing happens.
     *
     * @throws IllegalStateException If the item is not owned by a form.
     */
    public void notifyStateChanged()
    {
        if(!isContainedInForm())
        {
            throw new IllegalStateException(
                MsgRepository.ITEM_EXCEPTION_NOT_OWNED_BY_FORM);
        }
        // Notify item state listener
        ((Form) getParent()).notifyItemStateChanged(this);
    }

    /**
     * Is this item's size locked.
     */
    boolean isSizeLocked()
    {
        return (lockedPrefWidth >= 0) || (lockedPrefHeight >= 0);
    }

    /**
     * Gets locked preferred width of this item.
     *
     * @return Locked preferred width. If width is not locked, returns -1.
     */
    int getLockedPreferredWidth()
    {
        return lockedPrefWidth;
    }

    /**
     * Gets locked preferred height of this item.
     *
     * @return Locked preferred height. If height is not locked, returns -1.
     */
    int getLockedPreferredHeight()
    {
        return lockedPrefHeight;
    }

    /**
     * Gets LAYOUT_2 width of this item.
     */
    int getLayoutWidth()
    {
        if(hasLayout(LAYOUT_SHRINK))
        {
            return getMinimumWidth();
        }
        return getPreferredWidth();
    }

    /**
     * Gets LAYOUT_2 height of this item.
     */
    int getLayoutHeight()
    {
        if(hasLayout(LAYOUT_VSHRINK))
        {
            return getMinimumHeight();
        }
        return getPreferredHeight();
    }

    /**
     * If the item is owned by an Alert.
     */
    boolean isContainedInAlert()
    {
        return ((wParent != null) && (getParent() instanceof Alert));
    }

    /**
     * If the item is owned by an Form.
     */
    boolean isContainedInForm()
    {
        return ((wParent != null) && (getParent() instanceof Form));
    }

    /**
     * Return internal layout with optional custom flags.
     *
     * @return layout directive
     */
    int internalGetLayout()
    {
        return getLayout();
    }

    /**
     * Updates the parent if it's a Form.
     */
    void updateParent(int updateReason)
    {
        updateParent(updateReason, null);
    }

    /**
     * Updates the parent if it's a Form.
     *
     * @param param additional parameter
     */
    void updateParent(int updateReason, Object param)
    {
        if((updateReason & UPDATE_SIZE_CHANGED) != 0)
        {
            invalidateCachedSizes();
        }
        if(isContainedInForm())
        {
        	((Form) getParent()).updateItemState(this, updateReason, param);
        }
    }

    boolean hasLayout(int aLayout)
    {
        return (getLayout() & aLayout) != 0;
    }

    /**
     * Check that new layout is bitwise OR combination of layout
     * directives:
     *
     * @param layout Layout combination to be check.
     * @return true If provided layout is valid.
     */
    static boolean isValidLayout(int layout)
    {
        if((layout & (0xffffffff ^ LAYOUT_BITMASK)) != 0)
        {
            return false;
        }
        return true;
    }

    /**
     * Get Item horizontal layout directive: LAYOUT_LEFT, LAYOUT_RIGHT,
     * LAYOUT_CENTER, DEFAULT_LAYOUT .
     *
     * @param layout item Layout.
     * @return horizontal layout of an Item.
     */
    static int getHorizontalLayout(int layout)
    {
        return layout & Item.LAYOUT_HORIZONTAL_MASK;
    }

    /**
     * Get Item vertical Layout directive: LAYOUT_TOP, LAYOUT_BOTTOM,
     * LAYOUT_VCENTER.
     *
     * @param layout item Layout.
     * @return vertical layout of an Item.
     */
    static int getVerticalLayout(int layout)
    {
        return layout & Item.LAYOUT_VERTICAL_MASK;
    }

    /**
     * Is item focusable.
     */
    boolean isFocusable()
    {
        return false;
    }

    /**
     * Sets current item as focused
     */
    void internalSetFocused(boolean isFocused)
    {
        if(isFocusable())
        {
            focused = isFocused;
        }
    }

    /**
     * Check that item has current focus on it.
     *
     * @return true if item has focus.
     */
    boolean isFocused()
    {
        return focused;
    }

    /**
     * Sets current item visibility
     */
    void internalSetVisible(boolean isVisible)
    {
        this.visible = isVisible;
    }

    /**
     * Check that item is visible.
     *
     * @return true if item is visible
     */
    boolean isVisible()
    {
        return visible;
    }

    /**
     * Returns the number of commands.
     *
     * @return number of commands.
     */
    int getNumCommands()
    {
        return commands.size();
    }

    /**
     * Gets default command of this item.
     *
     * @return Default command or null if no default set.
     */
    Command getDefaultCommand()
    {
        return defaultCommand;
    }

    /**
     * Returns the Default Command or if not set, then the first Command.
     */
    Command getMSKCommand()
    {
        Command ret = null;
        if(defaultCommand != null)
        {
            ret = defaultCommand;
        }
        else if(getNumCommands() == 1)
        {
            ret = (Command) commands.elementAt(0);
        }
        return ret;
    }

    /**
     * Calls the command action on the owning Items command listener.
     *
     * @param command the Command
     */
    void callCommandAction(Command command)
    {
        if(itemCommandListener != null && command != null)
        {
            EventDispatcher eventDispatcher = EventDispatcher.instance();
            LCDUIEvent event = eventDispatcher.newEvent(
                                   LCDUIEvent.ITEM_COMMANDACTION,
                                   this);
            event.command = command;
            event.itemCommandListener = itemCommandListener;
            eventDispatcher.postEvent(event);
        }
    }

    /**
     * Gets commands of this item.
     *
     * @return Vector of commands added to this item.
     *      Vector may be empty but not null.
     */
    Vector getCommands()
    {
        return commands;
    }

    /**
     * Gets ItemCommandListener.
     * @return Current ItemCommandListener or null if
     *      no listener set.
     */
    ItemCommandListener getItemCommandListener()
    {
        return itemCommandListener;
    }

    void setItemControlStateChangeListener(ItemControlStateChangeListener listener)
    {
        controlListener = listener;
    }

    ItemControlStateChangeListener getItemControlStateChangeListener()
    {
        return controlListener;
    }

    /*
     * Dispatcher thread calls.
     */
    void doCallback(LCDUIEvent event)
    {
        switch(event.type)
        {
        case LCDUIEvent.ITEM_COMMANDACTION:
            event.itemCommandListener.commandAction(event.command, this);
            break;
        }
    }
}
