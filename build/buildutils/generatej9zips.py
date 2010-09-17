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

import re, shutil, sys, os.path, traceback, zipfile
from optparse import OptionParser

class J9PackageConverter:
    def __init__(self, srcRoot, destRoot):
        self.srcRoot = srcRoot + "/j9_s60"
        self.destRoot = destRoot
        self.wastebasket = set()
        self.currentSrcRoot = None
        self.currentDestRoot = None

    def convert(self):
        # self.createPackages("weme")
        self.createPackages("wece")
        return


    def createPackages(self, delivery):
        self.currentSrcRoot = self.srcRoot + "/" + delivery

        dirToDelete = self.currentSrcRoot + "/opt"
        if os.path.exists(dirToDelete):
            shutil.rmtree(dirToDelete)

        self.currentSrcRoot += "/non-opt/epoc32/release/"

        self.currentDestRoot = self.destRoot + "/" + delivery +"/"
        if not os.path.exists(self.currentDestRoot):
            os.makedirs(self.currentDestRoot)

        self.dozip("armv592.zip", j9ArmFiles)
        self.dozip("winscw92.zip", j9WinsFiles)
        self.dozip("locale.variants.fp.zip", j9LocaleVariantFilesFp)
        self.dozip("locale.variants.cdc.zip", j9LocaleVariantFilesCdc)
        #self.doCopy(j9IncFiles, root)
        self.dozip("properties.zip", j9PropertyFiles)
        self.dozip("security.zip", j9SecurityFiles)
        self.dozip("misc.zip", j9MiscFiles)
        self.doCopy(j9FilesToCopy)
        self.doDelete(j9FilesToIgnore)
        if delivery == "wece":
            self.doCopy(j9JavaSrcFiles)
        else:
            self.doDelete(j9WemeFilesToIgnore)

    def clean(self):
        for file in self.wastebasket:
            if os.path.exists(file):
                #print "Removing file "+file
                os.remove(file)

        def visitFun(arg, dirname, names):
            for file in names:
                if os.path.isfile(dirname + "/" + file):
                    print "ERROR: Dir " + dirname + " contained file " + file
                
        os.path.walk(self.srcRoot, visitFun, None)
        return


    def dozip(self, filename, files):
        zipFileName = self.currentDestRoot + "/" + filename
        if os.path.exists(zipFileName):
            os.remove(zipFileName)
        #print zipFileName
        zipF = zipfile.ZipFile(zipFileName, "w", zipfile.ZIP_DEFLATED)
        for file in files:
            srcFile = file[0]
            dstFile =  os.path.basename(srcFile)
            if len(file) > 1:
                dstFile = file[1] + '/' + dstFile
            srcFile = self.currentSrcRoot + srcFile
            if os.path.exists(srcFile):
                #print srcFile + "-->" + dstFile
                zipF.write(srcFile, dstFile)
                self.wastebasket.add(srcFile)
            else:
                print "ERROR: Missing file " + srcFile
                raise Exception('Copy failed because file was missing')    
        zipF.close()
    
    def doDelete(self, files):
        for file in files:
            src = file[0]
            src = self.currentSrcRoot + src
            if os.path.exists(src):
                self.wastebasket.add(src)
            else:
                print "INFO: File marked to be ignored was missing " + src
    
    
    def doCopy(self, files):
        for file in files:
            src, dst = file
            src = self.currentSrcRoot + src
            if os.path.exists(src):
                if dst == None:
                    dst = os.path.basename(src)
                dst = self.currentDestRoot + "/" + dst
                directory = os.path.dirname(dst)
                if not os.path.exists(directory):
                    os.makedirs(directory)
                shutil.copy(src, dst)
                self.wastebasket.add(src)
            else:
                print "ERROR: Missing file " + src
                raise Exception('Copy failed because file was missing')    


def main():
    parser = OptionParser(
        usage = "python -u %prog [options] <src root> <dst root>",
        description = "Converts the J9 delivery to S60 format. " + \
            "The <src root> must point to dircetory containing the unzipped " + \
            "j9_s60_xxxxxx.zip file received as apart of IBM delivery " + \
            "i.e. the <src root> must contain directory j9_s60. " + \
            "By default the tool converts weme binaries and deletes the " + \
            "original files. If <src root> is omitted then the src root " + \
            "is the current directory. If <dst root> is omitted then " + \
            "it will be <src root>")

    parser.add_option("--no-delete", dest="noDel",
                      action="store_true", default=False,
                      help="Do not delete the original files")


    (opts, args) = parser.parse_args()

    if len(args) > 0:
        srcRoot = args[0]
    else:
        srcRoot = "."

    if len(args) > 1:
        destRoot = args[1]
    else:
        destRoot = root
    try:
        j9PackageConverter = J9PackageConverter(srcRoot, destRoot)
        j9PackageConverter.convert()
    except:
        print "Error during conversion!"
        traceback.print_exc()
        sys.exit(-1)

    if not opts.noDel:
        j9PackageConverter.clean()


