#!/bin/perl
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
# script to process one or more changes implemented via a single Synergy task importing code from an external system (e.g., Subversion or  a zip file):
#	- checks that the task is in completed state
#	- associates the task to the change object
#	- (optionally) sets "external task id(s)" -attribute to a value supplied
#	- (optionally) moves the change to the next state (provided on the command line), including setting modifiable_in to integration_db
#
# Usage 1: 	If a change id is provided as argument, the script process a single change object as above
#			perl change2released.pl -user username -change "xx1xx1#23456" -task 213 
#
# Usage 2: 	If an input file is provided as arguments, the script will read change ids (and optional external task ids) from the filw and
#		process all the changes listed in the file
#		with those tasks and transition the change to the target state 
#
#

my $program_name;
BEGIN {
    # Get the name of the program executable:
    ($program_name) = $0 =~ m{([^/]+)$};
}

my $VERSION = do { (my $v = q%version: 1 %) =~ s/.*://; sprintf("%d.%02d", 0, split(/\./, $v)) };


use FindBin qw($Bin $Script);
use lib "$Bin/lib";

use ChangeSynergy::csapi;
use Getopt::Long;
use Data::Dumper;

my $username = "";
my $password = "";
my $crId = "";
my $taskId = "";
my $newState = "";
my $protocol = "https";
#my $host = "sachswww.nmp.nokia.com";
my $host = "trchswww.nmp.nokia.com";
#my $port = "58613";
my $port = "58602";
#my $db = "/nokia/sa_nmp/groups/gscm/dbs/sa1trqa2";
my $db = "/nokia/tr_nmp/groups/gscm/dbs/tr1jrt";
my $inFile = "";

# read options
my $result = GetOptions ("user=s" => \$username,
                         "password=s" => \$password,
                         "debug" => \$debug,
                         "preview" => \$preview,
                         "version" => \$show_version,
                         "protocol=s" => \$protocol,
                         "host=s" => \$host,
                         "port=s" => \$port,
                         "db=s" => \$db,
                         "in=s" => \$inFile,
                         "state=s" => \$newState,
                         "task=s" => \$taskId,
                         "change=s" => \$crId);

print "$program_name ver. $VERSION\n";

# Handle standard command line parameters
if ($show_version) {
	# Exit immediately after having printed version information
	exit 0;
} 

if (!$taskId)
{
	die "Error: No Task Id provided\n";
}

# Get the password from the terminal unless already provided on the command line
unless ($password) {
	eval {
		require Term::ReadKey;
		print STDERR "CIM Password for user '$username' (input will be hidden): "; 
		Term::ReadKey::ReadMode('noecho');
		$password = Term::ReadKey::ReadLine(0);
		Term::ReadKey::ReadMode('normal'); 
		print STDERR "\n";
	};
	if ($@) {
		print STDERR "Warning: Perl module Term::ReadKey not available." .
			" Password echo cannot be switched off!\n";
		print STDERR "CIM Password for user '$username': "; 
		$password = <STDIN>;
	}		
	chomp $password;
}


print "Using $protocol://$host:$port, database $db\n";

my $externalTaskAttribute = "external_task_ids";

my $aUser;

my $csapi = new ChangeSynergy::csapi();
my $globals = new ChangeSynergy::Globals();

sub displayCRData
{
	my $cr = shift;
	my $j = $cr->getDataSize();
	for($i=0;$i<$j;$i++)
	{
		print "\t" . $cr->getDataObject($i)->getName() . ": " . $cr->getDataObject($i)->getValue() . "\n";
	}
} 

sub associateTaskToChange
{
	my $crId = shift;
	my $taskId = shift;
	
	# check if the task exists
	my $tmp = $csapi->QueryStringData($aUser, "Basic Summary", "cvtype='task' and task('$taskId\')", "task_synopsis|status|release");
	my $j = $tmp->getDataSize();
	if ($j)
	{
		my $taskData = $tmp->getDataObject(0);
		my $taskStatus = $taskData->getDataObjectByName("status")->getValue();
		my $taskRelease = $taskData->getDataObjectByName("release")->getValue();
		if ($debug)
		{
			for($i=0;$i<$j;$i++)
			{
				print "\tinstance:\t" . $taskData->getDataObjectByName("instance")->getValue() . "\n";
				print "\tstatus:  \t" . "$taskStatus\n";
				print "\trelease:  \t" . "$taskRelease\n";
				print "\tsynopsis:\t" . $taskData->getDataObjectByName("task_synopsis")->getValue() . "\n";
				#my $j2 = $taskData->getDataSize();
				#for($i2=0;$i2<$j2;$i2++)
				#{
				#	print "\t" . $taskData->getDataObject($i2)->getName() . ": " . $taskData->getDataObject($i2)->getValue() . "\n";
				#}
			}
		}

		if ($taskStatus ne 'completed')
		{
			die "Error: task is not completed\n";
		}
		
		if (!$preview)
		{
			print "Associating Task \'$taskId\' to Change \'$crId\'\n";

			my $result = $csapi->CreateRelation($aUser, "TRUE", $crId, $taskId, "associated_task", $globals->{CCM_PROBLEM_TASK});
			print $result->getResponseData() . "\n";
		}
		else
		{
			print "Would associate Task \'$taskId\' to Change \'$crId\'\n";
		}
	}
	else
	{
		die "Error: task \'$taskId\' does not exist\n";
	}
} 

# main program 

eval
{
	$csapi->setUpConnection("$protocol", "$host", "$port");

	$aUser = $csapi->Login("$username", "$password", "User", "$db");

  print "eval11\n";

	# 1. find all the change objects to process
	
	my @crTable = ();
	my %extTaskIdTable = ();

	my $query = "";
	if ($crId)
	{
		if ($debug)
		{
			print "Using Change id: \"$crId\"\n";
		}
        push(@crTable, $crId);
	}
	elsif ($inFile)
	{
		my $fh;
		my $extTaskId = "";
		
		open($fh, $inFile) or die "Failed to read input file $inFile: $!\n";
		if ($debug)
		{
			print "Using input file: \"$inFile\"\n";
		}
		while (<$fh>) {
			chomp;

			# Skip comments and blanks
			next if (substr($_, 0, 1) eq '#' || /^\s*$/);

			($crId, $extTaskId) = split(/[\s]+/, $_);
			
			if ($debug)
			{
				print "Read Change id \"$crId\"\n";
				if ($extTaskId) 
				{
					print "- external task id \"$extTaskId\"\n";
				}
			}
			push(@crTable, $crId);
			if ($extTaskId) 
			{
				$extTaskIdTable{$crId} = $extTaskId;
			}
		}
	}
	else
	{
        die "No change id or input file provided\n";
	}

	# 2. Loop over the change objects and process them
	
	foreach $crId (@crTable)
	{
		my @taskList = ();
		
		print "Processing change: \'$crId\'\n";
	
		# 3. find the tasks to associate with this change object

		if ($taskId)
		{
			# use the one from the command line
			print "Using task: \'$taskId\'\n";
			push(@taskList, $taskId);
		}

		# 4: 	associate all tasks and change the state
		# 	bail out on failure and move to the next CR
		eval
		{
			foreach $t (@taskList)
			{
				print "Processing task \'$t\'\n";
				&associateTaskToChange($crId, $t);
			}

			my $externalTaskId = $extTaskIdTable{$crId};

			my $crData = $csapi->GetCRData($aUser, $crId, "crstatus|integration_db|modifiable_in|$externalTaskAttribute");
			
			if ($newState)
			{
				my $crStatus = "change_" . $newState;

				if (!$preview)
				{
					print "Transitioning Change \'$crId\' to $crStatus\n";
				}
				else
				{
					print "Would transition Change \'$crId\' to $crStatus\n";
				}

				# get the transition form
				
				# my $crData = $csapi->GetCRData($aUser, $crId, "crstatus|integration_db|modifiable_in|$externalTaskAttribute");
				my $tmp = $csapi->TransitionCRData($aUser, $crId, $crData->getTransitionLink(1)->getTransition());
				if ($debug)
			 	{
					&displayCRData($tmp);
				}

				# set the status on the form
				$tmp->getDataObjectByName("crstatus")->setValue($crStatus);
		  		# $tmp->getDataObjectByName("crstatus")->setValue($cr->getTransitionLink(1)->getToState());

				if (!$preview)
				{
					# transition the CR
					$result = $csapi->TransitionCR($aUser, $tmp);
					print $result->getResponseData() . "\n";
				}

				# and finally, set the modifiable in
				
				my $integration_db = $crData->getDataObjectByName("integration_db")->getValue();
				if (!$preview)
				{
					print "Setting modifiable_in to " . $integration_db . "\n";

					$crData->getDataObjectByName("modifiable_in")->setValue($integration_db);
				}
				else
				{
					print "Would set modifiable_in to " . $integration_db . "\n";
				}
			}

			if (!$preview)
			{
				if ($externalTaskId)
				{
					print "Setting $externalTaskAttribute to " . $externalTaskId . "\n";

					$crData->getDataObjectByName($externalTaskAttribute)->setValue($externalTaskId);
				}

				$result = $csapi->ModifyCR($aUser, $crData);
				print $result->getResponseData() . "\n";
			}
			else
			{
				if ($externalTaskId)
				{
					print "Would set $externalTaskAttribute to " . $externalTaskId . "\n";
				}
			}
		};

		if ($@)
		{
			print $@;
		}
	}
};

if ($@)
{
	print $@;
}

