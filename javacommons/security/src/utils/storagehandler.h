/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef STORAGEHANDLER_H
#define STORAGEHANDLER_H

#include "javastorage.h"
#include "javastorageexception.h"
#include "javauid.h"

using namespace java::storage;
using namespace java::util;

namespace java
{
namespace security
{

class StorageHandler
{
public:
    StorageHandler();

    /**
     * Read application suite valid certificates.
     *
     * @param aUid Application suite Uid.
     * @param[out] aCerts Valid certificate indexes.
     */
    void readValidCerts(const Uid& aUid, std::list<int>& aCerts);

    /**
     * Read application suite certificate chains. They are read from given
     * attributes based on certificate chain index.
     *
     * @param aAttributes attributes where to read.
     * @param aIndex certificate chain index.
     * @param[out] aChain chain. Empty if certificate not found.
     */
    void getChainFromIndex(
        const java::storage::JavaStorageApplicationList_t& aAttributes,
        const int aIndex,
        std::string& aChain);


    virtual ~StorageHandler();
private:
    void findEntry(const JavaStorageApplicationList_t&,
                   const std::wstring&,
                   std::wstring& eValue) ;
    void addToValidCerts(const std::wstring& aValue, std::list<int>& aCerts);
    JavaStorage* iStorage;
};

} //end namespace security
} //end namespace java

#endif // STORAGEHANDLER_H

