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
package javax.microedition.amms.control.tuner;

import javax.microedition.media.*;

import java.util.Date;

/**
 * Please refer to JSR 234 for more details.
 */

public interface RDSControl extends javax.microedition.media.Control
{

    /**
     * Please refer to JSR 234 for more details.
     */
    String RDS_NEW_DATA = "RDS_NEW_DATA";

    /**
     * Please refer to JSR 234 for more details.
     */
    String RDS_NEW_ALARM = "RDS_ALARM";

    /**
     * Please refer to JSR 234 for more details.
     */
    String RADIO_CHANGED = "radio_changed";

    /**
     * Please refer to JSR 234 for more details.
     */
    boolean isRDSSignal();

    /**
     * Please refer to JSR 234 for more details.
     */
    String getPS();

    /**
     * Please refer to JSR 234 for more details.
     */
    String getRT();

    /**
     * Please refer to JSR 234 for more details.
     */
    short getPTY();

    /**
     * Please refer to JSR 234 for more details.
     */
    String getPTYString(boolean longer);

    /**
     * Please refer to JSR 234 for more details.
     */
    short getPI();

    /**
     * Please refer to JSR 234 for more details.
     */
    int[] getFreqsByPTY(short PTY);

    /**
     * Please refer to JSR 234 for more details.
     */
    int[][] getFreqsByTA(boolean TA);

    /**
     * Please refer to JSR 234 for more details.
     */
    String[] getPSByPTY(short PTY);

    /**
     * Please refer to JSR 234 for more details.
     */
    String[] getPSByTA(boolean TA);

    /**
     * Please refer to JSR 234 for more details.
     */
    Date getCT();

    /**
     * Please refer to JSR 234 for more details.
     */
    boolean getTA();

    /**
     * Please refer to JSR 234 for more details.
     */
    boolean getTP();

    /**
     * Please refer to JSR 234 for more details.
     */
    void setAutomaticSwitching(boolean automatic)
    throws MediaException;

    /**
     * Please refer to JSR 234 for more details.
     */
    boolean getAutomaticSwitching();

    /**
     * Please refer to JSR 234 for more details.
     */
    void setAutomaticTA(boolean automatic) throws MediaException;

    /**
     * Please refer to JSR 234 for more details.
     */
    boolean getAutomaticTA();

}
