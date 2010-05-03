


Configure properly the path to WTK (Wireless Toolkit)
-----------------------------------------------------
Check that the property 
	<property name="wtk.home" value="c:/WTK25"/>
set inside the following file:
JAVA_SRC_ROOT\tools\junit\JsrTestRunner\build\build.xml

points to the root of the WTK installation


RUNNING THE TESTS
=================
Execute the following commands at current location

1) ant

After executing the above command the JsrTestRunner.jad , JsrTestRunner.jar in the current directory
OmjPIMTests.jar files will be created at epoc32\release\winscw\udeb\z\resource\java\jvm\lib\common\

For testing on device,

Transfer JsrTestRunner jad & jar files to c:\Data\installs of the device .
Transfer OmjPIMTests.jar to epoc32\release\winscw\udeb\z\resource\java\jvm\lib\common\ of the device.
Install and run the JsrTestRunner MIDlet.
