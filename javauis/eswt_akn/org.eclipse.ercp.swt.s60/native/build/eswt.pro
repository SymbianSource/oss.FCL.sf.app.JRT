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

TEMPLATE=lib
TARGET=eswt
CONFIG += omj java stl
CONFIG -= qt

INCLUDEPATH +=  /epoc32/include/domain/applications \
                /epoc32/include/applications \
                /epoc32/include/domain/middleware \
                ../inc 
SOURCES += ../src/cswtclient.cpp \
           ../src/methodcall.cpp \
           ../src/org_eclipse_ercp_swt_expanded_internal_OS.cpp \
           ../src/org_eclipse_ercp_swt_mobile_internal_OS.cpp \
           ../src/org_eclipse_swt_internal_symbian_OS.cpp \
           ../src/swtbitmapgc.cpp \
           ../src/swtbrowser.cpp \
           ../src/swtbrowserconnectionmanager.cpp \
           ../src/swtbrowserconnectionobserver.cpp \
           ../src/swtbrowserconnector.cpp \
           ../src/swtbrowsercontainer.cpp \
           ../src/swtbrowserlayoutobserver.cpp \
           ../src/swtbrowserlinkresolver.cpp \
           ../src/swtbrowserloadeventobserver.cpp \
           ../src/swtbrowserpreferences.cpp \
           ../src/swtbrowsersoftkeyobservers.cpp \
           ../src/swtbrowserspecialloadobserver.cpp \
           ../src/swtbrowserstatechangeobserver.cpp \
           ../src/swtbutton.cpp \
           ../src/swtcanvas.cpp \
           ../src/swtcaptionedcontrol.cpp \
           ../src/swtclient.cpp \
           ../src/swtclipboard.cpp \
           ../src/swtcolor.cpp \
           ../src/swtcolordialog.cpp \
           ../src/swtcombo.cpp \
           ../src/swtcommand.cpp \
           ../src/swtcommandarranger.cpp \
           ../src/swtcommandcontainer.cpp \
           ../src/swtcommandmenu.cpp \
           ../src/swtcommonfiledialogs.cpp \
           ../src/swtcomposite.cpp \
           ../src/swtconstrainedtext.cpp \
           ../src/swtcontrolbase.cpp \
           ../src/swtcontrolhelper.cpp \
           ../src/swtcontrolmenu.cpp \
           ../src/swtdateeditor.cpp \
           ../src/swtdecorations.cpp \
           ../src/swtdialogbroker.cpp \
           ../src/swtdirectorydialog.cpp \
           ../src/swtdisplay.cpp \
           ../src/swtdisplaybase.cpp \
           ../src/swteditindicator.cpp \
           ../src/swtedwin.cpp \
           ../src/swtedwincustomdrawer.cpp \
           ../src/swteventqueue.cpp \
           ../src/swtevents.cpp \
           ../src/swtfactory.cpp \
           ../src/swtfiledialog.cpp \
           ../src/swtflipwatch.cpp \
           ../src/swtfont.cpp \
           ../src/swtgcbase.cpp \
           ../src/swtgrid.cpp \
           ../src/swtgrimagedata.cpp \
           ../src/swtgrpalettedata.cpp \
           ../src/swthyperlink.cpp \
           ../src/swtimage.cpp \
           ../src/swtimagedata.cpp \
           ../src/swtimagedataloader.cpp \
           ../src/swtimagetextitem.cpp \
           ../src/swtinput.cpp \
           ../src/swtinstancecounts.cpp \
           ../src/swtjavabufferdrawer.cpp \
           ../src/swtkeymapper.cpp \
           ../src/swtlabel.cpp \
           ../src/swtlist.cpp \
           ../src/swtlink.cpp \
           ../src/swtlistbase.cpp \
           ../src/swtlistbox.cpp \
           ../src/swtlistboxlists.cpp \
           ../src/swtlistboxlistsitemdrawer.cpp \           
           ../src/swtlistview.cpp \
           ../src/swtmaskhandler.cpp \
           ../src/swtmenu.cpp \
           ../src/swtmenuarranger.cpp \
           ../src/swtmenubar.cpp \
           ../src/swtmenuitem.cpp \
           ../src/swtmessagebox.cpp \
           ../src/swtmobiledevice.cpp \
           ../src/swtmobileshell.cpp \
           ../src/swtmultipagedialog.cpp \
           ../src/swtpalettedata.cpp \
           ../src/swtpointereventsuppressor.cpp \
           ../src/swtprogressbar.cpp \
           ../src/swtquerydialog.cpp \
           ../src/swtrotateimage.cpp \
           ../src/swtscreen.cpp \
           ../src/swtscrollablebase.cpp \
           ../src/swtscrollbar.cpp \
           ../src/swtserver.cpp \
           ../src/swtshell.cpp \
           ../src/swtslider.cpp \
           ../src/swtsortedlist.cpp \
           ../src/swtstickykeyshandler.cpp \
           ../src/swttable.cpp \
           ../src/swttablecolumn.cpp \
           ../src/swttableheader.cpp \
           ../src/swttableitem.cpp \
           ../src/swttableitemdrawer.cpp \
           ../src/swttablelistbox.cpp \
           ../src/swttablelistboxview.cpp \
           ../src/swttasktip.cpp \
           ../src/swttext.cpp \
           ../src/swttextbase.cpp \
           ../src/swttextextension.cpp \
           ../src/swttimedmessagebox.cpp \
           ../src/swttimer.cpp \
           ../src/swttree.cpp \
           ../src/swtuiutils.cpp \
           ../src/swtwindowgc.cpp \
           ../src/utils.cpp


         
