@rem
@rem Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem
rem Script for running JavaInstaller unit tests from S60 eshell
rem Start SifServer before running any tests
sifserver
javainstaller uninstallall -silent -forceuninstall
javainstaller test -mainclass=com.nokia.mj.impl.installer.downloader.NotificationPosterTest
javainstaller test -mainclass=com.nokia.mj.impl.installer.downloader.DownloaderTest
javainstaller test -mainclass=com.nokia.mj.impl.installer.storagehandler.OtaStatusHandlerTest
javainstaller test -mainclass=com.nokia.mj.impl.installer.integrityservice.IntegrityServiceTest
javainstaller test -mainclass=com.nokia.mj.impl.installer.jadjarmatcher.JadJarMatcherTest
javainstaller test -mainclass=com.nokia.mj.impl.installer.storagehandler.StorageHandlerTest
javainstaller test -mainclass=com.nokia.mj.impl.installer.utils.AttributeValidatorTest
javainstaller test -mainclass=com.nokia.mj.impl.installer.utils.DriveInfoTest
javainstaller test -mainclass=com.nokia.mj.impl.installer.utils.SysUtilTest
javainstaller test -mainclass=com.nokia.mj.impl.installer.utils.UidTest
copy z:\resource\systemams\defaultmidleticon.mbm c:\java\lib
javainstaller test -mainclass=com.nokia.mj.impl.installer.applicationregistrator.ApplicationRegistratorTest
rem javainstaller test -mainclass=com.nokia.mj.impl.installer.applicationregistrator.SifRegistratorTest
javainstaller test -mainclass=com.nokia.mj.impl.installer.jsrpluginnotifier.JsrPluginNotifierTest
javainstaller test -mainclass=com.nokia.mj.impl.installer.customisationproperties.CustomisationPropertiesTest
javainstaller test -mainclass=com.nokia.mj.impl.installer.utils.AttributeReaderTest
javainstaller test -mainclass=com.nokia.mj.impl.installer.InstallerResultMessageTest
javainstaller test -mainclass=com.nokia.mj.impl.installer.GetComponentInfoTest
javainstaller test -mainclass=com.nokia.mj.impl.installer.InstallerEngineTest -param1=run1
javainstaller test -mainclass=com.nokia.mj.impl.installer.InstallerEngineTest -param1=run2
javainstaller test -mainclass=com.nokia.mj.impl.installer.InstallerEngineTest -param1=run3
javainstaller test -mainclass=com.nokia.mj.impl.installer.InstallerEngineTest -param1=run4
