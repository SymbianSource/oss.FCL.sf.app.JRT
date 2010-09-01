/*
* Copyright (c) 1999 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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


public class AlertType
{
    private static final int EAlarm = 0;
    private static final int EConfirmation = 1;
    private static final int EError = 2;
    private static final int EInfo = 3;
    private static final int EWarning = 4;
    //
    public static final AlertType ALARM = new AlertType(EAlarm);
    public static final AlertType CONFIRMATION = new AlertType(EConfirmation);
    public static final AlertType ERROR = new AlertType(EError);
    public static final AlertType INFO = new AlertType(EInfo);
    public static final AlertType WARNING = new AlertType(EWarning);
    //
    private final int iType;
    private final Toolkit iToolkit;


    protected AlertType()
    {
        this(EInfo);
    }

    private AlertType(int aType)
    {
        iToolkit = Toolkit.getToolkit();
        iType = aType;
    }

    public boolean playSound(Display aDisplay)
    {
        if (aDisplay == null)
        {
            throw new NullPointerException();
        }
        synchronized (iToolkit)
        {
            return _playSound(iToolkit.getHandle(),iType);
        }
    }

    final int Type()
    {
        return iType;
    }

    private native boolean _playSound(int aToolkit,int aSoundType);
}
