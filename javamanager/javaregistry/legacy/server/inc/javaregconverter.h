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
* Description:  javaregconverter definition
*
*/


#ifndef JAVAREGCONVERTER_H
#define JAVAREGCONVERTER_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATION
class TAppVersion;

namespace Java
{
namespace Manager
{
namespace Registry
{
// FORWARD DECLARATION
class CJavaProperty;

// CLASS DECLARATION
/**
 * This class has only static methods, that help to store some types,
 * and structures in descriptors, and to retrieve them. It is mainly used
 * by the javaregistry internal API.
 *
 * @lib javaregistryclient.lib
 * @since S60 v3.2
 */
class JavaRegConverter
{
public:

    /**
     * JavaRegConverter::GetTAppVersion method converts the descriptor
     * parameter to a TAppVersion.
     *
     * @since S60 v3.2
     * @param aValue A descriptor storing a TAppVersion.
     * @param aDefault The value to be returned if aValue does'nt contain
     *                 valid data.
     * @return A TAppVersion stored in the descriptor parameter.
     */
    static TAppVersion GetTAppVersion(
        const TDesC& aValue,
        TAppVersion aDefault);

    /**
     * JavaRegConverter::GetTAppVersionL method converts the descriptor
     * parameter to a TAppVersion.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aValue A descriptor storing a TAppVersion.
     * @return A TAppVersion stored in the descriptor parameter.
     */
    static TAppVersion GetTAppVersionL(const TDesC& aValue);

    /**
     * JavaRegConverter::StoreTAppVersionL method stores the TAppVersion
     * parameter into the descriptor paramameter, that is instantiated by
     * this method.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aValue The TAppVersion value to be stored.
     * @param aDes [out] Descriptor parameter, storing, and the returning
     * the TAppVersion value.
     */
    static void StoreTAppVersionL(
        const TAppVersion& aValue,
        HBufC*& aDes);


    /**
     * JavaRegConverter::GetTInt method converts the descriptor
     * parameter to a TInt32.
     *
     * @since S60 v3.2
     * @param aValue A descriptor storing a TInt32.
     * @param aDefault The value to be returned if aValue does'nt contain
     *                 valid data.
     * @return A TInt32 stored in the descriptor parameter.
     */
    static TInt32 GetTInt(const TDesC& aValue, TInt32 aDefault);

    /**
     * JavaRegConverter::GetTIntL method converts the descriptor
     * parameter to a TInt32.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aValue A descriptor storing a TInt32.
     * @return A TInt32 stored in the descriptor parameter.
     */
    static TInt32 GetTIntL(const TDesC& aValue);


    /**
     * JavaRegConverter::StoreTIntL method stores the TInt32
     * parameter into the descriptor paramameter, that is instantiated by
     * this method.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aValue The TInt32 value to be stored.
     * @param aDes [out] Descriptor parameter, storing, and the returning
     * the TInt32 value.
     */
    static void StoreTIntL(const TInt32 aValue, HBufC*& aDes);

    /**
     * JavaRegConverter::GetTBool method converts the descriptor
     * parameter to a TBool.
     *
     * @since S60 v3.2
     * @param aValue A descriptor storing a TBool.
     * @param aDefault The value to be returned if aValue does'nt contain
     *                 valid data.
     * @return A TBool stored in the descriptor parameter.
     */
    static TBool GetTBool(const TDesC& aValue, TBool aDefault);


    /**
     * JavaRegConverter::GetTBoolL method converts the descriptor
     * parameter to a TBool.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aValue A descriptor storing a TBool.
     * @return A TBool stored in the descriptor parameter.
     */
    static TBool GetTBoolL(const TDesC& aValue);


    /**
     * JavaRegConverter::StoreTBoolL method stores the TBool
     * parameter into the descriptor paramameter, that is instantiated by
     * this method.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aValue The TBool value to be stored.
     * @param aDes [out] Descriptor parameter, storing, and the returning
     * the TBool value.
     */
    static void StoreTBoolL(const TBool aValue, HBufC*& aDes);



    /**
     * JavaRegConverter::GetCertificateChainsL method converts the
     * descriptor parameter to a RPointerArray<HBufC8> storing
     * certificate chains.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aValue A descriptor storing certificate chains.
     * @param aCertChains [out] The method converts the certificate chains
     *                          into this RPointerArray<HBufC>.
     */
    static void GetCertificateChainsL(
        const TDesC& aValue,
        RPointerArray<HBufC8>& aCertChains);


    /**
     * JavaRegConverter::StoreCertificateChainsL method stores the
     * certificate chains ( given in the first parameter ),
     * into the descriptor paramameter, that is instantiated by
     * this method.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aValue The certificate chains to be stored.
     * @param aDes [out] Descriptor parameter, storing, and the returning
     * the certificate chains.
     */
    static void StoreCertificateChainsL(
        const RPointerArray<HBufC8>& aValue,
        HBufC*& aDes);


    /**
     * JavaRegConverter::GetUnicodeDescriptorsL method converts the
     * descriptor parameter to a RPointerArray<HBufC> storing
     * unicode descriptors.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aValue A descriptor storing unicode descriptors.
     * @param aCertChains [out] The method converts the unicode descriptors
     *                          into this RPointerArray<HBufC>.
     */
    static void GetUnicodeDescriptorsL(
        const TDesC& aValue,
        RPointerArray<HBufC>& aDescriptors);
    /**
     * JavaRegConverter::StoreUnicodeDescriptorsL method stores the
     * unicode descriptors ( given in the first parameter ),
     * into the descriptor paramameter, that is instantiated by
     * this method.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aValue The certificate chains to be stored.
     * @param aDes [out] Descriptor parameter, storing, and the returning
     * the certificate chains.
     */
    static void StoreUnicodeDescriptorsL(
        const RPointerArray<HBufC>& aValue,
        HBufC*& aDes);
};

}// namespace Registry
}// namespace Manager
}// namespace Java

#endif // JAVAREGCONVERTER_H

// End of File
