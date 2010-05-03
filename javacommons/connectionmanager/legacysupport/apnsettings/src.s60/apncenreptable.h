/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  APNSettings implementation header.
*
*/


#ifndef APNCENREPTABLE_H
#define APNCENREPTABLE_H

#include "mapnsettings.h"
#include "connectionmanager.h"

namespace CENREP
{

class CAPNCenRepTable: public CBase, public MAPNSettings
{
public:

    static CAPNCenRepTable* NewLC();

    virtual ~CAPNCenRepTable();

    virtual void CreateL();

    virtual void OpenL();
    virtual void Close(void);

    virtual TUint32 GetAPNIdL(TUint32 aMSId);
    virtual void SetAPNL(TUint32 aMSId, TUint32 aAPNId);
    virtual void RemoveL(TUint32 aMSId);
    virtual void SetUseSystemDefaultAPNL(TUint32 aMSId);
    virtual void SetUseJavaDefaultAPNL(TUint32 aMSId);
    virtual TBool IsUseSystemDefaultAPNL(TUint32 aMSId);
    virtual TBool IsUseJavaDefaultAPNL(TUint32 aMSId);
    virtual TUint32 GetJavaDefaultAPNL();
    virtual void SetJavaDefaultAPNL(TUint32 aAPNId);

private:

};

} // end of namespace CENREP

#endif // APNCENREPTABLE_H
