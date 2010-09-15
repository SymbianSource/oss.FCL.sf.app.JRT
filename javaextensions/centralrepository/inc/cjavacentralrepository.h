/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef C_JAVACENTRALREPOSITORY_H
#define C_JAVACENTRALREPOSITORY_H


#include <e32std.h>

class CRepository;

/**
 * Java Central Repository.
 */
class CJavaCentralRepository : public CBase
{
public:

    /**
     * Two-phased constructor.
     * @param aRepositoryUid The UID of the repository to be accessed
     */
    static CJavaCentralRepository* NewL(TUid aRepositoryUid);

    /**
     * Two-phased constructor.
     * @param aRepositoryUid The UID of the repository to be accessed
     */
    static CJavaCentralRepository* NewLC(TUid aRepositoryUid);

    /**
     * Destructor.
     */
    virtual ~CJavaCentralRepository();

    /**
     * Closes opened central repository.
     */
    void Close();

    /**
     * Reads a descriptor setting.
     *
     * @param aKey Key of setting to be read.
     * @param aValue Returns the value of the setting if it is a descriptor.
     * @leave System wide code if setting can not be read.
     */
    void GetL(TUint32 aKey, TDes& aValue);

    /**
     * Reads a integer setting.
     *
     * @param aKey Key of setting to be read.
     * @param aValue Returns the value of the setting if it is an integer.
     * @leave System wide code if setting can not be read.
     */
    void GetL(TUint32 aKey, TInt& aValue);

    /**
     * Stores descriptor setting.
     *
     * @param key The key of setting to be stored.
     * @param value The value of the setting to be stored.
     * @leave System wide error code if value cannot be stored.
     */
    void SetL(TUint32 aKey, TDesC& aValue);

    /**
     * Stores integer setting.
     *
     * @param aKey Key of setting to be stored.
     * @param aValue The value of the setting to be stored.
     * @leave System wide error code if value cannot be stored.
     */
    void SetL(TUint32 aKey, TInt aValue);

    HBufC* GetStringL(TUint32 aKey);

    TInt GetIntL(TUint32 aKey);

private:

    CJavaCentralRepository(TUid aRepositoryUid);

    void ConstructL();

private: // data

    /**
     * Access to a repository
     * Own
     */
    CRepository* iRepository;

    /**
     * Repository ID
     */
    TUid iRepositoryUid;

};

#endif // C_JAVACENTRALREPOSITORY_H
