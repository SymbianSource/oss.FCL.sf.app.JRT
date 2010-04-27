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

#ifndef JAVACERTSTORETOKENTYPE_H
#define JAVACERTSTORETOKENTYPE_H

#include <e32base.h>
#include <ct.h>

namespace java
{
namespace security
{

class CJavaCertStoreTokenType : public CCTTokenType
{
public:

    /**
     * This method is used when this object is instantiated by ECOM framework.
     */
    static CCTTokenType* NewL(void);

    /**
     * This methos is used when this object is created outside ECOM framework.
     */
    static CCTTokenType* New2L(void);

    virtual ~CJavaCertStoreTokenType();

    virtual void List(RCPointerArray<HBufC>& aTokens, TRequestStatus& aStatus);

    virtual void CancelList();

    virtual void OpenToken(const TDesC& aTokenInfo, MCTToken*& aToken, TRequestStatus& aStatus);

    virtual void OpenToken(TCTTokenHandle aHandle, MCTToken*& aToken, TRequestStatus& aStatus);

    virtual void CancelOpenToken();

    virtual TUid Type() const;

private:

    //Datamembers
    TBool mIsInstantiatedByECom;
    RFs mFs;

    //Internal methods.
    CJavaCertStoreTokenType(TBool aIsInstantiatedByECom);
    void ConstructL();

    //Not implemented.
    CJavaCertStoreTokenType(const CJavaCertStoreTokenType &x);
    CJavaCertStoreTokenType &operator=(const CJavaCertStoreTokenType &x);

};

} // end of namespace java
} // end of namespace security


#endif // JAVACERTSTORETOKENTYPE_H

