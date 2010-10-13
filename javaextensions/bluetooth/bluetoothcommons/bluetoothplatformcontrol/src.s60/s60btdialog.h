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



#ifndef S60BTDIALOG_H
#define S60BTDIALOG_H

#include <e32base.h>
#include <btnotif.h>
#include <btextnotifiers.h>


namespace java
{
namespace bluetooth
{



/**
 * This class shows dialogs for BT power management and
 * discoverability mode.
 *
 */
NONSHARABLE_CLASS(CS60BTDialog)
{
public:

    static CS60BTDialog* NewLC();

    ~CS60BTDialog();

    /**
     * Displays BT query dialog.
     *
     * @param[in] aAppName application name.
     * @param[in] aInstalling True if called during installation,
     *                        False if called during runtime.
     * @param[in] aQueryType identifies which query to show.
     * @return boolean telling user response to query.
     */
    TBool ShowBTQueryDialog(const TDesC& aAppName, const TBool aInstalling,
                            TBTGenericQueryNoteType aQueryType);

    /**
     * Cancels the asynchronous request to the query list dialog.
     *
     */
    void DoCancel();


private:

    CS60BTDialog();

    void ConstructL();

private: // data
    TRequestStatus mStatus;
    RNotifier mNotifier;
    TBTGenericQueryNotiferParamsPckg mQueryParamsPckg;
    TPckgBuf<TBool> mQueryResult;
};

}  //end namespace java
}  //end namespace bluetooth

#endif // S60BTDIALOG_H
