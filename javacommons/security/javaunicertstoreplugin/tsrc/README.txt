
Execution of the tests:

1. Go to the build\ directory.
2. Compile tests using following commands:
   bldmake bldfiles
   make -f %JAVA_SRC_ROOT%/build/Makefile.nonqt
3. Execute test by following commmand: 
   \epoc32\release\winscw\udeb\javasecurity_certstoreplugintests.exe
4. Clean tests by following command:  
   make -f %JAVA_SRC_ROOT%/build/Makefile.nonqt clean
   Note: Cleaning is important because javacaptain.exe is moved under emulator's c:\sys\bin directory by build of the tests.
         Clean command moves it back to the \epoc32\release\winscw\udeb\ directory.
         Moving javacaptain.exe must be done because test data cannot be write to the z: drive.

Test environment:
This unit test set uses actual java captain to retrieve certificate information
from the javacertstore which extension plugin of the java captain.
Purpose of the Extensionplugin (part of the test code) is stop javacertstore extension plugin 
from the java captain. This is needed because javacertstore caches its data and otherwise we
would not have been able to change certificate data in the file system.