# Symbian version is created from Qt version 'major . (minor << 8) + patch'
VERSION = 12.74.121 # "12.74.121 --> 12.19065 on symbian"

MMP_RULES += "SOURCEPATH      ../src"
MMP_RULES += "START RESOURCE  eswtcore.rss"
MMP_RULES += "TARGETPATH      resource/java"
MMP_RULES += "HEADER"
MMP_RULES += "LANGUAGE_IDS"
MMP_RULES += "END // RESOURCE"
MMP_RULES += "START RESOURCE  eswt_reg.rss"
MMP_RULES += "TARGETPATH      private/10003a3f/apps"
MMP_RULES += "END // RESOURCE"



LIBS += -laknicon \
        -laknhlist \
        -laknlayout \
        -laknpictograph \
        -laknskins \
        -lapengine \
        -lapgrfx \
        -lapmime \
        -lapparc \
        -lapsettingshandlerui \
        -lavkon \
        -lbafl \
        -lbitgdi \
        -lbitmaptransforms \
        -lbmpanim \
        -lcdlengine \
        -lcentralrepository \
        -lcommdb \
        -lcommondialogs \
        -lcommonengine \
        -lcommonui \
        -lcone \
        -lecom \
        -lefsrv \
        -legul \
        -leikcdlg \
        -leikcoctl \
        -leikcore \
        -leikctl \
        -leikdlg \
        -leiksrvc \
        -lesock \
        -lestlib \
        -lestor \
        -leswtapifacade \
        -letel \
        -letelmm \
        -letext \
        -leuser \
        -lfavouritesengine \
        -lfbscli \
        -lfeatdiscovery \
        -lfeatmgr \
        -lfntstr \
        -lfepbase \
        -lform \
        -lgdi \
        -lhal \
        -limageconversion \
        -linetprotutil \
        -lpbkeng \
        -lpbkview \
        -lservicehandler \
        -lsysutil \
        -ltagma \
        -lws32 \
        -ltouchfeedback \
        -laknlistloadertfx \
        -lptiengine \
        -ljavacoreui \
        -ljavautils \
        -lalfclient \
        -lgfxtrans \
        -luiklaf

include(../../../../../build/omj.pri)
