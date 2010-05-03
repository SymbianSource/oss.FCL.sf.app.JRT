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


#ifndef CSTSFILEDATAMANAGER_H
#define CSTSFILEDATAMANAGER_H

//  INCLUDES
#include <e32base.h>
#include <JavaUtils.h>

// FORWARD DECLARATIONS

// CONSTANTS
const TInt KSTSPINLabelMaxLength = 64;

namespace java
{
namespace satsa
{

// CLASS DECLARATION
class CSTSAuthType;

/**
 *  Facade type of class for getting card file data
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSFileDataManager): public CActive
{

private: // type definitions
    enum TSTSState
    {
        ENotReady,
        EReady,
        EInitializing,
        EGettingACIFSize,
        ERetrievingACIF,
        EGettingACFSize,
        ERetrievingACF,
        EGettingAuthObjsInfo,
        EGettingWimLabelAndPath
    };

public: // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    static CSTSFileDataManager* NewL();

    /**
     * Destructor.
     */
    virtual ~CSTSFileDataManager();

public: // New functions

    /**
     * Makes initialization procedure
     * @since 3.0
     */
    void InitializeL();

    /**
     * Gets Access Control Index File content. InitializeL must be called
     * before this method is used.
     * @since 3.0
     * @return Reference to content of ACIF
     */
    const TDesC8& RetrieveACIFContentL();

    /**
     * Gets Access Control File content. InitializeL must be called
     * before this method is used.
     * @since 3.0
     * @param aFileIdOrPath File id or Path of ACF
     * @return Reference to content of ACF
     */
    const TDesC8& RetrieveACFContentL(const TDesC8& aFileIdOrPath);

    /**
     * Gets Authentication objects information from AODF file. InitializeL
     * must be called before this method is used.
     * @since 3.0
     * @param aAuthTypes Array where proper authTypes are put
     * @param aAuthIdList Reference to a list of auhtIds which
     *       information will be got
     */
    void GetAuthObjectsL(CArrayPtr< CSTSAuthType>* aAuthTypes,
                         const RArray<TInt>& aAuthIdList);

    /**
     * Gets WIM application label. InitializeL must be called
     * before this method is used.
     * @since 3.0
     * @return Reference to WIM label
     */
    const TDesC& RetrieveWIMLabelL();

    /**
     * Gets path to WIM/PKCS#15 application. InitializeL must be called
     * before this method is used.
     * @since 3.0
     * @return Reference to WIM path
     */
    const TDesC8& RetrieveWIMPathL();

protected: // New functions

    /**
     * Protected construction to allow derivation
     */
    void ConstructL();

    /**
     * C++ default constructor.
     */
    CSTSFileDataManager();

    /**
     * Waits for request and leaves if error
     * @since 3.0
     */
    void WaitAndCheckL();

    /**
     * Retrieves WIM label and WIM path and saves values to
     * member buffer. Saves information that retrieve is done and
     * does not do it again if it has been already done.
     * @since 3.0
     */
    void doRetrieveWIMLabelAndPathL();

protected: // Functions from base classes

    /**
     * From CActive RunL
     * @return void
     */
    void RunL();

    /**
     * From CActive Cancel request
     * @return void
     */
    void DoCancel();

    /**
     * From CActive RunError
     * @return The leave code
     */
    TInt RunError(TInt aError);

private:

protected: // Data

    // Access Control File data, owned
    HBufC8* iACFData;
    // Access Control Index file data, owned
    HBufC8* iACIFData;
    // WIM path buffer, owned
    HBufC8* iWimPath;
    // WIM label buffer, owned
    HBufC16* iWimLabel16;

    TBool iWimLabelAndPathDone;

    //interface of card file data retrieving methods
    CWimJavaUtils* iMidpProv; //owned

    //state of this object
    TSTSState iState;

    //error code
    TInt iError;

    // Waits until operation is complete, owned
    CActiveSchedulerWait* iWait;

};

} // namespace satsa
} // namespace java
#endif // CSTSFILEDATAMANAGER_H
// End of File
