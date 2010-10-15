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
* Description:  Tests for function server
*
*/

#include <e32base.h>
#include "TestHarness.h"
#include "testutils.h"

#include "mjvmnativeportlayer.h"
#include "mjavafile.h"

TEST_GROUP(TestJvmPortPriority)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};

TEST_GROUP(TestJvmPort)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};

_LIT(KTestFileName, "c:\\javaFileTest.txt");
_LIT(KTestReplaceFileName, "c:\\javaFileReplaceTest.txt");
_LIT(KTestDrmFileName1, "c:\\test.dm");
_LIT(KTestDrmFileName2, "c:\\test.dcf");
_LIT(KTestJarFileName, "c:\\test.jar");
_LIT(KTestDrmEncryptedFileName, "c:\\testDrm.dcf");
_LIT(KTestDrmEncryptedInstallerFileName, "c:\\testDrm.jar");

_LIT8(KTestText, "Testing..");
_LIT8(KTestText2, "Testing2..");
_LIT8(KJarText, "This is a jar file.");
_LIT8(KDrmText, "This is a drm protected file.");
_LIT8(KDM_CONTENT, "This is a dm file.");
_LIT8(KDCF_CONTENT, "This is a dcf file.");

TEST_GROUP(TestJavaFile)
{
    RFs fs;
    javaruntime::MJvmNativePortLayer* port;
    javaruntime::MJavaFile* file;
    TEST_SETUP()
    {
        port = 0;
        fs.Connect();
        fs.Delete(KTestFileName);
        fs.Delete(KTestReplaceFileName);
    }

    TEST_TEARDOWN()
    {
        delete port;
        delete file;
        fs.Close();
    }
};
// #undef CHECKWITHNOTE
// #define CHECKWITHNOTE(x, y) CHECK(x)
void checkPriority(const TThreadPriority& expected,
                   const TThreadPriority& recieved)
{
    CHECKWITHNOTE(expected == recieved, "Priority check failed. Expecting: "<<expected<<", received: "<<recieved);
}

// Testing priority table.
TEST(TestJvmPortPriority, priorityTest)
{
    int i = 0;
    checkPriority(EPriorityMuchLess, javaruntime::GetSingleThreadPriority(i++));
    checkPriority(EPriorityMuchLess, javaruntime::GetSingleThreadPriority(i++));
    checkPriority(EPriorityMuchLess, javaruntime::GetSingleThreadPriority(i++));
    checkPriority(EPriorityMuchLess, javaruntime::GetSingleThreadPriority(i++));
    checkPriority(EPriorityMuchLess, javaruntime::GetSingleThreadPriority(i++));
    checkPriority(EPriorityLess, javaruntime::GetSingleThreadPriority(i++));
    checkPriority(EPriorityLess, javaruntime::GetSingleThreadPriority(i++));
    checkPriority(EPriorityLess, javaruntime::GetSingleThreadPriority(i++));
    checkPriority(EPriorityLess, javaruntime::GetSingleThreadPriority(i++));
    checkPriority(EPriorityNormal, javaruntime::GetSingleThreadPriority(i++));
    checkPriority(EPriorityNormal, javaruntime::GetSingleThreadPriority(i++));
    checkPriority(EPriorityMore, javaruntime::GetSingleThreadPriority(i++));

}
void getPort(javaruntime::MJvmNativePortLayer*& port)
{
    TRAPD(err, port = javaruntime::GetPortLayerL());
    CHECKWITHNOTE(err == KErrNone, "Error in getting MJvmNativePortLayer: "<<err);
    CHECKWITHNOTE(port, "Got null pointer when getting port layer");
}

void getFileAndPort(javaruntime::MJvmNativePortLayer*& port, javaruntime::MJavaFile*& file)
{
    if (port == 0)
    {
        getPort(port);
    }
    TRAPD(err, file = port->GetJavaFileL());
    CHECKWITHNOTE(err == KErrNone, "Error in getting MJavaFile: "<<err);
    CHECKWITHNOTE(file, "Got null pointer when getting Java file");
}

_LIT(KOutpuFullDir,"c:\\logs\\java\\");
_LIT(KOutpuFullDir2,"c:\\logs\\java2\\");
_LIT(KConsole,"c:\\logs\\java\\vmconsole.txt");

