/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
package com.nokia.test.java.eswt.progressbar;

import org.eclipse.swt.widgets.ProgressBar;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.SWT;

public class ProgressBarRunnable extends Composite
{

    private int value;
    private ProgressBar pb;
    public ProgressBarRunnable(Composite parent, int style)
    {

        super(parent, SWT.NO_BACKGROUND);
        pb = new ProgressBar(this,style);
        value = 0;
        pb.setSelection(value);
    }

    protected void internal_createHandle()
    {

    }

    public void setMaximum(int value)
    {
        this.pb.setMaximum(value);
    }
    public void setMinimum(int value)
    {
        this.pb.setMinimum(value);
    }
    public void getMaximum()
    {
        this.pb.getMaximum();
    }
    public void getMinimum()
    {
        this.pb.getMinimum();
    }

}
