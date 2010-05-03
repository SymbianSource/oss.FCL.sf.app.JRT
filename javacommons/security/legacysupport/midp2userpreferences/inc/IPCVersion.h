/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 * @file
 * @internalComponent
 * @released
 *
 * Header file to configure/use versioned implementations of the IPC mechanism.
 */

#ifndef IPCVERSION_H
#define IPCVERSION_H

#include <e32std.h>

#include <j2me/os/ipcparams.h>

/*
 * Client Interfaces
 */

class RSessionBaseV : public RSessionBase
{
public:
    // NB Always uses EAutoAttach
    TInt ShareSession(TAttachMode /*unused*/ = RSessionBase::EAutoAttach)
    {
        return RSessionBase::ShareAuto();
    }
};

/*
 * the following inline functions can be used as a standard Share()
 * interface when connecting to other subsystem's servers.
 */

// NB Always uses EAutoAttach
inline TInt ShareSession(RSessionBase& session, RSessionBase::TAttachMode /*unused*/ = RSessionBase::EAutoAttach)
{
    return session.ShareAuto();
}


/*
 * Server Interfaces
 */

class CSharableSessionV;
class CSessionV;
class CServerV;
typedef RMessage2           RMessageV;
typedef RMessagePtr2        RMessagePtrV;

class CSharableSessionV : public CSession2
{
protected:
    CSharableSessionV() {}
    virtual void CreateL(const CServerV&)
    {}
    void CreateL()
    {
        CreateL(*reinterpret_cast<const CServerV*>(Server()));
    }
};


class CSessionV : public CSharableSessionV
{
protected:
    CSessionV() {}
    CSessionV(RThread) {}
};


class CServerV : public CPolicyServer
{
protected:
    virtual CSharableSessionV* NewSessionL(const TVersion&) const = 0;
    CServerV(TInt aPriority, const TPolicy& aPolicy, TServerType aType) : CPolicyServer(aPriority, aPolicy, aType)
    {}

private:
    CSession2* NewSessionL(const TVersion& aVersion, const RMessageV&) const
    {
        return NewSessionL(aVersion);
    }
};


#endif // IPCVERSION_H