// Testing log writings works.
TEST(TestJvmPort, logWritingTest)
{
    RFs fs;
    fs.Connect();
    TUint dummy;
    TBool dirExists = fs.Att(KOutpuFullDir, dummy) == KErrNone;

    TInt res;

    // If the c:\logs\java directory exixts rename it to java2
    if (dirExists)
    {
        res = fs.Rename(KOutpuFullDir, KOutpuFullDir2);
        CHECKWITHNOTE(res == KErrNone, "Rename failed: "<<res);
    }

    // Testing that writing to nonexisting dir works.
    javaruntime::MJvmNativePortLayer* port;
    getPort(port);
    TRAP(res, port->WriteToStdoutL(_L8("StdOutTestNonExisting\n")));
    CHECKWITHNOTE(res == KErrNone, "Error in writing to std.out: "<<res);
    TRAP(res, port->WriteToStderrL(_L8("StdErrTestNonExisting\n")));
    CHECKWITHNOTE(res == KErrNone, "Error in writing to std.err: "<<res);

    CHECKWITHNOTE(fs.Att(KOutpuFullDir, dummy) != KErrNone, "The logs\\java dir should not be present: ");

    delete port;
    port = 0;

    // Create c:\logs\java directory.
    res = fs.MkDir(KOutpuFullDir);
    CHECKWITHNOTE(res == KErrNone, "MkDir failed: "<<res);

    // Needs to reinit the port, because the logs dir exists now.
    getPort(port);
    TRAP(res, port->WriteToStdoutL(_L8("StdOutTest\n")));
    CHECKWITHNOTE(res == KErrNone, "Error in writing to std.out: "<<res);
    TRAP(res, port->WriteToStderrL(_L8("StdErrTest\n")));
    CHECKWITHNOTE(res == KErrNone, "Error in writing to std.err: "<<res);

    // Check that the vmconsole.txt contains correct content.
    RFile file;
    res = file.Open(fs, KConsole, EFileShareReadersOrWriters);
    CHECKWITHNOTE(res == KErrNone, "file open failed: "<<res);
    TBuf8<30> bufR;
    res = file.Read(bufR);
    CHECKWITHNOTE(res == KErrNone, "file read failed: "<<res);
    file.Close();
    res = bufR.Compare(_L8("StdOutTest\nStdErrTest\n"));
    CHECKWITHNOTE(res == 0, "Compare failed: "<<res);

    // Delete the vmconsole.txt. RFileLogger seems to keep the file open for
    // a while.
    int tryCount = 0;
    do
    {
        res = fs.Delete(KConsole);
        if (res == KErrInUse)
        {
            tryCount++;
            User::After(500 * 1000);
        }
    }
    while (res == KErrInUse && tryCount < 200);

    CHECKWITHNOTE(res == 0, "Delete failed: "<<res);

    // Delete c:\logs\java directory.
    res = fs.RmDir(KOutpuFullDir);;
    CHECKWITHNOTE(res == 0, "RmDir failed: "<<res);

    // If the c:\logs\java directory exixted before the tests, return it back.
    if (dirExists)
    {
        res = fs.Rename(KOutpuFullDir2, KOutpuFullDir);
        CHECKWITHNOTE(res == 0, "Rename2 failed: "<<res);
    }
    fs.Close();
    delete port;
}

// Testing that opening a file works ok and when trying to re-open KErrInUse
// is returned.
TEST(TestJavaFile, fileOpen)
{
    getFileAndPort(port, file);
    int res = file->Open(fs, KTestJarFileName, EFileRead);
    CHECKWITHNOTE(res == KErrNone, "Failed to open file: "<<res);

    res = file->Open(fs, KTestJarFileName, EFileRead);
    CHECKWITHNOTE(res == KErrInUse, "Open, unexpected result: "<<res);
    file->Close();
}

// Testing that opening unexisting files.
TEST(TestJavaFile, fileOpenUnexisting)
{
    getFileAndPort(port, file);
    int res = file->Open(fs, _L("c:\\nonexisting.txt"), EFileRead);
    CHECKWITHNOTE(res == KErrNotFound, "Open, unexpected result: "<<res);

    res = file->Open(fs, _L("c:\\nonexisting.dcf"), EFileRead);
    CHECKWITHNOTE(res == KErrNotFound, "Open2, unexpected result: "<<res);

    res = file->Open(fs, _L("c:\\nonexisting.txt"), EFileRead);
    CHECKWITHNOTE(res == KErrNotFound, "Open3, unexpected result: "<<res);
    file->Close();

    res = file->Open(fs, _L("c:\\nonexisting.dcf"), EFileRead);
    CHECKWITHNOTE(res == KErrNotFound, "Open4, unexpected result: "<<res);

    file->Close();
}

