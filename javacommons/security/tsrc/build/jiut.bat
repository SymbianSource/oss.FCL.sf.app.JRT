@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
rem Script for running OMJSecurity unit tests from S60 eshell
javainstaller test -mainclass=com.nokia.mj.impl.security.OMJSecurityTests
javainstaller test -mainclass=com.nokia.mj.impl.security.common.SecurityCommonTests
javainstaller test -mainclass=com.nokia.mj.impl.security.midp.appaccessauthorization.ApplicationAccessAuthorizationTests
javainstaller test -mainclass=com.nokia.mj.impl.security.midp.authentication.AuthenticationModuleTests
javainstaller test -mainclass=com.nokia.mj.impl.security.midp.authentication.InteractiveAuthenticationModuleTests -server=172.22.146.192
javainstaller test -mainclass=com.nokia.mj.impl.security.midp.authorization.SecurityPolicyModuleTests
javainstaller test -mainclass=com.nokia.mj.impl.security.midp.authorization.PermissionGranterTests
javainstaller test -mainclass=com.nokia.mj.impl.security.midp.authorization.AccessControllerTests
javainstaller test -mainclass=com.nokia.mj.impl.security.midp.authorization.InteractiveAccessControllerTests
javainstaller test -mainclass=com.nokia.mj.impl.security.midp.common.SecurityDescriptorAttributesTests
javainstaller test -mainclass=com.nokia.mj.impl.security.midp.common.SecurityManifestAttributesTests
javainstaller test -mainclass=com.nokia.mj.impl.security.midp.common.SecurityExtensionsTests
javainstaller test -mainclass=com.nokia.mj.impl.security.midp.storage.SecurityStorageTests
javainstaller test -mainclass=com.nokia.mj.impl.security.packageprotection.PackageProtectorTests
javainstaller test -mainclass=com.nokia.mj.tools.security.midp.PolicyEditorTests
MIDPAuthenticationTests
SecurityLegacySupportTests
javasecurity_certstoreplugintests
