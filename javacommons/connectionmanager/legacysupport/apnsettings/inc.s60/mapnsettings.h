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
* Description:  APNSettings interface.
*
*/


#ifndef MAPNSETTINGS_H
#define MAPNSETTINGS_H

#include <e32base.h>

const TUint32 KAPNSettingsVersion = 0x0000;

namespace CENREP
{

/**
 * APN Id return value if no APN is specified for the given suite.
 * This constant decouples the persistence media (i.e CenRep) and the API.
 */
const TUint32 KMIDletSuiteAPNNotSpecified   = KMaxTUint32;


class MAPNSettings
{
public:
    virtual void CreateL() = 0;
    virtual void OpenL() = 0;
    virtual void Close(void) = 0;
    virtual TUint32 GetAPNIdL(TUint32 aMSId) = 0;
    virtual void SetAPNL(TUint32 aMSId, TUint32 aAPNId) = 0;
    virtual void RemoveL(TUint32 aMSId) = 0;
    virtual void SetUseSystemDefaultAPNL(TUint32 aMSId) = 0;
    virtual void SetUseJavaDefaultAPNL(TUint32 aMSId) = 0;
    virtual TBool IsUseSystemDefaultAPNL(TUint32 aMSId) = 0;
    virtual TBool IsUseJavaDefaultAPNL(TUint32 aMSId) = 0;
    virtual TUint32 GetJavaDefaultAPNL() = 0;
    virtual void SetJavaDefaultAPNL(TUint32 aAPNId) = 0;
    virtual ~MAPNSettings() = 0;

};
inline MAPNSettings::~MAPNSettings()
{
}

IMPORT_C MAPNSettings* GetAPNSettingsLC(TUint32);

} // end of namespace CENREP

#endif // MAPNSETTINGS_H