// Testing that creation of file to unaccessable location fails.
TEST(TestJavaFile, fileCreateUnAccessible)
{
    getFileAndPort(port, file);
    int res = file->Create(fs, _L("z:\\nonexisting.txt"), EFileRead);
    CHECKWITHNOTE(res == KErrAccessDenied, "Create, unexpected result: "<<res);
    file->Close();
}

// Testing that Replacing of file to unaccessable location fails.
TEST(TestJavaFile, fileReplaceUnAccessible)
{
    getFileAndPort(port, file);
    int res = file->Replace(fs, _L("z:\\nonexisting.txt"), EFileRead);
    CHECKWITHNOTE(res == KErrAccessDenied, "Replace, unexpected result: "<<res);
    file->Close();
}

// Testing that file creation succceeds. As a side test testing that Open,
// Create and replace will fail when using file object that is in use.
TEST(TestJavaFile, fileCreate1)
{
    getFileAndPort(port, file);
    int res = file->Create(fs, KTestFileName, EFileRead);
    CHECKWITHNOTE(res == KErrNone, "Failed to create file: "<<res);
    TUint dummy;
    CHECKWITHNOTE(fs.Att(KTestFileName, dummy) == KErrNone, "File does't exist");

    res = file->Open(fs, KTestFileName, EFileRead);
    CHECKWITHNOTE(res == KErrInUse, "ReOpen, unexpected result: "<<res);

    res = file->Replace(fs, KTestFileName, EFileRead);
    CHECKWITHNOTE(res == KErrInUse, "ReReplace, unexpected result: "<<res);

    res = file->Create(fs, KTestFileName, EFileRead);
    CHECKWITHNOTE(res == KErrInUse, "ReCreate, unexpected result: "<<res);

    file->Close();
}

// Testing read, write flush and seek for normal files.
TEST(TestJavaFile, fileCreateWriteReads)
{
    getFileAndPort(port, file);

    // Create the test file.
    int res = file->Create(fs, KTestFileName, EFileRead);
    CHECKWITHNOTE(res == KErrNone, "Failed to create file: "<<res);
    TUint dummy;
    CHECKWITHNOTE(fs.Att(KTestFileName, dummy) == KErrNone, "File does't exist");

    TBuf8<15> bufR;
    TBuf8<15> bufW;
    bufW.Append(KTestText);

    // Write content to buffer.
    res = file->Write(bufW);
    CHECKWITHNOTE(res == KErrNone, "Write failed: "<<res);

    // Flush content to buffer.
    res = file->Flush();
    CHECKWITHNOTE(res == KErrNone, "Flush failed: "<<res);

    // Set current position to the start of the file.
    TInt pos = 0;
    res = file->Seek(ESeekStart, pos);
    CHECKWITHNOTE(res == KErrNone, "Seek failed: "<<res);

    // Read the whole content of the file.
    res = file->Read(bufR);
    CHECKWITHNOTE(res == KErrNone, "Read failed: "<<res);

    // Compare the the read and written are the same.
    res = bufW.Compare(bufR);
    CHECKWITHNOTE(res == 0, "Compare failed: "<<res);

    // Read a portion of the content from the file.
    bufR.Zero();
    pos = 2;
    res = file->Seek(ESeekStart, pos);
    CHECKWITHNOTE(res == KErrNone, "Seek2 failed: "<<res);
    res = file->Read(bufR, 5);
    CHECKWITHNOTE(res == KErrNone, "Read2 failed: "<<res);
    res = bufW.Mid(2,5).Compare(bufR);
    CHECKWITHNOTE(res == 0, "Compare2 failed: "<<res);

    // Read a portion of the content from the file. Testing
    // ESeekEnd.
    bufR.Zero();
    pos = -6;
    res = file->Seek(ESeekEnd, pos);
    CHECKWITHNOTE(res == KErrNone, "Seek3 failed: "<<res);
    res = file->Read(bufR, 4);
    CHECKWITHNOTE(res == KErrNone, "Read3 failed: "<<res);
    res = bufW.Mid(3,4).Compare(bufR);
    CHECKWITHNOTE(res == 0, "Compare2 failed: "<<res);

    file->Close();
}

