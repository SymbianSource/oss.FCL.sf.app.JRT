Building
--------

Requirements:
    * Apache Ant.
    * Apache Tomcat. (Tested with 6.0.26)

Change "catalina.home" property in build.xml file to match your Tomcat
installation directory. Default is as follows:

    <property name="catalina.home" value="C:/apps/apache-tomcat-6.0.26"/>

To compile the project and to create .war archive just use either of following
commands:

    ant
    ant dist

To compile only use:

    ant compile

And to clean all build artifacts use:

    ant clean

Deployment
----------

After build is complete, "installertest.war" file is created into the "build"
directory. Deploy it to Tomcat instance as any other .war file. For HTTP
authentication tests it is assumed that user named "guest" with password
"guest" and with role "guest" exits.

Running the tests
-----------------

Before running tests add option "-server=server_ip_address:port" to
NotificationPosterTest, DownloaderTest and InstallerEngineTest
test suites in jiut.bat (for S60) or in build.xml (for Linux)
in "javainstaller\installer\tsrc\build" directory.
