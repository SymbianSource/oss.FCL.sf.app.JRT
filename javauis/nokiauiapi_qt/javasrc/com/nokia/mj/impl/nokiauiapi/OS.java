/*******************************************************************************
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/
package com.nokia.mj.impl.nokiauiapi;

import org.eclipse.swt.internal.Library;

public final class OS
{
    static
    {
        Library.loadLibrary("javanokiaui");
    }

    native public static final void MTouchFeedback_InstantFeedback(int style);
    native public static final boolean MTouchFeedback_TouchFeedbackSupported();
    native public static final void MTouchFeedback_SetFeedbackArea(int controlHandle, int id, int x, int y, int width, int height, int style);
    native public static final void MTouchFeedback_RemoveFeedbackArea(int controlHandle, int aAreaIndex);
    native public static final void MTouchFeedback_RemoveFeedbackForControl(int controlHandle);
    native public static final void MTouchFeedback_MoveFeedbackAreaToFirstPriority(int controlHandle, int id);
    
    //softnotification
    native public static final int createNativePeer(int mideletUid, int notificationID, Object peer);
    native public static final void destroy(int softNotificationHandle);
    native public static final int getId(int softNotificationHandle);
    native public static final int setText(int softNotificationHandle, String primaryText, String secondaryText);
    native public static final int removeSoftNotification(int softNotificationHandle);
    native public static final int showSoftNotification(int softNotificationHandle);
    native public static final int setImagePath(int softNotificationHandle, String imagePath);

}
