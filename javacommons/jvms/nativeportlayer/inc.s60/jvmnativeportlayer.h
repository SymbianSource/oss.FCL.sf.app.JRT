/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CJvmNativePortLayer
*
*/

#ifndef JVMNATIVEPORTLAYER_H
#define JVMNATIVEPORTLAYER_H

#include "javaosheaders.h"
#include "javaredirector.h"
using java::util::Redirector;

namespace javaruntime
{

OS_NONSHARABLE_CLASS(CJvmNativePortLayer): public CBase, public MJvmNativePortLayer
{
public:
    static MJvmNativePortLayer* NewL();
public:

    virtual ~CJvmNativePortLayer();

    virtual MJavaFile* GetJavaFileL();
    virtual void WriteToStdoutL(const TDesC8& aData);
    virtual void WriteToStderrL(const TDesC8& aData);

private:

    CJvmNativePortLayer();

    void Construct();

private:
    TBool      iLogging;
    Redirector iRedirector;
};
} // end of namespace javaruntime

#endif // JVMNATIVEPORTLAYER_H


