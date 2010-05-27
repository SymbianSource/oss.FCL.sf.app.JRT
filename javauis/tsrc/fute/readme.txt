Instructions on how to build functional test MIDlets
====================================================

This file gives instructions on how to use the build.xml files for MIDlet suite 
compilation and packing.


Requirements
------------

* Get a local copy of the directory containing the UI FuTe MIDlets from the 
version control system. Get the whole structure, because the xml files in the 
root will be needed.

* Apache Ant installed. It is used for compiling the MIDlet source files and 
packing the classes and resources into MIDlet packages.

* Environment variable JAVA_HOME value is set to the path where JDK is 
installed.

* Sun Java Wireless Toolkit for CLDC (WTK) installed to "C:\WTK". The location 
is defined in the file "properties.xml":

    o <property name="wtk.home" value="C:/WTK"/> 

* Java public api classes jar file "java_public_api.jar" should be copied to WTK 
lib directory, in sub directory "ext". The file can be found i.e. in the SDK 
package. You can use also some other jar/zip package that contains the needed 
classes, it is used only for compiling the MIDlets. The location of the file is 
defined in the file "properties.xml":

    o <property name="java_public_api" value="${wtk.home}/lib/ext/java_public_api.jar"/> 

* Antenna library file should be copied to WTK lib directory, in sub directory 
"ext". It can be found from the Internet: http://antenna.sourceforge.net/. The 
location of the file is defined in the file "properties.xml":

    o <taskdef resource="antenna.properties" classpath="${wtk.home}/lib/ext/antenna-bin-1.0.2.jar"/> 


Building & packing MIDlets
--------------------------

Building the MIDlets can be done in the MIDlet project directory itself, or in 
some of the parent directories. If it is done in the MIDlet project directory, 
only that MIDlet will be built. If it is done in the root of the UI FuTe 
MIDlets, then all of them will be built.

The default action for Ant is "pack". It will compile and create the MIDlet 
packages. Compilation is done by using either one of these commands:

    ant
    ant pack

If you want to clean the build directories, it can be done with command:

    ant clean

The compiled classes and MIDlets are stored in a directory named "build" to the 
MIDlet suite project directory.
