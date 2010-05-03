package org.eclipse.swt.internal.qt;

import org.eclipse.ercp.swt.mobile.Command;

public final class CommandUtils {

	public static final void sort(Command[] array) {
	    int length = array.length;
	    if (length <= 1)
	        return;

	    for (int gap = length / 2; gap > 0; gap /= 2) {
	        for (int i = gap; i < length; i++) {
	            for (int j = i - gap; j >= 0; j -= gap) {
	                if (array[j].getPriority() < array[j + gap].getPriority()) {
	                    Command swap = array[j];
	                    array[j] = array[j + gap];
	                    array[j + gap] = swap;
	                }
	            }
	        }
	    }
	}

    public  static final int getQtSoftKeyRole( int type){
        switch (type) {
        case Command.GENERAL:
        case Command.OK:
        case Command.HELP:
            return OS.QACTION_POSITIVESOFTKEY;
        case Command.CANCEL:
        case Command.DELETE:
        case Command.BACK:
        case Command.EXIT:
        case Command.STOP:
            return OS.QACTION_NEGATIVESOFTKEY;
        case Command.SELECT:
            return OS.QACTION_SELECTSOFTKEY;
        case Command.COMMANDGROUP:
        default:
            return OS.QACTION_NOSOFTKEY;
        }
    }

	public static final boolean isNegativeType(final int type){
		return getQtSoftKeyRole(type) == OS.QACTION_NEGATIVESOFTKEY;
    }

}
