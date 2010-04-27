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
* Description:  ApplicationManagementEventsInterface
*
*/

#ifndef APPLICATIONMANAGEMENTEVENTSINTERFACE_H
#define APPLICATIONMANAGEMENTEVENTSINTERFACE_H

#include <set>
#include "javauid.h"

using java::util::Uid;

namespace java
{
namespace captain
{

typedef std::set<Uid> uids_t;

class ApplicationManagementEventsInterface
{
public:
    virtual ~ApplicationManagementEventsInterface() {};

    virtual void amAdded(const uids_t& uids)   = 0;
    virtual void amUpdated(const uids_t& uids) = 0;
    virtual void amDeleted(const uids_t& uids) = 0;
};

} // namespace captain
} // namespace java

#endif // APPLICATIONMANAGEMENTEVENTSINTERFACE_H

