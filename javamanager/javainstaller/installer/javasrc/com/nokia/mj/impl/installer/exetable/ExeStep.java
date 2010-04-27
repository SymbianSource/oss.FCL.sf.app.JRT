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


package com.nokia.mj.impl.installer.exetable;

/**
 * Base class for execution steps.
 *
 * @author Nokia Corporation
 * @version $Rev: 9041 $ $Date: 2009-12-07 18:36:08 +0200 (Mon, 07 Dec 2009) $
 */
public class ExeStep
{
    public void execute(ExeBall aBall)
    {
        // nop
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }

    public int estimateDuration(ExeBall aBall)
    {
        return 10;
    }
}
