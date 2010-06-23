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


package com.nokia.mj.tools.security.midp;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.tools.security.midp.PolicyEditor;
import com.nokia.mj.impl.security.midp.authorization.SecurityPolicy;
import com.nokia.mj.impl.security.midp.common.PolicyBasedPermission;
import com.nokia.mj.impl.security.midp.common.PolicyBasedPermissionImpl;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettings;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettingsImpl;
import com.nokia.mj.impl.security.midp.common.PermissionMappingTable;
import com.nokia.mj.impl.security.midp.common.MIDPPermission;
import com.nokia.mj.impl.utils.Tokenizer;

import java.io.FileReader;
import java.io.BufferedReader;
import java.io.File;
import java.io.ObjectInputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Vector;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * PolicyEditor unit tests.
 */
public class PolicyEditorTests extends TestCase implements InstallerMain
{

    private static String TEST_DATA_DIR;

    static
    {
        String platform = System.getProperty("os.name");
        if (platform != null && platform.equalsIgnoreCase("linux"))
        {
            TEST_DATA_DIR = System.getProperty("JAVA_BIN_ROOT") + "/securitytestdata/";
        }
        else
        {
            TEST_DATA_DIR = "C:\\java\\securitytestdata\\";
        }
    }

    private static final String SRC_DIR_PATH = TEST_DATA_DIR + "securitypolicies" + File.separator + "testinputdata"  + File.separator;
    private static final String INVALID_POLICIES_DIR_PATH = SRC_DIR_PATH + "invalidpolicies" + File.separator;
    private static final String VALID_POLICIES_DIR_PATH = SRC_DIR_PATH + "validpolicies" + File.separator;
    private static final String DEST_DIR_PATH = TEST_DATA_DIR + "securitypolicies" + File.separator + "testoutputdata" + File.separator;
    private static final String VALID_POLICIES_DEST_DIR_PATH = DEST_DIR_PATH + File.separator + "validpolicies" + File.separator;
    private static final String INVALID_POLICIES_DEST_DIR_PATH = DEST_DIR_PATH + File.separator + "invalidpolicies" + File.separator;
    private static final String VALIDATORS_PATH = VALID_POLICIES_DIR_PATH + "validators" + File.separator;

