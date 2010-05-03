/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for Java Secure Backup Core's JsbcActive class
*
*/


#ifndef JSBCACTIVE_H
#define JSBCACTIVE_H

#include <e32property.h>
#include <e32base.h>

namespace conn
{
class CActiveBackupClient;
}// namespace conn

namespace java
{
namespace backup
{

// forward declaration
class CJsbcDataOwner;

/**
 *  CJsbcActive class is handling the BUR of Java Domain
 *
 *  This class is instantiated when a BUR operation is starting. This
 *  class monitors the changes of BUR operations and connects to Secure
 *  Backup Engine.
 */
class CJsbcActive : public CActive
{

public:
    static CJsbcActive* NewL();
    ~CJsbcActive();

    /**
     * Get method for iError
     *
     * @return iError
     */
    TInt Error();

    // from base class CActive

    /**
     * From CActive
     * RunL method of this Active Object.
     */
    void RunL();

    /**
     * From CActive
     * DoCancel method of this Active Object.
     */
    void DoCancel();

    /**
     * From CActive
     * RunError method of this Active Object.
     *
     * @param aError error code
     * @return KErrNone
     */
    TInt RunError(TInt aError);

private:

    CJsbcActive();

    void ConstructL();

    /**
     * Subscribe to the P&S flag so that RunL() will be called when the
     * flag changes
     */
    void SubscribeToPSFlag();

    /**
     * Handle changes in backup state
     *
     * @param aBackupStateValue the new backup state value
     */
    void ProcessBackupStateL(TInt aBackupStateValue);

private: // data

    /**
     * SBE's P&S Flag
     */
    RProperty iProperty;

    /**
     * Client SBE interface
     * Own.
     */
    conn::CActiveBackupClient* iActiveBackupClient;

    /**
     * Pointer to the implementation of the M-Class
     * Own.
     */
    CJsbcDataOwner* iCallbackImpl;

    /**
     * Storing error code if RunL leaves.
     * Own.
     */
    TInt iError;

};

} // namespace backup
} // namespace java


#endif // JSBCACTIVE_H
