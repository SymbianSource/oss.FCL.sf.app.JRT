/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CMMAMMFRESOLVER_H
#define CMMAMMFRESOLVER_H

//  INCLUDES
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include "cmmaplayer.h"


_LIT(KRmFileExtension, ".rm");
_LIT(KRaFileExtension, ".ra");

//  CLASS DECLARATION
/**
* This class can be used to get RMMFControllerImplInfoArray and content
* types from MMF.
*/
NONSHARABLE_CLASS(CMMAMMFResolver): public CBase

{
public: // Construction
    /**
     * Constructs a new mmf resolver object.
     * @return The new object created.  Note that the new object will be
     * left on the cleanup stack.
     */
    static CMMAMMFResolver* NewLC();

public: // New methods.
    /**
     * Retrieves all controller plugins that support the requirements of
     * the caller.
     */
    void ListImplementationsL();

    /**
     * Returns implementations.
     * Array will not contain any implementation if ListImplementationsL()
     * is not called.
     * @return Found implementations.
     */
    RMMFControllerImplInfoArray* Implementations();

    /**
     * Returns implementations. Notice: Ownership is transferred to caller.
     * Array will not contain any implementation if ListImplementationsL()
     * is not called.
     * @return Found implementations. Ownership is transferred.
     */
    RMMFControllerImplInfoArray* ImplementationsOwnership();

    /**
     * Returns content type that was found in ListImplementationsL method.
     * Ownership is transfered.
     * @return Content type or NULL if type is not available.
     */
    HBufC* ContentTypeOwnership();

    /**
     * Returns content type that was found in ListImplementationsL method.
     * @return Content type or NULL if type is not available.
     */
    HBufC* ContentType();

    /**
     * Sets filename
     * @param aFileName File name to be set or NULL
     */

    void SetFileNameL(const TDesC* aFileName);

    /**
     * Returns possible filename or NULL if not set
     * Ownership is transfered.
     * @return Content type or NULL if type is not available.
     */
    HBufC* FileNameOwnership();

    /**
     * Get all supported content types.
     * @param aMimeTypeArray Will contain supported mime types.
     */
    void GetSupportedContentTypesL(CDesC16Array& aMimeTypeArray);

    /**
     * Return selection parameters.
     */
    CMMFControllerPluginSelectionParameters*
    SelectionParameters();

    /**
     * Sets the play format support required.
     * @param aRequiredSupport The play format support required.
     */
    void SetRequiredPlayFormatSupportL(
        CMMFFormatSelectionParameters& aRequiredSupport);

    /**
     * Sets the record format support required.
     * @param aRequiredSupport The record format support required.
     */
    void SetRequiredRecordFormatSupportL(
        CMMFFormatSelectionParameters& aRequiredSupport);

private:
    /**
     * Returns record or play formats according to the setup.
     */
    const RMMFFormatImplInfoArray* Formats(
        CMMFControllerImplementationInformation* aImplementation);
    /**
     * Finds content type from iImplementations array.
     */
    void ResolveContentTypeL();

private:
    TBool IsRealVideoTypeL(const TDesC& aFileName);

    TBool IsRealMimeTypeSupported(const TDesC& aMimeType);

protected:
    //   C++ constructor
    CMMAMMFResolver();
    void ConstructL();

private:
    ~CMMAMMFResolver();

private:
    // Owned. Ownership can be tranferred with ImplementationsOwnership
    // method.
    RMMFControllerImplInfoArray* iImplementations;

    // Owned. Ownership can be transferred with ContentType method.
    HBufC* iContentType;

    // Owned. Ownership can be transferred with FileName method.
    HBufC* iFileName;

    // Owned.
    CMMFControllerPluginSelectionParameters* iControllerSelection;

    // The required play format support
    CMMFFormatSelectionParameters* iRequiredPlayFormatSupport;

    // The required record format support
    CMMFFormatSelectionParameters* iRequiredRecordFormatSupport;
};

#endif // CMMAMMFRESOLVER_H
