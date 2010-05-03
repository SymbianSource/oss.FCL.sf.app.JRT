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
package com.nokia.mj.impl.uitestutils;

public final class Modifier {

    // Bitmask for sending modifiers 
    // Values taken from /usr/include/X11/X.h
    public static final int Shift                           = (1<<0);
    public static final int CapsLock                        = (1<<1);
    public static final int Control                         = (1<<2);
    public static final int Alt                             = (1<<3);
    public static final int NumLock                         = (1<<4);
    public static final int ScrllLock                       = (1<<4);
    public static final int AltGr                           = (1<<7);
}