// Testing all variants of Write methods, Size, and SetSize. As a side test
// testing that Open, Create and replace will fail when using file object
// that is in use.
TEST(TestJavaFile, fileCreateWritesRead)
{
    getFileAndPort(port, file);
    int res = file->Create(fs, KTestFileName, EFileRead);
    CHECKWITHNOTE(res == KErrNone, "Failed to create file: "<<res);
    TUint dummy;
    CHECKWITHNOTE(fs.Att(KTestFileName, dummy) == KErrNone, "File does't exist");

    res = file->Open(fs, KTestFileName, EFileRead);
    CHECKWITHNOTE(res == KErrInUse, "ReOpen, unexpected result: "<<res);

    res = file->Replace(fs, KTestFileName, EFileRead);
    CHECKWITHNOTE(res == KErrInUse, "ReReplace, unexpected result: "<<res);

    res = file->Create(fs, KTestFileName, EFileRead);
    CHECKWITHNOTE(res == KErrInUse, "ReCreate, unexpected result: "<<res);


    // Write some content to file.
    TBuf8<45> bufR;
    TBuf8<15> bufW;
    bufW.Append(KTestText);
    res = file->Write(bufW);
    CHECKWITHNOTE(res == KErrNone, "Write failed: "<<res);
    res = file->Write(bufW, 8);  // Testing..Testing.
    CHECKWITHNOTE(res == KErrNone, "Write failed2: "<<res);
    res = file->Write(2, bufW);  // TeTesting..sting.
    CHECKWITHNOTE(res == KErrNone, "Write failed3: "<<res);
    res = file->Write(8, bufW, 2); // TeTestinTe.sting.
    CHECKWITHNOTE(res == KErrNone, "Write failed4: "<<res);
    TInt pos = 0;
    res = file->Seek(ESeekEnd, pos);
    CHECKWITHNOTE(res == KErrNone, "Seek failed: "<<res);
    res = file->Write(bufW, 4);  // TeTestinTe.sting.Test
    CHECKWITHNOTE(res == KErrNone, "Write failed5: "<<res);
    file->Flush();
    CHECKWITHNOTE(res == KErrNone, "Flush failed: "<<res);

    // Set current position to the start of the file.
    pos = 0;
    res = file->Seek(ESeekStart, pos);
    CHECKWITHNOTE(res == KErrNone, "Seek failed2: "<<res);

    // Read the whole content of the file.
    res = file->Read(bufR);
    CHECKWITHNOTE(res == KErrNone, "Read failed: "<<res);
    _LIT8(KTestRes, "TeTestinTe.sting.Test");

    // Compare that the file content is correct.
    res = bufR.Compare(KTestRes);
    CHECKWITHNOTE(res == 0, "Compare failed: "<<res);

    // Testing that Size works ok.
    TInt size;
    res = file->Size(size);
    CHECKWITHNOTE(res == 0, "Size call failed: "<<res);
    CHECKWITHNOTE(size == 21, "Size failed: "<<size);

    // Testing that SetSize works ok.
    res = file->SetSize(10);
    CHECKWITHNOTE(size == 21, "SetSize failed: "<<res);

    // Read the content ater size change.
    bufR.Zero();
    res = file->Seek(ESeekStart, pos);
    res = file->Read(bufR);
    CHECKWITHNOTE(res == KErrNone, "Read failed2: "<<res);

    // The new size should be smaller that before SetSize.
    pos = 0;
    res = file->Size(size);
    CHECKWITHNOTE(res == 0, "Size call failed2: "<<res);
    CHECKWITHNOTE(size == 10, "Size failed2: "<<size);

    // Compare that the file content is correct.
    res = bufR.Compare(KTestRes().Left(10));
    CHECKWITHNOTE(res == 0, "Compare failed: "<<res);

    file->Close();
}

