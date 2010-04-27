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
* Description:  PluginData
*
*/

#ifndef PLUGINDATA_H
#define PLUGINDATA_H

#include <string>
#include <memory>

#include "dynamiclibloader.h"

#include "plugininterface.h"

namespace java
{
namespace captain
{

class CoreInterface;

class PluginData
{
public:
    PluginData(PluginInterface* aPluginInterface,
               const std::string& aName)
            :mPluginInterface(aPluginInterface),mLoadCount(1), mName(aName)
    {
    }
    virtual ~PluginData()
    {
        mPluginInterface->stopPlugin();
        delete mPluginInterface;
    }

    virtual unsigned int addLoadCount()
    {
        return ++mLoadCount;
    }
    virtual unsigned int decLoadCount()
    {
        return (mLoadCount == 0 ? mLoadCount : --mLoadCount);
    }

    virtual std::string getName() const
    {
        return mName;
    }
protected:
    std::auto_ptr<java::util::DynamicLibLoader> mDllLoader;

private:
    PluginInterface*                            mPluginInterface;
    unsigned int                                mLoadCount;
    const std::string                           mName;

    //Not implemented.
    PluginData &operator=(const PluginData&);
    PluginData(const PluginData&);
};
} // namespace captain
} // namespace java

#endif // PLUGINDATA_H

