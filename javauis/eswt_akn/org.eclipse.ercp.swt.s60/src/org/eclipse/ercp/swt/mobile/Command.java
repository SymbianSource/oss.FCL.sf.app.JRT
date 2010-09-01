/*******************************************************************************
 * Copyright (c) 2004, 2007 Nokia Corporation.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Yu You (Nokia Corp.) - initial API specification
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.ercp.swt.mobile;

import java.util.Vector;
import org.eclipse.swt.*;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.*;
import org.eclipse.ercp.swt.mobile.internal.*;

/**
 *
 * Command is a metaphor that represents a general action. Command contains no
 * information about the behavior that happens when command is activated. The
 * concrete action is defined in a SelectionListener. Commands may be
 * implemented in any user interface construct that has semantics for activating
 * a single action. Some implementations may implement commands as widgets, such
 * as buttons or menu items, or voice tags. The implementation, however, should
 * not affect applications' layout too much when realized as widgets.
 *
 * <p>
 * A Command must be associated with a control to represent its logical meaning.
 * The command can be further bound to a hardware key or device softkey. A
 * command always becomes accessible within a control context, within which the
 * control becomes active and focused. The control context includes the command
 * control itself and its ancestors' controls.
 * </p>
 *
 * <p>
 * As various devices have a different number of keys, the conventional way to
 * place commands by indexing is not appropriate. The placement of commands is
 * very dynamic. For example, new commands can be added, or their command labels
 * can be overwritten by the implementation at run-time in order to facilitate
 * some context-related operations.
 * </p>
 *
 * <p>
 * <code>Command type</code> is a hint used by the
 * implementation to associate commands with different keys.
 * It is common to bind more than one command of the same type to a control.
 * The implementation must guarantee that all commands within the current
 * control context can be accessible to the user, for example, by displaying a
 * drop-down or pop-up command menu. The concrete method is device-specific and
 * implementation-dependent.
 * </p>
 *
 * <p>
 * When multiple commands are in competition for key assignment, the
 * <code>priority</code> argument is used to determine assignment.
 * The lowest value (zero) has the lowest priority.
 * If priority values of competing commands are the same, higher priority is given
 * to commands created first.
 * Commands in the currently focused control are always given higher
 * priority than commands from ancestor controls regardless of their
 * <code>priority</code> value.
 * </p>
 *
 * <p>
 * Commands not assigned to keys are placed on a command menu.
 * The label of the command menu is implementation-dependent. Commands and
 * CommandGroups within the command menu are ordered by priority.
 * The <code>command type</code> is not used for ordering menus.
 * </p>
 *
 * <p>
 * A default command means that a command can be activated via a device
 * selection key or any other "short-cuts" like the concept of default button in
 * a shell. Usually the default command has a different visual type attached to
 * make it distinguishable from other commands. Within a control context, there
 * can only be one default command at any time. A command becomes the default
 * one via the method "setDefaultCommand", and any previous default command will
 * become non-default. This change can be done programmatically by applications.
 * </p>
 *
 * <p>
 * When pressing the key associated with a command, a CommandEvent will be
 * delivered to all the command's SelectionListeners, if any.
 * </p>
 *
 * <p>
 * Commands can have a long label (<code>setLongLabel(String)</code>).
 * The long label is optional. If the long label is not present on a command,
 * the default text label (<code>setText(String)</code>) is used. The
 * default label should be as short as possible so that it consumes a
 * minimum of screen real estate. The long label can be longer and more
 * descriptive, but it should be no longer than a few words. For example, a
 * command's default label might be "Play", and its long label might be "Play
 * Sound Clip".
 * </p>
 *
 * <p>
 * Note that although this class is a subclass of
 * {@link org.eclipse.swt.widgets.Item Item} and allows setting the image even
 * if the underlying platform does not. But in this case the image is not
 * displayed.
 * </p>
 * <h3>Example 1: Single Command per Control within a control context (List and
 * its parent)</h3>
 * <code><pre>
 * List list = new List(parent, SWT.SINGLE); // a single selection list
 * Command cmd1 = new Command(list, Command.SELECT, 1);
 * cmd1.setText(&quot;Open&quot;);
 * Command cmd2 = new Command(parent, Command.GENERAL, 0);
 * cmd2.setText(&quot;New&quot;);
 * cmd2.addSelectionListener(new SelectionListener() {
 *  public void widgetSelected(SelectionEvent e) {
 *      System.out.println(&quot;Command &quot; + e.data + &quot; pressed.&quot;);
 *      // e.widget contains the Command object.
 *  }
 * });
 * </pre></code>
 *
 * <h3>Example 2: Multiple Commands per Control and DefaultCommand</h3>
 * <code><pre>
 * List list = new List(composite1, SWT.SINGLE);
 * Command cmd1 = new Command(list, Command.SELECT, 0);
 * cmd1.setText(&quot;Open&quot;);
 * cmd1.setDefaultCommand();
 * Command cmd2 = new Command(list, Command.SELECT, 0);
 * cmd2.setText(&quot;Edit&quot;);
 * Command cmd3 = new Command(list, Command.SELECT, 0);
 * cmd3.setText(&quot;Delete&quot;);
 * Command cmd4 = new Command(composite1, Command.EXIT, 0); // create a EXIT type
 * cmd4.setText(&quot;Exit&quot;);
 * </pre></code>
 *
 * <dl>
 * <dt><b>Command types: </b></dt>
 * <dd>COMMANDGROUP: represents a special command that contains sub-commands
 * </dd>
 * <dd>GENERAL, SELECT, OK, CANCEL, DELETE, BACK, EXIT, STOP, HELP: Logical
 * command types</dd>
 * <dt><b>Events: </b></dt>
 * <dd>Selection</dd>
 * </dl>
 * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 */
