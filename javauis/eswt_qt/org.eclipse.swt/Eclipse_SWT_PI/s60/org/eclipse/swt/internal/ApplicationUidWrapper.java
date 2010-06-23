/*******************************************************************************
 * Copyright (c) 2009,2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/
package org.eclipse.swt.internal;

import com.nokia.mj.impl.rt.support.ApplicationInfo;


public final class ApplicationUidWrapper {

	public static int uid() {
        int midletUid = -1;
        try{
            String uidS = ApplicationInfo.getInstance().getUid().getStringValue();
            // In Symbian the UID is in format '[<uid>]' where <uid> is in hex
            // format. So wee need to take the brackets away.
            // Long is needed in conversion because UIDs greater than 0x80000000
            // would fail if Integer would be used. However typecast from long
            // to int is safe since UID in Symbian is 32 bit.
            long uidL = Long.parseLong(uidS.substring(1,uidS.length()-1), 16);
            return (int)uidL;
        }
        catch (Throwable t) {/* Not handled*/ }
        return midletUid;
    }
}