j9ArmFiles = [
         ('armv5/urel/j9.dll',),
         ('armv5/urel/j9dbg23.dll',),
         ('armv5/urel/j9dmp23.dll',),
         ('armv5/urel/j9fdm23.dll',),
         ('armv5/urel/j9gcchk23.dll',),
         ('armv5/urel/j9jpi23.dll',),
         ('armv5/urel/j9jvmti23.dll',),
         ('armv5/urel/j9mjit23.dll',),
         ('armv5/urel/j9mjitd23.dll',),
         ('armv5/urel/j9prf23.dll',),
         ('armv5/urel/j9vmall23.dll',),
         ('armv5/urel/j9vrb23.dll',),
         ('armv5/urel/jclcdc11_23.dll',),
         ('armv5/urel/jclcldc11_23.dll',),
         ('armv5/urel/jclfoun11_23.dll',),
         ('armv5/urel/jnichk.dll',),
         ('armv5/urel/j9.dll.map',),
         ('armv5/urel/j9dbg23.dll.map',),
         ('armv5/urel/j9dmp23.dll.map',),
         ('armv5/urel/j9fdm23.dll.map',),
         ('armv5/urel/j9gcchk23.dll.map',),
         ('armv5/urel/j9jpi23.dll.map',),
         ('armv5/urel/j9jvmti23.dll.map',),
         ('armv5/urel/j9mjit23.dll.map',),
         ('armv5/urel/j9mjitd23.dll.map',),
         ('armv5/urel/j9prf23.dll.map',),
         ('armv5/urel/j9vmall23.dll.map',),
         ('armv5/urel/j9vrb23.dll.map',),
         ('armv5/urel/jclcdc11_23.dll.map',),
         ('armv5/urel/jclcldc11_23.dll.map',),
         ('armv5/urel/jclfoun11_23.dll.map',),
         ('armv5/urel/jnichk.dll.map',)
          ]

j9WinsFiles = [
         ('winscw/udeb/j9.dll',),
         ('winscw/udeb/j9dbg23.dll',),
         ('winscw/udeb/j9dmp23.dll',),
         ('winscw/udeb/j9fdm23.dll',),
         ('winscw/udeb/j9gcchk23.dll',),
         ('winscw/udeb/j9jpi23.dll',),
         ('winscw/udeb/j9jvmti23.dll',),
         ('winscw/udeb/j9mjit23.dll',),
         ('winscw/udeb/j9mjitd23.dll',),
         ('winscw/udeb/j9prf23.dll',),
         ('winscw/udeb/j9vmall23.dll',),
         ('winscw/udeb/j9vrb23.dll',),
         ('winscw/udeb/jclcdc11_23.dll',),
         ('winscw/udeb/jclcldc11_23.dll',),
         ('winscw/udeb/jclfoun11_23.dll',),
         ('winscw/udeb/jnichk.dll',)
          ]

j9IncFiles = [
         ('inc/j9cfg.h','inc/'),
         ('inc/j9comp.h','inc/'),
         ('inc/jni.h','inc/'),
         ('inc/jniport.h','inc/'),
         ('inc/jvmpi.h','inc/'),
         ('inc/jvmri.h','inc/')
          ]

j9LocaleVariantFilesFp = [
         ('winscw/udeb/z/resource/ive/lib/jclFoundation11/locale-variants/locale_var1.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclFoundation11/locale-variants/locale_var2.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclFoundation11/locale-variants/locale_var3.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclFoundation11/locale-variants/locale_var4.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclFoundation11/locale-variants/locale_var5.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclFoundation11/locale-variants/locale_var6.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclFoundation11/locale-variants/locale_var7.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclFoundation11/locale-variants/locale_var8.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclFoundation11/locale-variants/locale_var9.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclFoundation11/locale-variants/locale_var10.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclFoundation11/locale-variants/locale_var11.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclFoundation11/locale-variants/locale_var12.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclFoundation11/locale-variants/locale_var13.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclFoundation11/locale-variants/locale_var14.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclFoundation11/locale-variants/locale_var15.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclFoundation11/locale-variants/locale_var16.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclFoundation11/locale-variants/locale_var17.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclFoundation11/locale-variants/locale_var18.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclFoundation11/locale-variants/locale_var19.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclFoundation11/locale-variants/locale_var20.zip',)
          ]

j9LocaleVariantFilesCdc = [
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/locale-variants/locale_var1.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/locale-variants/locale_var2.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/locale-variants/locale_var3.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/locale-variants/locale_var4.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/locale-variants/locale_var5.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/locale-variants/locale_var6.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/locale-variants/locale_var7.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/locale-variants/locale_var8.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/locale-variants/locale_var9.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/locale-variants/locale_var10.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/locale-variants/locale_var11.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/locale-variants/locale_var12.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/locale-variants/locale_var13.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/locale-variants/locale_var14.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/locale-variants/locale_var15.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/locale-variants/locale_var16.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/locale-variants/locale_var17.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/locale-variants/locale_var18.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/locale-variants/locale_var19.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/locale-variants/locale_var20.zip',),
          ]


