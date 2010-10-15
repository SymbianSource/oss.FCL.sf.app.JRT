This directory contains test for jvmnativeport.

To compile test run make frome the tsrc folder.

The uses uses DRM protected files which means that the right objects must be
added to key store. Compilation puts right objet into correct folder. Before 
running tests a tool DRMEncryptor must be run in the device. From the application
select Options/Encrypt and wait for dialog telling 'Encryption done. 1 rights 
object processed.' 

To run tests start eshell and type tst.bat

Description of used files:

test.dcf:    This non encrypted file that is used to test that data can be read
             through CAF.
test.dm:     This non encrypted file that is used to test that data can be read
             through CAF.
test.jar:    This special jar file that contains only plain text.
testDrm.dcf: This is drm protected file containing text:
             'This is a drm protected file.'
testDrm.dr:  Rights object for the drm protected files
testDrm.jar: Same as testDrm.dcf, only extension has been changed to jar to tesr
             one installer realted branch.
