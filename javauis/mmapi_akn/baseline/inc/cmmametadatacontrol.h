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
* Description:  This interface is for getting metadata from a media.
*
*/


#ifndef CMMAMETADATACONTROL_H
#define CMMAMETADATACONTROL_H

//  INCLUDES
#include "cmmacontrol.h"

//  CLASS DECLARATION
/**
*   This class is an abstract interface for getting metadata from a media.
*
*
*/

NONSHARABLE_CLASS(CMMAMetaDataControl): public CMMAControl
{
protected:
    CMMAMetaDataControl();

public:
    /**
    * Get the number of all metadata keys from given metadata control.
    *
    * @param aMetaDataControl Metadata control.
    * @param aCount Address of an integer to store the metadata keys count
    */
    static void StaticKeysCountL(CMMAMetaDataControl* aMetaDataControl, TInt *aCount);

    /**
    * Get one metadata key from given metadata control.
    *
    * @param aMetaDataControl Metadata control.
    * @param aValue Address of a pointer to a descriptor to which the
    *        value is put. Ownership of the descriptor is transferred to
    *        the caller.
    * @param aIndex The index of the key to be fetched.
    */
    static void StaticKeyL(CMMAMetaDataControl* aMetaDataControl, HBufC** aKey, TInt aIndex);

    /**
    * Get the value associated with a single metadata key.
    *
    * @param aMetaDataControl Metadata control.
    * @param aValue Address of a pointer to a descriptor to which the
    *        value is put. Ownership of the descriptor is transferred to
    *        the caller.
    * @param aKey The key for which the value is to be fetched.
    */
    static void StaticKeyValueL(CMMAMetaDataControl* aMetaDataControl,
                                HBufC** aValue,
                                TDesC* aKey);

protected:
    virtual TInt KeyCountL() = 0;
    virtual HBufC* KeyL(TInt aIndex) = 0;

    virtual HBufC* KeyValueL(const TDesC& aKey) = 0;

public: // From CMMAControl
    const TDesC& ClassName() const;
};

#endif // CMMAMETADATACONTROL_H
