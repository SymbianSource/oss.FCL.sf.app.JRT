These binaries contains eSWT on top of S60 QT and open LCDUI on top of eSWT.
The binaries are exported to correct places if OMJ is used for QT development
(which is not default at the moment). Sources of S60 open LCDUI + eSWT + QT
are not part of the OMJ version control.

Be careful when modifying this folder. The Linux uses eswt.jar in 
bootclasspath during compile time.