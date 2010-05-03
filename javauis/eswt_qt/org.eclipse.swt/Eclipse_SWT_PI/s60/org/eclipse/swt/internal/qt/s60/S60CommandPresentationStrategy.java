package org.eclipse.swt.internal.qt.s60;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.swt.internal.qt.CommandPresentationStrategy;
import org.eclipse.swt.internal.qt.CommandUtils;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.CommandArranger.CommandCollection;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Widget;

public class S60CommandPresentationStrategy extends CommandPresentationStrategy {
    private Command[] positiveKeyCommands;
    private Command negativeKeyCommand;
    private Command defaultCommand;

    public S60CommandPresentationStrategy() {
        super();
    }

    protected void dispose() {
        positiveKeyCommands = null;
        negativeKeyCommand = null;
        defaultCommand =null;
    }


    protected void handleCommandListChange(Command added, Command removed,
            CommandCollection commands) {
        cleanNegativeCommand();
        cleanPositiveCommands();
        updateCommandPositions(commands.getCommands(null));
        placeNegativeCommand();
        placePositiveCommands();
    }

    protected void handleDefaultCommandChange(Command defaultCommand) {
        this.defaultCommand = defaultCommand;
        cleanPositiveCommands();
        placePositiveCommands();
    }


    protected void handleFocusChange(Control focused,
            CommandCollection oldCommands, CommandCollection commands) {

        //Clean-up the existing commands
        cleanPositiveCommands();
        cleanNegativeCommand();
        //determine where the commands go
        if( commands != null && commands.getSize()>0 ){
            Command[] add = commands.getCommands(null);
            updateCommandPositions(add);
        }
        //Place them to their places
        placePositiveCommands();
        placeNegativeCommand();

    }

    private void updateCommandPositions(Command[] commands) {
        positiveKeyCommands = new Command[commands.length];
        int  positiveKeyIndex=0;
        for (int i = 0; i < commands.length; i++) {
            Command cmd = commands[i];
            if ( cmd.isDefaultCommand()){
                defaultCommand = cmd;
                continue;
            }
            if( CommandUtils.isNegativeType(cmd.type) ){
                if (negativeKeyCommand == null || negativeKeyCommand.isDisposed()){
                    negativeKeyCommand = cmd;
                }else
                if( negativeKeyCommand.getPriority() <= cmd.getPriority() ){
                    positiveKeyCommands[positiveKeyIndex] = negativeKeyCommand;
                    positiveKeyIndex++;
                    negativeKeyCommand = cmd;
                }else{
                    positiveKeyCommands[positiveKeyIndex] = cmd;
                    positiveKeyIndex++;
                }
                continue;
            }
            positiveKeyCommands[positiveKeyIndex]=cmd;
            positiveKeyIndex++;
        }
        if ((positiveKeyIndex) < positiveKeyCommands.length ){// needs to shrink
            Command[] rightSized = new Command[positiveKeyIndex];
            System.arraycopy(positiveKeyCommands, 0, rightSized, 0, rightSized.length);
            positiveKeyCommands = rightSized;
        }
    }


    private void cleanPositiveCommands() {
        boolean useBar = false;
        if ( (positiveKeyCommands != null && positiveKeyCommands.length >1) ||
                (defaultCommand != null && positiveKeyCommands != null) ){
            useBar = true;
        }
        if (defaultCommand != null && !defaultCommand.isDisposed()
                && !defaultCommand.control.isDisposed()) {
            if (useBar) {
                OS.QWidget_removeAction(defaultCommand.control.getShell()
                        .internal_getOwnMenuBar(), topHandle(defaultCommand));
            }
            else{
                 OS.QWidget_removeAction(topHandle(defaultCommand.control), topHandle(defaultCommand));
            }
        }
        if (positiveKeyCommands != null) {
            for (int i = 0; i < positiveKeyCommands.length; i++) {
                Command cmd = positiveKeyCommands[i];
                if (cmd == null || cmd.isDisposed()  || cmd.control.isDisposed()){
                	continue;
                }
                int handle = 0;
                if (useBar) {
                    handle = cmd.control.getShell().internal_getOwnMenuBar();
                } else {
                    handle = topHandle(positiveKeyCommands[0].control);
                }
                OS.QWidget_removeAction(handle, topHandle(cmd));

            }
        }
    }

   private void cleanNegativeCommand() {
        if(negativeKeyCommand != null && !negativeKeyCommand.isDisposed() && !negativeKeyCommand.control.isDisposed() ){
            OS.QWidget_removeAction(topHandle(negativeKeyCommand.control), topHandle(negativeKeyCommand));
        }
    }

    private void placeNegativeCommand() {
        if(negativeKeyCommand != null ){
            OS.QWidget_addAction(Internal_PackageSupport.topHandle(negativeKeyCommand.control),
                    topHandle(negativeKeyCommand));
        }
    }

    private void placePositiveCommands() {
        if (defaultCommand != null ) {
            int defaultCmdHandle = topHandle(defaultCommand);
            if (positiveKeyCommands != null) {
                OS.QMenuBar_addAction(defaultCommand.control.getShell()
                        .internal_getOwnMenuBar(), defaultCmdHandle);
            } else {
                OS.QWidget_addAction(Internal_PackageSupport
                        .topHandle(defaultCommand.control), defaultCmdHandle);
            }
        }
        if (positiveKeyCommands != null) {
            if (positiveKeyCommands.length == 1 && defaultCommand == null) {
                OS.QWidget_addAction(Internal_PackageSupport
                        .topHandle(positiveKeyCommands[0].control),
                        topHandle(positiveKeyCommands[0]));
            } else {
                CommandUtils.sort(positiveKeyCommands);
                for (int i = 0; i < positiveKeyCommands.length; i++) {
                    OS.QMenuBar_addAction(positiveKeyCommands[i].control
                            .getShell().internal_getOwnMenuBar(),
                            topHandle(positiveKeyCommands[i]));
                }
            }
        }
    }

    protected void handleMenuBarChanged(int newMenuBar,
            CommandCollection commands) {
        placePositiveCommands();
    }


    private static final int topHandle(Widget w) {
        return Internal_PackageSupport.topHandle(w);
    }
}
