/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


package com.nokia.mj.impl.security.midp.storage;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.security.midp.storage.SecurityStorage;
import com.nokia.mj.impl.security.midp.common.PolicyBasedPermission;
import com.nokia.mj.impl.security.midp.common.PolicyBasedPermissionImpl;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettings;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettingsImpl;
import com.nokia.mj.impl.security.midp.common.AuthorizationRestriction;
import com.nokia.mj.impl.security.midp.common.AuthorizationRestrictionAttribute;
import com.nokia.mj.impl.security.midp.common.AuthenticationAttribute;
import com.nokia.mj.impl.security.midp.common.SecurityAttributes;
import com.nokia.mj.impl.security.midp.common.GeneralSecuritySettings;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.storage.StorageFactory;
import com.nokia.mj.impl.storage.StorageSession;
import com.nokia.mj.impl.storage.StorageException;
import com.nokia.mj.impl.storage.StorageNames;
import com.nokia.mj.impl.storage.StorageAttribute;
import com.nokia.mj.impl.storage.StorageEntry;
import java.util.Vector;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * SecurityDescriptorAttributes unit tests.
 */
public class SecurityStorageTests extends TestCase implements InstallerMain
{

    // data being tested
    private SecurityStorage storage = null;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new SecurityStorageTests("testOpenAndClose", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SecurityStorageTests)tc).testOpenAndClose();
            }
        }));
        suite.addTest(new SecurityStorageTests("testGrantedPermissions", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SecurityStorageTests)tc).testGrantedPermissions();
            }
        }));
        suite.addTest(new SecurityStorageTests("testAuthenticationData", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SecurityStorageTests)tc).testAuthenticationData();
            }
        }));
        suite.addTest(new SecurityStorageTests("testUserSettingsData", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SecurityStorageTests)tc).testUserSettingsData();
            }
        }));
        suite.addTest(new SecurityStorageTests("testAppAccessAuthorizationRestrictionsData", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SecurityStorageTests)tc).testAppAccessAuthorizationRestrictionsData();
            }
        }));
        suite.addTest(new SecurityStorageTests("testMisc", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SecurityStorageTests)tc).testMisc();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public SecurityStorageTests()
    {
    }

    public SecurityStorageTests(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    // End j2meunit test framework setup

    protected void setUp()
    {
        storage = new SecurityStorage();
    }

    protected void tearDown()
    {
        storage.close();
    }

    protected void testOpenAndClose()
    {
        // another session
        StorageSession session = StorageFactory.createSession();
        storage = new SecurityStorage(session);
        assertTrue(storage != null);
        storage = new SecurityStorage();
        assertTrue(storage != null);
    }

    protected void testGrantedPermissions()
    {
        Vector readPermissions = null;
        Vector grantedPermissions = null;
        Vector grantedPermissions2 = null;
        Uid appUID = Uid.createUid("myUID");
        Uid appUID2 = Uid.createUid("myUID2");
        // null values
        storage.readGrantedPermissions(null);
        storage.writeGrantedPermissions(null, null, null);
        storage.removeGrantedPermissions(null);
        assertTrue(true);
        // full read/write/remove cycle with default values for current interaction mode
        storage.removeGrantedPermissions(appUID);
        grantedPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(grantedPermissions == null);
        grantedPermissions = new Vector();
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        storage.removeGrantedPermissions(appUID);
        assertTrue(true);
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "permission1",
                                          "target1",
                                          "action1",
                                          PolicyBasedPermission.ALLOWED_TYPE));
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "permission2",
                                          "target2",
                                          "action2",
                                          PolicyBasedPermission.ASSIGNED_TYPE));
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "permission3",
                                          "target3",
                                          "action3",
                                          new UserSecuritySettingsImpl("settingsname",
                                                                       UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                       new int[] { UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                   UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                                   UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                                   UserSecuritySettings.NO_INTERACTION_MODE
                                                                                 })));
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        readPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(readPermissions != null
                   && readPermissions.size() == grantedPermissions.size()
                   && checkGrantedPermissions(readPermissions, grantedPermissions));
        storage.removeGrantedPermissions(appUID);
        readPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(readPermissions == null);
        // full read/write/remove cycle with different valid values for permissions and user settings
        storage.removeGrantedPermissions(appUID);
        grantedPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(grantedPermissions == null);
        grantedPermissions = new Vector();
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        storage.removeGrantedPermissions(appUID);
        assertTrue(true);
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "permission1",
                                          "target1",
                                          "",
                                          PolicyBasedPermission.ALLOWED_TYPE));
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "permission2",
                                          "",
                                          null,
                                          PolicyBasedPermission.ASSIGNED_TYPE));
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "permission3",
                                          "target3",
                                          null,
                                          new UserSecuritySettingsImpl("settingsname",
                                                                       UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                       null)));
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        readPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(readPermissions != null
                   && readPermissions.size() == grantedPermissions.size()
                   && checkGrantedPermissions(readPermissions, grantedPermissions));
        storage.removeGrantedPermissions(appUID);
        readPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(readPermissions == null);
        // full read/write/remove cycle with same permission settings
        storage.removeGrantedPermissions(appUID);
        grantedPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(grantedPermissions == null);
        grantedPermissions = new Vector();
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        storage.removeGrantedPermissions(appUID);
        assertTrue(true);
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "permission1",
                                          "target1",
                                          "",
                                          new UserSecuritySettingsImpl("settingsname",
                                                                       UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                       null)));
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "permission3",
                                          "target3",
                                          null,
                                          new UserSecuritySettingsImpl("settingsname",
                                                                       UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                       null)));
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        readPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(readPermissions != null
                   && readPermissions.size() == grantedPermissions.size()
                   && checkGrantedPermissions(readPermissions, grantedPermissions));
        storage.removeGrantedPermissions(appUID);
        readPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(readPermissions == null);
        // full read/write/remove cycle with both valid & invalid values for user settings
        storage.removeGrantedPermissions(appUID);
        grantedPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(grantedPermissions == null);
        grantedPermissions = new Vector();
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        storage.removeGrantedPermissions(appUID);
        assertTrue(true);
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "permission1",
                                          "target1",
                                          "actionList1",
                                          new UserSecuritySettingsImpl(UserSecuritySettings.PHONE_CALL_SETTINGS,
                                                                       100,
                                                                       new int[] {UserSecuritySettings.ONESHOT_INTERACTION_MODE, 2000,
                                                                                  UserSecuritySettings.SESSION_INTERACTION_MODE
                                                                                 })));
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "permission2",
                                          "target2",
                                          "actionList2",
                                          new UserSecuritySettingsImpl(UserSecuritySettings.PHONE_CALL_SETTINGS,
                                                                       UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                       new int[] {UserSecuritySettings.BLANKET_INTERACTION_MODE})));
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "permission3",
                                          "target3",
                                          "",
                                          new UserSecuritySettingsImpl(UserSecuritySettings.NET_ACCESS_SETTINGS,
                                                                       UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                       new int[] {2000})));
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        readPermissions = storage.readGrantedPermissions(appUID);
        // grantedPermissions with all the unknown values filtered out, NO as available interaction mode and NO as current interaction mode where the interaction mode was unknown
        Vector expectedPermissions = new Vector();
        expectedPermissions.addElement(new PolicyBasedPermissionImpl(
                                           "permission1",
                                           "target1",
                                           "actionList1",
                                           new UserSecuritySettingsImpl(UserSecuritySettings.PHONE_CALL_SETTINGS,
                                                                        UserSecuritySettings.NO_INTERACTION_MODE,
                                                                        new int[] {UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                   UserSecuritySettings.SESSION_INTERACTION_MODE, UserSecuritySettings.NO_INTERACTION_MODE
                                                                                  })));
        expectedPermissions.addElement(new PolicyBasedPermissionImpl(
                                           "permission2",
                                           "target2",
                                           "actionList2",
                                           new UserSecuritySettingsImpl(UserSecuritySettings.PHONE_CALL_SETTINGS,
                                                                        UserSecuritySettings.NO_INTERACTION_MODE,
                                                                        new int[] {UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                   UserSecuritySettings.SESSION_INTERACTION_MODE, UserSecuritySettings.NO_INTERACTION_MODE
                                                                                  })));
        expectedPermissions.addElement(new PolicyBasedPermissionImpl(
                                           "permission3",
                                           "target3",
                                           "",
                                           new UserSecuritySettingsImpl(UserSecuritySettings.NET_ACCESS_SETTINGS,
                                                                        UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                        new int[] {UserSecuritySettings.NO_INTERACTION_MODE})));
        assertTrue(readPermissions != null
                   && readPermissions.size() == expectedPermissions.size()
                   && checkGrantedPermissions(readPermissions, expectedPermissions));
        storage.removeGrantedPermissions(appUID);
        readPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(readPermissions == null);
    }

    protected void testAuthenticationData()
    {
        AuthenticationStorageData readData = null;
        AuthenticationStorageData writtenData = null;
        AuthenticationStorageData expectedData = null;
        String protectionDomain = null;
        String protectionDomainCategory = null;
        Uid appUID = Uid.createUid("myUID");
        // null values
        readData = storage.readAuthenticationStorageData(null);
        storage.writeAuthenticationStorageData(null, null);
        storage.removeAuthenticationStorageData(null);
        protectionDomain = storage.readProtectionDomain(null);
        assertTrue(readData == null && protectionDomain == null);
        readData = storage.readAuthenticationStorageData(null, null, null, 0);
        assertTrue(readData == null);
        readData = storage.readAuthenticationStorageData("name", null, null, 0);
        assertTrue(readData == null);
        readData = storage.readAuthenticationStorageData("name", "version", null, 0);
        assertTrue(readData == null);
        // full read/write/remove cycle
        storage.removeAuthenticationStorageData(appUID);
        readData = storage.readAuthenticationStorageData(appUID);
        protectionDomain = storage.readProtectionDomain(appUID);
        assertTrue(readData == null && protectionDomain == null);
        readData = storage.readAuthenticationStorageData("name", "version", "vendor", 0);
        assertTrue(readData == null);
        readData = storage.readAuthenticationStorageData("name", "version", "vendor", SecurityStorage.AUTHENTICATION_DOMAIN_NAME_QUERY);
        assertTrue(readData == null);
        readData = storage.readAuthenticationStorageData("name", "version", "vendor", SecurityStorage.AUTHENTICATION_DOMAIN_CATEGORY_QUERY);
        assertTrue(readData == null);
        readData = storage.readAuthenticationStorageData("name", "version", "vendor", SecurityStorage.AUTHENTICATION_ROOT_HASH_QUERY);
        assertTrue(readData == null);
        readData = storage.readAuthenticationStorageData("name", "version", "vendor", SecurityStorage.AUTHENTICATION_JAR_HASH_QUERY);
        assertTrue(readData == null);
        readData = storage.readAuthenticationStorageData("name", "version", "vendor", SecurityStorage.AUTHENTICATION_VALID_CERTS_QUERY);
        assertTrue(readData == null);
        readData = storage.readAuthenticationStorageData("name", "version", "vendor",
                   SecurityStorage.AUTHENTICATION_DOMAIN_NAME_QUERY | SecurityStorage.AUTHENTICATION_DOMAIN_CATEGORY_QUERY
                   | SecurityStorage.AUTHENTICATION_ROOT_HASH_QUERY | SecurityStorage.AUTHENTICATION_JAR_HASH_QUERY
                   | SecurityStorage.AUTHENTICATION_VALID_CERTS_QUERY);
        assertTrue(readData == null);
        // no values at all -> protection domain is mandatory
        writtenData = new AuthenticationStorageData(null, null, null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, writtenData);
        readData = storage.readAuthenticationStorageData(appUID);
        protectionDomain = storage.readProtectionDomain(appUID);
        assertTrue(readData == null && protectionDomain == null);
        storage.removeAuthenticationStorageData(appUID);
        assertTrue(true);
        // all possible combinations of authentication data
        writtenData = new AuthenticationStorageData("protDomain", "protDomainCategory", null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, writtenData);
        readData = storage.readAuthenticationStorageData(appUID);
        storage.removeAuthenticationStorageData(appUID);
        assertTrue(checkAuthenticationStorageData(writtenData,readData));
        Vector validatedChainIndexes = new Vector();
        writtenData = new AuthenticationStorageData("protDomain", "protDomainCategory", "", "", validatedChainIndexes);
        expectedData = new AuthenticationStorageData("protDomain", "protDomainCategory", "jarHash", "certHash", null, null);
        storage.writeAuthenticationStorageData(appUID, writtenData);
        readData = storage.readAuthenticationStorageData(appUID);
        protectionDomain = storage.readProtectionDomain(appUID);
        protectionDomainCategory = storage.readProtectionDomainCategory(appUID);
        storage.removeAuthenticationStorageData(appUID);
        assertTrue(checkAuthenticationStorageData(expectedData,readData) && protectionDomain.equals("protDomain") && protectionDomainCategory.equals("protDomainCategory"));
        validatedChainIndexes = new Vector();
        validatedChainIndexes.addElement(new Integer(1));
        validatedChainIndexes.addElement(new Integer(2));
        validatedChainIndexes.addElement(new Integer(3));
        validatedChainIndexes.addElement(new Integer(4));
        validatedChainIndexes.addElement(new Integer(5));
        writtenData = new AuthenticationStorageData("protDomain", "protDomainCategory","jarHash", "certHash", validatedChainIndexes);
        storage.writeAuthenticationStorageData(appUID, writtenData);
        readData = storage.readAuthenticationStorageData(appUID);
        storage.removeAuthenticationStorageData(appUID);
        assertTrue(checkAuthenticationStorageData(writtenData,readData));
        // duplicates inside validated chain indexes are ignored
        validatedChainIndexes = new Vector();
        validatedChainIndexes.addElement(new Integer(1));
        validatedChainIndexes.addElement(new Integer(1));
        writtenData = new AuthenticationStorageData("protDomain", "category", "jarHash", "certHash", validatedChainIndexes);
        validatedChainIndexes = new Vector();
        validatedChainIndexes.addElement(new Integer(1));
        expectedData = new AuthenticationStorageData("protDomain", "category", "jarHash", "certHash", validatedChainIndexes);
        storage.writeAuthenticationStorageData(appUID, writtenData);
        readData = storage.readAuthenticationStorageData(appUID);
        storage.removeAuthenticationStorageData(appUID);
        assertTrue(checkAuthenticationStorageData(expectedData,readData));
        // update authentication data
        validatedChainIndexes = new Vector();
        storage.removeAuthenticationStorageData(appUID);
        validatedChainIndexes.addElement(new Integer(1));
        writtenData = new AuthenticationStorageData("protDomain", "protDomainCategory","jarHash", "certHash", validatedChainIndexes);
        storage.writeAuthenticationStorageData(appUID, writtenData);
        AuthenticationStorageData updatedData = new AuthenticationStorageData("protDomainUpdate", "protDomainCategoryUpdate","jarHashUpdate", "certHashUpdate", validatedChainIndexes);
        storage.writeAuthenticationStorageData(appUID, updatedData, true);
        readData = storage.readAuthenticationStorageData(appUID);
        storage.removeAuthenticationStorageData(appUID);
        assertTrue(checkAuthenticationStorageData(updatedData,readData));
        // populate the application package table with name, vendor and version
        StorageSession session = StorageFactory.createSession();
        session.open();
        StorageEntry entry = null;
        session.remove(StorageNames.APPLICATION_PACKAGE_TABLE, appUID);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.PACKAGE_NAME,
                               "appName"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VENDOR,
                               "appVendor"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VERSION,
                               "appVersion"));
        session.write(StorageNames.APPLICATION_PACKAGE_TABLE, entry);
        // auth data is not yet available
        readData = storage.readAuthenticationStorageData("appName", "appVersion", "appVendor",
                   SecurityStorage.AUTHENTICATION_DOMAIN_NAME_QUERY | SecurityStorage.AUTHENTICATION_DOMAIN_CATEGORY_QUERY
                   | SecurityStorage.AUTHENTICATION_ROOT_HASH_QUERY | SecurityStorage.AUTHENTICATION_JAR_HASH_QUERY
                   | SecurityStorage.AUTHENTICATION_VALID_CERTS_QUERY);
        assertTrue(readData == null);
        // populate storage with authentication data
        validatedChainIndexes = new Vector();
        validatedChainIndexes.addElement(new Integer(1));
        validatedChainIndexes.addElement(new Integer(2));
        validatedChainIndexes.addElement(new Integer(3));
        validatedChainIndexes.addElement(new Integer(4));
        validatedChainIndexes.addElement(new Integer(5));
        writtenData = new AuthenticationStorageData("protDomainName", "protDomainCategory","jarHash", "certHash", validatedChainIndexes);
        storage.writeAuthenticationStorageData(appUID, writtenData);
        readData = storage.readAuthenticationStorageData("appName", "appVersion", "appVendor", 0);
        assertTrue(readData == null);
        readData = storage.readAuthenticationStorageData("appName", "appVersion", "appVendor", SecurityStorage.AUTHENTICATION_DOMAIN_NAME_QUERY);
        assertTrue(readData != null
                   && readData.getProtectionDomain() != null && readData.getProtectionDomain().equals("protDomainName")
                   && readData.getProtectionDomainCategory() == null
                   && readData.getJarHashValue() == null
                   && readData.getRootHashValue() == null
                   && readData.getValidatedChainIndexes() == null);
        readData = storage.readAuthenticationStorageData("appName", "appVersion", "appVendor", SecurityStorage.AUTHENTICATION_DOMAIN_CATEGORY_QUERY);
        assertTrue(readData != null
                   && readData.getProtectionDomain() == null
                   && readData.getProtectionDomainCategory() != null && readData.getProtectionDomainCategory().equals("protDomainCategory")
                   && readData.getJarHashValue() == null
                   && readData.getRootHashValue() == null
                   && readData.getValidatedChainIndexes() == null);
        readData = storage.readAuthenticationStorageData("appName", "appVersion", "appVendor", SecurityStorage.AUTHENTICATION_JAR_HASH_QUERY);
        assertTrue(readData != null
                   && readData.getProtectionDomain() == null
                   && readData.getProtectionDomainCategory() == null
                   && readData.getJarHashValue() != null && readData.getJarHashValue().equals("jarHash")
                   && readData.getRootHashValue() == null
                   && readData.getValidatedChainIndexes() == null);
        readData = storage.readAuthenticationStorageData("appName", "appVersion", "appVendor", SecurityStorage.AUTHENTICATION_ROOT_HASH_QUERY);
        assertTrue(readData != null
                   && readData.getProtectionDomain() == null
                   && readData.getProtectionDomainCategory() == null
                   && readData.getJarHashValue() == null
                   && readData.getRootHashValue() != null && readData.getRootHashValue().equals("certHash")
                   && readData.getValidatedChainIndexes() == null);
        readData = storage.readAuthenticationStorageData("appName", "appVersion", "appVendor", SecurityStorage.AUTHENTICATION_VALID_CERTS_QUERY);
        assertTrue(readData != null
                   && readData.getProtectionDomain() == null
                   && readData.getProtectionDomainCategory() == null
                   && readData.getJarHashValue() == null
                   && readData.getRootHashValue() == null
                   && readData.getValidatedChainIndexes() != null && readData.getValidatedChainIndexes().length == 5);
        readData = storage.readAuthenticationStorageData("appName", "appVersion", "appVendor",
                   SecurityStorage.AUTHENTICATION_DOMAIN_NAME_QUERY | SecurityStorage.AUTHENTICATION_DOMAIN_CATEGORY_QUERY
                   | SecurityStorage.AUTHENTICATION_ROOT_HASH_QUERY | SecurityStorage.AUTHENTICATION_JAR_HASH_QUERY
                   | SecurityStorage.AUTHENTICATION_VALID_CERTS_QUERY);
        assertTrue(readData != null
                   && readData.getProtectionDomain() != null && readData.getProtectionDomain().equals("protDomainName")
                   && readData.getProtectionDomainCategory() != null && readData.getProtectionDomainCategory().equals("protDomainCategory")
                   && readData.getJarHashValue() != null && readData.getJarHashValue().equals("jarHash")
                   && readData.getRootHashValue() != null && readData.getRootHashValue().equals("certHash")
                   && readData.getValidatedChainIndexes() != null && readData.getValidatedChainIndexes().length == 5);
        session.remove(StorageNames.APPLICATION_PACKAGE_TABLE, appUID);
    }

    protected void testUserSettingsData()
    {
        Uid appUID = Uid.createUid("myUID");
        Uid appUID2 = Uid.createUid("myUID2");
        UserSecuritySettings readSettings = null;
        UserSecuritySettings[] multipleSettings = null;
        boolean blanketPromptShown = false;
        // update settings which are not present into the storage
        storage.writeUserSecuritySettings(appUID, "settingsname",UserSecuritySettings.SESSION_INTERACTION_MODE);
        readSettings = storage.readUserSecuritySettings(appUID, "settingsname");
        multipleSettings = storage.readUserSecuritySettings(appUID);
        blanketPromptShown = storage.readUserSecuritySettingsPromptFlag(appUID, "settingsname");
        assertTrue(readSettings == null && !blanketPromptShown && multipleSettings == null);
        // populate storage with some data
        Vector perms = new Vector();
        perms.addElement(new PolicyBasedPermissionImpl(
                             "permission",
                             "target",
                             "actionList",
                             new UserSecuritySettingsImpl("settingsname",
                                                          UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                          new int[] {UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                     UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                     UserSecuritySettings.BLANKET_INTERACTION_MODE
                                                                    })));
        storage.writeGrantedPermissions(appUID, null, perms);
        // change the current interaction mode to different values and check the update succeeds
        storage.writeUserSecuritySettings(appUID, "settingsname",UserSecuritySettings.SESSION_INTERACTION_MODE);
        readSettings = storage.readUserSecuritySettings(appUID, "settingsname");
        multipleSettings = storage.readUserSecuritySettings(appUID);
        assertTrue(readSettings != null
                   && readSettings.getCurrentInteractionMode() == UserSecuritySettings.SESSION_INTERACTION_MODE
                   && multipleSettings != null
                   && multipleSettings.length == 1
                   && multipleSettings[0].getCurrentInteractionMode() == UserSecuritySettings.SESSION_INTERACTION_MODE);
        storage.writeUserSecuritySettings(appUID, "settingsname",UserSecuritySettings.ONESHOT_INTERACTION_MODE);
        readSettings = storage.readUserSecuritySettings(appUID, "settingsname");
        multipleSettings = storage.readUserSecuritySettings(appUID);
        assertTrue(readSettings != null
                   && readSettings.getCurrentInteractionMode() == UserSecuritySettings.ONESHOT_INTERACTION_MODE
                   && multipleSettings != null
                   && multipleSettings.length == 1
                   && multipleSettings[0].getCurrentInteractionMode() == UserSecuritySettings.ONESHOT_INTERACTION_MODE);
        storage.writeUserSecuritySettings(appUID, "settingsname",UserSecuritySettings.NO_INTERACTION_MODE);
        readSettings = storage.readUserSecuritySettings(appUID, "settingsname");
        multipleSettings = storage.readUserSecuritySettings(appUID);
        assertTrue(readSettings != null
                   && readSettings.getCurrentInteractionMode() == UserSecuritySettings.NO_INTERACTION_MODE
                   && multipleSettings != null
                   && multipleSettings.length == 1
                   && multipleSettings[0].getCurrentInteractionMode() == UserSecuritySettings.NO_INTERACTION_MODE);
        storage.writeUserSecuritySettings(appUID, "settingsname",UserSecuritySettings.BLANKET_INTERACTION_MODE);
        readSettings = storage.readUserSecuritySettings(appUID, "settingsname");
        multipleSettings = storage.readUserSecuritySettings(appUID);
        assertTrue(readSettings != null
                   && readSettings.getCurrentInteractionMode() == UserSecuritySettings.BLANKET_INTERACTION_MODE
                   && multipleSettings != null
                   && multipleSettings.length == 1
                   && multipleSettings[0].getCurrentInteractionMode() == UserSecuritySettings.BLANKET_INTERACTION_MODE);
        // change the blanket current interaction mode to different values and check the update succeeds
        storage.writeUserSecuritySettings(appUID, "settingsname",UserSecuritySettings.SESSION_INTERACTION_MODE, true);
        blanketPromptShown = storage.readUserSecuritySettingsPromptFlag(appUID, "settingsname");
        assertTrue(blanketPromptShown);
        storage.writeUserSecuritySettings(appUID, "settingsname",UserSecuritySettings.SESSION_INTERACTION_MODE, false);
        blanketPromptShown = storage.readUserSecuritySettingsPromptFlag(appUID, "settingsname");
        assertTrue(!blanketPromptShown);
        storage.writeUserSecuritySettingsPromptFlag(appUID, "settingsname",true);
        blanketPromptShown = storage.readUserSecuritySettingsPromptFlag(appUID, "settingsname");
        assertTrue(blanketPromptShown);
        storage.removeGrantedPermissions(appUID);
        // change ALL the settings to different values and check that the updates are done accordingly
        perms = new Vector();
        perms.addElement(new PolicyBasedPermissionImpl(
                             "permission",
                             "target",
                             "actionList",
                             new UserSecuritySettingsImpl("settingsname1",
                                                          UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                          new int[] {UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                     UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                     UserSecuritySettings.BLANKET_INTERACTION_MODE
                                                                    })));
        perms.addElement(new PolicyBasedPermissionImpl(
                             "permission",
                             "target",
                             "actionList",
                             new UserSecuritySettingsImpl("settingsname2",
                                                          UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                          new int[] {UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                     UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                     UserSecuritySettings.BLANKET_INTERACTION_MODE
                                                                    })));
        perms.addElement(new PolicyBasedPermissionImpl(
                             "permission",
                             "target",
                             "actionList",
                             new UserSecuritySettingsImpl("settingsname3",
                                                          UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                          new int[] {UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                     UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                     UserSecuritySettings.BLANKET_INTERACTION_MODE
                                                                    })));
        perms.addElement(new PolicyBasedPermissionImpl(
                             "permission",
                             "target",
                             "actionList",
                             new UserSecuritySettingsImpl("settingsname4",
                                                          UserSecuritySettings.NO_INTERACTION_MODE,
                                                          new int[] {UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                     UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                     UserSecuritySettings.BLANKET_INTERACTION_MODE
                                                                    })));
        storage.writeGrantedPermissions(appUID, null, perms);
        multipleSettings = storage.readUserSecuritySettings(appUID);
        assertTrue(multipleSettings != null
                   && multipleSettings.length == 4
                   && multipleSettings[0].getCurrentInteractionMode() == UserSecuritySettings.ONESHOT_INTERACTION_MODE
                   && (storage.readUserSecuritySettingsPromptFlag(appUID, "settingsname1") == false)
                   && multipleSettings[1].getCurrentInteractionMode() == UserSecuritySettings.SESSION_INTERACTION_MODE
                   && (storage.readUserSecuritySettingsPromptFlag(appUID, "settingsname2") == false)
                   && multipleSettings[2].getCurrentInteractionMode() == UserSecuritySettings.BLANKET_INTERACTION_MODE
                   && (storage.readUserSecuritySettingsPromptFlag(appUID, "settingsname3") == false)
                   && multipleSettings[3].getCurrentInteractionMode() == UserSecuritySettings.NO_INTERACTION_MODE
                   && (storage.readUserSecuritySettingsPromptFlag(appUID, "settingsname4") == false));
        storage.writeUserSecuritySettings(appUID2, UserSecuritySettings.ONESHOT_INTERACTION_MODE, false);
        multipleSettings = storage.readUserSecuritySettings(appUID);
        assertTrue(multipleSettings != null
                   && multipleSettings.length == 4
                   && multipleSettings[0].getCurrentInteractionMode() == UserSecuritySettings.ONESHOT_INTERACTION_MODE
                   && (storage.readUserSecuritySettingsPromptFlag(appUID, "settingsname1") == false)
                   && multipleSettings[1].getCurrentInteractionMode() == UserSecuritySettings.SESSION_INTERACTION_MODE
                   && (storage.readUserSecuritySettingsPromptFlag(appUID, "settingsname2") == false)
                   && multipleSettings[2].getCurrentInteractionMode() == UserSecuritySettings.BLANKET_INTERACTION_MODE
                   && (storage.readUserSecuritySettingsPromptFlag(appUID, "settingsname3") == false)
                   && multipleSettings[3].getCurrentInteractionMode() == UserSecuritySettings.NO_INTERACTION_MODE
                   && (storage.readUserSecuritySettingsPromptFlag(appUID, "settingsname4") == false));
        storage.writeUserSecuritySettings(appUID, UserSecuritySettings.ONESHOT_INTERACTION_MODE, false);
        multipleSettings = storage.readUserSecuritySettings(appUID);
        assertTrue(multipleSettings != null
                   && multipleSettings.length == 4
                   && multipleSettings[0].getCurrentInteractionMode() == UserSecuritySettings.ONESHOT_INTERACTION_MODE
                   && (storage.readUserSecuritySettingsPromptFlag(appUID, "settingsname1") == false)
                   && multipleSettings[1].getCurrentInteractionMode() == UserSecuritySettings.ONESHOT_INTERACTION_MODE
                   && (storage.readUserSecuritySettingsPromptFlag(appUID, "settingsname2") == false)
                   && multipleSettings[2].getCurrentInteractionMode() == UserSecuritySettings.ONESHOT_INTERACTION_MODE
                   && (storage.readUserSecuritySettingsPromptFlag(appUID, "settingsname3") == false)
                   && multipleSettings[3].getCurrentInteractionMode() == UserSecuritySettings.ONESHOT_INTERACTION_MODE
                   && (storage.readUserSecuritySettingsPromptFlag(appUID, "settingsname4") == false));
        storage.writeUserSecuritySettings(appUID, UserSecuritySettings.BLANKET_INTERACTION_MODE, true);
        multipleSettings = storage.readUserSecuritySettings(appUID);
        assertTrue(multipleSettings != null
                   && multipleSettings.length == 4
                   && multipleSettings[0].getCurrentInteractionMode() == UserSecuritySettings.BLANKET_INTERACTION_MODE
                   && (storage.readUserSecuritySettingsPromptFlag(appUID, "settingsname1") == true)
                   && multipleSettings[1].getCurrentInteractionMode() == UserSecuritySettings.BLANKET_INTERACTION_MODE
                   && (storage.readUserSecuritySettingsPromptFlag(appUID, "settingsname2") == true)
                   && multipleSettings[2].getCurrentInteractionMode() == UserSecuritySettings.BLANKET_INTERACTION_MODE
                   && (storage.readUserSecuritySettingsPromptFlag(appUID, "settingsname3") == true)
                   && multipleSettings[3].getCurrentInteractionMode() == UserSecuritySettings.BLANKET_INTERACTION_MODE
                   && (storage.readUserSecuritySettingsPromptFlag(appUID, "settingsname4") == true));
        storage.removeGrantedPermissions(appUID);
    }

    protected void testAppAccessAuthorizationRestrictionsData()
    {
        Uid appUID = Uid.createUid("myUID");
        AuthorizationRestriction[] readRestrictions = null;
        StorageSession session = StorageFactory.createSession();
        session.open();
        StorageEntry entry = null;
        AppAccessAuthorizationStorageData appAccessAuthData = null;
        // null values
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData(null, null, null, 0);
        assertTrue(appAccessAuthData == null);
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData("name", null, null, 0);
        assertTrue(appAccessAuthData == null);
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData("name", "version", null, 0);
        assertTrue(appAccessAuthData == null);
        // no restrictions present
        readRestrictions = storage.readAuthorizationRestrictions(null);
        assertTrue(readRestrictions == null);
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appUID);
        readRestrictions = storage.readAuthorizationRestrictions(appUID);
        assertTrue(readRestrictions == null);
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData("name", "version","vendor", 0);
        assertTrue(appAccessAuthData == null);
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData("name", "version","vendor", SecurityStorage.APP_ACCESS_AUTH_DOMAIN_QUERY);
        assertTrue(appAccessAuthData == null);
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData("name", "version","vendor", SecurityStorage.APP_ACCESS_AUTH_VENDOR_QUERY);
        assertTrue(appAccessAuthData == null);
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData("name", "version","vendor", SecurityStorage.APP_ACCESS_AUTH_SIGNERS_LIST_QUERY);
        assertTrue(appAccessAuthData == null);
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData("name", "version","vendor",
                            SecurityStorage.APP_ACCESS_AUTH_DOMAIN_QUERY
                            | SecurityStorage.APP_ACCESS_AUTH_VENDOR_QUERY
                            | SecurityStorage.APP_ACCESS_AUTH_SIGNERS_LIST_QUERY);
        assertTrue(appAccessAuthData == null);
        // populate the APPLICATION_PACKAGE_ATTRIBUTES_TABLE with only restriction without certificate
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.NAME,
                               AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VALUE,
                               "domain=SELF;vendor=ANY;signer=ANY"));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        readRestrictions = storage.readAuthorizationRestrictions(appUID);
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        assertTrue(readRestrictions != null
                   && readRestrictions.length == 1
                   && readRestrictions[0].getDomain().equals("SELF")
                   && readRestrictions[0].getVendor().equals("ANY")
                   && readRestrictions[0].getSigner().equals("ANY"));
        // populate the APPLICATION_PACKAGE_ATTRIBUTES_TABLE with only certificate but not a corresponding restriction
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.NAME,
                               AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VALUE,
                               "cert"));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        readRestrictions = storage.readAuthorizationRestrictions(appUID);
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        assertTrue(readRestrictions == null);
        // populate the APPLICATION_PACKAGE_ATTRIBUTES_TABLE with 2 restrictions and only 1 certificate -> 2 pairs retrieved
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.NAME,
                               AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VALUE,
                               "domain=SELF;vendor=ANY;signer=alias"));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.NAME,
                               AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "2"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VALUE,
                               "domain=ANY;vendor=vendor;signer=ANY"));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.NAME,
                               AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VALUE,
                               "alias cert"));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        readRestrictions = storage.readAuthorizationRestrictions(appUID);
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        assertTrue(readRestrictions != null
                   && readRestrictions.length == 2
                   && readRestrictions[0].getDomain().equals("SELF")
                   && readRestrictions[0].getVendor().equals("ANY")
                   && readRestrictions[0].getSigner().equals("cert")
                   && readRestrictions[1].getDomain().equals("ANY")
                   && readRestrictions[1].getVendor().equals("vendor")
                   && readRestrictions[1].getSigner().equals("ANY"));
        // populate the APPLICATION_PACKAGE_ATTRIBUTES_TABLE with 2 restrictions and only 2 certificate -> 2 pairs retrieved
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appUID);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.NAME,
                               AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VALUE,
                               "domain=SELF;vendor=vendor1;signer=alias1"));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.NAME,
                               AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "2"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VALUE,
                               "domain=ANY;vendor=vendor2;signer=alias2"));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.NAME,
                               AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VALUE,
                               "alias1 cert1"));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.NAME,
                               AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "2"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VALUE,
                               "alias2 cert2"));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        readRestrictions = storage.readAuthorizationRestrictions(appUID);
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appUID);
        assertTrue(readRestrictions != null
                   && readRestrictions.length == 2
                   && readRestrictions[0].getDomain().equals("SELF")
                   && readRestrictions[0].getVendor().equals("vendor1")
                   && readRestrictions[0].getSigner().equals("cert1")
                   && readRestrictions[1].getDomain().equals("ANY")
                   && readRestrictions[1].getVendor().equals("vendor2")
                   && readRestrictions[1].getSigner().equals("cert2"));
        // no app access authorization data at all
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData(null);
        assertTrue(appAccessAuthData == null);
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appUID);
        session.remove(StorageNames.APPLICATION_PACKAGE_TABLE, appUID);
        session.remove(StorageNames.MIDP_PACKAGE_TABLE, appUID);
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData(appUID);
        assertTrue(appAccessAuthData == null);
        // populate APPLICATION_PACKAGE_TABLE with VENDOR
        session.remove(StorageNames.APPLICATION_PACKAGE_TABLE, appUID);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VENDOR,
                               "vendor"));
        session.write(StorageNames.APPLICATION_PACKAGE_TABLE, entry);
        // populate MIDP_PACKAGE_TABLE with domain and valid certs
        session.remove(StorageNames.MIDP_PACKAGE_TABLE, appUID);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.SECURITY_DOMAIN,
                               "domain"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VALID_CERTS,
                               "1,2,3"));
        session.write(StorageNames.MIDP_PACKAGE_TABLE, entry);
        // populate APPLICATION_PACKAGE_ATTRIBUTES_TABLE with restrictions
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appUID);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.NAME,
                               AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VALUE,
                               "cert1"));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        // request only vendor
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData(appUID, SecurityStorage.APP_ACCESS_AUTH_VENDOR_QUERY);
        assertTrue(appAccessAuthData != null &&
                   appAccessAuthData.getVendorName().equals("vendor") &&
                   appAccessAuthData.getDomainName()== null &&
                   appAccessAuthData.getSignersList() == null);
        // request only domain
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData(appUID, SecurityStorage.APP_ACCESS_AUTH_DOMAIN_QUERY);
        assertTrue(appAccessAuthData != null &&
                   appAccessAuthData.getVendorName() == null &&
                   appAccessAuthData.getDomainName().equals("domain") &&
                   appAccessAuthData.getSignersList() == null);
        // request only signers list
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData(appUID, SecurityStorage.APP_ACCESS_AUTH_SIGNERS_LIST_QUERY);
        assertTrue(appAccessAuthData != null &&
                   appAccessAuthData.getVendorName() == null &&
                   appAccessAuthData.getDomainName() == null &&
                   appAccessAuthData.getSignersList() != null &&
                   appAccessAuthData.getSignersList().length == 1 &&
                   appAccessAuthData.getSignersList()[0].equals("cert1"));
        // request vendor and domain
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData(appUID, SecurityStorage.APP_ACCESS_AUTH_VENDOR_QUERY | SecurityStorage.APP_ACCESS_AUTH_DOMAIN_QUERY);
        assertTrue(appAccessAuthData != null &&
                   appAccessAuthData.getVendorName().equals("vendor") &&
                   appAccessAuthData.getDomainName().equals("domain") &&
                   appAccessAuthData.getSignersList() == null);
        // request vendor and signers
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData(appUID, SecurityStorage.APP_ACCESS_AUTH_VENDOR_QUERY | SecurityStorage.APP_ACCESS_AUTH_SIGNERS_LIST_QUERY);
        assertTrue(appAccessAuthData != null &&
                   appAccessAuthData.getVendorName().equals("vendor") &&
                   appAccessAuthData.getDomainName() == null &&
                   appAccessAuthData.getSignersList() != null &&
                   appAccessAuthData.getSignersList().length == 1 &&
                   appAccessAuthData.getSignersList()[0].equals("cert1"));
        // request domain and signers
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData(appUID, SecurityStorage.APP_ACCESS_AUTH_DOMAIN_QUERY | SecurityStorage.APP_ACCESS_AUTH_SIGNERS_LIST_QUERY);
        assertTrue(appAccessAuthData != null &&
                   appAccessAuthData.getVendorName() == null &&
                   appAccessAuthData.getDomainName().equals("domain") &&
                   appAccessAuthData.getSignersList() != null &&
                   appAccessAuthData.getSignersList().length == 1 &&
                   appAccessAuthData.getSignersList()[0].equals("cert1"));
        // request everything
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData(appUID);
        assertTrue(appAccessAuthData != null &&
                   appAccessAuthData.getVendorName().equals("vendor") &&
                   appAccessAuthData.getDomainName().equals("domain") &&
                   appAccessAuthData.getSignersList() != null &&
                   appAccessAuthData.getSignersList().length == 1 &&
                   appAccessAuthData.getSignersList()[0].equals("cert1"));
        // remove data from MIDP_PACKAGE_TABLE and request signers list
        session.remove(StorageNames.MIDP_PACKAGE_TABLE, appUID);
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData(appUID, SecurityStorage.APP_ACCESS_AUTH_SIGNERS_LIST_QUERY);
        assertTrue(appAccessAuthData == null);
        // populate APPLICATION_PACKAGE_ATTRIBUTES_TABLE with 3 certs, populate MIDP_PACKAGE_TABLE with valid certs only and request everything
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appUID);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.NAME,
                               AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VALUE,
                               "cert1"));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.NAME,
                               AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "2-1"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VALUE,
                               "cert2"));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.NAME,
                               AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "3-1"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VALUE,
                               "cert3"));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        session.remove(StorageNames.MIDP_PACKAGE_TABLE, appUID);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VALID_CERTS,
                               "1,3"));
        session.write(StorageNames.MIDP_PACKAGE_TABLE, entry);
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData(appUID,
                            SecurityStorage.APP_ACCESS_AUTH_SIGNERS_LIST_QUERY | SecurityStorage.APP_ACCESS_AUTH_VENDOR_QUERY | SecurityStorage.APP_ACCESS_AUTH_DOMAIN_QUERY);
        assertTrue(appAccessAuthData != null &&
                   appAccessAuthData.getVendorName().equals("vendor") &&
                   (appAccessAuthData.getDomainName() == null || appAccessAuthData.getDomainName().length() == 0) &&
                   appAccessAuthData.getSignersList() != null &&
                   appAccessAuthData.getSignersList().length == 2 &&
                   appAccessAuthData.getSignersList()[0].equals("cert1") &&
                   appAccessAuthData.getSignersList()[1].equals("cert3"));
        // play with filters and name|vendor|version combinations
        session.remove(StorageNames.APPLICATION_PACKAGE_TABLE, appUID);
        session.remove(StorageNames.MIDP_PACKAGE_TABLE, appUID);
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appUID);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.PACKAGE_NAME,
                               "appName"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VENDOR,
                               "appVendor"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VERSION,
                               "appVersion"));
        session.write(StorageNames.APPLICATION_PACKAGE_TABLE, entry);
        // auth data is not yet available
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData("appName", "appVersion", "appVendor",
                            SecurityStorage.APP_ACCESS_AUTH_DOMAIN_QUERY | SecurityStorage.APP_ACCESS_AUTH_SIGNERS_LIST_QUERY);
        assertTrue(appAccessAuthData == null);
        // populate storage with authentication data
        session.remove(StorageNames.MIDP_PACKAGE_TABLE, appUID);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.SECURITY_DOMAIN,
                               "domain"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VALID_CERTS,
                               "1"));
        session.write(StorageNames.MIDP_PACKAGE_TABLE, entry);
        // populate APPLICATION_PACKAGE_ATTRIBUTES_TABLE with restrictions
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appUID);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.NAME,
                               AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VALUE,
                               "cert1"));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData("appName", "appVersion", "appVendor", 0);
        assertTrue(appAccessAuthData == null);
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData("appName", "appVersion", "appVendor", SecurityStorage.APP_ACCESS_AUTH_DOMAIN_QUERY);
        assertTrue(appAccessAuthData != null
                   && appAccessAuthData.getDomainName() != null && appAccessAuthData.getDomainName().equals("domain")
                   && appAccessAuthData.getVendorName() == null
                   && appAccessAuthData.getSignersList() == null);
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData("appName", "appVersion", "appVendor", SecurityStorage.APP_ACCESS_AUTH_VENDOR_QUERY);
        assertTrue(appAccessAuthData != null
                   && appAccessAuthData.getDomainName() == null
                   && appAccessAuthData.getVendorName().equals("appVendor")
                   && appAccessAuthData.getSignersList() == null);
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData("appName", "appVersion", "appVendor", SecurityStorage.APP_ACCESS_AUTH_SIGNERS_LIST_QUERY);
        assertTrue(appAccessAuthData != null
                   && appAccessAuthData.getDomainName() == null
                   && appAccessAuthData.getVendorName() == null
                   && appAccessAuthData.getSignersList() != null && appAccessAuthData.getSignersList().length == 1);
        appAccessAuthData = storage.readAppAccessAuthorizationStorageData("appName", "appVersion", "appVendor",
                            SecurityStorage.APP_ACCESS_AUTH_DOMAIN_QUERY
                            | SecurityStorage.APP_ACCESS_AUTH_VENDOR_QUERY
                            | SecurityStorage.APP_ACCESS_AUTH_SIGNERS_LIST_QUERY);
        assertTrue(appAccessAuthData != null
                   && appAccessAuthData.getDomainName() != null && appAccessAuthData.getDomainName().equals("domain")
                   && appAccessAuthData.getVendorName().equals("appVendor")
                   && appAccessAuthData.getSignersList() != null && appAccessAuthData.getSignersList().length == 1);
        session.remove(StorageNames.APPLICATION_PACKAGE_TABLE, appUID);
        session.remove(StorageNames.MIDP_PACKAGE_TABLE, appUID);
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appUID);
    }

    protected void testMisc()
    {
        Uid appUID = Uid.createUid("myUID");
        StorageSession session = StorageFactory.createSession();
        session.open();
        StorageEntry entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        // non existent values
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        session.remove(StorageNames.APPLICATION_PACKAGE_TABLE, entry);
        assertTrue(storage.readSuiteVersion(appUID) == null
                   && storage.readSuiteMediaId(appUID) == null);
        // populate the tables
        entry.addAttribute(new StorageAttribute(
                               StorageNames.NAME,
                               SecurityAttributes.MIDP_VERSION_ATTRIBUTE_NAME));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VALUE,
                               "version"));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.MEDIA_ID,
                               "mediaid"));
        session.write(StorageNames.APPLICATION_PACKAGE_TABLE, entry);
        assertTrue(storage.readSuiteVersion(appUID).equals("version")
                   && storage.readSuiteMediaId(appUID).equals("mediaid"));
        // security warnings mode - read value for unknown uid -> default
        session.remove(StorageNames.MIDP_PACKAGE_TABLE, appUID);
        assertTrue(storage.readSecurityWarningsMode(Uid.createUid("myUnknownUID")) == GeneralSecuritySettings.DEFAULT_SECURITY_MODE);
        // security warnings mode - read GeneralSecuritySettings.DEFAULT_SECURITY_MODE
        session.remove(StorageNames.MIDP_PACKAGE_TABLE, appUID);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.SECURITY_WARNINGS,
                               "" + GeneralSecuritySettings.DEFAULT_SECURITY_MODE));
        session.write(StorageNames.MIDP_PACKAGE_TABLE, entry);
        assertTrue(storage.readSecurityWarningsMode(appUID) == GeneralSecuritySettings.DEFAULT_SECURITY_MODE);
        // security warnings mode - read GeneralSecuritySettings.USER_SECURITY_MODE
        session.remove(StorageNames.MIDP_PACKAGE_TABLE, appUID);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.SECURITY_WARNINGS,
                               "" + GeneralSecuritySettings.USER_SECURITY_MODE));
        session.write(StorageNames.MIDP_PACKAGE_TABLE, entry);
        assertTrue(storage.readSecurityWarningsMode(appUID) == GeneralSecuritySettings.USER_SECURITY_MODE);
        // security warnings mode - unknown value in storage -> return GeneralSecuritySettings.DEFAULT_SECURITY_MODE
        session.remove(StorageNames.MIDP_PACKAGE_TABLE, appUID);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.SECURITY_WARNINGS,
                               "5"));
        session.write(StorageNames.MIDP_PACKAGE_TABLE, entry);
        assertTrue(storage.readSecurityWarningsMode(appUID) == GeneralSecuritySettings.DEFAULT_SECURITY_MODE);
        // security warnings mode - invalid value in storage -> return GeneralSecuritySettings.DEFAULT_SECURITY_MODE
        session.remove(StorageNames.MIDP_PACKAGE_TABLE, appUID);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.SECURITY_WARNINGS,
                               "value"));
        session.write(StorageNames.MIDP_PACKAGE_TABLE, entry);
        assertTrue(storage.readSecurityWarningsMode(appUID) == GeneralSecuritySettings.DEFAULT_SECURITY_MODE);
        // security warnings mode - try to write invalid -> return GeneralSecuritySettings.DEFAULT_SECURITY_MODE
        session.remove(StorageNames.MIDP_PACKAGE_TABLE, appUID);
        session.remove(StorageNames.MIDP_PACKAGE_TABLE, appUID);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.SECURITY_WARNINGS,
                               "value"));
        session.write(StorageNames.MIDP_PACKAGE_TABLE, entry);
        storage.writeSecurityWarningsMode(appUID, 5);
        assertTrue(storage.readSecurityWarningsMode(appUID) == GeneralSecuritySettings.DEFAULT_SECURITY_MODE);
        // security warnings mode - write GeneralSecuritySettings.DEFAULT_SECURITY_MODE -> return GeneralSecuritySettings.DEFAULT_SECURITY_MODE
        session.remove(StorageNames.MIDP_PACKAGE_TABLE, appUID);
        session.remove(StorageNames.MIDP_PACKAGE_TABLE, appUID);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.SECURITY_WARNINGS,
                               "value"));
        session.write(StorageNames.MIDP_PACKAGE_TABLE, entry);
        storage.writeSecurityWarningsMode(appUID, GeneralSecuritySettings.DEFAULT_SECURITY_MODE);
        assertTrue(storage.readSecurityWarningsMode(appUID) == GeneralSecuritySettings.DEFAULT_SECURITY_MODE);
        // security warnings mode - write GeneralSecuritySettings.USER_SECURITY_MODE -> return GeneralSecuritySettings.USER_SECURITY_MODE
        session.remove(StorageNames.MIDP_PACKAGE_TABLE, appUID);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.SECURITY_WARNINGS,
                               "value"));
        session.write(StorageNames.MIDP_PACKAGE_TABLE, entry);
        storage.writeSecurityWarningsMode(appUID, GeneralSecuritySettings.USER_SECURITY_MODE);
        assertTrue(storage.readSecurityWarningsMode(appUID) == GeneralSecuritySettings.USER_SECURITY_MODE);
        // cleanup
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        session.remove(StorageNames.MIDP_PACKAGE_TABLE, appUID);
        session.remove(StorageNames.APPLICATION_PACKAGE_TABLE, entry);
    }


    private boolean checkAuthenticationStorageData(AuthenticationStorageData d1, AuthenticationStorageData d2)
    {
        if ((d1 == null && d2 != null)
                || (d1 != null && d2 == null))
        {
            return false;
        }
        if (d1 == null && d2 == null)
        {
            return true;
        }
        if ((d1.getProtectionDomain() == null && d2.getProtectionDomain() != null)
                || (d1.getProtectionDomain() != null && d1.equals(d2.getProtectionDomain()))
                || (d1.getJarHashValue() == null && d2.getJarHashValue() != null)
                || (d1.getJarHashValue() != null && d1.equals(d2.getJarHashValue()))
                || (d1.getRootHashValue() == null && d2.getRootHashValue() != null)
                || (d1.getRootHashValue() != null && d1.equals(d2.getRootHashValue()))
                || (d1.getProtectionDomainCategory() == null && d2.getProtectionDomainCategory() != null)
                || (d1.getProtectionDomainCategory() != null && d1.equals(d2.getProtectionDomainCategory()))
                || !arraysEqual(d1.getValidatedChainIndexes(), d2.getValidatedChainIndexes()))
        {
            return false;
        }
        return true;
    }

    private boolean checkGrantedPermissions(Vector p1, Vector p2)
    {
        boolean[] permissionsChecked = new boolean[p1.size()];
        initBoolArray(permissionsChecked);
        for (int i=0; i<p1.size(); i++)
        {
            for (int j=0; j<p2.size(); j++)
            {
                if (p1.elementAt(i).equals(p2.elementAt(j)))
                {
                    permissionsChecked[i] = true;
                    break;
                }
            }
        }
        return checkBoolArray(permissionsChecked);
    }

    private boolean arraysEqual(int[] array1, int[] array2)
    {
        if (array1 == null && array2 == null)
        {
            return true;
        }
        if ((array1 == null && array2 != null) ||
                (array1 != null && array2 == null) ||
                (array1.length != array2.length))
        {
            return false;
        }
        for (int i=0; i<array1.length; i++)
        {
            boolean found = false;
            for (int j=0; j<array2.length; j++)
            {
                if (array1[i] == array2[j])
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                return false;
            }
            return true;
        }
        return true;
    }


    private void initBoolArray(boolean[] boolArray)
    {
        for (int i=0; i<boolArray.length; i++)
        {
            boolArray[i] = false;
        }
    }

    private boolean checkBoolArray(boolean[] boolArray)
    {
        for (int i=0; i<boolArray.length; i++)
        {
            if (boolArray[i] == false)
            {
                return false;
            }
        }
        return true;
    }
}
