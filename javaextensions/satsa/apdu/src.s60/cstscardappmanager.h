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


#ifndef CSTSCARDAPPMANAGER_H
#define CSTSCARDAPPMANAGER_H

//  INCLUDES
#include <e32base.h>

namespace java
{
namespace satsa
{

// FORWARD DECLARATIONS
class CSTSApduExchanger;
class CSTSSelectFile;
class CSTSRespApdu;

// CLASS DECLARATION
/**
 *  Handels the card application selection related operations.
 *  Used for selecting and deselecting card applications.
 *
 *  @since 3.0
 */

NONSHARABLE_CLASS(CSTSCardAppManager): public CBase
{
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CSTSCardAppManager* NewL(CSTSApduExchanger* aApduExchanger,
    const TDesC8& aAID);

    /**
     * Destructor.
     */
    virtual ~CSTSCardAppManager();

public: // New functions

    /**
     * Selects card application.
     * @since 3.0
     * @return Error code. KErrNone, if no problems occured.
     */
    TInt SelectApplication();

    /**
     * Deselects card application.
     * @since 3.0
     * @return Error code. KErrNone, if no problems occured.
     */
    TInt DeselectApplication();

private:

    /**
     * C++ default constructor.
     */
    CSTSCardAppManager(CSTSApduExchanger* aApduExchanger);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL(const TDesC8& aAID);

    TInt DoExchange();

private: // Data
    //for sending apdus, not owned
    CSTSApduExchanger* iApduExchanger;
    CSTSSelectFile* iSelectFileApdu; //owned
    CSTSRespApdu* iRespApdu;//owned

};

} // namespace satsa
} // namespace java
#endif // CSTSCARDAPPMANAGER_H
// End of File
