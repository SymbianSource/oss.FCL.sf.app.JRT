#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: 
#

import socket, sys, os
import traceback

def main():
    try:
        java_src_root = os.environ["JAVA_SRC_ROOT"]
        values = {}
        
        # init
        os.mkdir( java_src_root + "/build/sis/nouserpromptspolicy/tmp" )
        
        # generate the policies in external format
        replaceInFile(java_src_root + "/javacommons/security/data/s60_manufacturer.txt", java_src_root + "/build/sis/nouserpromptspolicy/tmp/s60_manufacturer.txt", values)
        replaceInFile(java_src_root + "/javacommons/security/data/msa_manufacturer.txt", java_src_root + "/build/sis/nouserpromptspolicy/tmp/msa_manufacturer.txt", values)
        replaceInFile(java_src_root + "/javacommons/security/data/att_manufacturer.txt", java_src_root + "/build/sis/nouserpromptspolicy/tmp/att_manufacturer.txt", values)
        values["Manufacturer"] = "Operator"
        replaceInFile(java_src_root + "/javacommons/security/data/s60_manufacturer.txt", java_src_root + "/build/sis/nouserpromptspolicy/tmp/s60_operator.txt", values)
        replaceInFile(java_src_root + "/javacommons/security/data/msa_manufacturer.txt", java_src_root + "/build/sis/nouserpromptspolicy/tmp/msa_operator.txt", values)
        replaceInFile(java_src_root + "/javacommons/security/data/att_manufacturer.txt", java_src_root + "/build/sis/nouserpromptspolicy/tmp/att_operator.txt", values)
        values["Manufacturer"] = "OperatorExtra"
        replaceInFile(java_src_root + "/javacommons/security/data/s60_manufacturer.txt", java_src_root + "/build/sis/nouserpromptspolicy/tmp/s60_operatorextra.txt", values)
        replaceInFile(java_src_root + "/javacommons/security/data/msa_manufacturer.txt", java_src_root + "/build/sis/nouserpromptspolicy/tmp/msa_operatorextra.txt", values)
        replaceInFile(java_src_root + "/javacommons/security/data/att_manufacturer.txt", java_src_root + "/build/sis/nouserpromptspolicy/tmp/att_operatorextra.txt", values)
        values["Manufacturer"] = "IdentifiedThirdParty"
        replaceInFile(java_src_root + "/javacommons/security/data/s60_manufacturer.txt", java_src_root + "/build/sis/nouserpromptspolicy/tmp/s60_trustedthirdparty.txt", values)
        replaceInFile(java_src_root + "/javacommons/security/data/msa_manufacturer.txt", java_src_root + "/build/sis/nouserpromptspolicy/tmp/msa_trustedthirdparty.txt", values)
        replaceInFile(java_src_root + "/javacommons/security/data/att_manufacturer.txt", java_src_root + "/build/sis/nouserpromptspolicy/tmp/att_trustedthirdparty.txt", values)
        values["Manufacturer"] = "UnidentifiedThirdParty"
        replaceInFile(java_src_root + "/javacommons/security/data/s60_manufacturer.txt", java_src_root + "/build/sis/nouserpromptspolicy/tmp/s60_untrusted.txt", values)
        replaceInFile(java_src_root + "/javacommons/security/data/msa_manufacturer.txt", java_src_root + "/build/sis/nouserpromptspolicy/tmp/msa_untrusted.txt", values)
        replaceInFile(java_src_root + "/javacommons/security/data/att_manufacturer.txt", java_src_root + "/build/sis/nouserpromptspolicy/tmp/att_untrusted.txt", values)
        values["All"] = "All"
        replaceInFile(java_src_root + "/javacommons/security/data/all.txt", java_src_root + "/build/sis/nouserpromptspolicy/tmp/all.txt", values)
        
        # generate the policies in internal format
        cmd = "java -cp " + java_src_root + "/javatools/javasecuritycustomization/policyeditor/bin/securitypolicyeditor.jar;" + java_src_root + "/javatools/javasecuritycustomization/policyeditor/lib/engine.jar com.nokia.mj.tools.security.midp.PolicyEditor " + java_src_root + "/build/sis/nouserpromptspolicy/tmp " + java_src_root + "/build/sis/nouserpromptspolicy"
        os.system(cmd)

        # cleanup
        remove_tree( java_src_root + "/build/sis/nouserpromptspolicy/tmp" )

    except:
        traceback.print_exc()
        sys.exit(1)


def replaceInFile(srcFile, dstFile, values={}):
    f = open(srcFile, "r")
    data = f.read()
    f.close()
    
    for key, value in values.items():
        data = data.replace("%s" % key, value)
    
    f = open(dstFile, "w")
    f.write(data)
    f.close()

def remove_tree(path):
    for i in os.listdir(path):
        elem = path + "/" + i
        if os.path.isdir(elem):
            remove_tree(elem)
        else:
            os.remove(elem)
    os.rmdir(path)


if __name__ == "__main__":
    main()
