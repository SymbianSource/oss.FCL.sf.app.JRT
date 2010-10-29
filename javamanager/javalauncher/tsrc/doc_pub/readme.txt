
- testmidletstart is simple native console program. Started from eshell to test that Java 
  applications can be started from native applications. The name of the Java application to be started 
  has currently been hard coded to SPtesti (midlet-name).


- midlets/sptest contains the SPtesti midlet.
  If you modify the .java code, you can easily compile the .java file like this:
javac -source 1.4 -target 1.4 -cp \epoc32\release\winscw\udeb\java_signature_test.jar testimidletti.java
  Then you can update the .jar file using WinZip.

  When SPtesti midlet is started, it shows the command line and instance count in the form.
  When the midlet is restarted, it shows the new command line and new instance count.


- midlets/plat_req contains platRequest midlet. 
  The .jad file has some sample platform request URLs that edited in the midlet and then used 
  when making platform request.
  When making platform requests that start native applications, this midlet must be signed to 
  operator or manufacturer domain.
