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
* Description:  Versit serializer native side.
 *
*/


#ifndef CPIMSERIALIZER_H
#define CPIMSERIALIZER_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <vutil.h> // Versit
#include <jni.h>
#include "pimbaseserializer.h"
#include "pimcommon.h"

// FORWARD DECLARATIONS
class CPIMManager;
class CPIMVersit;
class CPIMItem;
class pimbaseitem;
class CPIMEventItem;
class pimbaserepeatrule;

// CLASS DECLARATION

/**
 *  Provides access to item serialization functionalities.
 */
NONSHARABLE_CLASS(CPIMSerializer): public CBase,
        public pimbaseserializer
{

public: // data types

    /**
     * Defines the character set used for encoding versit objects
     */
    enum TEncoding
    {
        EUTF8 = 0,
        EISO88591
    };

public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPIMSerializer* NewL(CPIMManager* aManager);

    /**
     * Destructor.
     */
    virtual ~CPIMSerializer();

public: // New functions

    /**
     * Converts given Versit object buffer into PIM items.
     * Supported Versit formats:
     * @li vCard 2.1
     * @li vCalendar 1.0
     *
     * @param aJniEnv Jni environment
     * @param aBuffer a buffer containing one Versit object, that is one
     *        vCard or one vCalendar container.
     *
     * @return Array of new items created during conversion. \b Ownership
     *         of the array is transferred to the caller.
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aBuffer does not contain valid Versit
     *     object.
     * @li \c KErrNotSupported - \a aEncoding is not supported.
     * @li Other - internal error.
     */
    jintArray FromSerialFormatL(JNIEnv* aJniEnv,
                                const TDesC8& aBuffer, TEncoding aEncoding);

    jintArray fromSerialFormat(JNIEnv* aJniEnv,
                               jbyteArray aBytes, int aByteLength,
                               int aEncoding, jintArray aError);

    /**
     * Converts given PIM item to a Versit object.
     * Supported Versit formats:
     * @li vCard 2.1
     * @li vCalendar 1.0
     *
     * @param aItem Item to convert
     *
     * @return A descriptor containing the new Versit object. \b Ownership
     *         of the descriptor is transferred to the caller.
     *
     * @par Leaving
     * @li \c KErrNotSupported - \a aEncoding is not supported.
     * @li \c KErrArgument - \a aDataFormat is not supported.
     * @li Other - internal error.
     */
    HBufC8* ToSerialFormatL(const CPIMItem& aItem, TEncoding aEncoding);

    jbyteArray toSerialFormat(pimbaseitem* aItem,
                              int aEncoding, JNIEnv* aJniEnv, jintArray aError);

    /**
     * Provides supported serial formats.
     *
     * @param aPimListType PIM list type.
     *
     * @return Array of supported serial formats for \a aPimListType.
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aPimListType is not valid PIM list type.
     */
    const CDesCArray& SupportedSerialFormatsL(
        TPIMListType aPimListType);

    jobjectArray supportedSerialFormats(TPIMListType aPimListType,
                                        JNIEnv* aJniEnv, jintArray aError);

private: // Constructors

    /**
     * C++ default constructor.
     */
    CPIMSerializer();

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL(CPIMManager* aManager);

private: // new methods
    /**
     * Mapping from TEncoding to Versit::TVersitCharSet
     */
    Versit::TVersitCharSet MapEncodingL(TEncoding aEncoding);

private: // data

    /** Converter manager. */
    CPIMVersit* iVersit;

};

#endif // CPIMSERIALIZER_H
// End of File
