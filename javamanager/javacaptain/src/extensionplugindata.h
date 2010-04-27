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
* Description:  ExtensionPluginData
*
*/

#ifndef EXTENSIONPLUGINDATA_H
#define EXTENSIONPLUGINDATA_H

#include <string>
#include <memory>

#include "dynamiclibloader.h"

namespace java
{
namespace captain
{

class ExtensionPluginData
{
public:
    ExtensionPluginData(ExtensionPluginInterface* aExtensionPlugin,
                        const std::string& aName)
            :mExtensionPlugin(aExtensionPlugin),
            mLoadCount(1), mName(aName)
    {
    }

    virtual ~ExtensionPluginData()
    {
        mExtensionPlugin->stopPlugin();
        delete mExtensionPlugin;
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

    ExtensionPluginInterface* mExtensionPlugin;

protected:
    std::auto_ptr<java::util::DynamicLibLoader> mDllLoader;

private:
    unsigned int                                mLoadCount;
    const std::string                           mName;

    //Not implemented.
    ExtensionPluginData &operator=(const ExtensionPluginData&);
    ExtensionPluginData(const ExtensionPluginData&);
};

class ExtensionPluginDataDynamic : public ExtensionPluginData
{
public:
    ExtensionPluginDataDynamic(ExtensionPluginInterface* aExtensionPlugin,
                               std::auto_ptr<java::util::DynamicLibLoader> aDllLoader,
                               const std::string& aName)
            :ExtensionPluginData(aExtensionPlugin, aName)
    {
        mDllLoader = aDllLoader;
    }

    virtual ~ExtensionPluginDataDynamic()
    {
    }

private:
    //Not implemented.
    ExtensionPluginDataDynamic &operator=(const ExtensionPluginDataDynamic&);
    ExtensionPluginDataDynamic(const ExtensionPluginDataDynamic&);
};

} // namespace captain
} // namespace java

#endif // EXTENSIONPLUGINDATA_H

