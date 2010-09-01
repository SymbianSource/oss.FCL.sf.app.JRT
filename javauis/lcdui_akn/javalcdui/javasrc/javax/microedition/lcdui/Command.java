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

public class Command
{
    public static final int SCREEN = 1;
    public static final int BACK = 2;
    public static final int CANCEL =3 ;
    public static final int OK = 4;
    public static final int HELP = 5;
    public static final int STOP = 6;
    public static final int EXIT = 7;
    public static final int ITEM = 8;

    private String  iShortLabel;
    private String  iLongLabel;
    private int     iCommandType;
    private int     iPriority;

    public Command(String aShortLabel, int aCommandType, int aPriority)
    {
        this(aShortLabel, null, aCommandType, aPriority);
    }

    public Command(String aShortLabel,String aLongLabel,int aCommandType,int aPriority)
    {
        if (aShortLabel == null)
        {
            throw new NullPointerException();
        }
        if (aCommandType < SCREEN || aCommandType > ITEM)
        {
            throw new IllegalArgumentException();
        }
        iShortLabel = aShortLabel;
        iLongLabel = aLongLabel;
        iCommandType = aCommandType;
        iPriority = aPriority;
    }

    Command(int aCommandType)
    {
        iShortLabel  = "";
        iLongLabel   = null;
        iCommandType = aCommandType;
        iPriority    = 0;
    }

    public String getLabel()
    {
        return iShortLabel;
    }

    public String getLongLabel()
    {
        return iLongLabel;
    }

    public int getCommandType()
    {
        return iCommandType;
    }

    public int getPriority()
    {
        return iPriority;
    }
}


