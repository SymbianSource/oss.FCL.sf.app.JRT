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
package javax.microedition.amms.control;

import javax.microedition.media.*;

/**
 * Please refer JSR 234 for more details.
 */
public interface EffectControl extends javax.microedition.media.Control
{

    /**
     * Please refer JSR 234 for more details.
     */
    public static final int SCOPE_LIVE_ONLY = 1;

    /**
     * Please refer JSR 234 for more details.
     */
    public static final int SCOPE_RECORD_ONLY = 2;

    /**
     * Please refer JSR 234 for more details.
     */
    public static final int SCOPE_LIVE_AND_RECORD = 3;


    /**
     * Please refer JSR 234 for more details.
     */
    public void setEnabled(boolean enable);

    /**
     * Please refer JSR 234 for more details.
     */
    public boolean isEnabled();

    /**
     * Please refer JSR 234 for more details.
     */
    public void setScope(int scope) throws MediaException;

    /**
     * Please refer JSR 234 for more details.
     */
    public int getScope();

    /**
     * Please refer JSR 234 for more details.
     */
    public void setEnforced(boolean enforced);


    /**
     * Please refer JSR 234 for more details.
     */
    public boolean isEnforced();

    /**
     * Please refer JSR 234 for more details.
     */
    void setPreset(String preset);

    /**
     * Please refer JSR 234 for more details.
     */
    public String getPreset();

    /**
     * Please refer JSR 234 for more details.
     */
    public String[] getPresetNames();
}
