#
# Copyright (c) 2009,2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: Java Runtime DLL UID3 allocations
#

# Allocate only UID3s here only for DLL targets

# S60 5.0 must use same UID3 for components which are already on ROM, so
# that IAD SIS can eclipse them correctly.
contains(TEMPLATE,lib):isEmpty(TARGET.UID3):contains(PROJECT_DEFINES,RD_JAVA_S60_RELEASE_5_0) {

  contains(TARGET, eswt): TARGET.UID3 = 0x2000CFEC
  contains(TARGET, eswtdirectcontent): TARGET.UID3 = 0x2000CFEC
  contains(TARGET, jvmnativeport): TARGET.UID3 = 0x2000CFEC
  contains(TARGET, midp2cenrep): TARGET.UID3 = 0x2000CFEC
  contains(TARGET, midp2permissiondb): TARGET.UID3 = 0x2000CFEC
  contains(TARGET, midp2userpreferences): TARGET.UID3 = 0x2000CFEC

}

contains(TEMPLATE,lib):isEmpty(TARGET.UID3):contains(PROJECT_DEFINES,RD_JAVA_S60_RELEASE_10_1) {
  contains(TARGET, javacentrep): TARGET.UID3 = 0x2002DCE1
}

contains(TEMPLATE,lib):isEmpty(TARGET.UID3) {

contains(TARGET, eswt): TARGET.UID3 = 0x2002DC93   # On >= 9.2 only
contains(TARGET, eswtdirectcontent): TARGET.UID3 = 0x2002DC94   # On >= 9.2 only
contains(TARGET, eswtqt): TARGET.UID3 = 0x2002DC95
contains(TARGET, javaamms): TARGET.UID3 = 0x2002DC96
contains(TARGET, javabluecove): TARGET.UID3 = 0x2002DC97
contains(TARGET, javabluetooth): TARGET.UID3 = 0x2002DC98
contains(TARGET, javabluetoothcommons): TARGET.UID3 = 0x2002DC99
contains(TARGET, javabtgoepscplugin): TARGET.UID3 = 0x2002DC9A
contains(TARGET, javabtl2capscplugin): TARGET.UID3 = 0x2002DC9B
contains(TARGET, javabtsppscplugin): TARGET.UID3 = 0x2002DC9C
contains(TARGET, javacaptain_ext_btdeviceclassmanager): TARGET.UID3 = 0x2002DC9D
contains(TARGET, javacaptain_ext_config): TARGET.UID3 = 0x2002DC9E
contains(TARGET, javacaptain_ext_ondemand_7): TARGET.UID3 = 0x2002DC9F
contains(TARGET, javacaptain_ext_preinstallerstarter): TARGET.UID3 = 0x2002DCA0
contains(TARGET, javacaptain_ext_prewarm): TARGET.UID3 = 0x2002DCA1
contains(TARGET, javacaptain_ext_pushregistryplugin): TARGET.UID3 = 0x2002DCA2
contains(TARGET, javacaptain_ext_storageserverplugin): TARGET.UID3 = 0x2002DCA3
contains(TARGET, javacbsscplugin): TARGET.UID3 = 0x2002DCA4
contains(TARGET, javacomm): TARGET.UID3 = 0x2002DCA5
contains(TARGET, javacomms): TARGET.UID3 = 0x2002DCA6
contains(TARGET, javaconnectionmanager): TARGET.UID3 = 0x2002DCA7
contains(TARGET, javacoreui): TARGET.UID3 = 0x2002DCA8
contains(TARGET, javadatagram): TARGET.UID3 = 0x2002DCA9
contains(TARGET, javadatagramscplugin): TARGET.UID3 = 0x2002DCAA
contains(TARGET, javadebugapi): TARGET.UID3 = 0x2002DCAB
contains(TARGET, javafile): TARGET.UID3 = 0x2002DCAC
contains(TARGET, javafileutils): TARGET.UID3 = 0x2002DCAD
contains(TARGET, javagcf): TARGET.UID3 = 0x2002DCAE
contains(TARGET, javaglobalindicators): TARGET.UID3 = 0x2002DCAF
contains(TARGET, javahttp): TARGET.UID3 = 0x2002DCB1
contains(TARGET, javahttps): TARGET.UID3 = 0x2002DCB2
contains(TARGET, javaiapinfo): TARGET.UID3 = 0x2002DCB3
contains(TARGET, javainstaller): TARGET.UID3 = 0x2002DCB4
contains(TARGET, javainstallerstarter): TARGET.UID3 = 0x2002DCB5
contains(TARGET, javainstallerui): TARGET.UID3 = 0x2002DCB6
contains(TARGET, javaipc): TARGET.UID3 = 0x2002DCB7
contains(TARGET, javajvmargsmodifier): TARGET.UID3 = 0x2002DCB8
contains(TARGET, javajvmargsmodifierfile): TARGET.UID3 = 0x2002DCB9
contains(TARGET, javalcdui): TARGET.UID3 = 0x2002DCBA
contains(TARGET, javalegacyutils): TARGET.UID3 = 0x2002DCBB
contains(TARGET, javalocation): TARGET.UID3 = 0x2002DCBC
contains(TARGET, javam2g): TARGET.UID3 = 0x2002DCBD
contains(TARGET, javam3g): TARGET.UID3 = 0x2002DCBE
contains(TARGET, javamidpruntime): TARGET.UID3 = 0x2002DCBF
contains(TARGET, javamidpstarter): TARGET.UID3 = 0x2002DCC0
contains(TARGET, javammsscplugin): TARGET.UID3 = 0x2002DCC1
contains(TARGET, javamobilemedia): TARGET.UID3 = 0x2002DCC2
contains(TARGET, javamobinfo): TARGET.UID3 = 0x2002DCC3
contains(TARGET, javanokiasound): TARGET.UID3 = 0x2002DCC4
contains(TARGET, javapim): TARGET.UID3 = 0x2002DCC5
contains(TARGET, javapreinstaller): TARGET.UID3 = 0x2002DCC6
contains(TARGET, javaptivariation): TARGET.UID3 = 0x2002DCC7
contains(TARGET, javapushcontroller): TARGET.UID3 = 0x2002DCC8
contains(TARGET, javapushregistry): TARGET.UID3 = 0x2002DCC9
contains(TARGET, javaremconobserver): TARGET.UID3 = 0x2002DCCA
contains(TARGET, javarms): TARGET.UID3 = 0x2002DCCB
contains(TARGET, javaruntimestarterutils): TARGET.UID3 = 0x2002DCCC
contains(TARGET, javaruntimeui): TARGET.UID3 = 0x2002DCCD
contains(TARGET, javasatsa): TARGET.UID3 = 0x2002DCCE
contains(TARGET, javasecurity): TARGET.UID3 = 0x2002DCCF
contains(TARGET, javasensor): TARGET.UID3 = 0x2002DCD0
contains(TARGET, javasizehelperclient): TARGET.UID3 = 0x2002DCD1
contains(TARGET, javasizehelperserver): TARGET.UID3 = 0x2002DCD2
contains(TARGET, javasmsscplugin): TARGET.UID3 = 0x2002DCD3
contains(TARGET, javasocket): TARGET.UID3 = 0x2002DCD4
contains(TARGET, javasocketscplugin): TARGET.UID3 = 0x2002DCD5
contains(TARGET, javasoftnotification): TARGET.UID3 = 0x2002DCD6
contains(TARGET, javassl): TARGET.UID3 = 0x2002DCD7
contains(TARGET, javastorage): TARGET.UID3 = 0x2002DCD8
contains(TARGET, javathreaddumper): TARGET.UID3 = 0x2002DCD9
contains(TARGET, javautils): TARGET.UID3 = 0x2002DCDA
contains(TARGET, javawebservices): TARGET.UID3 = 0x2002DCDB
contains(TARGET, javawma): TARGET.UID3 = 0x2002DCDC
contains(TARGET, javawmamms): TARGET.UID3 = 0x2002DCDD
contains(TARGET, jvmnativeport): TARGET.UID3 = 0x2002DCDE  # On >= 9.2 only
contains(TARGET, javaapplicationsettingsview): TARGET.UID3 = 0x2002DCDF
contains(TARGET, javabroadcast): TARGET.UID3 = 0x2002DCE0
contains(TARGET, javastarter): TARGET.UID3 = 0x2002E6A7

contains(TARGET, openlcdui): TARGET.UID3 = 0x2002DCE2
contains(TARGET, tckrunner): TARGET.UID3 = 0x2002DCE3
contains(TARGET, javacaptain_ext_settingslistener): TARGET.UID3 = 0x200315D8
contains(TARGET, javacaptain_ext_scrupdater): TARGET.UID3 = 0x2002DD10
contains(TARGET, javacaptain_ext_ondemand_2): TARGET.UID3 = 0x2002DD01
contains(TARGET, javanokiaui): TARGET.UID3 = 0x2002E697
contains(TARGET, javacaptain_ext_autostarter): TARGET.UID3 = 0x2002EA7E
contains(TARGET, javacontrolpanel): TARGET.UID3 = 0x2002FF62
contains(TARGET, eswtqtwebkit): TARGET.UID3 = 0x2002FFA9
contains(TARGET, javaopengles): TARGET.UID3 = 0x20031DD1
contains(TARGET, javacentrep): TARGET.UID3 = 0x2002DCE1
contains(TARGET, javachapi): TARGET.UID3 = 0x2002DCE4
contains(TARGET, javaccapi): TARGET.UID3 = 0x200345B9
contains(TARGET, javaccapiext): TARGET.UID3 = 0x200345BA
contains(TARGET, javandefscplugin): TARGET.UID3 = 0x200345BB
contains(TARGET, javasoftindicatorplugin): TARGET.UID3 = 0x20031E8E
contains(TARGET, javacaptain_ext_javasoftnoteplugin): TARGET.UID3 = 0x20034596

isEmpty(TARGET.UID3): error($${TEMPLATE} $${TARGET} : UID3 not defined. Please allocate UID3 and configure it either on $${TARGET}.pro file or in command symbian_uids.pri)

}

#else: message($${TEMPLATE} $${TARGET} UID = $${TARGET.UID3})
#contains(TEMPLATE,lib):isEmpty(TARGET.UID3) {
#isEmpty(TARGET.UID3): message($${TEMPLATE} $${TARGET} : UID3 not defined!)
#contains(TARGET.UID3,0x2000CFEC):message($${TEMPLATE} $${TARGET} UID = $${TARGET.UID3})
#}