public class Command extends Item
{
    private int handle;

    /**
     * A command that can be bound to any hardware keys or softkeys.
     *
     * <P>
     * Value <code>1</code> is assigned to <code>GENERAL</code>.
     * </P>
     *
     */
    public static final int GENERAL = 1;

    /**
     * A command that represents the context-sensitive action.
     *
     * <P>
     * Value <code>2</code> is assigned to <code>SELECT</code>.
     * </P>
     *
     */
    public static final int SELECT = 2;

    /**
     * A special command used for grouping commands. This command does not fire
     * SelectionEvents if there are any commands associated. Instead, pressing
     * down the command results in displaying sub-commands.
     * <P>
     * Value <code>10</code> is assigned to <code>COMMANDGROUP</code>.
     * </P>
     * </DL>
     *
     */
    public static final int COMMANDGROUP = 10;

    /**
     * A navigation command that returns the user to the logical OK action.
     *
     * <P>
     * Value <code>3</code> is assigned to <code>OK</code>.
     * </P>
     *
     */
    public static final int OK = 3;

    /**
     * A command that is a standard negative answer to an action.
     *
     * <p>
     * With this command type, the application hints to the implementation that
     * the user wants to dismiss the current action.
     * </p>
     * <P>
     * Value <code>4</code> is assigned to <code>CANCEL</code>.
     * </P>
     *
     */
    public static final int CANCEL = 4;

    /**
     * This command hints to the implementation to destroy data.
     * </p>
     * <P>
     * Value <code>5</code> is assigned to <code>DELETE</code>.
     * </P>
     *
     */
    public static final int DELETE = 5;

    /**
     * A command that hints to the implementation to navigate backward.
     *
     * <P>
     * Value <code>6</code> is assigned to <code>BACK</code>.
     * </P>
     *
     */
    public static final int BACK = 6;

    /**
     * A command that indicates an exiting action.
     * <P>
     * Value <code>7</code> is assigned to <code>EXIT</code>.
     * </P>
     * </DL>
     *
     */
    public static final int EXIT = 7;

    /**
     * A command that stops some currently running process, operation, etc.
     * Nothing is stopped automatically by the implementation.
     *
     * <P>
     * Value <code>8</code> is assigned to <code>STOP</code>.
     * </P>
     *
     */
    public static final int STOP = 8;

    /**
     * A command that specifies a request for on-line help. No help information
     * is shown automatically by the implementation. The application is
     * responsible for answering the event.
     *
     * <P>
     * Value <code>9</code> is assigned to <code>HELP</code>.
     * </P>
     *
     */
    public static final int HELP = 9;

    private Control control;
    private int type;
    private int priority;
    private int accelerator;
    private boolean enabled;
    private String longLabel;
    private Command parent;
    private Vector children;


