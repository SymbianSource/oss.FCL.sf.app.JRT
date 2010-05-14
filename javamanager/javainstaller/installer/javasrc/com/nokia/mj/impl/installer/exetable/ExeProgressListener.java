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
 * Execution engine progress listener interface.
 */
public interface ExeProgressListener
{
    /**
     * Execution engine calls started() method to tell that
     * engine has been started.
     */
    public void started();

    /**
     * Execution engine calls ended() method to tell that
     * engine execution has ended.
     */
    public void ended();

    /**
     * Execution engine calls error() method to tell that
     * an exception has been thrown during engine execution.
     * Even if error() gets called, the engine will still call
     * ended() in the end.
     */
    public void error(Exception aException);

    /**
     * Execution engine calls setMax() method to tell what is the
     * maximum progress indicator value. This method is called once
     * before execution steps are executed. Between each execution
     * step set() method gets called to indicate execution progress.
     * @param maxValue maximum progress indicator value
     */
    public void setMax(int maxValue);

    /**
     * Execution engine calls set() method between each execution
     * step to indicate execution progress. When all steps are
     * execeuted progress value increases to limit told with
     * setMax() method.
     * @param currentValue current progress indicator value
     */
    public void set(int currentValue);
}
