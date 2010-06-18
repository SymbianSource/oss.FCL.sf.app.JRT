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
##################################################################
##  Tool used to managing the security policies and the set of  ##
##  X.509 certificates used as trust anchors in verifying the   ##
##  authenticity of signing certificates used for signing       ##
##  java applications                                           ##
##                                                              ##
##  This tool can be used at two different phases:              ##
##  1) at ROM image creation time (used by variant engineers)   ##
##     before generating the variant ROM image and before       ##
##     using S60 Configuration tool.                            ##
##     When used at this phase,                                 ##
##     the tool instructs the ROM image creation process on     ##
##     which policies and certificates to include into the ROM  ##
##     image                                                    ##
##  2) after ROM image creation time. When used at this phase   ##
##     the tool generates deployment packages (SIS) to be       ##
##     installed into "already flashed ROM images"              ##
##                                                              ##
##  This tool assumes the existence of a S60 environment,       ##
##  therefore it must be used from within a S60 environment     ##
##################################################################

#use strict;
use Getopt::Std;
use XML::Simple;
use File::Spec;
use File::Copy;

# s60 specific paths
my $CERTS_BUILD_DIR_1                             = "/epoc32/data/z/private/200211dc/security/trustroots/device/certificates/";
my $CERTS_BUILD_DIR_2                             = "/epoc32/release/armv5/urel/z/private/200211dc/security/trustroots/device/certificates/";
my $POLICIES_BUILD_DIR_1                          = "/epoc32/data/z/resource/java/security/policies/";
my $POLICIES_BUILD_DIR_2                          = "/epoc32/release/armv5/urel/z/resource/java/security/policies/";
my $IBY_DIR                                       = "/epoc32/rom/include/core/app/";
my $IBY_TEST_DIR                                  = "/epoc32/rom/include/core/tools/";
my $CONFML_DATA_DIR                               = "/epoc32/rom/config/confml_data/s60/";
# global variables
my $config_file;
my $cert_to_remove;
my $policy_to_remove;
my $deployment_destination;
my $signing_cert;
my $signing_key;
my $add_header                                     = 0;
my $deploy                                         = 1;
my $iby_file_name;
my $iby_name;
my $sis_file_name;
my $pkg_file_name;
my $pkg_name;
my $package_type;
my $s60_version;
my $signed_sis                                     = "false";
my $ROM                                            = "rom";
my $SIS                                            = "sis";
my $openssl                                        = "openssl.exe";
my $METADATA_EXT                                   = ".metadata";
my $STATE_EXT                                      = ".state";
my $JAVA_IBY_FILENAME                              = "java.iby";
my $JAVA_TEST_IBY_FILENAME                         = "javatest.iby";
my $JAVA_VARIANT_CERTS_IBY_FILENAME                = "java_variant_certs.iby";
my $JAVA_VARIANT_CERTS_PKG_FILENAME                = "java_variant_certs.pkg";
my $JAVA_VARIANT_CERTS_SIS_FILENAME                = "java_variant_certs.sis";
my $JAVA_VARIANT_CERTS_AND_POLICIES_IBY_FILENAME   = "java_variant_certs_policies.iby";
my $JAVA_VARIANT_CERTS_AND_POLICIES_PKG_FILENAME   = "java_variant_certs_policies.pkg";
my $JAVA_VARIANT_CERTS_AND_POLICIES_SIS_FILENAME   = "java_variant_certs_policies.sis";
my $JAVA_VARIANT_POLICY_IBY_FILENAME               = "java_variant_policy.iby";
my $JAVA_VARIANT_POLICY_PKG_FILENAME               = "java_variant_policy.pkg";
my $JAVA_VARIANT_POLICY_SIS_FILENAME               = "java_variant_policy.sis";
my $JAVA_CUSTOM_SECURITY_PKG_FILENAME              = "java_custom_security.pkg";
my $JAVA_CUSTOM_SECURITY_SIS_FILENAME              = "java_custom_security.sis";
my $NON_REMOVABLE_PACKAGE_TYPE                     = ",(0x2001FD68), 1,0,0, TYPE=PU,RU\n%{\"Nokia\"}\n:\"Nokia\"\n[0x1028315F], 0, 0, 0, {\"Series60ProductID\"}\n";
my $REMOVABLE_PACKAGE_TYPE                         = ",(0x2001FD68), 1,0,0, TYPE=SP,RU\n%{\"Nokia\"}\n:\"Nokia\"\n[0x1028315F], 0, 0, 0, {\"Series60ProductID\"}\n";
# IBY and PKG data
my $CERTS_DATACAGE                                 = "\\private\\200211dc\\security\\trustroots\\device\\certificates\\";
my $CERTS_DEST                                     = "\\resource\\java\\security\\trustroots\\";
my $CERTS_STATE_DATACAGE                           = "\\private\\200211dc\\security\\trustroots\\device\\state\\";
my $POLICIES_DATACAGE_SRC                          = "\\resource\\java\\security\\policies\\";
my $POLICIES_DATACAGE_SRC_5_0                      = "\\private\\102033E6\\resource\\security\\policies\\";
my $POLICIES_DATACAGE_DEST                         = "RESOURCE_FILES_DIR\\java\\security\\policies\\";

##############################################################
##  Displays the usage help                                 ##
##############################################################
sub usage
{
  system("cls");
  print "\nUsage:\n\javasecuritycustomizationtool.pl listcerts | addcerts | deploycerts | removecert <cert_name> | listpolicies | addpolicy | deploypolicy | removepolicy <policy_name> | addall | deployall | setpolicy | setwarningsmode \n";
  print "\nWhere:\n";
  print "listcerts       Lists all the X.509 certificates used as trust anchors in\n";
  print "                verifying the authenticity of signed java applications and\n";
  print "                also used in binding java applications to protection domains\n";
  print "addcerts        Adds new X.509 certificates into the existing collection\n";
  print "                of X.509 certificates used as trust anchors in verifying the\n";
  print "                authenticity of signed java applications and also used in\n";
  print "                binding java applications to protection domains\n";
  print "                The properties of the certificates to be added are specified\n";
  print "                by the file 'configdata.xml'\n";
  print "deploycerts     Creates a SIS package, containing new X.509 certificates,\n";
  print "                to be installed into a device having java security system in \n";
  print "                place. These new certificates are used as trust anchors in\n";
  print "                verifying the authenticity of signed java applications and\n";
  print "                also used in binding java applications to protection domains\n";
  print "                The properties of the certificates to be deployed are specified\n";
  print "                by the file 'configdata.xml'\n";
  print "removecert      Removes a single X.509 certificate from the existing collection\n";
  print "                of X.509 certificates used as trust anchors in verifying the\n";
  print "                authenticity of signed java applications and also used in\n";
  print "                binding java applications to protection domains\n";
  print "listpolicies    Lists all the policies used for granting permissions to java\n";
  print "                applications\n";
  print "addpolicy       Adds a new policy into the existing collection of policies\n";
  print "                used for granting permissions to java applications\n";
  print "                The properties of the policy to be added are specified\n";
  print "                by the file 'configdata.xml'\n";
  print "deploypolicy    Creates a SIS package, containing a new policy, to be\n";
  print "                installed into a device having java security system in place\n";
  print "                The properties of the policy to be deployed are specified by\n";
  print "                the file 'configdata.xml'\n";
  print "removepolicy    Removes a single policy from the collection of policies\n";
  print "                used for granting permissions to java applications\n";
  print "addall          Adds new X.509 certificates and a new policy into the existing\n";
  print "                collection of certificates/policies used for\n";
  print "                authentication/authorization of java applications. This is the\n";
  print "                equivalent of the combined command line arguments 'addcerts'\n";
  print "                and 'addpolicy'\n";
  print "deployall       Creates a SIS package, containing new X.509 certificates and a\n";
  print "                new policy, to be installed into a device having java security\n";
  print "                system in place. This is a the equivalent of the combined\n";
  print "                command line arguments 'deploycerts' and 'deploypolicy'\n";
  print "                The properties of the certificates and policy to be deployed\n";
  print "                are specified by the file 'configdata.xml'\n";
  print "setpolicy       Creates a SIS package, which selects the policy to be used for\n";
  print "                granting permissions to java applications. This SIS is to be\n";
  print "                installed into a device having java security system in place\n";
  print "                The name of the selected policy is specified by the file\n";
  print "                'configdata.xml'\n";
  print "setwarningsmode Creates a SIS package, which selects the mode used for handling\n";
  print "                security warnings. This SIS is to be installed into a device\n";
  print "                having java security system in place\n";
  print "                The warnings mode is specified by the file 'configdata.xml'\n";
  print "\n\nThis tool can be used at two different phases:\n";
  print "  1) at ROM image creation time (used by variant engineers) before generating\n";
  print "     the variant ROM image and before using S60 Configuration tool. Used at\n";
  print "     this phase the tool produces configuration data for the ROM image creation\n";
  print "     process. The command line arguments to be used at this phase are:\n";
  print "     'listcerts', 'addcerts', 'removecert', 'listpolicies', 'addpolicy',\n";
  print "     'removepolicy', and 'addall'\n";
  print "  2) after ROM image creation time. Used at this phase the tool generates\n";
  print "     deployment packages (SIS) to be installed into 'already flashed ROM\n";
  print "     images'. The command line arguments to be used at this phase are:\n"; 
  print "     'deploycerts', 'deploypolicy', 'setpolicy', 'setwarningsmode' and\n"; 
  print "     'deployall'\n";
  print "\nThis tool assumes the existence of a S60 environment, therefore it must be used\n";
  print "from within a S60 environment\n";
  exit;
}

