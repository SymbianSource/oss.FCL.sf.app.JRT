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
* Description:  javareguidarrayconv definition
*
*/


#ifndef JAVAREGUIDARRAYCONVERTER_H
#define JAVAREGUIDARRAYCONVERTER_H

// INCLUDES
#include <e32std.h>

namespace Java
{
namespace Manager
{
namespace Registry
{

// CLASS DECLARATION
/**
 * This class has only static methods, that help to store uids
 * in descriptors, and to retrieve them. It is mainly used
 * by the javaregistry internal API.
 *
 * @lib javaregistryclient.lib
 * @since S60 v3.2
 */
class JavaRegUidArrayConverter
{
public:

    /**
     * JavaRegUidArrayConverter::GetTUids method converts the descriptor
     * parameter to a RArray<TUid> storing entry uids.
     *
     * @since S60 v3.2
     * @param aValue A descriptor storing certificate chains.
     * @param aUids [out] Returning entry uids.
     * @return One of the system-wide error codes.
     */
    static TInt GetTUids(const TDesC& aValue, RArray<TUid>& aUids);

    /**
     * JavaRegUidArrayConverter::GetTUidsL method converts the descriptor
     * parameter to a RArray<TUid> storing entry uids.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aValue A descriptor storing certificate chains.
     * @param aUids [out] Returning entry uids.
     */
    static void GetTUidsL(const TDesC& aValue, RArray<TUid>& aUids);

    /**
     * JavaRegUidArrayConverter::StoreTUidsL method stores the
     * entry uids ( given in the first parameter ),
     * into the descriptor paramameter, that is instantiated by
     * this method.
     * The function may leave with one of the system-wide error codes.
     *
     * @since S60 v3.2
     * @param aValue The uids to be stored.
     * @param aDes [out] Descriptor parameter, storing, and the returning
     *        the uids.
     */
    static void StoreTUidsL(const RArray<TUid>& aValue, HBufC*& aDes);
};

}//namespace Registry
}//namespace Manager
}//namespace Java

#endif // JAVAREGUIDARRAYCONVERTER_H

// End of File