    /**
     * Constructs a new instance of this class given an associated Command, a
     * type value describing its behaviour and priority for positioning hints.
     * <p>
     * The constructor creates a command associated with a Command (cannot be
     * null and must be COMMANDGROUP type). The implementation must group
     * commands within the same command group.
     * </p>
     *
     * @param command
     *            the associated Command. It must be COMMANDGROUP type.
     *            Otherwise throw an exception.
     * @param type
     *            the type of Command to construct
     * @param priority
     *            the priority value. The lowest value zero has the lowest
     *            priority.
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if the command type is not
     *                COMMANDGROUP</li>
     *                <li>ERROR_INVALID_ARGUMENT - if the type is not one of
     *                the valid command types</li>
     *                <li>ERROR_INVALID_ARGUMENT - if the priority is less than
     *                zero</li>
     *                <li>ERROR_NULL_ARGUMENT - if the command is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                <li>ERROR_INVALID_SUBCLASS - if this class is not an
     *                allowed subclass</li>
     *                </ul>
     *
     * @see #GENERAL
     * @see #SELECT
     * @see #OK
     * @see #CANCEL
     * @see #DELETE
     * @see #BACK
     * @see #EXIT
     * @see #STOP
     * @see #HELP
     */
    public Command(Command command, int type, int priority)
    {
        super(command, SWT.NONE);

        checkWidget();
        if (command.type != Command.COMMANDGROUP) internal_error(SWT.ERROR_INVALID_ARGUMENT);
        if (priority < 0) internal_error(SWT.ERROR_INVALID_ARGUMENT);
        if (type < Command.GENERAL || type > Command.COMMANDGROUP) internal_error(SWT.ERROR_INVALID_ARGUMENT);

        this.control = command.control;
        this.parent  = command;
        this.type = type;
        this.priority = priority;
        this.enabled = true;
        createWidget();
        parent.addCommand(this);
    }

    /**
     * Constructs a new instance of this class given an associated Control, a
     * type value describing its behaviour and priority for positioning hints.
     * <p>
     * The type value is one of the type constants defined in class
     * <code>Command</code>
     * </p>
     *
     * @param control
     *            the associated Control
     * @param type
     *            the type of Command to construct
     * @param priority
     *            the priority value. The lowest value zero has the lowest
     *            priority.
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if the type is not one of
     *                the valid command types</li>
     *                <li>ERROR_INVALID_ARGUMENT - if the priority is less than
     *                zero</li>
     *                <li>ERROR_NULL_ARGUMENT - if the control is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                <li>ERROR_INVALID_SUBCLASS - if this class is not an
     *                allowed subclass</li>
     *                </ul>
     *
     * @see #GENERAL
     * @see #SELECT
     * @see #OK
     * @see #CANCEL
     * @see #DELETE
     * @see #BACK
     * @see #EXIT
     * @see #STOP
     * @see #HELP
     */
    public Command(Control control, int type, int priority)
    {
        super(control, SWT.NONE);

        checkWidget();
        if (priority < 0) internal_error(SWT.ERROR_INVALID_ARGUMENT);
        if (type < Command.GENERAL || type > Command.COMMANDGROUP) internal_error(SWT.ERROR_INVALID_ARGUMENT);

        this.control = control;
        this.type = type;
        this.priority = priority;
        this.enabled = true;
        createWidget();
        CommandCollection.getCommandChildren(control).addCommand(this);
    }

    // Command extends Item but is not intended to be subclassed
    protected void checkSubclass()
    {
        if (!internal_isValidSubclass()) internal_error(SWT.ERROR_INVALID_SUBCLASS);
    }

    void createWidget()
    {
        int parentHandle = (parent == null) ? 0 : parent.handle;
        handle = OS.Command_New(this, control.handle, parentHandle, type, priority);
    }

    // from Widget
    protected void internal_releaseHandle()
    {
        if (handle != 0)
        {
            OS.Command_Dispose(handle);
            handle = 0;
        }
        super.internal_releaseHandle();
    }