##############################################################
##  Lists all the certificates                              ##
##############################################################
sub list_certs()
{
  @files = <$CERTS_BUILD_DIR_1*>;
  @certs;
  @domains;
  @paths;
  foreach $file (@files) {
    if (rindex($file, $METADATA_EXT) < 0)
    {
      $ext = rindex($file, ".");
      my $file_without_ext = substr($file, 0, $ext);
      $cert_name = substr($file, length("$CERTS_BUILD_DIR_1"));
      if (! grep {$_ eq $cert_name} @certs) {
        push(@certs, $cert_name);
        push(@paths, $CERTS_BUILD_DIR_1);
        open (METADATA_FILE, "$file_without_ext$METADATA_EXT") or die "Cannot open $file_without_ext$METADATA_EXT\n";
        my @metadata = <METADATA_FILE>;
        close(METADATA_FILE);
        my $domain;
        foreach $metadata_item (@metadata) {
            $metadata_item_key = substr($metadata_item, 0, rindex($metadata_item, "="));
	          if ($metadata_item_key eq "name")
	          {
	              $domain = substr($metadata_item, rindex($metadata_item, "=") + 1);
                push(@domains, $domain);
	              break;
	          }
        }
      }
    }
  }

  @files = <$CERTS_BUILD_DIR_2*>;
  foreach $file (@files) {
    if (rindex($file, $METADATA_EXT) < 0)
    {
      $ext = rindex($file, ".");
      my $file_without_ext = substr($file, 0, $ext);
      $cert_name = substr($file, length("$CERTS_BUILD_DIR_2"));
      if (! grep {$_ eq $cert_name} @certs) {
        push(@certs, $cert_name);
        push(@paths, $CERTS_BUILD_DIR_2);
        open (METADATA_FILE, "$file_without_ext$METADATA_EXT") or die "Cannot open $file_without_ext$METADATA_EXT\n";
        my @metadata = <METADATA_FILE>;
        close(METADATA_FILE);
        my $domain;
        foreach $metadata_item (@metadata) {
            $metadata_item_key = substr($metadata_item, 0, rindex($metadata_item, "="));
	          if ($metadata_item_key eq "name")
	          {
	              $domain = substr($metadata_item, rindex($metadata_item, "=") + 1);
                push(@domains, $domain);
	              break;
	          }
        }
      }
    }
  }

  # display only the files which are found inside java.iby or javatest.iby
  open (JAVA_IBY, "$IBY_DIR$JAVA_IBY_FILENAME");
  my @java_iby_lines = <JAVA_IBY>;
  close(JAVA_IBY);
  my @java_test_iby_lines;
  if (-f "$IBY_TEST_DIR$JAVA_TEST_IBY_FILENAME")
  {
      open (JAVA_TEST_IBY, "$IBY_TEST_DIR$JAVA_TEST_IBY_FILENAME");
      @java_test_iby_lines = <JAVA_TEST_IBY>;
      close(JAVA_TEST_IBY);
  }
  my @java_variant_certs_iby_lines;
  if (-f "$IBY_DIR$JAVA_VARIANT_CERTS_IBY_FILENAME")
  {
      open (JAVA_VARIANT_CERTS_IBY, "$IBY_DIR$JAVA_VARIANT_CERTS_IBY_FILENAME");
      @java_variant_certs_iby_lines = <JAVA_VARIANT_CERTS_IBY>;
      close(JAVA_VARIANT_CERTS_IBY);
  }
  my @java_variant_certs_policies_iby_lines;
  if (-f "$IBY_DIR$JAVA_VARIANT_CERTS_AND_POLICIES_IBY_FILENAME")
  {
      open (JAVA_VARIANT_CERTS_POLICIES_IBY, "$IBY_DIR$JAVA_VARIANT_CERTS_AND_POLICIES_IBY_FILENAME");
      @java_variant_certs_policies_iby_lines = <JAVA_VARIANT_CERTS_POLICIES_IBY>;
      close(JAVA_VARIANT_CERTS_POLICIES_IBY);
  }
  $index = 0;
  my $found = 0;
  foreach $cert (@certs) {
        $found = 0;
        foreach $java_iby_line (@java_iby_lines) {
            if (rindex($java_iby_line, $cert) >= 0) {
                $found = 1;
                last;
            }
        }
        if ($found == 0)
        {
            foreach $java_variant_certs_iby_line (@java_variant_certs_iby_lines) {
                if (rindex($java_variant_certs_iby_line, $cert) >= 0) {
                    $found = 1;
                    last;
                }
            }
        }
        if ($found == 0)
        {
            foreach $java_variant_certs_policies_iby_line (@java_variant_certs_policies_iby_lines) {
                if (rindex($java_variant_certs_policies_iby_line, $cert) >= 0) {
                    $found = 1;
                    last;
                }
            }
        }
        #if ($found == 0)
        #{
            #foreach $java_test_iby_line (@java_test_iby_lines) {
                #if (rindex($java_test_iby_line, $cert) >= 0) {
                    #$found = 1;
                    #last;
                #}
            #}
        #}
        if ($found == 1)
        {
            print "Certificate:\n";
            print "  Name: ", $cert , "\n";
            print "  Domain: ", $domains[$index];
            print "  Path: ", $paths[$index], "\n\n";
        }
        $index = $index + 1;
    }
}

