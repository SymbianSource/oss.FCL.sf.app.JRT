/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Please refer JSR 135 for more details.
*
*/


package javax.microedition.media.control;

public interface RateControl extends javax.microedition.media.Control
{
    int setRate(int aMillirate);

    int getRate();

    int getMaxRate();

    int getMinRate();
}

