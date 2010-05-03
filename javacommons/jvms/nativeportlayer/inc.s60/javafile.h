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
* Description:  CJavaFile
*
*/

#ifndef JAVAFILE_H
#define JAVAFILE_H

#include <e32base.h>
#include <f32file.h>
#include <string>

#include <caf/caf.h>
#include <caf/content.h>
#include <caf/data.h>

#include "javaosheaders.h"

namespace javaruntime
{

class MJavaFile;

OS_NONSHARABLE_CLASS(CJavaFile): public CBase, public MJavaFile
{

public:
    static MJavaFile* NewL();

    virtual ~CJavaFile();

    virtual TInt Open(RFs& aFs,const TDesC& aFileName, TUint aFileMode);
    virtual void Close();
    virtual TInt Create(RFs& aFs,const TDesC& aName,TUint aFileMode);
    virtual TInt Read(TDes8& aDes) const;
    virtual TInt Read(TDes8& aDes,TInt aLength) const;
    virtual TInt Replace(RFs& aFs,const TDesC& aName,TUint aFileMode);
    virtual TInt Write(const TDesC8& aDes);
    virtual TInt Write(const TDesC8& aDes,TInt aLength);
    virtual TInt Write(TInt aPos,const TDesC8& aDes);
    virtual TInt Write(TInt aPos,const TDesC8& aDes,TInt aLength);
    virtual TInt Lock(TInt aPos,TInt aLength) const;
    virtual TInt UnLock(TInt aPos,TInt aLength) const;
    virtual TInt Seek(TSeek aMode,TInt& aPos) const;
    virtual TInt Flush();
    virtual TInt Size(TInt& aSize) const;
    virtual TInt SetSize(TInt aSize);

protected:
    CJavaFile();

private:
    enum TJavaFileMode
    {
        EJavaFileClosed,
        EJavaFileOpenForCaf,
        EJavaFileOpenForNormalFile
    };
    TBool isDrmFile(const TDesC& aFileName);

private:
    void ConstructL();
    void OpenCafL(const TDesC& aFileName);

private:
    ContentAccess::CContent*    iCafContent; //Owns
    ContentAccess::CData*       iCafData; //Owns
    TJavaFileMode               iJavaFileMode;
    RFile                       iNormalFile;
    std::wstring                mFileName;
};

} // end of namespace javaruntime

IMPORT_C void Dummy1(); //These are keeping the ordinals in J9 understandable order. TODO remove these at some point
IMPORT_C void Dummy2();
IMPORT_C void Dummy3();
IMPORT_C void Dummy4();
IMPORT_C void Dummy5();
IMPORT_C void Dummy6();
IMPORT_C void Dummy7();
IMPORT_C void Dummy8();
IMPORT_C void Dummy9();
#endif // JAVAFILE_H

