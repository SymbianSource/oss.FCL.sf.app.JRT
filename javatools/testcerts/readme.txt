Overview
========
This directory contains the test root certificates bound to Manufacturer, Operator 
and Trusted Third Party domains and a keystore for signing a MIDlet suite so that 
it will be bound to one of these domains as needed.

The following scripts can be used to sign a given MIDlet suite:
- signMidlet_Manufacturer.cmd is a script to be used for signing a suite to manufacturer domain 
- signMidlet_Operator.cmd is a script to be used for signing a suite to operator domain 
- signMidlet_Trusted_Third_Party.cmd is a script to be used for signing a suite to identified third party domain 

These scripts need to be modified according to your installation - namely the locations 
of the Wireless Toolkit and JDK need to be fixed. Additionally, all of these scripts need
to be ran from the drive which corresponds to the used S60 environment. 
See the detailed notes below. 

Examples
========
signMidlet_Manufacturer.cmd test.jad test.jar 
signMidlet_operator.cmd test.jad test.jar 
signMidlet_Trusted_Third_Party.cmd test.jad test.jar 

Details
=======
Running of the scripts requires the following: 
- Wireless Toolkit and Java Developer Kit have been installed to your machine 
- The script's JDK variable has to be configured so that it points to the installation directory of the Java Developer Kit environment (e.g. C:\j2sdk1.4.2_09\) 
- The script's WTK variable has to be configured so that it points to the installation directory of the Wireless Toolkit environment (e.g. C:\WTK23\) 

Further requirements for the MIDlet suite (no way specific to this particular use case but nevertheless could cause easily failures in the wanted behaviour): 
- The JAD file MUST contain the proper size of the JAR file
- Installation MUST be started from JAD (the signing information is in JAD, not in JAR)
