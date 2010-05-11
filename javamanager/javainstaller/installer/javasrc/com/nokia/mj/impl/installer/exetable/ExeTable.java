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

import java.util.Vector;

/**
 * Execution table contains the steps to be executed in execution engine.
 */
public final class ExeTable
{
    private Vector iSteps = new Vector();
    private int iCurrentStep = 0;

    /**
     * Adds given step to this execution table.
     */
    public void add(ExeStep step)
    {
        iSteps.add(step);
    }

    ExeStep getNext()
    {
        ExeStep step = null;
        if (iCurrentStep < iSteps.size())
        {
            step = (ExeStep)iSteps.get(iCurrentStep);
            iCurrentStep++;
        }
        return step;
    }

    ExeStep getPrev()
    {
        ExeStep step = null;
        if (iCurrentStep > 0)
        {
            iCurrentStep--;
            step = (ExeStep)iSteps.get(iCurrentStep);
        }
        return step;
    }

    void reset()
    {
        iCurrentStep = 0;
    }

    int size()
    {
        return iSteps.size();
    }
}
