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

#ifndef JAVACERTSTORETOKEN_H
#define JAVACERTSTORETOKEN_H

#include <e32base.h>
#include <ct.h>

namespace java
{
namespace security
{

//class CJavaCertStoreImpl;
class CJavaCertStoreTokenType;

class CJavaCertStoreToken : public CBase, public MCTToken
{
public:

    static CJavaCertStoreToken* New(TInt iId, CJavaCertStoreTokenType& aType);

    virtual ~CJavaCertStoreToken();

    //MCTToken
    virtual MCTTokenType& TokenType();
    virtual const TDesC& Label();
    virtual TCTTokenHandle Handle();

protected:

    virtual TInt& ReferenceCount();
    virtual void DoGetInterface(TUid aRequiredInterface,  MCTTokenInterface*& aReturnedInterface,
                                TRequestStatus& aStatus);
    virtual TBool DoCancelGetInterface();
    virtual const TDesC& Information(TTokenInformation);

private:
    //Datamembers.
    CJavaCertStoreTokenType&    iType;
    TInt                        iId;
    TInt                        iRefCount;
    //CJavaCertStoreImpl*         iStore;

    //Internal operations.
    CJavaCertStoreToken(TInt aId, CJavaCertStoreTokenType& aType);

};

} // end of namespace java
} // end of namespace security

#endif // JAVACERTSTORETOKEN_H

