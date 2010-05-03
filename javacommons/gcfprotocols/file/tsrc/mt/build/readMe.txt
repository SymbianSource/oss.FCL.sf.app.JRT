
/*******************************************************************************
*                         File API Module tests                                *
*                                                                              *
*******************************************************************************/

Instructions to execute File API module tests.
    1. Compile the executable that initializes file api test environment.
        
        Change directory to component's build folder
        cd \java\javaextensions\jsr\file\tsrc\mt\filetestinit\build
        
        Build the executable like any other component
        make -f \java\build\makefile.comp

    2. Compile the module tests
        
        Change directory to module test build folder
        cd \java\javaextensions\jsr\file\tsrc\mt\build\
        
        Compile the jar file
        ant

    3. Clean the test environment
        (please read the bat file before cleaning.
        Just in case there is something in those paths that are not to be deleted)
        
        cd \java\javaextensions\jsr\file\tsrc\mt\filetestinit\doc.s60
        cleanFileTests.bat

    4. To Execute the tests in S60, copy jiut.bat present in 
            \java\javaextensions\jsr\file\tsrc\mt\build\
         to
            \epoc32\winscw\c\

        Execute the bat file in eshell.
        
    5. Results are generated in 
        \epoc32\winscw\c\data\images\results\
            
Configuration:

1. Individual test cases can be executed by replacing
        javainstaller test -mainclass=com.nokia.mj.impl.file.Test_ALL
    line in jiut.bat with any of the test cases
        javainstaller test -mainclass=com.nokia.mj.impl.file.TestFile_0010
    
2. In order to change the root path, see the file
    \java\javaextensions\jsr\file\tsrc\mt\javasrc\com\nokia\mj\impl\file\utils\FileTestBase

3. Legacy DRM related test cases are also put into a single file.
    \java\javaextensions\jsr\file\tsrc\mt\javasrc\com\nokia\mj\impl\file\TestFile_DRM.java
    Just in case its needed.
    
    a. Unzip winscw.zip present in 
        \java\javaextensions\jsr\file\tsrc\mt\filetestinit\doc.s60\
        to \epoc32\winscw\
    
    b. Uncomment lines 
        //  if(FileTestConsts::KIncludeDRMInits)
        //  {
        //      FileTestUtils::doInitDRMContents();
        //  }
        in \java\javaextensions\jsr\file\tsrc\mt\filetestinit\src\fileconnectiontestinit.cpp