    // class to be tested
    PolicyEditor editor = null;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new PolicyEditorTests("testInvalidPolicies", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((PolicyEditorTests)tc).testInvalidPolicies();
            }
        }));
        suite.addTest(new PolicyEditorTests("testValidPolicies", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((PolicyEditorTests)tc).testValidPolicies();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public PolicyEditorTests()
    {
    }

    public PolicyEditorTests(String aTestName, TestMethod aTestMethod)
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
        editor = new PolicyEditor();
    }

    protected void tearDown()
    {
    }

    // tests for invalid policies
    private void testInvalidPolicies()
    {
        // go through the invalid files -> copy each of them into a dedicated directory
        File[] invalidPolicyFiles = new File(INVALID_POLICIES_DIR_PATH).listFiles();
        if (invalidPolicyFiles.length > 0)
        {
            for (int i=0; i<invalidPolicyFiles.length; i++)
            {
                if (invalidPolicyFiles[i].isFile())
                {
                    // make a new directory and copy the invalid policy file there
                    String policyName = invalidPolicyFiles[i].getName();
                    int index = policyName.indexOf(".");
                    if (index != -1)
                    {
                        policyName = policyName.substring(0, index);
                    }
                    File policyDestDir = new File(INVALID_POLICIES_DEST_DIR_PATH + policyName);
                    policyDestDir.mkdir();
                    copyFile(invalidPolicyFiles[i].getAbsolutePath(), policyDestDir.getAbsolutePath() + File.separator + invalidPolicyFiles[i].getName());
                    try
                    {
                        editor.main(new String[] {policyDestDir.getAbsolutePath(), INVALID_POLICIES_DEST_DIR_PATH});
                        assertTrue(invalidPolicyFiles[i].getAbsolutePath() + ": is invalid, therefore its editing should not suucceed",false);
                    }
                    catch (IllegalArgumentException e)
                    {
                        assertTrue(true);
                    }
                }
            }
        }
    }

    private void testValidPolicies()
    {
        // run PolicyEditor with valid arguments -> the serialized policies will be created
        File[] validPolicyFiles = new File(VALID_POLICIES_DIR_PATH).listFiles();
        if (validPolicyFiles.length > 0)
        {
            for (int i=0; i<validPolicyFiles.length; i++)
            {
                if (validPolicyFiles[i].isFile())
                {
                    try
                    {
                        editor.main(new String[] {VALID_POLICIES_DIR_PATH, VALID_POLICIES_DEST_DIR_PATH});
                    }
                    catch (IllegalArgumentException e)
                    {
                        assertTrue(validPolicyFiles[i].getAbsolutePath() + ": is valid, therefore its editing should suucceed",false);
                    }
                }
            }
        }
        // go through the generated policies and check them one by one
        validPolicyFiles = new File(VALID_POLICIES_DEST_DIR_PATH).listFiles();
        if (validPolicyFiles.length > 0)
        {
            for (int i=0; i<validPolicyFiles.length; i++)
            {
                if (validPolicyFiles[i].isFile())
                {
                    FileInputStream fis = null;
                    ObjectInputStream in = null;
                    try
                    {
                        fis = new FileInputStream(validPolicyFiles[i]);
                        in = new ObjectInputStream(fis);
                        SecurityPolicy policy = (SecurityPolicy)in
                                                .readObject();
                        assertTrue(checkPolicy(validPolicyFiles[i].getName(), policy));
                        in.close();
                    }
                    catch (IOException ex)
                    {
                        assertTrue(validPolicyFiles[i].getAbsolutePath() + ": is valid, therefore its editing should suucceed",false);
                    }
                    catch (ClassNotFoundException ex)
                    {
                        assertTrue(validPolicyFiles[i].getAbsolutePath() + ": is valid, therefore its editing should suucceed",false);
                    }
                }
            }
        }
        assertTrue(true);
    }

    private void copyFile(String srcPath, String destPath)
    {
        FileInputStream src = null;
        FileOutputStream dest = null;
        try
        {
            src = new FileInputStream(srcPath);
            dest = new FileOutputStream(destPath);
            byte[] data = new byte[1024];
            int len;

            while ((len = src.read(data)) != -1)
                dest.write(data, 0, len);
        }
        catch (IOException e)
        {
        }
        finally
        {
            if (src != null)
                try
                {
                    src.close();
                }
                catch (IOException ex) {}
            if (dest != null)
                try
                {
                    dest.close();
                }
                catch (IOException ex) {}
        }
    }

    private boolean checkPolicy(String policyName, SecurityPolicy policy)
    {
        // load the corresponding validator
        try
        {
            if (policy == null)
            {
                return false;
            }
            String validatorName = VALIDATORS_PATH + policyName.substring(0, policyName.length() - 4) + ".txt";
            String domain = readDomain(validatorName);
            Vector permissions = readPermissions(validatorName);
            return (domain.equals(policy.getProtectionDomain()) && checkPermissions(permissions, policy.getPermissions()));
        }
        catch (Exception e)
        {
        }
        return false;
    }

    private String readDomain(String validatorPath)
    {
        BufferedReader inputStream = null;

        try
        {
            inputStream =
                new BufferedReader(new FileReader(validatorPath));

            String l;
            while ((l = inputStream.readLine()) != null)
            {
                if (l.startsWith("domain="))
                {
                    return l.substring("domain=".length());
                }
            }
        }
        catch (Exception e)
        {
        }
        try
        {
            inputStream.close();
        }
        catch (Exception ex) {}
        return null;
    }

    private Vector readPermissions(String validatorPath)
    {
        Vector permissions = new Vector();
        BufferedReader inputStream = null;

        try
        {
            inputStream =
                new BufferedReader(new FileReader(validatorPath));

            String l;
            while ((l = inputStream.readLine()) != null)
            {
                if (!l.startsWith("domain=") && l.length() > 0)
                {
                    permissions.addElement(l);
                }
            }
        }
        catch (Exception e)
        {
        }
        try
        {
            inputStream.close();
        }
        catch (Exception ex) {}
        return permissions;
    }

    private boolean checkPermissions(Vector perms1, PolicyBasedPermission[] perms2)
    {
        if (perms1.size() != perms2.length)
        {
            return false;
        }
        boolean[] permsChecked = new boolean[perms1.size()];
        initBoolArray(permsChecked);
        for (int i=0; i<perms1.size(); i++)
        {
            String[] permTokens = Tokenizer.split((String)perms1.get(i), "?");
            int currentToken = 0;
            int type = getType(permTokens[currentToken]);
            currentToken++;
            UserSecuritySettings settings = null;
            String name = null;
            String target = null;
            String actionList = null;
            if (permTokens.length == 3)
            {
                String[] settingsProps = Tokenizer.split(permTokens[currentToken],":");
                String settingsName = settingsProps[0];
                int currentInteractionMode = getInteractionMode(settingsProps[1]);
                int[] allowedInteractionModes = null;
                if (settingsProps.length > 2)
                {
                    allowedInteractionModes = new int[settingsProps.length - 2];
                    for (int j=2; j<settingsProps.length; j++)
                    {
                        allowedInteractionModes[j-2] = getInteractionMode(settingsProps[j]);
                    }
                }
                settings = new UserSecuritySettingsImpl(settingsName, currentInteractionMode, allowedInteractionModes);
                currentToken++;
            }
            String[] permDetails = Tokenizer.split(permTokens[currentToken], ",");
            currentToken = 0;
            name = permDetails[currentToken];
            MIDPPermission mappedPerm = PermissionMappingTable.get(name);
            if (mappedPerm != null)
            {
                name = mappedPerm.getName();
            }
            currentToken++;
            if (permDetails.length > currentToken)
            {
                target = permDetails[currentToken];
                currentToken++;
            }
            if (permDetails.length > currentToken)
            {
                actionList = permDetails[currentToken];
                actionList = actionList.replace(':',',');
            }
            PolicyBasedPermissionImpl perm1 = new PolicyBasedPermissionImpl(name, target, actionList, type, settings);
            for (int j=0; j<perms2.length; j++)
            {
                PolicyBasedPermissionImpl perm2 = new PolicyBasedPermissionImpl(
                    perms2[j].getName(),
                    perms2[j].getTarget(),
                    perms2[j].getActionList(),
                    perms2[j].getType(),
                    (perms2[j].getUserSecuritySettings() != null ?
                     (new UserSecuritySettingsImpl(
                          perms2[j].getUserSecuritySettings().getName(),
                          perms2[j].getUserSecuritySettings().getCurrentInteractionMode(),
                          perms2[j].getUserSecuritySettings().getAllowedInteractionModes())) :
                     null));
                if (perms2[j].getType() == type
                        && perm1.equals(perm2))
                {
                    if (permsChecked[i] == true)
                    {
                        return false;
                    }
                    permsChecked[i] = true;
                    break;
                }
            }
        }
        return checkBoolArray(permsChecked);
    }

    private int getType(String sType)
    {
        if (sType.equalsIgnoreCase("user"))
        {
            return PolicyBasedPermission.USER_TYPE;
        }
        else if (sType.equalsIgnoreCase("allowed"))
        {
            return PolicyBasedPermission.ALLOWED_TYPE;
        }
        else if (sType.equalsIgnoreCase("assigned"))
        {
            return PolicyBasedPermission.ASSIGNED_TYPE;
        }
        else if (sType.equalsIgnoreCase("user_assigned"))
        {
            return PolicyBasedPermission.USER_ASSIGNED_TYPE;
        }
        return -1;
    }

    private int getInteractionMode(String sInteractionMode)
    {
        if (sInteractionMode.equalsIgnoreCase("no"))
        {
            return UserSecuritySettings.NO_INTERACTION_MODE;
        }
        else if (sInteractionMode.equalsIgnoreCase("oneshot"))
        {
            return UserSecuritySettings.ONESHOT_INTERACTION_MODE;
        }
        else if (sInteractionMode.equalsIgnoreCase("session"))
        {
            return UserSecuritySettings.SESSION_INTERACTION_MODE;
        }
        else if (sInteractionMode.equalsIgnoreCase("blanket"))
        {
            return UserSecuritySettings.BLANKET_INTERACTION_MODE;
        }
        return -1;
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
