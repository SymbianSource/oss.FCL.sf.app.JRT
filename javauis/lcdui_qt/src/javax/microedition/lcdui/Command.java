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

import java.util.Enumeration;
import java.util.Hashtable;

import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.widgets.Control;

/**
 * Implementation of LCDUI <code>Command</code> class.
 */
public class Command {

    /**
     * Command type SCREEN.
     */
    public static final int SCREEN = 1;

    /**
     * Command type BACK.
     */
    public static final int BACK = 2;

    /**
     * Command type CANCEL.
     */
    public static final int CANCEL = 3;

    /**
     * Command type OK.
     */
    public static final int OK = 4;

    /**
     * Command type HELP.
     */
    public static final int HELP = 5;

    /**
     * Command type STOP.
     */
    public static final int STOP = 6;

    /**
     * Command type EXIT.
     */
    public static final int EXIT = 7;

    /**
     * Command type ITEM.
     */
    public static final int ITEM = 8;

    private static final String EMPTY_STRING = "";

    private String shortLabel;
    private String longLabel;
    private int type;
    private int priority;

    // List of eSWT's Commands associated with this Command.
    // There must be one eSWT Command per Control.
    private Hashtable eswtCommands = new Hashtable();

    // List of priorities created. This is used when LCDUI priorities
    // are mapped to eSWT priorities.
    private static Hashtable priorities;

    static {
        priorities = new Hashtable();
        priorities.put(new Integer(Integer.MIN_VALUE),
                       new Integer(0));
        priorities.put(new Integer(Integer.MAX_VALUE),
                       new Integer(Integer.MAX_VALUE));
    }

    /**
     * Creates new Command.
     *
     * @param shortLabel Label of Command.
     * @param type Type of command.
     * @param priority Priority of command.
     */
    public Command(String shortLabel, int type, int priority) {
        this(shortLabel, null, type, priority);
    }

    /**
     * Creates new Command.
     *
     * @param shortLabel Label of command.
     * @param longLabel Long label of command.
     * @param type Type of command.
     * @param priority Priority of command.
     */
    public Command(String shortLabel,
                   String longLabel,
                   int type,
                   int priority) {
        if (shortLabel == null) {
            throw new NullPointerException(
                    MsgRepository.COMMAND_EXCEPTION_LABEL_IS_NULL);
        }
        if (type < SCREEN || type > ITEM) {
            throw new IllegalArgumentException(
                    MsgRepository.COMMAND_EXCEPTION_INVALID_TYPE);
        }

        this.type = type;
        this.shortLabel = shortLabel;
        this.longLabel = longLabel;
        this.priority = priority;

        if (EMPTY_STRING.equals(this.shortLabel)) {
            this.shortLabel = getDefaultLabel(this.type);
        }
    }

    /**
     * Gets the label of the command.
     *
     * @return Label of this command.
     */
    public String getLabel() {
        return shortLabel;
    }

    /**
     * Get the long label of the command.
     *
     * @return Long label of this command.
     */
    public String getLongLabel() {
        return longLabel;
    }

    /**
     * Gets the command type.
     *
     * @return Type of this command.
     */
    public int getCommandType() {
        return type;
    }

    /**
     * Gets the priority of the command.
     *
     * @return Priority of this command.
     */
    public int getPriority() {
        return priority;
    }

    /**
     * Gets eSWT Command associated with this command on the queried Control.
     *
     * @param control the queried Control.
     * @return eSWT Command.
     */
    org.eclipse.ercp.swt.mobile.Command getESWTCommand(Control control) {
        if (control != null) {
            return (org.eclipse.ercp.swt.mobile.Command)
                eswtCommands.get(control);
        }
        return null;
    }

    /**
     * Add a SelectionListener to this command's eSWT command widget on the
     * specified control.
     *
     * @param control
     * @param listener
     */
    void eswtAddCommandSelectionListener(Control control,
            SelectionListener listener) {
        if (listener != null) {
            org.eclipse.ercp.swt.mobile.Command cmd = getESWTCommand(control);
            if (cmd != null && !cmd.isDisposed()) {
                // Remove listener if it is already added. Same listener
                // should be in same command only once:
                cmd.removeSelectionListener(listener);
                cmd.addSelectionListener(listener);
            }
        }
    }