// Testing Replace method. As a side test testing that Open, Create and
// replace will fail when using file object that is in use.
TEST(TestJavaFile, fileReplace)
{
    getFileAndPort(port, file);

    // Checking that file that we are about to create doesn't exist.
    TUint dummy;
    CHECKWITHNOTE(fs.Att(KTestReplaceFileName, dummy) != KErrNone, "File exists before");

    // Using Replace to create the file.
    int res = file->Replace(fs, KTestReplaceFileName, EFileRead);
    CHECKWITHNOTE(res == KErrNone, "Failed to Replace file: "<<res);
    CHECKWITHNOTE(fs.Att(KTestReplaceFileName, dummy) == KErrNone, "File does't exist");

    res = file->Open(fs, KTestReplaceFileName, EFileRead);
    CHECKWITHNOTE(res == KErrInUse, "ReOpen, unexpected result: "<<res);

    res = file->Replace(fs, KTestReplaceFileName, EFileRead);
    CHECKWITHNOTE(res == KErrInUse, "ReReplace, unexpected result: "<<res);

    res = file->Create(fs, KTestReplaceFileName, EFileRead);
    CHECKWITHNOTE(res == KErrInUse, "ReCreate, unexpected result: "<<res);

    // Write content to created file.
    TBuf8<45> bufR;
    TBuf8<15> bufW;
    bufW.Append(KTestText);
    res = file->Write(bufW);
    CHECKWITHNOTE(res == KErrNone, "Write failed: "<<res);

    // Read the content and compare that the content is correct.
    TInt pos = 0;
    res = file->Seek(ESeekStart, pos);
    CHECKWITHNOTE(res == KErrNone, "Seek failed: "<<res);
    res = file->Read(bufR);
    CHECKWITHNOTE(res == KErrNone, "Read failed: "<<res);
    res = bufR.Compare(KTestText);
    CHECKWITHNOTE(res == 0, "Compare failed: "<<res);
    file->Close();

    // Replace the existing file.
    res = file->Replace(fs, KTestReplaceFileName, EFileRead);
    CHECKWITHNOTE(res == KErrNone, "Failed to Replace file2: "<<res);
    CHECKWITHNOTE(fs.Att(KTestReplaceFileName, dummy) == KErrNone, "File does't exist2");

    // Check that the file size is zero i.e. a new file.
    TInt size;
    res = file->Size(size);
    CHECKWITHNOTE(res == 0, "Size call failed: "<<res);
    CHECKWITHNOTE(size == 0, "Size failed: "<<size);

    // Write content to recreated file.
    bufW.Zero();
    bufR.Zero();
    bufW.Append(KTestText2);
    res = file->Write(bufW);
    CHECKWITHNOTE(res == KErrNone, "Write failed2: "<<res);

    // Read the content and compare that the content is correct.
    res = file->Seek(ESeekStart, pos);
    CHECKWITHNOTE(res == KErrNone, "Seek failed2: "<<res);
    res = file->Read(bufR);
    CHECKWITHNOTE(res == KErrNone, "Read failed2: "<<res);
    res = bufR.Compare(KTestText2);
    CHECKWITHNOTE(res == 0, "Compare failed2: "<<res);
    file->Close();
}

// Testing Lock and Unlock methods.
TEST(TestJavaFile, fileLock)
{
    getFileAndPort(port, file);

    // Creating two files objects pointing to same directory.
    int res = file->Create(fs, KTestFileName, EFileWrite | EFileShareAny);
    CHECKWITHNOTE(res == KErrNone, "Failed to Create file: "<<res);

    javaruntime::MJavaFile* file2;
    getFileAndPort(port, file2);
    res = file2->Open(fs, KTestFileName, EFileRead | EFileShareAny);
    CHECKWITHNOTE(res == KErrNone, "Failed to Open file: "<<res);

    // Write some content.
    res = file->Write(KTestText);
    CHECKWITHNOTE(res == KErrNone, "Failed to write to file: "<<res);

    // Write some content.
    res = file->Lock(3, 4);
    CHECKWITHNOTE(res == KErrNone, "Failed to lock the file: "<<res);

    // Lock the area
    TInt pos = 4;
    res = file2->Seek(ESeekStart, pos);
    CHECKWITHNOTE(res == KErrNone, "Seek failed: "<<res);

    // Check that locked the area is unaccessable.
    TBuf8<45> bufR;
    res = file2->Read(bufR, 2);
    CHECKWITHNOTE(res == KErrLocked, "Expecting locked when reading: "<<res);

    // Unlock the area
    res = file->UnLock(3, 4);
    CHECKWITHNOTE(res == KErrNone, "Failed to unlock the file: "<<res);

    // Check that unlocked the area is now accessable.
    res = file2->Read(bufR, 2);
    CHECKWITHNOTE(res == KErrNone, "Read failed: "<<res);
    res = KTestText().Mid(4,2).Compare(bufR);
    CHECKWITHNOTE(res == 0, "Compare failed: "<<res);

    file->Close();
    file2->Close();
    delete file2;
}