##############################################################
##  Removes a certain certificate                           ##
##############################################################
sub remove_cert()
{
    if (! -f "$CERTS_BUILD_DIR_1$cert_to_remove")
    {
        print "\n\nERROR: ", $cert_to_remove, " not found.\n";
        exit;
    } else
    {
        
        $ext = rindex($cert_to_remove, ".");
        $file_without_ext = substr($cert_to_remove, 0, $ext);
        
        # modify java.iby
       	mkdir "tmp";
        open (JAVA_IBY, "$IBY_DIR$JAVA_IBY_FILENAME");
        open (JAVA_NEW_IBY, "+>tmp/$JAVA_IBY_FILENAME") or die "Cannot create tmp file (tmp/$JAVA_IBY_FILENAME)\n";
        my @java_iby_lines = <JAVA_IBY>;
        my $found = 0;
        foreach $java_iby_line (@java_iby_lines) {
            if (rindex($java_iby_line, $cert_to_remove) < 0 && rindex($java_iby_line, "$file_without_ext$METADATA_EXT") < 0)
            {
                print JAVA_NEW_IBY $java_iby_line;
            }
            else
            {
                $found = 1;
            }
        }
        close(JAVA_IBY);
        close(JAVA_NEW_IBY);
        move("tmp/$JAVA_IBY_FILENAME", "$IBY_DIR");
        if ($found == 0)
        {
            if (-f "$IBY_DIR$JAVA_VARIANT_CERTS_IBY_FILENAME")
            {
                open (JAVA_IBY, "$IBY_DIR$JAVA_VARIANT_CERTS_IBY_FILENAME");
                open (JAVA_NEW_IBY, "+>tmp/$JAVA_VARIANT_CERTS_IBY_FILENAME") or die "Cannot create tmp file (tmp/$JAVA_VARIANT_CERTS_IBY_FILENAME)\n";
                my @java_iby_lines = <JAVA_IBY>;
                my $found = 0;
                foreach $java_iby_line (@java_iby_lines) {
                    if (rindex($java_iby_line, $cert_to_remove) < 0 && rindex($java_iby_line, "$file_without_ext$METADATA_EXT") < 0)
                    {
                        print JAVA_NEW_IBY $java_iby_line;
                    }
                }
                close(JAVA_IBY);
                close(JAVA_NEW_IBY);
                move("tmp/$JAVA_VARIANT_CERTS_IBY_FILENAME", "$IBY_DIR");
            }         
            # check the combined iby file as well
            if (-f "$IBY_DIR$JAVA_VARIANT_CERTS_AND_POLICIES_IBY_FILENAME")
            {
                open (JAVA_IBY, "$IBY_DIR$JAVA_VARIANT_CERTS_AND_POLICIES_IBY_FILENAME");
                open (JAVA_NEW_IBY, "+>tmp/$JAVA_VARIANT_CERTS_AND_POLICIES_IBY_FILENAME") or die "Cannot create tmp file (tmp/$JAVA_VARIANT_CERTS_AND_POLICIES_IBY_FILENAME)\n";
                my @java_iby_lines = <JAVA_IBY>;
                my $found = 0;
                foreach $java_iby_line (@java_iby_lines) {
                    if (rindex($java_iby_line, $cert_to_remove) < 0 && rindex($java_iby_line, "$file_without_ext$METADATA_EXT") < 0)
                    {
                        print JAVA_NEW_IBY $java_iby_line;
                    }
                }
                close(JAVA_IBY);
                close(JAVA_NEW_IBY);
                move("tmp/$JAVA_VARIANT_CERTS_AND_POLICIES_IBY_FILENAME", "$IBY_DIR");
            }

        }
        
      	unlink("$CERTS_BUILD_DIR_1$cert_to_remove");
      	unlink("$CERTS_BUILD_DIR_1$file_without_ext$METADATA_EXT");
      	
      	unlink("$CERTS_BUILD_DIR_2$cert_to_remove");
      	unlink("$CERTS_BUILD_DIR_2$file_without_ext$METADATA_EXT");
      	
        system("rmdir tmp /s /q");
    }
}

