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

package org.eclipse.swt.internal.extension;

import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Label;

public final class LabelExtension extends Label {

    public static final int STANDARDICON_ALARM = 0;
    public static final int STANDARDICON_CONFIRMATION = 1;
    public static final int STANDARDICON_ERROR = 2;
    public static final int STANDARDICON_INFO = 3;
    public static final int STANDARDICON_WARNING = 4;

    public LabelExtension(Composite parent, int style) {
        super(parent, style, true);
    }

    /**
     * Set a standard icon to the Label. Will set QPixmap directly to the native
     * Label. Note that this won't set the Image like setImage. getImage won't
     * return the standard icon.
     * 
     * @param iconType
     *            One of the standard icon constants LabelExtension.STANDARDICON_*.
     * @param iconWidth Desired width
     * @param iconHeight Desired height
     */
    public void setStandardIcon(int iconType, int iconWidth, int iconHeight) {
        int standardIcon = 0;

        switch (iconType) {
        case STANDARDICON_ALARM:
            standardIcon = OS.QSTYLE_SP_MESSAGEBOXWARNING;
            break;
        case STANDARDICON_CONFIRMATION:
            standardIcon = OS.QSTYLE_SP_MESSAGEBOXQUESTION;
            break;
        case STANDARDICON_ERROR:
            standardIcon = OS.QSTYLE_SP_MESSAGEBOXCRITICAL;
            break;
        case STANDARDICON_INFO:
            standardIcon = OS.QSTYLE_SP_MESSAGEBOXINFORMATION;
            break;
        case STANDARDICON_WARNING:
            standardIcon = OS.QSTYLE_SP_MESSAGEBOXWARNING;
            break;
        default:
            return;
        }
        
        Internal_PackageSupport.setStandardIcon(this, standardIcon, iconWidth, iconHeight);
    }
}