// Testing that jar file can be opened. This test is for checking a branch in
// IsDrm() method.
TEST(TestJavaFile, openJar)
{
    getFileAndPort(port, file);

    // Open the jar file.
    TInt res = file->Open(fs, KTestJarFileName, EFileRead);
    CHECKWITHNOTE(res == KErrNone, "Open, unexpected result: "<<res);

    // Read and compare content.
    TBuf8<45> bufR;
    res = file->Read(bufR);
    CHECKWITHNOTE(res == KErrNone, "Read failed: "<<res);
    res = bufR.Compare(KJarText);
    CHECKWITHNOTE(res == 0, "Compare failed: "<<res);
    file->Close();
}

// Testing the file open through CAF with files that are not DRM protected.
void testOpenCafFile(RFs& fs,
                     javaruntime::MJavaFile* file,
                     const TDesC& fileName,
                     const TDesC8& compareString)
{
    // Open file using CAF.
    TInt res = file->Open(fs, fileName, EFileRead);
    CHECKWITHNOTE(res == KErrNone, "Failed to open file: "<<res);

    // Read the content and compare.
    TBuf8<45> bufR;
    res = file->Read(bufR);
    CHECKWITHNOTE(res == KErrNone, "Read failed: "<<res);
    res = bufR.Compare(compareString);
    CHECKWITHNOTE(res == 0, "Compare failed: "<<res);

    // Read the partial content and compare. Using ESeekStart.
    bufR.Zero();
    TInt pos = 2;
    res = file->Seek(ESeekStart, pos);
    CHECKWITHNOTE(res == KErrNone, "Seek failed: "<<res);
    res = file->Read(bufR, 5);
    CHECKWITHNOTE(res == KErrNone, "Read2 failed: "<<res);
    res = compareString.Mid(2,5).Compare(bufR);
    CHECKWITHNOTE(res == 0, "Compare2 failed: "<<res);

    // Read the partial content and compare. Using ESeekEnd.
    bufR.Zero();
    pos = -6;
    res = file->Seek(ESeekEnd, pos);
    CHECKWITHNOTE(res == KErrNone, "Seek3 failed: "<<res);
    res = file->Read(bufR, 4);
    CHECKWITHNOTE(res == KErrNone, "Read3 failed: "<<res);
    res = compareString.Mid(compareString.Length() - 6, 4).Compare(bufR);
    CHECKWITHNOTE(res == 0, "Compare2 failed: "<<res);

    // Check that Size method works also in case of CAF.
    TInt size;
    res = file->Size(size);
    CHECKWITHNOTE(res == 0, "Size call failed: "<<res);
    CHECKWITHNOTE(size == compareString.Length(), "Size failed: "<<size);

    file->Close();
}


// Testing CAF with .dm file extension.
TEST(TestJavaFile, readDrmDmFile)
{
    getFileAndPort(port, file);
    testOpenCafFile(fs, file, KTestDrmFileName1, KDM_CONTENT);
}

// Testing CAF with .dcf file extension.
TEST(TestJavaFile, readDrmDcfFile)
{
    getFileAndPort(port, file);
    testOpenCafFile(fs, file, KTestDrmFileName2, KDCF_CONTENT);
}

// Testing methods that are not supported by CAF. Expecting correct error codes.
TEST(TestJavaFile, readDrmNotSupportedMethods)
{
    // Open file using CAF.
    getFileAndPort(port, file);
    TInt res = file->Open(fs, KTestDrmFileName1, EFileRead);
    CHECKWITHNOTE(res == KErrNone, "Failed to open file: "<<res);

    // Trying to re-open the file - this should fail.
    res = file->Open(fs, KTestDrmFileName1, EFileRead);
    CHECKWITHNOTE(res == KErrInUse, "Open, unexpected result: "<<res);

    res = file->SetSize(10);
    CHECKWITHNOTE(res == KErrNotSupported, "SetSize, unexpected result: "<<res);

    _LIT8(KDummy, "dummy");
    res = file->Write(KDummy);
    CHECKWITHNOTE(res == KErrNotSupported, "Write, unexpected result: "<<res);

    res = file->Write(KDummy, 1);
    CHECKWITHNOTE(res == KErrNotSupported, "Write2, unexpected result: "<<res);

    res = file->Write(1, KDummy);
    CHECKWITHNOTE(res == KErrNotSupported, "Write3, unexpected result: "<<res);

    res = file->Write(1, KDummy, 1);
    CHECKWITHNOTE(res == KErrNotSupported, "Write4, unexpected result: "<<res);

    res = file->Lock(1, 1);
    CHECKWITHNOTE(res == KErrNotSupported, "Lock, unexpected result: "<<res);

    res = file->UnLock(1, 1);
    CHECKWITHNOTE(res == KErrNotSupported, "UnLock, unexpected result: "<<res);

    res = file->Flush();
    CHECKWITHNOTE(res == KErrNotSupported, "Flush, unexpected result: "<<res);

    file->Close();
}

