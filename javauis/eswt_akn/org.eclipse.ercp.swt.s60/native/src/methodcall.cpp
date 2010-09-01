/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#include "methodcall.h"
#include "swtdisplay.h"


// ======== LOCAL FUNCTIONS ========


/**
 * Function executor for non-leaving functors
 *
 * @sa <code>TExecutor</code>
 */
void ExecuteRegularFunctor(const MSwtFunctor& aFunctor, CSwtDisplay* aDisplay)
{
    if (aDisplay)
    {
#ifdef _DEBUG
        TInt error;
        error = aDisplay->ExecuteInNativeUiThread(aFunctor);
        ASSERT(error==KErrNone);
#else
        aDisplay->ExecuteInNativeUiThread(aFunctor);
#endif
    }
    else
    {
#ifdef _DEBUG
        TInt error;
        error = CSwtDisplay::Current().ExecuteInNativeUiThread(aFunctor);
        ASSERT(error==KErrNone);
#else
        CSwtDisplay::Current().ExecuteInNativeUiThread(aFunctor);
#endif
    }
}


/**
 * Function executor for leaving functors
 *
 * @sa <code>TExecutor</code>
 */
void ExecuteLeavingFunctorL(const MSwtFunctor& aFunctor, CSwtDisplay* aDisplay)
{
    TInt error;
    if (aDisplay)
    {
        error = aDisplay->ExecuteInNativeUiThread(aFunctor);
    }
    else
    {
        error = CSwtDisplay::Current().ExecuteInNativeUiThread(aFunctor);
    }
    if (error) // Do not use LeaveIfError(), we want to handle positive error codes as well
    {
        User::Leave(error);
    }
}
