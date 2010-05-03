/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  M3GCore function call serialization and synchronization
*
*/

#ifndef CSYNCHRONIZATION_H
#define CSYNCHRONIZATION_H

// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION
/**
 * @class CSynchronization
 */
class CSynchronization : public CBase
{
public:
    /**
     * Two-phased constructor.
         */
    static CSynchronization* InstanceL();

    /**
         * Dtor
         */
    ~CSynchronization();

    /**
       * Locks the semaphore
       */
    void Lock();

    /**
       * Unlocks the semaphore
       */
    void Unlock();

    /**
    * Sets error code for error occured during lock period
    */
    void SetErrorCode(TInt aCode);

    /**
    * Returns error string set with SetErrorString
    */
    TInt GetErrorCode();

private: // METHODS
    /**
      * Constructor
      */
    CSynchronization();

    /**
    * Two-phased constructor.
    * @return New object
    */
    static CSynchronization* NewL();

    /**
     * Constructor
     */
    void ConstructL();

private: // MEMBER VARIABLES
    static CSynchronization* iSelf;
    RCriticalSection iGuard;
    TInt iErrorCode;
};

#endif // CSYNCHRONIZATION_H
