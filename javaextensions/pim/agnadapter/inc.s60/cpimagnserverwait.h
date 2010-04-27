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
* Description:  Synchronizes operations with Agenda server
 *
*/


#ifndef CPIMAGNSERVERWAIT_H
#define CPIMAGNSERVERWAIT_H

//  INCLUDES
#include <e32base.h>
#include <calprogresscallback.h>


// CLASS DECLARATION

/**
 *  Waits for completion of current Agenda operation on request.
 */
class CPIMAgnServerWait: public CTimer, public MCalProgressCallBack
{
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPIMAgnServerWait* NewL();

    /**
     * Two-phased constructor. Puts the object to cleanup stack.
     */
    static CPIMAgnServerWait* NewLC();

    /**
     * Destructor.
     */
    virtual ~CPIMAgnServerWait();

public:
    // New functions

    /**
     * Waits for Agenda server to notify that current operation is
     * completed.
     *
     *
     * @param aTimeout Maximum time to wait, or 0 for no timeout.
     *        Optional, defaults to 0.
     *
     * @return KErrNone on success, one of systemwide error codes on error.
     */
    TInt WaitCompleteL(TTimeIntervalMicroSeconds32 aTimeout);
    inline TInt WaitCompleteL();

public:
    // Functions from MCalProgressCallBack

    void Progress(TInt aPercentageCompleted);
    void Completed(TInt aError);
    TBool NotifyProgress();

    //Timer

    void RunL();
    TInt RunError(TInt error);

private:

    /**
     * C++ default constructor.
     */
    CPIMAgnServerWait();

    void ConstructL();


};

// INLINE FUNCTIONS
inline TInt CPIMAgnServerWait::WaitCompleteL()
{
    return WaitCompleteL(0);
}

#endif // CPIMAGNSERVERWAIT_H
// End of File