##############################################################
##  Adds new certificates                                   ##
##############################################################
sub add_certs()
{
  $config_file = "./configdata.xml";

  my $xml = new XML::Simple(suppressempty => '');
  my $xmldata = $xml->XMLin($config_file, forcearray => 1);

  my $root_file;
  my $root_domain;
  my $root_canDelete;
  my $root_canDisable;
  my $root_file_without_ext;
  my $root_file_with_ext;
  my $root_state;
      
 	mkdir "tmp";
 	mkdir "tmp/certs/";
 	if ($deployment_destination eq $ROM)
 	{
    open (IBY_FILE, "+>tmp/$iby_file_name") or die "Cannot create tmp file (tmp/$iby_file_name)\n";
    print IBY_FILE   "#ifndef __" . "$iby_name" ."__\n#define __" . "$iby_name" ."__\n\n#include <data_caging_paths_for_iby.hrh>\n\n";
  }
  else
  {
    open (PKG_FILE, "+>tmp/$pkg_file_name") or die "Cannot create tmp file (tmp/$pkg_file_name)\n";
    print PKG_FILE   "&EN\n#{\"$pkg_name\"}" ."$package_type";
    open (POLICY_FILE, "+>tmp/update_certs") or die "Cannot create tmp file (tmp/update_certs)\n";
    close POLICY_FILE;
  }
  
  open (MIDPROOTSLIST_FILE, "+>tmp/midprootslist") or die "Cannot create temp file (tmp/certs/midprootslist)\n";
  foreach my $root (@{$xmldata->{root}}) {
    
      # read the xml node
      $root_file = $root->{file}->[0];
      $root_domain = $root->{domain}->[0];
      $root_canDelete = $root->{canDelete}->[0];
      $root_canDisable = $root->{canDisable}->[0];
     	
     	if ($deployment_destination eq $SIS)
 	    {
          $root_state = $root->{state}->[0];
          if (($root_state ne "") && ($root_state ne "enabled") && ($root_state ne "disabled") && ($root_state ne "removed")) {
          	print "ERROR: The values for \"state\" can be either \"enabled\" or \"disabled\" or \"removed\". Please check the configuration file $config_file\n";
            close IBY_FILE;
            close PKG_FILE;
            system("rmdir tmp /s /q");
      	    exit;
        }
 	    }
          
      # validate the root file
     	if (! -f $root_file) {
        print "\nERROR: root file $root_file doesn't exist. Please check the configuration file $config_file\n";
        close IBY_FILE;
        close PKG_FILE;
        system("rmdir tmp /s /q");
        exit;
      }
      # validate start date of the root
      ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
      $current_year = $year + 1900;
      system("openssl x509 -inform DER -in $root_file -noout -startdate > tmp/startdate.txt");
      open(start_date_file, "tmp/startdate.txt"); 
      my @start_date = <start_date_file>;
      close(start_date_file);
      unlink("tmp/startdate.txt");
      @start_date_details = split(/ /, $start_date[0]);
      my $start_date_year = $start_date_details[3];
      if ($start_date_year > $current_year) {
        print "\nERROR: root $root_file not yet valid.\n";
        close IBY_FILE;
        close PKG_FILE;
        system("rmdir tmp /s /q");
        exit;
      }
      # validate end date of the root
      system("openssl x509 -inform DER -in $root_file -noout -enddate > tmp/enddate.txt");
      open(end_date_file, "tmp/enddate.txt"); 
      my @end_date = <end_date_file>;
      close(end_date_file);
      unlink("tmp/enddate.txt");
      @end_date_details = split(/ /, $end_date[0]);
      $year_index = 0;
      $found_items = 0;
      foreach $end_date_detail (@end_date_details) {
        if ($found_items == 3)
        {
            last;   
        }
        if ($end_date_detail ne "")
        {
           $found_items = $found_items + 1; 
        }
        $year_index = $year_index + 1;
      }
      my $end_date_year = $end_date_details[$year_index];
      if ($end_date_year < $current_year) {
        print "\nERROR: root $root_file is expired.\n";
        close IBY_FILE;
        close PKG_FILE;
        system("rmdir tmp /s /q");
        exit;
      }
      #validate the domain
      if (($root_domain ne "Manufacturer") && ($root_domain ne "Operator") && ($root_domain ne "OperatorExtra") && ($root_domain ne "IdentifiedThirdParty")) {
      	print "\nERROR: the values for \"domain\" can be either \"Manufacturer\" or \"Operator\" or \"OperatorExtra\" or \"IdentifiedThirdParty\". Please check the configuration file $config_file\n";
        close IBY_FILE;
        close PKG_FILE;
        system("rmdir tmp /s /q");
      	exit;
      }
      #validate the canDelete element
      if (($root_canDelete ne "") && ($root_canDelete ne "true") && ($root_canDelete ne "false")) {
      	print "\nERROR: the values for \"canDelete\" can be either \"true\" or \"false\". Please check the configuration file $config_file\n";
        close IBY_FILE;
        close PKG_FILE;
        system("rmdir tmp /s /q");
      	exit;
      }
      #validate the canDisable element
      if (($root_canDisable ne "") && ($root_canDisable ne "true") && ($root_canDisable ne "false")) {
      	print "ERROR: The values for \"canDisable\" can be either \"true\" or \"false\". Please check the configuration file $config_file\n";
        close IBY_FILE;
        close PKG_FILE;
        system("rmdir tmp /s /q");
      	exit;
      }
      # generate the cert metadata and copy the cert & metadata to a tmp folder
      system("openssl x509 -inform DER -in $root_file -noout -issuer_hash > tmp/hash.txt");
      open(hash_file, "tmp/hash.txt"); 
      my @lines = <hash_file>;
      close(hash_file);
      unlink("tmp/hash.txt");
      $start_index = rindex($root_file, "/");
      $end_index = rindex($root_file, ".");
      if ($start_index >= 0)
      {
        $root_file_without_ext = substr($root_file, $start_index + 1, $end_index - $start_index - 1);
        $root_file_with_ext = substr($root_file, $start_index + 1);
      }
      else
      {
        $root_file_without_ext = substr($root_file, 0, $end_index);
        $root_file_with_ext = $root_file;
      }
      open (EXT_FILE, "+>tmp/certs/$root_file_without_ext$METADATA_EXT") or die "Cannot create temp file (tmp/certs/$root_file_without_ext$METADATA_EXT)\n";
      print EXT_FILE "name=$root_domain\n";
      if ($root_domain eq "Manufacturer")
      {
          print EXT_FILE "category=MFD\n";
      }
      if ($root_domain eq "Operator")
      {
          print EXT_FILE "category=OPD\n";
      }
      if ($root_domain eq "OperatorExtra")
      {
          print EXT_FILE "category=OPD\n";
      }
      if ($root_domain eq "IdentifiedThirdParty")
      {
          print EXT_FILE "category=ITPD\n";
      }
      if ($root_canDelete eq "" || $root_canDelete eq "false") {
          print EXT_FILE "removable=0\n";
      }
      else
      {
          print EXT_FILE "removable=1\n";
      }
      if ($root_canDisable eq "" || $root_canDisable eq "false") {
          print EXT_FILE "disablable=0\n";
      }
      else
      {
          print EXT_FILE "disablable=1\n";
      }
      print EXT_FILE "hash=$lines[0]\n";
      close EXT_FILE;
      copy($root_file, "tmp/certs/") or die "File $root_file cannot be copied to tmp/";
      # strip off the path
     	if ($deployment_destination eq $ROM)
 	    {
          print IBY_FILE "data=ABI_DIR\\BUILD_DIR\\z" ."$CERTS_DATACAGE";
          print IBY_FILE $root_file_with_ext;
          print IBY_FILE " " ."$CERTS_DATACAGE";
          print IBY_FILE $root_file_with_ext;
          print IBY_FILE "\ndata=ABI_DIR\\BUILD_DIR\\z" ."$CERTS_DATACAGE";
          print IBY_FILE $root_file_without_ext;
          print IBY_FILE "$METADATA_EXT " ."$CERTS_DATACAGE";
          print IBY_FILE $root_file_without_ext;
          print IBY_FILE "$METADATA_EXT\n";
      }
      else
      {
          print MIDPROOTSLIST_FILE $root_file_with_ext;       
          print MIDPROOTSLIST_FILE "\n";       
          print PKG_FILE   "\"./tmp/certs/";
          print PKG_FILE   $root_file_with_ext;
          if ($s60_version eq "5.0")
          {
              print PKG_FILE   "\"-\"c:" . "$CERTS_DATACAGE";
          }
          else
          {
              print PKG_FILE   "\"-\"c:" . "$CERTS_DEST";
          }
          print PKG_FILE   $root_file_with_ext;
          print PKG_FILE   "\"\n\"./tmp/certs/";
          print PKG_FILE   $root_file_without_ext;
          if ($s60_version eq "5.0")
          {
              print PKG_FILE   "$METADATA_EXT\"-\"c:" ."$CERTS_DATACAGE";
          }
          else
          {
              print PKG_FILE   "$METADATA_EXT\"-\"c:" ."$CERTS_DEST";
          }
          print PKG_FILE   $root_file_without_ext;
          print PKG_FILE   "$METADATA_EXT\"\n";
          open (STATE_FILE, "+>tmp/certs/$root_file_without_ext$STATE_EXT") or die "Cannot create temp file (tmp/certs/$root_file_without_ext$STATE_EXT)\n";
          binmode STATE_FILE;
          my $state;
          if ($root_state eq "enabled" || $root_state eq "")
          {
            $state = pack("h8", "3000");
          }
          else
          {
              if ($root_state eq "disabled")
              {
                $state = pack("h8", "2000");
              }
              else
              {
                 if ($root_state eq "removed")
                 {
                    $state = pack("h8", "1000");
                  }
              }
          }
          print STATE_FILE $state;
          close STATE_FILE;
          print PKG_FILE   "\"./tmp/certs/";
          print PKG_FILE   $root_file_without_ext;
          print PKG_FILE   "$STATE_EXT\"-\"c:" ."$CERTS_STATE_DATACAGE";
          print PKG_FILE   $root_file_without_ext;
          print PKG_FILE   "$STATE_EXT\"\n";
      }
  }
  close MIDPROOTSLIST_FILE;
 	if ($deployment_destination eq $SIS)
 	{
    print PKG_FILE   "\"./tmp/update_certs\"-\"c:\\private\\102033E6\\security\\tmp\\update_certs\"\n";
    if ($s60_version ne "5.0")
    {
        print PKG_FILE   "\"./tmp/midprootslist\"-\"c:\\resource\\java\\security\\trustroots\\midprootslist\"\n";
    }
  }
  if ($deploy == 1)
  {
    print PKG_FILE   "\"\\epoc32\\release\\armv5\\urel\\javasecuritycustomization.exe\"-\"c:\\sys\\bin\\javasecuritycustomization.exe\", FR, RB, RW\n";
  }
  
  # copy all the certificates and coresponding metadatas to right locations
  @files = <tmp/certs/*>;
  foreach $file (@files) {
    if ($deployment_destination eq $ROM)
    {
        copy($file, $CERTS_BUILD_DIR_2);
        copy($file, $CERTS_BUILD_DIR_1);
    }
  }
 	if ($deployment_destination eq $ROM)
  {
      if ($deploy == 1)
      {
        print IBY_FILE "\n\n#endif\n";
      }
      close IBY_FILE;
  }
  else
  {
      close PKG_FILE;
  }
  
  if ($deploy == 1)
  {
      # generate the iby file and copy it to right location
     	if ($deployment_destination eq $ROM)
      {
          move("tmp/$iby_file_name", "$IBY_DIR");
      }
      else
      {
          system("makesis ./tmp/$pkg_file_name ./tmp/tmp.sis");
          if (-f $signing_cert && -f $signing_key)
          {
              $ret = system("signsis ./tmp/tmp.sis $sis_file_name $signing_cert $signing_key");
              if ($ret != 0)
              {
                $signed_sis = false;
              }
              else
              {
                $signed_sis = true;
              }
            	unlink("./tmp/tmp.sis");
          }
          else
          {
              move("./tmp/tmp.sis", "./$sis_file_name");  
          }
        	unlink("./tmp/$pkg_file_name");
      }
    	system("rmdir tmp /s /q");
  }
}

##############################################################
##  Adds a new policy                                       ##
##############################################################
sub add_policy()
{
  $config_file = "./configdata.xml";

  my $xml = new XML::Simple(suppressempty => '');
  my $xmldata = $xml->XMLin($config_file);

  my $policy_name;
  my $utp_policy_path;
  my $ttp_policy_path;
  my $operator_policy_path;
  my $manufacturer_policy_path;

  # create tmp directory  
 	mkdir "tmp";
 	mkdir "tmp/policies/";
 	mkdir "tmp/policies/external/";
 	mkdir "tmp/policies/internal/";
 	
  # read the xml node
  $policy_name = $xmldata->{policy}->{name};
  $utp_policy_path = $xmldata->{policy}->{unidentifiedthirdparty};
  $ttp_policy_path = $xmldata->{policy}->{identifiedthirdparty};
  $operator_policy_path = $xmldata->{policy}->{operator};
  $manufacturer_policy_path = $xmldata->{policy}->{manufacturer};
  
  # validate the policy name
  $tmp_str = lc $policy_name;
  if (($policy_name eq "") || ($tmp_str eq "s60") || ($tmp_str eq "msa") || ($tmp_str eq "att")) 
  {
    if ($policy_name eq "")
    {
      	print "\nERROR: the name of the policy is mandatory. Please check the configuration file $config_file\n";
    }
    else
    {
      	print "\nERROR: the names \"s60\", \"msa\" and \"att\" are reserved therefore the new policy can not use any of these names. Please check the configuration file $config_file\n";
    }
    close IBY_FILE;
    system("rmdir tmp /s /q");
    exit;
  }
  
  # validate the policy files
 	if (! -f $utp_policy_path || ! -f $ttp_policy_path || ! -f $operator_policy_path || ! -f $manufacturer_policy_path) 
 	{
    print "\nERROR: the new policy must contain specifications for all the four protection domains. Please check the configuration file $config_file\n";
    system("rmdir tmp /s /q");
    exit;
  }
  
  # prepare the data for the securitypolicyeditor tool
  copy($utp_policy_path, "tmp/policies/external/" . "$policy_name" . "_untrusted.txt") or die "File $utp_policy_path cannot be copied to tmp/policies/external/";
  copy($ttp_policy_path, "tmp/policies/external/" . "$policy_name" . "_trustedthirdparty.txt") or die "File $ttp_policy_path cannot be copied to tmp/policies/external/";
  copy($operator_policy_path, "tmp/policies/external/" . "$policy_name" . "_operator.txt") or die "File $operator_policy_path cannot be copied to tmp/policies/external/";
  copy($manufacturer_policy_path, "tmp/policies/external/" . "$policy_name" . "_manufacturer.txt") or die "File $manufacturer_policy_path cannot be copied tmp/policies/external/";
  
  # delegate the securitypolicytool to generate the internal policies
  $ret = system("java -cp ./policyeditor/bin/securitypolicyeditor.jar;./policyeditor/lib/engine.jar com.nokia.mj.tools.security.midp.PolicyEditor tmp/policies/external/ tmp/policies/internal/");
  if ($ret != 0)
  {
    system("rmdir tmp /s /q");
    exit;
  }

 	if ($deployment_destination eq $SIS)
  {
    open (POLICY_FILE, "+>tmp/policies/new_policy.txt") or die "Cannot create tmp file (tmp/policies/new_policy.txt)\n";
    print POLICY_FILE "$policy_name";
    close POLICY_FILE;
    $policies_dest = "c:" ."$POLICIES_DATACAGE_SRC";
    if ($s60_version eq "5.0")
    {
        $policies_dest = "c:" ."$POLICIES_DATACAGE_SRC_5_0";
    }
    
    if ($add_header == 1)
    {
        open (PKG_FILE, "+>tmp/$pkg_file_name") or die "Cannot create tmp file (tmp/$pkg_file_name)\n";
        print PKG_FILE   "&EN\n#{\"$pkg_name\"}" ."$package_type";
    }
    else
    {
        open (PKG_FILE, ">>tmp/$pkg_file_name") or die "Cannot open tmp file (tmp/$pkg_file_name)\n";
    }
    print PKG_FILE   "\"./tmp/policies/internal/" . "$policy_name" . "_untrusted.ser\"-\"$policies_dest" .  "$policy_name" . "_untrusted.ser\"\n";
    print PKG_FILE   "\"./tmp/policies/internal/" . "$policy_name" . "_trustedthirdparty.ser\"-\"$policies_dest" .  "$policy_name" . "_trustedthirdparty.ser\"\n";
    print PKG_FILE   "\"./tmp/policies/internal/" . "$policy_name" . "_operator.ser\"-\"$policies_dest" .  "$policy_name" . "_operator.ser\"\n";
    print PKG_FILE   "\"./tmp/policies/internal/" . "$policy_name" . "_manufacturer.ser\"-\"$policies_dest" .  "$policy_name" . "_manufacturer.ser\"\n";
    print PKG_FILE   "\"./tmp/policies/new_policy.txt\"-\"c:\\private\\102033E6\\security\\tmp\\new_policy.txt\"\n";
    print PKG_FILE   "\"\\epoc32\\release\\armv5\\urel\\javasecuritycustomization.exe\"-\"c:\\sys\\bin\\javasecuritycustomization.exe\", FR, RB, RW\n";
    close PKG_FILE;
  }
  else
  {
    if ($add_header == 1)
    {
        open (IBY_FILE, "+>tmp/$iby_file_name") or die "Cannot create tmp file (tmp/$iby_file_name)\n";
        print IBY_FILE   "#ifndef __" ."$iby_name" ."__\n#define __" . "$iby_name" ."__\n\n#include <data_caging_paths_for_iby.hrh>\n\n";
    }
    else
    {
        open (IBY_FILE, ">>tmp/$iby_file_name") or die "Cannot open tmp file (tmp/$iby_file_name)\n";
    }
    print IBY_FILE "data=ABI_DIR\\BUILD_DIR\\z" ."$POLICIES_DATACAGE_SRC" . "$policy_name" . "_untrusted.ser " ."$POLICIES_DATACAGE_DEST" . "$policy_name" . "_untrusted.ser\n";
    print IBY_FILE "data=ABI_DIR\\BUILD_DIR\\z" ."$POLICIES_DATACAGE_SRC" . "$policy_name" . "_trustedthirdparty.ser ". "$POLICIES_DATACAGE_DEST" . "$policy_name" . "_trustedthirdparty.ser\n";
    print IBY_FILE "data=ABI_DIR\\BUILD_DIR\\z" ."$POLICIES_DATACAGE_SRC" . "$policy_name" . "_operator.ser " ."$POLICIES_DATACAGE_DEST" . "$policy_name" . "_operator.ser\n";
    print IBY_FILE "data=ABI_DIR\\BUILD_DIR\\z" ."$POLICIES_DATACAGE_SRC" . "$policy_name" . "_manufacturer.ser " ."$POLICIES_DATACAGE_DEST" . "$policy_name" . "_manufacturer.ser\n";
    print IBY_FILE "\n\n#endif\n";
    close IBY_FILE;
  }

  # deploy the policy  
  if ($deploy == 1)
  {
     	if ($deployment_destination eq $SIS)
      {
        system("makesis ./tmp/$pkg_file_name ./tmp/tmp.sis");
        if (-f $signing_cert && -f $signing_key)
        {
            $ret = system("signsis ./tmp/tmp.sis $sis_file_name $signing_cert $signing_key");
            if ($ret != 0)
            {
              $signed_sis = false;
            }
            else
            {
              $signed_sis = true;
            }
        }
        else
        {
            move("./tmp/tmp.sis", "./$sis_file_name");  
        }
      }
      else
      {
        move("tmp/$iby_file_name", "$IBY_DIR");
        # copy all the policies to right locations
        @files = <tmp/policies/internal/*>;
        foreach $file (@files) {
            copy($file, $POLICIES_BUILD_DIR_2);
            copy($file, $POLICIES_BUILD_DIR_1);
        }
        # update the S60 configuration tool conf data
        my $xs = new XML::Simple(rootname   => 'configuration', searchpath => ".", suppressempty => '', forcearray => 1);
        my $input_confml = $xs->XMLin("$CONFML_DATA_DIR" ."javasecurity.confml");
        $input_confml->{feature}->{'Java Security Configuration'}->{setting}->{Policy}->{option}->{"" . "$policy_name"} = {value => "" . "$policy_name"};
        my $output_confml = $xs->XMLout($input_confml);
        open (OUTPUT_CONFML_FILE, "+>tmp/javasecurity.confml") or die "Cannot create tmp file (tmp/javasecurity.confml)\n";
        print OUTPUT_CONFML_FILE   "$output_confml";
        close OUTPUT_CONFML_FILE;
        move("tmp/javasecurity.confml", "$CONFML_DATA_DIR") or die "Can not move tmp/javasecurity.confml to $CONFML_DATA_DIR";
      }
    
      # remove tmp directory
    	system("rmdir tmp /s /q");
  }
}

##############################################################
##  Selects a certain policy                                ##
##############################################################
sub set_policy()
{
  $config_file = "./configdata.xml";

  my $xml = new XML::Simple(suppressempty => '');
  my $xmldata = $xml->XMLin($config_file);

  my $policy_name;

  # create tmp directory  
 	mkdir "tmp";
 	
  # read the xml node
  $policy_name = $xmldata->{policy}->{name};
  
  # validate the policy name
  $tmp_str = lc $policy_name;
  if (($tmp_str ne "s60") && ($tmp_str ne "msa") && ($tmp_str ne "att"))
  {
   	print "\nERROR: the name of the policy is mandatory and it can be either \"s60\", \"msa\" or \"att\". Please check the configuration file $config_file\n";
    system("rmdir tmp /s /q");
    exit;
  }
  
  open (POLICY_FILE, "+>tmp/new_policy.txt") or die "Cannot create tmp file (tmp/new_policy.txt)\n";
  print POLICY_FILE "$policy_name";
  close POLICY_FILE;
  open (PKG_FILE, "+>tmp/$JAVA_CUSTOM_SECURITY_PKG_FILENAME") or die "Cannot create tmp file (tmp/$JAVA_CUSTOM_SECURITY_PKG_FILENAME)\n";
  print PKG_FILE   "&EN\n#{\"JavaSecurityPolicySelector\"}" ."$package_type";
  print PKG_FILE   "\".\\tmp\\new_policy.txt\"-\"c:\\private\\102033E6\\security\\tmp\\new_policy.txt\"\n";
  print PKG_FILE   "\"\\epoc32\\release\\armv5\\urel\\javasecuritycustomization.exe\"-\"c:\\sys\\bin\\javasecuritycustomization.exe\", FR, RB, RW\n";
  close PKG_FILE;
  system("makesis ./tmp/$JAVA_CUSTOM_SECURITY_PKG_FILENAME ./tmp/tmp.sis");
  if (-f $signing_cert && -f $signing_key)
  {
      $ret = system("signsis ./tmp/tmp.sis $JAVA_CUSTOM_SECURITY_SIS_FILENAME $signing_cert $signing_key");
      if ($ret != 0)
      {
        $signed_sis = false;
      }
      else
      {
        $signed_sis = true;
      }
  }
  else
  {
      move("./tmp/tmp.sis", "./$JAVA_CUSTOM_SECURITY_SIS_FILENAME");  
  }

  # remove tmp directory
	system("rmdir tmp /s /q");
}

##############################################################
##  Selects the security warnings mode                      ##
##############################################################
sub set_warnings_mode()
{
  $config_file = "./configdata.xml";

  my $xml = new XML::Simple(suppressempty => '');
  my $xmldata = $xml->XMLin($config_file);

  my $policy_name;

  # create tmp directory  
 	mkdir "tmp";
 	
  # read the xml node
  $warnings_mode = $xmldata->{warningsmode};
  
  # validate the policy name
  if (($warnings_mode ne "user") && ($warnings_mode ne "default"))
  {
   	print "\nERROR: the warnings mode is mandatory and it can be either \"user\" or \"default\". Please check the configuration file $config_file\n";
    system("rmdir tmp /s /q");
    exit;
  }
  
  open (WARNINGS_MODE_FILE, "+>tmp/new_warnings_mode.txt") or die "Cannot create tmp file (tmp/new_warnings_mode.txt)\n";
  if ($warnings_mode eq "user")
  {
    print WARNINGS_MODE_FILE "1";
  }
  else
  {
    print WARNINGS_MODE_FILE "2";
  }
  close WARNINGS_MODE_FILE;
  open (PKG_FILE, "+>tmp/$JAVA_CUSTOM_SECURITY_PKG_FILENAME") or die "Cannot create tmp file (tmp/$JAVA_CUSTOM_SECURITY_PKG_FILENAME)\n";
  print PKG_FILE   "&EN\n#{\"JavaSecurityWarningsModeSelector\"}" ."$package_type";
  print PKG_FILE   "\".\\tmp\\new_warnings_mode.txt\"-\"c:\\private\\102033E6\\security\\tmp\\new_warnings_mode.txt\"\n";
  print PKG_FILE   "\"\\epoc32\\release\\armv5\\urel\\javasecuritycustomization.exe\"-\"c:\\sys\\bin\\javasecuritycustomization.exe\", FR, RB, RW\n";
  close PKG_FILE;
  system("makesis ./tmp/$JAVA_CUSTOM_SECURITY_PKG_FILENAME ./tmp/tmp.sis");
  if (-f $signing_cert && -f $signing_key)
  {
      $ret = system("signsis ./tmp/tmp.sis $JAVA_CUSTOM_SECURITY_SIS_FILENAME $signing_cert $signing_key");
      if ($ret != 0)
      {
        $signed_sis = false;
      }
      else
      {
        $signed_sis = true;
      }
  }
  else
  {
      move("./tmp/tmp.sis", "./$JAVA_CUSTOM_SECURITY_SIS_FILENAME");  
  }

  # remove tmp directory
	system("rmdir tmp /s /q");
}

##############################################################
##  Lists all the policies                                  ##
##############################################################
sub list_policies()
{
  @files = <$POLICIES_BUILD_DIR_1*_*.ser>;
  @policies;
  foreach $file (@files) {
    $start_index = rindex($file, "/");
    $end_index = rindex($file, "_");
    $file_without_ext = substr($file, $start_index + 1, $end_index - $start_index - 1);
    if (! grep {$_ eq $file_without_ext} @policies) {
        push(@policies, $file_without_ext);
    }
  }
  @files = <$POLICIES_BUILD_DIR_2*_*.ser>;
  foreach $file (@files) {
    $start_index = rindex($file, "/");
    $end_index = rindex($file, "_");
    $file_without_ext = substr($file, $start_index + 1, $end_index - $start_index - 1);
    if (! grep {$_ eq $file_without_ext} @policies) {
        push(@policies, $file_without_ext);
    }
  }
  # display only the policies which are found inside java.iby or javatest.iby
  open (JAVA_IBY, "$IBY_DIR$JAVA_IBY_FILENAME");
  my @java_iby_lines = <JAVA_IBY>;
  close(JAVA_IBY);
  my @java_test_iby_lines;
  if (-f "$IBY_TEST_DIR$JAVA_TEST_IBY_FILENAME")
  {
      open (JAVA_TEST_IBY, "$IBY_TEST_DIR$JAVA_TEST_IBY_FILENAME");
      @java_test_iby_lines = <JAVA_TEST_IBY>;
      close(JAVA_TEST_IBY);
  }
  my @java_variant_policy_iby_lines;
  if (-f "$IBY_DIR$JAVA_VARIANT_POLICY_IBY_FILENAME")
  {
      open (JAVA_VARIANT_POLICY_IBY, "$IBY_DIR$JAVA_VARIANT_POLICY_IBY_FILENAME");
      @java_variant_policy_iby_lines = <JAVA_VARIANT_POLICY_IBY>;
      close(JAVA_VARIANT_POLICY_IBY);
  }
  my @java_variant_certs_policies_iby_lines;
  if (-f "$IBY_DIR$JAVA_VARIANT_CERTS_AND_POLICIES_IBY_FILENAME")
  {
      open (JAVA_VARIANT_CERTS_POLICIES_IBY, "$IBY_DIR$JAVA_VARIANT_CERTS_AND_POLICIES_IBY_FILENAME");
      @java_variant_certs_policies_iby_lines = <JAVA_VARIANT_CERTS_POLICIES_IBY>;
      close(JAVA_VARIANT_CERTS_POLICIES_IBY);
  }
  my $found = 0;
  foreach $policy (@policies) {
      $found = 0;
      foreach $java_iby_line (@java_iby_lines) {
          if (rindex($java_iby_line, "$policy" . "_") >= 0) {
            $found = 1;
            last;
          }
      }
      if ($found == 0)
      {
          foreach $java_variant_policy_iby_line (@java_variant_policy_iby_lines) {
              if (rindex($java_variant_policy_iby_line, "$policy" . "_") >= 0) {
                  $found = 1;
                  last;
              }
          }
      }
      if ($found == 0)
      {
          foreach $java_variant_certs_policies_iby_line (@java_variant_certs_policies_iby_lines) {
              if (rindex($java_variant_certs_policies_iby_line, "$policy" . "_") >= 0) {
                  $found = 1;
                  last;
              }
          }
      }
      #if ($found == 0)
      #{
          #foreach $java_test_iby_line (@java_test_iby_lines) {
              #if (rindex($java_test_iby_line, "$policy" . "_") >= 0) {
                  #$found = 1;
                  #last;
              #}
          #}
      #}
      if ($found == 1)
      {
        print "Policy:\n";
        print "  Name: ", $policy , "\n";
      }
  }
}

##############################################################
##  Removes a certain policy                               ##
##############################################################
sub remove_policy()
{
  $tmp_str = lc $policy_to_remove;
  if (($tmp_str eq "s60") || ($tmp_str eq "msa") || ($tmp_str eq "att"))
  {
      print "\n\nERROR: 's60', 'msa' and 'att' are build in policies and can not be removed.\n";
      exit;
  } 
  $policy_found = 0;
  @files = <$POLICIES_BUILD_DIR_1*_*.ser>;
  foreach $file (@files) {
    $index = rindex($file, $policy_to_remove);
    if ($index >= 0)
    {
      	unlink("$file");
        $policy_found = 1;
    }
  }
  @files = <$POLICIES_BUILD_DIR_2*_*.ser>;
  foreach $file (@files) {
      $index = rindex($file, $policy_to_remove);
      if ($index >= 0)
      {
          $policy_found = 1;
        	unlink("$file");
      }
  }
  if ($policy_found == 0)
  {
      print "\n\nERROR: The policy $policy_to_remove was not found.\n";
      exit;
  }
 # modify java_variant_policy.iby
 mkdir "tmp";
 if (-f "$IBY_DIR$JAVA_VARIANT_POLICY_IBY_FILENAME")
 {
     open (JAVA_IBY, "$IBY_DIR$JAVA_VARIANT_POLICY_IBY_FILENAME");
     open (JAVA_NEW_IBY, "+>tmp/$JAVA_VARIANT_POLICY_IBY_FILENAME") or die "Cannot create tmp file (tmp/$JAVA_VARIANT_POLICY_IBY_FILENAME)\n";
     my @java_iby_lines = <JAVA_IBY>;
     my $found = 0;
     foreach $java_iby_line (@java_iby_lines) {
        if (rindex($java_iby_line, $policy_to_remove) < 0 || rindex($java_iby_line, ".ser") < 0)
        {
            print JAVA_NEW_IBY $java_iby_line;
        }
        else
        {
            $found = 1;
        }
     }
     close(JAVA_IBY);
     close(JAVA_NEW_IBY);
     move("tmp/$JAVA_VARIANT_POLICY_IBY_FILENAME", "$IBY_DIR");
 }
 # same for the combined iby file
 if (-f "$IBY_DIR$JAVA_VARIANT_CERTS_AND_POLICIES_IBY_FILENAME")
 {
     open (JAVA_IBY, "$IBY_DIR$JAVA_VARIANT_CERTS_AND_POLICIES_IBY_FILENAME");
     open (JAVA_NEW_IBY, "+>tmp/$JAVA_VARIANT_CERTS_AND_POLICIES_IBY_FILENAME") or die "Cannot create tmp file (tmp/$JAVA_VARIANT_CERTS_AND_POLICIES_IBY_FILENAME)\n";
     my @java_iby_lines = <JAVA_IBY>;
     my $found = 0;
     foreach $java_iby_line (@java_iby_lines) {
        if (rindex($java_iby_line, $policy_to_remove) < 0 || rindex($java_iby_line, ".ser") < 0)
        {
            print JAVA_NEW_IBY $java_iby_line;
        }
        else
        {
            $found = 1;
        }
     }
     close(JAVA_IBY);
     close(JAVA_NEW_IBY);
     move("tmp/$JAVA_VARIANT_CERTS_AND_POLICIES_IBY_FILENAME", "$IBY_DIR");
 }
 
 # update the S60 configuration tool conf data
 my $xs = new XML::Simple(rootname   => 'configuration', searchpath => ".", suppressempty => '', forcearray => 1);
 my $input_confml = $xs->XMLin("$CONFML_DATA_DIR" ."javasecurity.confml");
 $input_confml->{data}->[0]->{KJavaSecurity}->[0]->{KPolicy}->[0] = 's60';
 delete $input_confml->{feature}->{'Java Security Configuration'}->{setting}->{Policy}->{option}->{"" . "$policy_to_remove"};
 my $output_confml = $xs->XMLout($input_confml);
 open (OUTPUT_CONFML_FILE, "+>tmp/javasecurity.confml") or die "Cannot create tmp file (tmp/javasecurity.confml)\n";
 print OUTPUT_CONFML_FILE   "$output_confml";
 close OUTPUT_CONFML_FILE;
 move("tmp/javasecurity.confml", "$CONFML_DATA_DIR") or die "Can not move tmp/javasecurity.confml to $CONFML_DATA_DIR";
 
 system("rmdir tmp /s /q");
}

sub init
{
    $config_file = "./configdata.xml";

    my $xml = new XML::Simple(suppressempty => '');
    my $xmldata = $xml->XMLin($config_file);
    
    $signing_cert = $xmldata->{signing}->{cert};
    $signing_key = $xmldata->{signing}->{key};
 
    $package_type = $xmldata->{deploytype};
    $s60_version = $xmldata->{s60_version};
    if (($package_type ne "") && ($package_type ne "removable") && ($package_type ne "non-removable"))
    {
        print "\nERROR: when specified, the type of the deployment package can have one of the two values: 'removable' or 'non-removable'. Please check the configuration file $config_file\n";
        exit;
    }
    if (($package_type eq "") || ($package_type eq "non-removable"))
    {
        $package_type = $NON_REMOVABLE_PACKAGE_TYPE;
    }
    else
    {
        $package_type = $REMOVABLE_PACKAGE_TYPE;
    }
}
  

##############################################################
##  Main function                                           ##
##############################################################
sub main
{
    # do initializations
    init();
    
    # parse arguments
    $numArgs = $#ARGV + 1;
    foreach $argnum (0 .. $#ARGV) 
    {
        if ($ARGV[$argnum] eq "addcerts")
        {
            $deployment_destination = $ROM;
            $deploy = 1;
            $iby_file_name = $JAVA_VARIANT_CERTS_IBY_FILENAME;
            $iby_name = "JAVA_VARIANT_CERTS_IBY";
            add_certs();
            print "\n\nAdding of certificates was succesfull.\n";
            exit;
        }
        if ($ARGV[$argnum] eq "listcerts")
        {
            list_certs();
            exit;
        }
        if ($ARGV[$argnum] eq "removecert")
        {
            if ($ARGV[$argnum + 1] eq "")
            {
                next;
            }
            $cert_to_remove = $ARGV[$argnum + 1];
            remove_cert();
            print "\n\nThe certificate $cert_to_remove was succesfully removed.\n";
            exit;
        }
        if ($ARGV[$argnum] eq "deploycerts")
        {
            $deployment_destination = $SIS;
            $deploy = 1;
            $pkg_file_name = $JAVA_VARIANT_CERTS_PKG_FILENAME;
            $pkg_name = "JavaCustomCertificates";
            $sis_file_name = $JAVA_VARIANT_CERTS_SIS_FILENAME;
            add_certs();
            if ($signed_sis eq "true")
            {
                print "\n\nThe deployment package '$JAVA_VARIANT_CERTS_SIS_FILENAME' was succesfully created and it can be installed into a device which has the signing certificate '$signing_cert' in place.\n";
            }
            else
            {
                print "\n\nThe deployment package '$JAVA_VARIANT_CERTS_SIS_FILENAME' was succesfully created. In order to be succesfully deployed into a device the deployment package needs to be signed by Nokia.\n";
            }
            exit;
        }
        if ($ARGV[$argnum] eq "listpolicies")
        {
            list_policies();
            exit;
        }
        if ($ARGV[$argnum] eq "addpolicy")
        {
            $deployment_destination = $ROM;
            $add_header = 1;
            $deploy = 1;
            $iby_file_name = $JAVA_VARIANT_POLICY_IBY_FILENAME;
            $iby_name = "JAVA_VARIANT_POLICY_IBY";
            add_policy();
            print "\n\nAdding of the new policy was succesfull.\n";
            exit;
        }
        if ($ARGV[$argnum] eq "removepolicy")
        {
            if ($ARGV[$argnum + 1] eq "")
            {
                next;
            }
            $policy_to_remove = $ARGV[$argnum + 1];
            remove_policy();
            print "\n\nThe policy $policy_to_remove was succesfully removed.\n";
            exit;
        }
        if ($ARGV[$argnum] eq "deploypolicy")
        {
            $deployment_destination = $SIS;
            $deploy = 1;
            $add_header = 1;
            $pkg_file_name = $JAVA_VARIANT_POLICY_PKG_FILENAME;
            $pkg_name = "JavaCustomSecurityPolicy";
            $sis_file_name = $JAVA_VARIANT_POLICY_SIS_FILENAME;
            add_policy();
            if ($signed_sis eq "true")
            {
                print "\n\nThe deployment package '$JAVA_VARIANT_POLICY_SIS_FILENAME' was succesfully created and it can be installed into a device which has the signing certificate '$signing_cert' in place.\n";
            }
            else
            {
                print "\n\nThe deployment package '$JAVA_VARIANT_POLICY_SIS_FILENAME' was succesfully created. In order to be succesfully deployed into a device the deployment package needs to be signed by Nokia.\n";
            }
            exit;
        }
        if ($ARGV[$argnum] eq "deployall")
        {
            $deployment_destination = $SIS;
            $deploy = 0;
            $pkg_file_name = $JAVA_VARIANT_CERTS_AND_POLICIES_PKG_FILENAME;
            $pkg_name = "JavaCustomSecurityCertsAndPolicy";
            $sis_file_name = $JAVA_VARIANT_CERTS_AND_POLICIES_SIS_FILENAME;
            add_certs();
            $add_header = 0;
            $deploy = 1;
            add_policy();
            if ($signed_sis eq "true")
            {
                print "\n\nThe deployment package '$JAVA_VARIANT_CERTS_AND_POLICIES_SIS_FILENAME' was succesfully created and it can be installed into a device which has the signing certificate '$signing_cert' in place.\n";
            }
            else
            {
                print "\n\nThe deployment package '$JAVA_VARIANT_CERTS_AND_POLICIES_SIS_FILENAME' was succesfully created. In order to be succesfully deployed into a device the deployment package needs to be signed by Nokia.\n";
            }
            exit;
        }
        if ($ARGV[$argnum] eq "addall")
        {
            $deployment_destination = $ROM;
            $deploy = 0;
            $iby_file_name = $JAVA_VARIANT_CERTS_AND_POLICIES_IBY_FILENAME;
            $iby_name = "JAVA_VARIANT_CERTS_POLICIES_IBY";
            add_certs();
            $add_header = 0;
            $deploy = 1;
            add_policy();
            print "\n\nAdding of the new certificates and policy was succesfull.\n";
            exit;
        }
        if ($ARGV[$argnum] eq "setpolicy")
        {
            set_policy();
            if ($signed_sis eq "true")
            {
                print "\n\nThe deployment package '$JAVA_CUSTOM_SECURITY_SIS_FILENAME' was succesfully created and it can be installed into a device which has the signing certificate '$signing_cert' in place.\n";
            }
            else
            {
                print "\n\nThe deployment package '$JAVA_CUSTOM_SECURITY_SIS_FILENAME' was succesfully created. In order to be succesfully deployed into a device the deployment package needs to be signed by Nokia.\n";
            }
            exit;
        }
        if ($ARGV[$argnum] eq "setwarningsmode")
        {
            set_warnings_mode();
            if ($signed_sis eq "true")
            {
                print "\n\nThe deployment package '$JAVA_CUSTOM_SECURITY_SIS_FILENAME' was succesfully created and it can be installed into a device which has the signing certificate '$signing_cert' in place.\n";
            }
            else
            {
                print "\n\nThe deployment package '$JAVA_CUSTOM_SECURITY_SIS_FILENAME' was succesfully created. In order to be succesfully deployed into a device the deployment package needs to be signed by Nokia.\n";
            }
            exit;
        }
    }
    usage();
}

&main();