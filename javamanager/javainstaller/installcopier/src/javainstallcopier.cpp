/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* JavaInstallCopier.exe is a utility for copying Java installation
* files from other processes private data cages to JavaInstaller's
* private data cage. JavaSifPlugin calls JavaInstallCopier if
* installation is started from a file which resides in a private
* data cage which JavaInstaller cannot access.
*
*/


#ifdef __SYMBIAN32__
#include <f32file.h>
#include <utf.h>
#endif // __SYMBIAN32__

#include "exceptionbase.h"
#include "javaoslayer.h"
#include "logger.h"
#include "runtimeexception.h"

using namespace java::runtime;
using namespace java::util;

int isCopyAllowed()
{
    int result = 0;
#ifdef __SYMBIAN32__
    // Only SifServer (0x10285BCB) is allowed to launch JavaInstallCopier.
    TSecureId creatorSecId(User::CreatorSecureId());
    if (0x10285BCB != creatorSecId.iId)
    {
        ELOG1(EJavaInstaller,
              "INSTALLCOPIER: Process 0x%x is not allowed to "
              "launch JavaInstallCopier", creatorSecId.iId);
        result = KErrPermissionDenied;
    }
#endif // __SYMBIAN32__
    return result;
}

int copyToInstallerDir(int argc, char *argv[])
{
    int result = 0;

#ifdef __SYMBIAN32__
    RFs rfs;
    result = rfs.Connect();
    if (KErrNone != result)
    {
        ELOG1(EJavaInstaller,
              "INSTALLCOPIER: Connecting to RFs failed, err=%d", result);
        return result;
    }

    CFileMan* fm = NULL;
    TRAP(result, fm = CFileMan::NewL(rfs));
    if (KErrNone != result)
    {
        ELOG1(EJavaInstaller,
              "INSTALLCOPIER: Creating CFileMan failed, err=%d", result);
        return result;
    }

    _LIT(KJavaInstallerTmp, "\\private\\102033E6\\installer\\tmp\\");
    for (int i = 1; i < argc && KErrNone == result; i++)
    {
        TPtrC8 srcPtr((const TText8*)argv[i]);
        TFileName srcPath;
        CnvUtfConverter::ConvertToUnicodeFromUtf8(srcPath, srcPtr);
        // Get the drive from the srcPath...
        TParse fp;
        rfs.Parse(srcPath, fp);
        // ...and prefix it to KJavaInstallerTmp path.
        TFileName dstPath = fp.Drive();
        dstPath.Append(KJavaInstallerTmp);

        result = fm->Copy(srcPath, dstPath, CFileMan::ERecurse|CFileMan::EOverWrite);

        std::wstring srcWs((wchar_t*)srcPath.Ptr(), srcPath.Length());
        std::wstring dstWs((wchar_t*)dstPath.Ptr(), dstPath.Length());
        if (KErrNone == result)
        {
            ILOG2(EJavaInstaller, "INSTALLCOPIER: Copied %S to %S",
                  srcWs.c_str(), dstWs.c_str());
        }
        else
        {
            ELOG3(EJavaInstaller,
                  "INSTALLCOPIER: Copying %S to %S failed, err=%d",
                  srcWs.c_str(), dstWs.c_str(), result);
        }
    }

    delete fm;
    fm = NULL;
    rfs.Close();
#else // __SYMBIAN32__
    for (int i = 1; i < argc && result == 0; i++)
    {
        WLOG1(EJavaInstaller, "INSTALLCOPIER: Ignored %s", argv[i]);
    }
#endif // __SYMBIAN32__

    return result;
}

int main(int argc, char *argv[])
{
    //ILOG(EJavaInstaller, "INSTALLCOPIER main()");
    JavaOsLayer::startUpTrace("INSTALLCOPIER main() start", -1, -1);

    int result = 0;
    try
    {
        result = isCopyAllowed();
        if (0 == result)
        {
            result = copyToInstallerDir(argc, argv);
        }
    }
    catch (RuntimeException& e)
    {
        ELOG1(EJavaInstaller, "INSTALLCOPIER main() RuntimeException caught: %s",
              e.toString().c_str());
    }
    catch (ExceptionBase& e)
    {
        ELOG1(EJavaInstaller, "INSTALLCOPIER main() ExceptionBase caught: %s",
              e.toString().c_str());
    }
    catch (std::exception& e)
    {
        ELOG1(EJavaInstaller, "INSTALLCOPIER main() Exception %s caught", e.what());
    }

    //ILOG1(EJavaInstaller, "INSTALLCOPIER main() exit %d", result);
    JavaOsLayer::startUpTrace("INSTALLCOPIER main() end", -1, -1);
    return result;
}
