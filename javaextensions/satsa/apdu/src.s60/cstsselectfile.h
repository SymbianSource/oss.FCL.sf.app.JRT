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


#ifndef CSTSSELECTFILE_H
#define CSTSSELECTFILE_H

//  INCLUDES
#include "cstscmdapdu.h"

namespace java
{
namespace satsa
{

// CLASS DECLARATION
/**
 *  Detailed command APDU class for Select File apdu.
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSSelectFile): public CSTSCmdApdu
{
public: // data types
    enum TSTSSelectFileApduType
    {
        ESTSAIDActivation,
        ESTSAIDTermination,
        ESTSSelectByFileId,
        ESTSSelectByPathFromMF,
        ESTSSelectByPathFromCurrentDF
    };

public: // Constructors and destructor
    /**
     * Two-phased constructor.
     * @param aData File id or path depending on select file apdu type
     * @param aType Type of Select File apdu
     * @param aStandard Apdu standard
     */
    static CSTSSelectFile* NewLC(const TDesC8& aData,
                                 TSTSSelectFileApduType aType,
                                 CSTSApdu::TSTSApduStandard aStandard);

    static CSTSSelectFile* NewL(const TDesC8& aData,
                                TSTSSelectFileApduType aType,
                                CSTSApdu::TSTSApduStandard aStandard);

    /**
     * Destructor.
     */
    virtual ~CSTSSelectFile();

public: // New functions

    /**
     * Sets type of select file apdu
     * @since 3.0
     */
    void SetApduType(TSTSSelectFileApduType aType);

private: // New functions

    /**
     * C++ default constructor.
     */
    CSTSSelectFile();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL(const TDesC8& aData,
                    TSTSSelectFileApduType aType,
                    CSTSApdu::TSTSApduStandard aStandard);

    /**
     * Generates P1 and P2 values depending of Select File apdu type
     * @since 3.0
     * @param aP1 Reference to P1
     * @param aP2 Reference to P2
     * @param aType Type of Select File apdu
     */
    void GenerateP1P2(TUint8& aP1,
                      TUint8& aP2,
                      TSTSSelectFileApduType aType);

private: // Data

    TSTSSelectFileApduType iType;

};

} // namespace satsa
} // namespace java
#endif // CSTSSELECTFILE_H
// End of File