    /**
     * Removes the SelectionListener from this command's eSWT command widget on
     * the specified control.
     *
     * @param control
     * @param listener
     */
    void eswtRemoveCommandSelectionListener(Control control,
            SelectionListener listener) {
        if (listener != null) {
            org.eclipse.ercp.swt.mobile.Command cmd = getESWTCommand(control);
            if (cmd != null && !cmd.isDisposed()) {
                cmd.removeSelectionListener(listener);
            }
        }
    }

    /**
     * Adds eSWT Command to this command.
     *
     * @param control The Control where eSWT Command is added. For example
     *            Shell.
     * @param isDefault If true, created eSWT command is set to be default one.
     */
    void addESWTCommand(Control control, final boolean isDefault) {
        final Control finalControl = control;
        ESWTUIThreadRunner.syncExec(new Runnable() {
            public void run() {
                eswtAddESWTCommand(finalControl, isDefault);
            }
        });
    }

    /**
     * eSWT callback to add an eSWT Command to vector of eSWT Commands.
     *
     * @param control eSWT control on which the command is created.
     * @param isDefault If true, created eSWT command is set to be default one.
     */
    void eswtAddESWTCommand(Control control, boolean isDefault) {
        if (!eswtCommands.containsKey(control)) {
            int eswtType = getEswtType(type);
            int eswtPriority = getESWTPriority(priority);

            org.eclipse.ercp.swt.mobile.Command eswtCommand =
                new org.eclipse.ercp.swt.mobile.Command(
                    control, eswtType, eswtPriority);
            eswtCommand.setText(shortLabel);
            if (longLabel != null) {
                eswtCommand.setLongLabel(longLabel);
            }
            if (isDefault) {
                eswtCommand.setDefaultCommand();
            }
            eswtCommands.put(control, eswtCommand);
        }
    }

    /**
     * Removes eSWT Command from this command.
     *
     * @param control The Control where eSWT Command is located.
     */
    void removeESWTCommand(Control control) {
        final Control finalControl = control;
        ESWTUIThreadRunner.syncExec(new Runnable() {
            public void run() {
                eswtRemoveESWTCommand(finalControl);
            }
        });
    }

    /**
     * eSWT callback to remove an eSWT Command to vector of eSWT Commands.
     */
    void eswtRemoveESWTCommand(Control control) {
        if (eswtCommands.containsKey(control)) {
            if (getESWTCommand(control) != null) {
                getESWTCommand(control).dispose();
            }
            eswtCommands.remove(control);
        }
    }

    /**
     * Get eSWT command type.
     *
     * @param cmdType LCDUI type
     * @return mapped eSWT command type.
     */
    private int getEswtType(int cmdType) {
        int eswtType = 0;
        // There are no Command types SCREEN and ITEM in eSWT so
        // the eSWT type GENERAL is used for those types.
        switch (cmdType) {
            case SCREEN:
                eswtType = org.eclipse.ercp.swt.mobile.Command.GENERAL;
                break;
            case BACK:
                eswtType = org.eclipse.ercp.swt.mobile.Command.BACK;
                break;
            case CANCEL:
                eswtType = org.eclipse.ercp.swt.mobile.Command.CANCEL;
                break;
            case OK:
                eswtType = org.eclipse.ercp.swt.mobile.Command.OK;
                break;
            case HELP:
                eswtType = org.eclipse.ercp.swt.mobile.Command.HELP;
                break;
            case STOP:
                eswtType = org.eclipse.ercp.swt.mobile.Command.STOP;
                break;
            case EXIT:
                eswtType = org.eclipse.ercp.swt.mobile.Command.EXIT;
                break;
            case ITEM:
                eswtType = org.eclipse.ercp.swt.mobile.Command.GENERAL;
                break;
            default:
                break;
        }
        return eswtType;
    }

