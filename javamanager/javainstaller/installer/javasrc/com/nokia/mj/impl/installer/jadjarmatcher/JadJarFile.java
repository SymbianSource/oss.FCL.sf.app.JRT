/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.installer.jadjarmatcher;

import java.util.Hashtable;

/**
 * JadJarFile contains names of Jad and Jar files and attributes from them.
 */
public class JadJarFile
{
    /*** ----------------------------- PUBLIC ------------------------------ */
    public String iJadFilename = null;
    public String iJarFilename = null;
    public Hashtable iJadAttributes = null;
    public Hashtable iJarAttributes = null;
    public boolean iMidletMessage = false;
    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */
}