// Testing file reading that is DRM protected. Reading through CAF. This test
// requires that rights object is added to key store. See readme.txt for
// instructions.
TEST(TestJavaFile, readDrmProtectedFile)
{
    // Open the DRM protected jar file.
    getFileAndPort(port, file);
    TInt res = file->Open(fs, KTestDrmEncryptedFileName, EFileRead);
    CHECKWITHNOTE(res == KErrNone, "Failed to open file: "<<res);

    // Read the content and compare.
    TBuf8<45> bufR;
    res = file->Read(bufR);
    CHECKWITHNOTE(res == KErrNone, "Read failed: "<<res);
    res = bufR.Compare(KDrmText);
    CHECKWITHNOTE(res == 0, "Compare failed: "<<res);

    file->Close();

    // Open the DRM protected file having .jar extension. This will not be
    // opened through CAF and therefore the content should not be in decrypted
    // format.
    res = file->Open(fs, KTestDrmEncryptedInstallerFileName, EFileRead);
    CHECKWITHNOTE(res == KErrNone, "Failed to open file2: "<<res);

    // Read the content.
    bufR.Zero();
    res = file->Read(bufR);
    CHECKWITHNOTE(res == KErrNone, "Read failed2: "<<res);

    // Check that we were not able to read the content as decrypted format.
    res = bufR.Compare(KDrmText);
    CHECKWITHNOTE(res != 0, "Compare failed2: "<<res);
    file->Close();
}

// This test tests that the jar file that is DRM protected can be opended in
// decrypted format when the process name is JavaInstaller.
TEST(TestJavaFile, readDrmProtectedInstaller)
{
    // OpenC uses statics and those will cause a memory allocations that will
    // be interpreted as a leak.
    EXPECT_N_LEAKS(33);

    // Faking the process name to be JavaInstaller.
    _LIT(KJavaInstallerProcessName, "JavaInstaller");
    _LIT(KOrigProcessName, "testjvmport");
    // Rename installer process and main thread.
    TInt res = User::RenameProcess(KJavaInstallerProcessName);

    // Open the DRM protected jar file.
    getFileAndPort(port, file);
    res = file->Open(fs, KTestDrmEncryptedInstallerFileName, EFileRead);
    CHECKWITHNOTE(res == KErrNone, "Failed to open file: "<<res);

    // Read the content and check that it was in decrypted format.
    TBuf8<45> bufR;
    res = file->Read(bufR);
    CHECKWITHNOTE(res == KErrNone, "Read failed: "<<res);
    res = bufR.Compare(KDrmText);
    CHECKWITHNOTE(res == 0, "Compare failed: "<<res);

    file->Close();

    // Testing that opening of jar file that is not DRM protected succeeds.
    res = file->Open(fs, KTestJarFileName, EFileRead);
    CHECKWITHNOTE(res == KErrNone, "Failed to open file2: "<<res);

    // Read the content and compare.
    bufR.Zero();
    res = file->Read(bufR);
    CHECKWITHNOTE(res == KErrNone, "Read failed2: "<<res);
    res = bufR.Compare(KJarText);
    CHECKWITHNOTE(res == 0, "Compare failed2: "<<res);
    file->Close();

    // Testing that opening nonexisting file leads to error.
    res = file->Open(fs, _L("c:\\nonexisting.jar"), EFileRead);
    CHECKWITHNOTE(res == KErrNotFound, "Open, unexpected result: "<<res);
    file->Close();

    // Rename the process name to original.
    User::RenameProcess(KOrigProcessName);
}
