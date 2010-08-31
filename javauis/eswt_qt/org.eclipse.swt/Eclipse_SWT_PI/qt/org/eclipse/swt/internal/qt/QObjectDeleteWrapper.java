/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Nokia Corporation - initial implementation 
 *******************************************************************************/

package org.eclipse.swt.internal.qt;

/**
 * QObject deletion should only be done by using these utility methods.
 */
public final class QObjectDeleteWrapper {

/**
 * Always use this to delete any QObjects. It will make decision if to delete
 * the QObject immediately or to post a deferred deletion event. 
 */
public static void deleteSafely(int handle) {
    if(JniUtils.handle == 0 || handle == 0) return;
    if(OS.JniUtils_safeToDelete(JniUtils.handle, handle)) {
        OS.QObject_delete(handle);
    } else {
        OS.QObject_deleteLater(handle);
    }
}

/**
 * Don't use. Deletes the QObject immediately without any safety checks. You
 * should use safelyDeleteQObject instead. 
 */
public static void deleteNowWithoutSafetyCheck(int handle) {
    OS.QObject_delete(handle);
}

}