    /**
     * Adds the listener to the collection of listeners who will be notified
     * when the command is activated, by sending it one of the messages defined
     * in the <code>SelectionListener</code> interface.
     * <p>
     * When <code>widgetSelected</code> is called, the stateMask field of the
     * event object is valid. <code>widgetDefaultSelected</code> is not
     * called.
     * </p>
     *
     * @param listener
     *            the listener which should be notified
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     *
     * @see SelectionListener
     * @see #removeSelectionListener
     */
    public void addSelectionListener(SelectionListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Selection,typedListener);
    }

    /**
     * Returns <code>true</code> if the receiver is enabled, and
     * <code>false</code> otherwise. A disabled control is typically not
     * selectable from the user interface and draws with an inactive or "grayed"
     * look.
     *
     * @return the receiver's enabled state
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     *
     * @see #isEnabled
     */
    public boolean getEnabled()
    {
        checkWidget();
        return enabled;
    }

    /**
     * Returns the command's long label, which shall be null if it has never
     * been set.
     *
     * @return the receiver's text. Can be null.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     * @see #setLongLabel(String)
     */
    public String getLongLabel()
    {
        checkWidget();
        return longLabel;
    }

    /**
     * Returns the command's priority value.
     *
     * @return the priority value.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     */
    public int getPriority()
    {
        checkWidget();
        return priority;
    }

    /**
     * Returns <code>true</code> if the command is a Default Command, and
     * <code>false</code> otherwise. By default a command is not a default
     * one.
     *
     * @return boolean the command's state
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     * @see #setDefaultCommand()
     */
    public boolean isDefaultCommand()
    {
        checkWidget();
        return OS.Command_IsDefaultCommand(handle);
    }

    /**
     * Returns <code>true</code> if the receiver is enabled and all of the
     * receiver's ancestors are enabled, and <code>false</code> otherwise. A
     * disabled control is typically not selectable from the user interface and
     * draws with an inactive or "grayed" look.
     *
     * @return boolean the receiver's enabled state
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     * @see #getEnabled
     */
    public boolean isEnabled()
    {
        checkWidget();
        if (parent != null)
            return getEnabled() && parent.isEnabled();
        return getEnabled() && control.isEnabled();
    }

    /**
     * Removes the listener from the collection of listeners who will be
     * notified when the command is activated.
     *
     * @param listener
     *            the listener which should be notified
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     *
     * @see SelectionListener
     * @see #addSelectionListener
     */
    public void removeSelectionListener(SelectionListener listener)
    {
        removeListener(SWT.Selection, listener);
    }

    /**
     * Sets the command accelerator. An accelerator is the bit-wise OR of zero
     * or more modifier masks and a key. Examples:
     * <code>SWT.MOD1 | SWT.MOD2 | 'T'</code>,
     * <code>SWT.CONTROL | SWT.SHIFT | 'T'</code>.
     *
     * This requires fullkeyboard or computer keyboard-like support and is an
     * implementation-dependent feature. It will be gracefully ignored if the
     * implementation does not support keyboard accelerator.
     *
     * @param accelerator
     *            an integer that is the bit-wise OR of masks and a key
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if accelerator value is
     *                invalid</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     *
     */
    public void setAccelerator(int accelerator)
    {
        checkWidget();
        this.accelerator = accelerator;
        OS.Command_SetAccelerator(handle, accelerator);
    }

    /**
     * Sets the default command.
     *
     */
    public void setDefaultCommand()
    {
        checkWidget();
        OS.Command_SetDefaultCommand(handle);
    }

    /**
     * Enables the command if the argument is <code>true</code>, and disables
     * it otherwise. A disabled control is typically not selectable from the
     * user interface and draws with an inactive or "grayed" look; or even
     * completely removed from the user interface.
     *
     * @param enabled
     *            the new enabled state
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     */
    public void setEnabled(boolean enabled)
    {
        checkWidget();
        if (this.enabled != enabled)
        {
            this.enabled = enabled;
            OS.Command_SetEnabled(handle, enabled);
        }
    }

    public void setText(String string)
    {
        super.setText(string);
        OS.Command_SetText(handle, string);
    }

    public void setImage(Image image)
    {
        super.setImage(image);
        OS.Command_SetImage(handle, (image == null) ? 0 : image.handle);
    }

    /**
     * Sets the command's long label text.
     *
     * Commands may optionally have long labels, enabling the application to
     * specify a short form for button or softkey, and a more verbose form when
     * realized as menu item.
     *
     * @param label
     *            the long descriptive label text. Can be null.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     * @see #getLongLabel()
     */
    public void setLongLabel(String label)
    {
        checkWidget();
        this.longLabel = label;
        if (label == null)
            OS.Command_SetLongLabel(handle, "");
        else
            OS.Command_SetLongLabel(handle, label);
    }

    protected void internal_releaseResources()
    {
        // dispose children
        if (children != null)
        {
            int count = children.size();
            for (int i=0; i<count; ++i)
            {
                Command command = (Command)children.elementAt(i);
                if (!command.isDisposed()) command.internal_release();
            }
            children.removeAllElements();
            children = null;
        }

        parent = null;
        control = null;

        super.internal_releaseResources();
    }

    protected void internal_prepareForRelease()
    {
        if (parent==null)
        {
            CommandCollection.getCommandChildren(control).removeCommand(this);
        }
        else
        {
            parent.removeCommand(this);
        }
    }

    private void addCommand(Command child)
    {
        if (children == null)
            children = new Vector();
        children.addElement(child);
    }

    private void removeCommand(Command child)
    {
        children.removeElement(child);
    }
}
