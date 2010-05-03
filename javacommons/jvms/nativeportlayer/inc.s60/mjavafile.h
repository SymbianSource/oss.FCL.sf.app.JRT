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
* Description:  MJavaFile
*
*/

#ifndef MJAVAFILE_H
#define MJAVAFILE_H

#include <f32file.h>

namespace javaruntime
{

class MJavaFile
{

public:

    virtual TInt Open(RFs& aFs,const TDesC& aFileName, TUint aFileMode) = 0;
    virtual void Close() = 0;
    virtual TInt Create(RFs& aFs,const TDesC& aName,TUint aFileMode) = 0;
    virtual TInt Read(TDes8& aDes) const = 0;
    virtual TInt Read(TDes8& aDes,TInt aLength) const = 0;
    virtual TInt Replace(RFs& aFs,const TDesC& aName,TUint aFileMode) = 0;
    virtual TInt Write(const TDesC8& aDes) = 0;
    virtual TInt Write(const TDesC8& aDes,TInt aLength) = 0;
    virtual TInt Write(TInt aPos,const TDesC8& aDes) = 0;
    virtual TInt Write(TInt aPos,const TDesC8& aDes,TInt aLength) = 0;
    virtual TInt Lock(TInt aPos,TInt aLength) const = 0;
    virtual TInt UnLock(TInt aPos,TInt aLength) const = 0;
    virtual TInt Seek(TSeek aMode,TInt& aPos) const = 0;
    virtual TInt Flush() = 0;
    virtual TInt Size(TInt& aSize) const = 0;
    virtual TInt SetSize(TInt aSize) = 0;

    virtual ~MJavaFile() {}

};

} // end of namespace javaruntime

#endif // MJAVAFILE_H