j9PropertyFiles = [
         ('winscw/udeb/z/resource/ive/bin/java.properties',),
         ('winscw/udeb/z/resource/ive/bin/java_ca.properties',),
         ('winscw/udeb/z/resource/ive/bin/java_cs.properties',),
         ('winscw/udeb/z/resource/ive/bin/java_de.properties',),
         ('winscw/udeb/z/resource/ive/bin/java_es.properties',),
         ('winscw/udeb/z/resource/ive/bin/java_fr.properties',),
         ('winscw/udeb/z/resource/ive/bin/java_hu.properties',),
         ('winscw/udeb/z/resource/ive/bin/java_it.properties',),
         ('winscw/udeb/z/resource/ive/bin/java_ja.properties',),
         ('winscw/udeb/z/resource/ive/bin/java_ko.properties',),
         ('winscw/udeb/z/resource/ive/bin/java_pl.properties',),
         ('winscw/udeb/z/resource/ive/bin/java_pt_BR.properties',),
         ('winscw/udeb/z/resource/ive/bin/java_ru.properties',),
         ('winscw/udeb/z/resource/ive/bin/java_sk.properties',),
         ('winscw/udeb/z/resource/ive/bin/java_sl.properties',),
         ('winscw/udeb/z/resource/ive/bin/java_tr.properties',),
         ('winscw/udeb/z/resource/ive/bin/java_zh.properties',),
         ('winscw/udeb/z/resource/ive/bin/java_zh_TW.properties',)
          ]

j9SecurityFiles = [
          ('winscw/udeb/z/resource/ive/lib/security/java.policy',),
          ('winscw/udeb/z/resource/ive/lib/security/java.security',)
          ]

j9MiscFiles = [
            ('winscw/udeb/z/resource/ive/lib/romclass_cln.prf',),
            ('winscw/udeb/z/resource/ive/lib/romclass_cln.sym',),
            ('winscw/udeb/z/resource/ive/lib/romclass_fdn11.prf',),
            ('winscw/udeb/z/resource/ive/lib/romclass_fdn11.sym',),
            ('winscw/udeb/z/resource/ive/lib/jclCdc11/classes.jxe', 'jclCdc11'),
            ('winscw/udeb/z/resource/ive/lib/jclCdc11/classes.sym', 'jclCdc11'),
            ('winscw/udeb/z/resource/ive/lib/jclCldc11/classes.jxe', 'jclCldc11'),
            ('winscw/udeb/z/resource/ive/lib/jclCldc11/classes.sym', 'jclCldc11'),
            ('winscw/udeb/z/resource/ive/lib/jclFoundation11/classes.jxe', 'jclFoundation11'),
            ('winscw/udeb/z/resource/ive/lib/jclFoundation11/classes.sym', 'jclFoundation11'),
            ('winscw/udeb/z/resource/ive/lib/jclFoundation11/ext/j9jce.jar', 'jclFoundation11/ext'),
            ('winscw/udeb/z/resource/ive/lib/jclFoundation11/ext/j9jsse.jar', 'jclFoundation11/ext'),
            ('winscw/udeb/z/resource/ive/lib/jclFoundation11/ext/jdbc.jar', 'jclFoundation11/ext'),
            ('winscw/udeb/z/resource/ive/lib/jclFoundation11/ext/rmip.jar', 'jclFoundation11/ext'),
            ('winscw/udeb/z/resource/ive/lib/jclFoundation11/opt-ext/j9jceprov.jar', 'jclFoundation11/opt_ext')
          ]


j9FilesToCopy = [
          ('winscw/udeb/z/resource/ive/lib/jclCldc11/classes.zip', 'classes.cldc.zip'),
          ('winscw/udeb/z/resource/ive/lib/jclCdc11/classes.zip', 'classes.cdc.zip'),
          ('winscw/udeb/z/resource/ive/lib/jclFoundation11/classes.zip', 'classes.fp.zip'),
          ('winscw/udeb/z/resource/ive/lib/jclFoundation11/locale.zip', 'locale.fp.zip'),
          ('winscw/udeb/z/resource/ive/lib/jclCdc11/locale.zip', 'locale.cdc.zip'),
          ('../../../../../../info.txt', None),
          ('../../../../../copyright.txt', None),
          #('../../../../../ReleaseNotes-12-10-2009.doc', 'releasenotes/ReleaseNotes.doc')
          ]

j9FilesToIgnore = [
         ('../../j9_s60.pkg',)
          ]

j9WemeFilesToIgnore = [
         ('winscw/udeb/z/resource/ive/lib/jclCldc11/source/source.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/source/source.zip',),
         ('winscw/udeb/z/resource/ive/lib/jclFoundation11/source/source.zip',)
          ]

j9JavaSrcFiles = [
         ('winscw/udeb/z/resource/ive/lib/jclCldc11/source/source.zip','javasrc/source.cldc11.zip'),
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/source/source.zip', 'javasrc/source.cdc11.zip'),
         ('winscw/udeb/z/resource/ive/lib/jclCdc11/source/locale-src.zip', 'javasrc/source.locale.zip'),
         ('winscw/udeb/z/resource/ive/lib/jclFoundation11/source/source.zip', 'javasrc/source.foundation11.zip')
          ]

if __name__ == "__main__":
    main()
