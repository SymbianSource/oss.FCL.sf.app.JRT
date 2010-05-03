/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32def.h>
#include <jni.h>
#include <string.h> // Needed for strcmp 

#ifdef RD_JAVA_JXE_LOOKUP
#include <j9comp.h>
extern "C"
{
    extern const U_64 j9Jar[];

    void* J9GetJXE()
    {
        return const_cast<void*>(reinterpret_cast<const void*>(j9Jar));
    }
}
#endif

#include "org_eclipse_swt_widgets_DCControl.h"


typedef void (*TFunc)();

typedef struct
{
    char          *token;
    unsigned int   procaddr;
} TABLE_ENTRY;

// lookup_table MUST be defined as 'const' or this file will cause a build error
// when comipling for the ARMI target
const TABLE_ENTRY lookup_table[]=
{
    { "Java_org_eclipse_swt_widgets_DCControl__1createDCControl", (unsigned int) Java_org_eclipse_swt_widgets_DCControl__1createDCControl },
    { "Java_org_eclipse_swt_widgets_DCControl__1drawNow", (unsigned int) Java_org_eclipse_swt_widgets_DCControl__1drawNow },
    { "Java_org_eclipse_swt_widgets_DCControl__1getControlHandle", (unsigned int) Java_org_eclipse_swt_widgets_DCControl__1getControlHandle },
    { "Java_org_eclipse_swt_widgets_DCControl__1setFullScreen", (unsigned int) Java_org_eclipse_swt_widgets_DCControl__1setFullScreen },
};

// table_size MUST be defined as 'const' or this file will cause a build error
// when comipling for the ARMI target
const int table_size = sizeof(lookup_table) / sizeof(TABLE_ENTRY);


IMPORT_C TFunc jni_lookup(const char* aName);
EXPORT_C TFunc jni_lookup(const char* aName)
{
    int       res=0;
    int       mid=0;
    int       top=0;
    int       bottom=table_size-1;

#ifdef RD_JAVA_JXE_LOOKUP
    if (!strcmp(aName, "J9GetJXE")) return (TFunc)J9GetJXE;
#endif
    // Loop while the number of the items left in the list is greater
    // than 2.  Each iteration will split the number of items left to search
    // in half
    while ((bottom - top) > 1)
    {
        // This case handles the normal serach case where the number of
        // items left to search is greater than 2
        mid=(top+bottom)/2;
        res=strcmp(aName,lookup_table[mid].token);
        if (res==0) return((TFunc) lookup_table[mid].procaddr);
        if (res>0) top=mid;
        else bottom=mid;
    }

    // If there are two items left in the list then the bottom item should be
    // checked for a match
    if (bottom != top)
    {
        // Check the bottom item to see if it is a match
        res=strcmp(aName,lookup_table[bottom].token);
        if (res == 0) return ((TFunc) lookup_table[bottom].procaddr);
    }

    // Check the top item to see if it is a match
    res=strcmp(aName,lookup_table[top].token);

    if (res == 0) return ((TFunc) lookup_table[top].procaddr);

    // Neither the top or bottom items were a match so the
    // method must not exist in the file
    return NULL;
}