    /**
     * If Command is created with empty label ("") then system should use
     * default label depending of Command type. This method will return the
     * default label. Method assumes that the type-field is initialized before
     * this method is called.
     *
     * @return Default label.
     */
    static String getDefaultLabel(int type) {
        String rValue;
        switch (type) {
            case SCREEN:
                rValue = MsgRepository.COMMAND_LABEL_SCREEN;
                break;
            case BACK:
                rValue = MsgRepository.COMMAND_LABEL_BACK;
                break;
            case CANCEL:
                rValue = MsgRepository.COMMAND_LABEL_CANCEL;
                break;
            case OK:
                rValue = MsgRepository.COMMAND_LABEL_OK;
                break;
            case HELP:
                rValue = MsgRepository.COMMAND_LABEL_HELP;
                break;
            case STOP:
                rValue = MsgRepository.COMMAND_LABEL_STOP;
                break;
            case EXIT:
                rValue = MsgRepository.COMMAND_LABEL_EXIT;
                break;
            case ITEM:
                rValue = MsgRepository.COMMAND_LABEL_ITEM;
                break;
            default:
                rValue = EMPTY_STRING;
                break;
        }
        return rValue;
    }

    /**
     * In eSWT lowest possible priority value is 0, but in LCDUI it's ok to have
     * negative priority values. This method converts LCDUI priority to eSWT
     * priority. In practise the method provides acceptable priorities for all
     * kind of applications, but in theory, in worst case scenario, if using
     * very uncommon priority numbering scheme and if there are more than 31
     * commands and every command has different priority than other commands,
     * then same priority might be returned more than once.
     *
     * The meaning of variables:
     * <li>a = nearest smaller lcdui priority.
     * <li>b = nearest larger lcdui priority.
     * <li>c = lcdui priority to be converted.
     * <li>a1 = eswt priority value associated with a.
     * <li>b1 = eswt priority value associated with b.
     * <li>c1 = eswt priority value associated with c.
     * <li>x = value in the middle of a1 and b1.
     * <li>ac = distance from a to c.
     * <li>cb = distance from c to b.
     * <li>a1x = distance from a1 to x.
     *
     * @param c Lcdui priority to be converted.
     * @see OpenLCDUI design document for more detailed description of the
     *      algorithm.
     */
    private int getESWTPriority(int c) {
        // Find out nearest values from keys (LCDUI priorities). "a" will
        // be nearest smaller value and "b" will be nearest larger value.
        Integer a = new Integer(Integer.MIN_VALUE);
        Integer b = new Integer(Integer.MAX_VALUE);
        for (Enumeration e = priorities.keys(); e.hasMoreElements();) {
            Integer i = (Integer) e.nextElement();
            if ((i.intValue() >= a.intValue()) && (i.intValue() <= c)) {
                a = i;
            }
            if ((i.intValue() <= b.intValue()) && (i.intValue() >= c)) {
                b = i;
            }
        }

        // Calculate eSWT priority candidate. "a1" is nearest
        // smaller value in eSWT priorities and "b1" is nearest
        // larger value and x is the candidate:
        int a1 = ((Integer) priorities.get(a)).intValue();
        int b1 = ((Integer) priorities.get(b)).intValue();

        int a1x = ((b1 - a1) / 2);
        int x = a1x + a1;

        // Calculate distances from new LCDUI priority to nearest
        // LCDUI priorities already saved:
        long ac = Math.abs((long) a.intValue() - (long) c);
        long cb = Math.abs((long) b.intValue() - (long) c);

        // Decide whether to use candidate or one of the nearest values:
        int c1 = 0;

        if (ac > cb) {
            if (cb < a1x) {
                // Use nearest larger value:
                c1 = (int) (b1 - cb);
            }
            else {
                // Use candidate:
                c1 = x;
            }
        }
        else {
            if (ac < a1x) {
                // Use nearest smaller value:
                c1 = (int) (a1 + ac);
            }
            else {
                // Use candidate:
                c1 = x;
            }
        }

        // Save new priority to the HashTable if not already exists.
        if (!priorities.contains(new Integer(c1))) {
            priorities.put(new Integer(c), new Integer(c1));
        }

        // Return flipped priority because in LCDUI lower priority
        // means higher importance, but in eSWT lower priority means
        // lower importance:
        return Integer.MAX_VALUE - c1;
    }

}
