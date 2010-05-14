/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Tests for function server
*
*/

#include "functionserver.h"
#include "fs_methodcall.h"

#include "TestHarness.h"
#include "exceptionbase.h"
#include "logger.h"
using namespace java::util;

int sIvokedMethod = -1;
int sA = -1;
int sB = -1;
int sC = -1;
int sD = -1;
int sE = -1;
int sF = -1;
int sG = -1;
int sH = -1;
int sI = -1;
int sIndex = 0;
int sServerSideInitCalled = false;
int sServerSideCleanCalled = false;
int sStartServerCalled = false;

void localMethod()
{sIvokedMethod = 1;}
void localMethod1(int a)
    {sIvokedMethod = 2; sA = a;} 
void localMethod2(int a, int b)
    {sIvokedMethod = 3; sA = a; sB = b;} 
void localMethod3(int a, int b, int c)
    {sIvokedMethod = 4; sA = a; sB = b; sC = c;} 
void localMethod4(int a, int b, int c, int d)
    {sIvokedMethod = 5; sA = a; sB = b; sC = c; sD = d;} 
void localMethod5(int a, int b, int c, int d, int e)
    {sIvokedMethod = 6; sA = a; sB = b; sC = c; sD = d; sE = e;} 
void localMethod6(int a, int b, int c, int d, int e, int f)
    {sIvokedMethod = 7; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f;} 
void localMethod7(int a, int b, int c, int d, int e, int f, int g)
    {sIvokedMethod = 8; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g;}
void localMethod8(int a, int b, int c, int d, int e, int f, int g, int h)
    {sIvokedMethod = 9; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g; sH = h;}
void localMethod9(int a, int b, int c, int d, int e, int f, int g, int h, int i)
    {sIvokedMethod = 10; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g; sH = h; sI = i;}

int localMethodR()
    {sIvokedMethod = 11;return sIvokedMethod;}
int localMethodR1(int a)
    {sIvokedMethod = 12; sA = a;return sIvokedMethod;}
int localMethodR2(int a, int b)
    {sIvokedMethod = 13; sA = a; sB = b;return sIvokedMethod;}
int localMethodR3(int a, int b, int c)
    {sIvokedMethod = 14; sA = a; sB = b; sC = c;return sIvokedMethod;}
int localMethodR4(int a, int b, int c, int d)
    {sIvokedMethod = 15; sA = a; sB = b; sC = c; sD = d;return sIvokedMethod;}
int localMethodR5(int a, int b, int c, int d, int e)
    {sIvokedMethod = 16; sA = a; sB = b; sC = c; sD = d; sE = e;return sIvokedMethod;}
int localMethodR6(int a, int b, int c, int d, int e, int f)
    {sIvokedMethod = 17; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f;return sIvokedMethod;}
int localMethodR7(int a, int b, int c, int d, int e, int f, int g)
    {sIvokedMethod = 18; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g;return sIvokedMethod;}
int localMethodR8(int a, int b, int c, int d, int e, int f, int g, int h)
    {sIvokedMethod = 19; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g; sH = h;return sIvokedMethod;}
int localMethodR9(int a, int b, int c, int d, int e, int f, int g, int h, int i) 
    {sIvokedMethod = 20; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g; sH = h; sI = i;return sIvokedMethod;}

void localMethodTL()
    {sIvokedMethod = 21; }
void localMethodT1L(int a)
    {sIvokedMethod = 22; sA = a;}
void localMethodT2L(int a, int b)
    {sIvokedMethod = 23; sA = a; sB = b;}
void localMethodT3L(int a, int b, int c)
    {sIvokedMethod = 24; sA = a; sB = b; sC = c;}
void localMethodT4L(int a, int b, int c, int d)
    {sIvokedMethod = 25; sA = a; sB = b; sC = c; sD = d;}
void localMethodT5L(int a, int b, int c, int d, int e)
    {sIvokedMethod = 26; sA = a; sB = b; sC = c; sD = d; sE = e;}
void localMethodT6L(int a, int b, int c, int d, int e, int f)
    {sIvokedMethod = 27; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f;}
void localMethodT7L(int a, int b, int c, int d, int e, int f, int g)
    {sIvokedMethod = 28; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g;}
void localMethodT8L(int a, int b, int c, int d, int e, int f, int g, int h)
    {sIvokedMethod = 29; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g; sH = h;}
void localMethodT9L(int a, int b, int c, int d, int e, int f, int g, int h, int i)
    {sIvokedMethod = 30; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g; sH = h; sI = i;}

void localLeavingMethodTL()
    {sIvokedMethod = 31; User::Leave(-sIvokedMethod);}
void localLeavingMethodT1L(int a)
    {sIvokedMethod = 32; sA = a;User::Leave(-sIvokedMethod);}
void localLeavingMethodT2L(int a, int b)
    {sIvokedMethod = 33; sA = a; sB = b;User::Leave(-sIvokedMethod);}
void localLeavingMethodT3L(int a, int b, int c)
    {sIvokedMethod = 34; sA = a; sB = b; sC = c;User::Leave(-sIvokedMethod);}
void localLeavingMethodT4L(int a, int b, int c, int d)
    {sIvokedMethod = 35; sA = a; sB = b; sC = c; sD = d;User::Leave(-sIvokedMethod);}
void localLeavingMethodT5L(int a, int b, int c, int d, int e)
    {sIvokedMethod = 36; sA = a; sB = b; sC = c; sD = d; sE = e;User::Leave(-sIvokedMethod);}
void localLeavingMethodT6L(int a, int b, int c, int d, int e, int f)
    {sIvokedMethod = 37; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f;User::Leave(-sIvokedMethod);}
void localLeavingMethodT7L(int a, int b, int c, int d, int e, int f, int g)
    {sIvokedMethod = 38; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g;User::Leave(-sIvokedMethod);}
void localLeavingMethodT8L(int a, int b, int c, int d, int e, int f, int g, int h)
    {sIvokedMethod = 39; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g; sH = h;User::Leave(-sIvokedMethod);}
void localLeavingMethodT9L(int a, int b, int c, int d, int e, int f, int g, int h, int i)
    {sIvokedMethod = 40; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g; sH = h; sI = i;User::Leave(-sIvokedMethod);}


class TestClass
{
public:
    void classMethod()
        {sIvokedMethod = 41;}
    void classMethod1(int a)
        {sIvokedMethod = 42; sA = a;}
    void classMethod2(int a, int b)
        {sIvokedMethod = 43; sA = a; sB = b;}
    void classMethod3(int a, int b, int c)
        {sIvokedMethod = 44; sA = a; sB = b; sC = c;}
    void classMethod4(int a, int b, int c, int d)
        {sIvokedMethod = 45; sA = a; sB = b; sC = c; sD = d;}
    void classMethod5(int a, int b, int c, int d, int e)
        {sIvokedMethod = 46; sA = a; sB = b; sC = c; sD = d; sE = e;}
    void classMethod6(int a, int b, int c, int d, int e, int f)
        {sIvokedMethod = 47; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f;}
    void classMethod7(int a, int b, int c, int d, int e, int f, int g)
        {sIvokedMethod = 48; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g;}

    int classMethodR()
        {sIvokedMethod = 51;return sIvokedMethod;}
    int classMethodR1(int a)
        {sIvokedMethod = 52; sA = a;return sIvokedMethod;}
    int classMethodR2(int a, int b)
        {sIvokedMethod = 53; sA = a; sB = b;return sIvokedMethod;}
    int classMethodR3(int a, int b, int c)
        {sIvokedMethod = 54; sA = a; sB = b; sC = c;return sIvokedMethod;}
    int classMethodR4(int a, int b, int c, int d)
        {sIvokedMethod = 55; sA = a; sB = b; sC = c; sD = d;return sIvokedMethod;}
    int classMethodR5(int a, int b, int c, int d, int e)
        {sIvokedMethod = 56; sA = a; sB = b; sC = c; sD = d; sE = e;return sIvokedMethod;}
    int classMethodR6(int a, int b, int c, int d, int e, int f)
        {sIvokedMethod = 57; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f;return sIvokedMethod;}
    int classMethodR7(int a, int b, int c, int d, int e, int f, int g)
        {sIvokedMethod = 58; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g;return sIvokedMethod;}

    void classMethodL()
        {sIvokedMethod = 61;}
    void classMethodL1(int a)
        {sIvokedMethod = 62; sA = a;}
    void classMethodL2(int a, int b)
        {sIvokedMethod = 63; sA = a; sB = b;}
    void classMethodL3(int a, int b, int c)
        {sIvokedMethod = 64; sA = a; sB = b; sC = c;}
    void classMethodL4(int a, int b, int c, int d)
        {sIvokedMethod = 65; sA = a; sB = b; sC = c; sD = d;}
    void classMethodL5(int a, int b, int c, int d, int e)
        {sIvokedMethod = 66; sA = a; sB = b; sC = c; sD = d; sE = e;}
    void classMethodL6(int a, int b, int c, int d, int e, int f)
        {sIvokedMethod = 67; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f;}
    void classMethodL7(int a, int b, int c, int d, int e, int f, int g)
        {sIvokedMethod = 68; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g;}

    void classMethodWithLeaveL()
        {sIvokedMethod = 71;User::Leave(-sIvokedMethod);}
    void classMethodWithLeave1L(int a)
        {sIvokedMethod = 72; sA = a;User::Leave(-sIvokedMethod);}
    void classMethodWithLeave2L(int a, int b)
        {sIvokedMethod = 73; sA = a; sB = b;User::Leave(-sIvokedMethod);}
    void classMethodWithLeave3L(int a, int b, int c)
        {sIvokedMethod = 74; sA = a; sB = b; sC = c;User::Leave(-sIvokedMethod);}
    void classMethodWithLeave4L(int a, int b, int c, int d)
        {sIvokedMethod = 75; sA = a; sB = b; sC = c; sD = d;User::Leave(-sIvokedMethod);}
    void classMethodWithLeave5L(int a, int b, int c, int d, int e)
        {sIvokedMethod = 76; sA = a; sB = b; sC = c; sD = d; sE = e;User::Leave(-sIvokedMethod);}
    void classMethodWithLeave6L(int a, int b, int c, int d, int e, int f)
        {sIvokedMethod = 77; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f;User::Leave(-sIvokedMethod);}
    void classMethodWithLeave7L(int a, int b, int c, int d, int e, int f, int g)
        {sIvokedMethod = 78; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g;User::Leave(-sIvokedMethod);}

    int classMethodRL()
        {sIvokedMethod = 81;return sIvokedMethod;}
    int classMethodR1L(int a)
        {sIvokedMethod = 82; sA = a;return sIvokedMethod;}
    int classMethodR2L(int a, int b)
        {sIvokedMethod = 83; sA = a; sB = b;return sIvokedMethod;}
    int classMethodR3L(int a, int b, int c)
        {sIvokedMethod = 84; sA = a; sB = b; sC = c;return sIvokedMethod;}
    int classMethodR4L(int a, int b, int c, int d)
        {sIvokedMethod = 85; sA = a; sB = b; sC = c; sD = d;return sIvokedMethod;}
    int classMethodR5L(int a, int b, int c, int d, int e)
        {sIvokedMethod = 86; sA = a; sB = b; sC = c; sD = d; sE = e;return sIvokedMethod;}
    int classMethodR6L(int a, int b, int c, int d, int e, int f)
        {sIvokedMethod = 87; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f;return sIvokedMethod;}
    int classMethodR7L(int a, int b, int c, int d, int e, int f, int g)
        {sIvokedMethod = 88; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g;return sIvokedMethod;}

    int classMethodWithLeaveRL()
        {sIvokedMethod = 91;User::Leave(-sIvokedMethod);return 0;}
    int classMethodWithLeaveR1L(int a)
        {sIvokedMethod = 92; sA = a;User::Leave(-sIvokedMethod);return 0;}
    int classMethodWithLeaveR2L(int a, int b)
        {sIvokedMethod = 93; sA = a; sB = b;User::Leave(-sIvokedMethod);return 0;}
    int classMethodWithLeaveR3L(int a, int b, int c)
        {sIvokedMethod = 94; sA = a; sB = b; sC = c;User::Leave(-sIvokedMethod);return 0;}
    int classMethodWithLeaveR4L(int a, int b, int c, int d)
        {sIvokedMethod = 95; sA = a; sB = b; sC = c; sD = d;User::Leave(-sIvokedMethod);return 0;}
    int classMethodWithLeaveR5L(int a, int b, int c, int d, int e)
        {sIvokedMethod = 96; sA = a; sB = b; sC = c; sD = d; sE = e;User::Leave(-sIvokedMethod);return 0;}
    int classMethodWithLeaveR6L(int a, int b, int c, int d, int e, int f)
        {sIvokedMethod = 97; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f;User::Leave(-sIvokedMethod);return 0;}
    int classMethodWithLeaveR7L(int a, int b, int c, int d, int e, int f, int g)
        {sIvokedMethod = 98; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g;User::Leave(-sIvokedMethod);return 0;}



    void classMethodC() const
        {sIvokedMethod = 101;}
    void classMethodC1(int a) const
        {sIvokedMethod = 102; sA = a;}
    void classMethodC2(int a, int b) const
        {sIvokedMethod = 103; sA = a; sB = b;}
    void classMethodC3(int a, int b, int c) const
        {sIvokedMethod = 104; sA = a; sB = b; sC = c;}
    void classMethodC4(int a, int b, int c, int d) const
        {sIvokedMethod = 105; sA = a; sB = b; sC = c; sD = d;}
    void classMethodC5(int a, int b, int c, int d, int e) const
        {sIvokedMethod = 106; sA = a; sB = b; sC = c; sD = d; sE = e;}
    void classMethodC6(int a, int b, int c, int d, int e, int f) const
        {sIvokedMethod = 107; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f;}
    void classMethodC7(int a, int b, int c, int d, int e, int f, int g) const
        {sIvokedMethod = 108; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g;}

    int classMethodRC() const
        {sIvokedMethod = 111;return sIvokedMethod;}
    int classMethodRC1(int a) const
        {sIvokedMethod = 112; sA = a;return sIvokedMethod;}
    int classMethodRC2(int a, int b) const
        {sIvokedMethod = 113; sA = a; sB = b;return sIvokedMethod;}
    int classMethodRC3(int a, int b, int c) const
        {sIvokedMethod = 114; sA = a; sB = b; sC = c;return sIvokedMethod;}
    int classMethodRC4(int a, int b, int c, int d) const
        {sIvokedMethod = 115; sA = a; sB = b; sC = c; sD = d;return sIvokedMethod;}
    int classMethodRC5(int a, int b, int c, int d, int e) const
        {sIvokedMethod = 116; sA = a; sB = b; sC = c; sD = d; sE = e;return sIvokedMethod;}
    int classMethodRC6(int a, int b, int c, int d, int e, int f) const
        {sIvokedMethod = 117; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f;return sIvokedMethod;}
    int classMethodRC7(int a, int b, int c, int d, int e, int f, int g) const
        {sIvokedMethod = 118; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g;return sIvokedMethod;}

    void classMethodCL() const
        {sIvokedMethod = 121;}
    void classMethodC1L(int a) const
        {sIvokedMethod = 122; sA = a;}
    void classMethodC2L(int a, int b) const
        {sIvokedMethod = 123; sA = a; sB = b;}
    void classMethodC3L(int a, int b, int c) const
        {sIvokedMethod = 124; sA = a; sB = b; sC = c;}
    void classMethodC4L(int a, int b, int c, int d) const
        {sIvokedMethod = 125; sA = a; sB = b; sC = c; sD = d;}
    void classMethodC5L(int a, int b, int c, int d, int e) const
        {sIvokedMethod = 126; sA = a; sB = b; sC = c; sD = d; sE = e;}
    void classMethodC6L(int a, int b, int c, int d, int e, int f) const
        {sIvokedMethod = 127; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f;}
    void classMethodC7L(int a, int b, int c, int d, int e, int f, int g) const
        {sIvokedMethod = 128; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g;}

    void classMethodWithLeaveCL() const
        {sIvokedMethod = 131;User::Leave(-sIvokedMethod);}
    void classMethodWithLeaveC1L(int a) const
        {sIvokedMethod = 132; sA = a;User::Leave(-sIvokedMethod);}
    void classMethodWithLeaveC2L(int a, int b) const
        {sIvokedMethod = 133; sA = a; sB = b;User::Leave(-sIvokedMethod);}
    void classMethodWithLeaveC3L(int a, int b, int c) const
        {sIvokedMethod = 134; sA = a; sB = b; sC = c;User::Leave(-sIvokedMethod);}
    void classMethodWithLeaveC4L(int a, int b, int c, int d) const
        {sIvokedMethod = 135; sA = a; sB = b; sC = c; sD = d;User::Leave(-sIvokedMethod);}
    void classMethodWithLeaveC5L(int a, int b, int c, int d, int e) const
        {sIvokedMethod = 136; sA = a; sB = b; sC = c; sD = d; sE = e;User::Leave(-sIvokedMethod);}
    void classMethodWithLeaveC6L(int a, int b, int c, int d, int e, int f) const
        {sIvokedMethod = 137; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f;User::Leave(-sIvokedMethod);}
    void classMethodWithLeaveC7L(int a, int b, int c, int d, int e, int f, int g) const
        {sIvokedMethod = 138; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g;User::Leave(-sIvokedMethod);}

    int classMethodRCL() const
        {sIvokedMethod = 141;return sIvokedMethod;}
    int classMethodRC1L(int a) const
        {sIvokedMethod = 142; sA = a;return sIvokedMethod;}
    int classMethodRC2L(int a, int b) const
        {sIvokedMethod = 143; sA = a; sB = b;return sIvokedMethod;}
    int classMethodRC3L(int a, int b, int c) const
        {sIvokedMethod = 144; sA = a; sB = b; sC = c;return sIvokedMethod;}
    int classMethodRC4L(int a, int b, int c, int d) const
        {sIvokedMethod = 145; sA = a; sB = b; sC = c; sD = d;return sIvokedMethod;}
    int classMethodRC5L(int a, int b, int c, int d, int e) const
        {sIvokedMethod = 146; sA = a; sB = b; sC = c; sD = d; sE = e;return sIvokedMethod;}
    int classMethodRC6L(int a, int b, int c, int d, int e, int f) const
        {sIvokedMethod = 147; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f;return sIvokedMethod;}
    int classMethodRC7L(int a, int b, int c, int d, int e, int f, int g) const
        {sIvokedMethod = 148; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g;return sIvokedMethod;}

    int classMethodWithLeaveRCL() const
        {sIvokedMethod = 151;User::Leave(-sIvokedMethod); return 0;}
    int classMethodWithLeaveRC1L(int a) const
        {sIvokedMethod = 152; sA = a;User::Leave(-sIvokedMethod);return 0;}
    int classMethodWithLeaveRC2L(int a, int b) const
        {sIvokedMethod = 153; sA = a; sB = b;User::Leave(-sIvokedMethod);return 0;}
    int classMethodWithLeaveRC3L(int a, int b, int c) const
        {sIvokedMethod = 154; sA = a; sB = b; sC = c;User::Leave(-sIvokedMethod);return 0;}
    int classMethodWithLeaveRC4L(int a, int b, int c, int d) const
        {sIvokedMethod = 155; sA = a; sB = b; sC = c; sD = d;User::Leave(-sIvokedMethod);return 0;}
    int classMethodWithLeaveRC5L(int a, int b, int c, int d, int e) const
        {sIvokedMethod = 156; sA = a; sB = b; sC = c; sD = d; sE = e;User::Leave(-sIvokedMethod);return 0;}
    int classMethodWithLeaveRC6L(int a, int b, int c, int d, int e, int f) const
        {sIvokedMethod = 157; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f;User::Leave(-sIvokedMethod);return 0;}
    int classMethodWithLeaveRC7L(int a, int b, int c, int d, int e, int f, int g) const
        {sIvokedMethod = 158; sA = a; sB = b; sC = c; sD = d; sE = e; sF = f; sG = g;User::Leave(-sIvokedMethod);return 0;}
};


class MyFunctionServer: public java::util::FunctionServer
{
public:

    MyFunctionServer() :
        java::util::FunctionServer("JavaTestFunctionServer")
    {
    }

    virtual void doServerSideInit()
    {
        java::util::FunctionServer::doServerSideInit();
        sServerSideInitCalled = ++sIndex;
    }
    virtual void doServerSideClean()
    {
        sServerSideCleanCalled = ++sIndex;
    }
    virtual void startServer()
    {
        sStartServerCalled = ++sIndex;
        java::util::FunctionServer::startServer();
    }
};
class MyFunctionServer2
{
    int a;
};

TEST_GROUP(TestFuncServerInit)
{
    TEST_SETUP()
    {
        sIvokedMethod = -1;
        sA = -1;
        sB = -1;
        sC = -1;
        sD = -1;
        sE = -1;
        sF = -1;
        sG = -1;
        sH = -1;
        sI = -1;
        sServerSideInitCalled = 0;
        sServerSideCleanCalled = 0;
        sStartServerCalled = 0;
        sIndex = 0;
    }

    TEST_TEARDOWN()
    {
    }
};


TEST(TestFuncServerInit, startAndStop)
{
    MyFunctionServer* fserver = new MyFunctionServer();
    try
    {
        fserver->createServerToNewThread();
        fserver->stopServer();
        delete fserver;
        fserver = 0;
    }
    catch(...)
    {
        FAIL("Unexpected exp in startAndStop\n");
    }

    CHECK(sServerSideInitCalled == 1);
    CHECK(sStartServerCalled == 2);
    CHECK(sServerSideCleanCalled == 3);
}







TEST_GROUP(TestFuncServerNotOpened)
{
    MyFunctionServer* fserver;
    TestClass* clazz;
    int a;
    int b;
    int c;
    int d;
    int e;
    int f;
    int g;
    int h;
    int i;

    TEST_SETUP()
    {
        fserver = new MyFunctionServer();

        sServerSideInitCalled = 0;
        sServerSideCleanCalled = 0;
        sStartServerCalled = 0;
        sIndex = 0;
        sIvokedMethod = -1;
        sA = -1;
        sB = -1;
        sC = -1;
        sD = -1;
        sE = -1;
        sF = -1;
        sG = -1;
        sH = -1;
        sI = -1;
        a = -1;
        b = -1;
        c = -1;
        d = -1;
        e = -1;
        f = -1;
        g = -1;
        h = -1;
        i = -1;
    }

    TEST_TEARDOWN()
    {
        CHECK(sA == a);
        CHECK(sB == b);
        CHECK(sC == c);
        CHECK(sD == d);
        CHECK(sE == e);
        CHECK(sF == f);
        CHECK(sG == g);
        CHECK(sH == h);
        CHECK(sI == i);
        delete fserver;
        fserver = 0;
    }
};


TEST(TestFuncServerNotOpened, closeWithoutOpen)
{
    try
    {
        fserver->stopServer();
        
    }
    catch(...)
    {
        FAIL("UNexpected exp in closeWithoutOpen\n");
    }
    CHECK(sServerSideInitCalled == 0);
    CHECK(sStartServerCalled == 0);
    CHECK(sServerSideCleanCalled == 0);
}


TEST(TestFuncServerNotOpened, sendWithoutOpen)
{
    try
    {
        CallMethod(localMethod, fserver);
    }
    catch (ExceptionBase& e)
    {
        CHECK(e.toString().find("Trying to execute code in non-existing FS: ") != std::string::npos)
        CHECK(e.toString().find(" server not created.") != std::string::npos)
    }
    catch(...)
    {
        FAIL("UNexpected exp in closeWithoutOpen\n");
    }
    CHECK(sIvokedMethod == -1);
    CHECK(sServerSideInitCalled == 0);
    CHECK(sStartServerCalled == 0);
    CHECK(sServerSideCleanCalled == 0);
}


TEST(TestFuncServerNotOpened, sendWithoutOpenL)
{
    try
    {
        TRAPD(err, CallMethodL(localLeavingMethodTL, fserver));
        CHECK(err == KErrServerTerminated);
    }
    catch(...)
    {
        FAIL("UNexpected exp in closeWithoutOpen\n");
    }
    CHECK(sIvokedMethod == -1);
    CHECK(sServerSideInitCalled == 0);
    CHECK(sStartServerCalled == 0);
    CHECK(sServerSideCleanCalled == 0);
}


TEST(TestFuncServerNotOpened, sendAfterClose)
{
    try
    {
        fserver->createServerToNewThread();
        fserver->stopServer();
        CallMethod(localMethod, fserver);
    }
    catch (ExceptionBase& e)
    {
        CHECK(e.toString().find("Trying to execute code in non-existing FS: ") != std::string::npos)
        CHECK(e.toString().find(" server closing.") != std::string::npos)
    }
    catch(...)
    {
        FAIL("UNexpected exp in closeWithoutOpen\n");
    }
    CHECK(sIvokedMethod == -1);
}

TEST(TestFuncServerNotOpened, sendAfterCloseL)
{
    try
    {
        fserver->createServerToNewThread();
        fserver->stopServer();
        TRAPD(err, CallMethodL(localLeavingMethodTL, fserver));
        CHECK(err == KErrServerTerminated);
    }
    catch(...)
    {
        FAIL("UNexpected exp in closeWithoutOpen\n");
    }
    CHECK(sIvokedMethod == -1);
}











TEST_GROUP(TestFuncServer)
{
    MyFunctionServer* fserver;
    TestClass* clazz;
    int a;
    int b;
    int c;
    int d;
    int e;
    int f;
    int g;
    int h;
    int i;

    TEST_SETUP()
    {
        fserver = new MyFunctionServer();
        clazz = new TestClass();

        sServerSideInitCalled = 0;
        sServerSideCleanCalled = 0;
        sStartServerCalled = 0;
        sIndex = 0;
        fserver->createServerToNewThread();
        sIvokedMethod = -1;
        sA = -1;
        sB = -1;
        sC = -1;
        sD = -1;
        sE = -1;
        sF = -1;
        sG = -1;
        sH = -1;
        sI = -1;
        a = -1;
        b = -1;
        c = -1;
        d = -1;
        e = -1;
        f = -1;
        g = -1;
        h = -1;
        i = -1;
    }

    TEST_TEARDOWN()
    {
        CHECK(sA == a);
        CHECK(sB == b);
        CHECK(sC == c);
        CHECK(sD == d);
        CHECK(sE == e);
        CHECK(sF == f);
        CHECK(sG == g);
        CHECK(sH == h);
        CHECK(sI == i);
        delete clazz;
        fserver->stopServer();
        delete fserver;
        fserver = 0;
        CHECK(sServerSideInitCalled == 1);
        CHECK(sStartServerCalled == 2);
        CHECK(sServerSideCleanCalled == 3);
    }
};

// Calling local void methods.
TEST(TestFuncServer, localVoid0)
{
    CallMethod(localMethod, fserver);
    CHECK(sIvokedMethod == 1);
}

TEST(TestFuncServer, localVoid1)
{
    a = 1;
    CallMethod(localMethod1, a, fserver);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localVoid2)
{
    a = 2;
    b = a + 1;
    CallMethod(localMethod2, a, b, fserver);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localVoid3)
{
    a = 3;
    b = a + 1;
    c = b + 1;
    CallMethod(localMethod3, a, b, c, fserver);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localVoid4)
{
    a = 4;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    CallMethod(localMethod4, a, b, c, d, fserver);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localVoid5)
{
    a = 5;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    CallMethod(localMethod5, a, b, c, d, e, fserver);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localVoid6)
{
    a = 6;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    CallMethod(localMethod6, a, b, c, d, e, f, fserver);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localVoid7)
{
    a = 7;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    CallMethod(localMethod7, a, b, c, d, e, f, g, fserver);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localVoid8)
{
    a = 8;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    h = g + 1;
    CallMethod(localMethod8, a, b, c, d, e, f, g, h, fserver);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localVoid9)
{
    a = 9;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    h = g + 1;
    i = h + 1;
    CallMethod(localMethod9, a, b, c, d, e, f, g, h, i, fserver);
    CHECK(sIvokedMethod == a + 1);
}


// Calling methods that returns something.

TEST(TestFuncServer, localReturn)
{
    int res = CallMethod(localMethodR, fserver);
    CHECK(res == 11);
    CHECK(sIvokedMethod == 11);
}

TEST(TestFuncServer, localReturn1)
{
    a = 11;
    int res = CallMethod(localMethodR1, a, fserver);
    CHECK(res == a + 1);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localReturn2)
{
    a = 12;
    b = a + 1;
    int res = CallMethod(localMethodR2, a, b, fserver);
    CHECK(res == a + 1);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localReturn3)
{
    a = 13;
    b = a + 1;
    c = b + 1;
    int res = CallMethod(localMethodR3, a, b, c, fserver);
    CHECK(res == a + 1);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localReturn4)
{
    a = 14;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    int res = CallMethod(localMethodR4, a, b, c, d, fserver);
    CHECK(res == a + 1);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localReturn5)
{
    a = 15;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    int res = CallMethod(localMethodR5, a, b, c, d, e, fserver);
    CHECK(res == a + 1);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localReturn6)
{
    a = 16;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    int res = CallMethod(localMethodR6, a, b, c, d, e, f, fserver);
    CHECK(res == a + 1);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localReturn7)
{
    a = 17;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    int res = CallMethod(localMethodR7, a, b, c, d, e, f, g, fserver);
    CHECK(res == a + 1);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localReturn8)
{
    a = 18;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    h = g + 1;
    int res = CallMethod(localMethodR8, a, b, c, d, e, f, g, h, fserver);
    CHECK(res == a + 1);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localReturn9)
{
    a = 19;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    h = g + 1;
    i = h + 1;
    int res = CallMethod(localMethodR9, a, b, c, d, e, f, g, h, i, fserver);
    CHECK(res == a + 1);
    CHECK(sIvokedMethod == a + 1);
}


// Calling methods through TRAP.

TEST(TestFuncServer, localTrap0)
{
    int res = CallMethodTrap(localMethodTL, fserver);
    CHECK(res == 0);
    CHECK(sIvokedMethod == 21);
}

TEST(TestFuncServer, localTrap1)
{
    a = 21;
    int res = CallMethodTrap(localMethodT1L, a, fserver);
    CHECK(res == 0);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localTrap2)
{
    a = 22;
    b = a + 1;
    int res = CallMethodTrap(localMethodT2L, a, b, fserver);
    CHECK(res == 0);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localTrap3)
{
    a = 23;
    b = a + 1;
    c = b + 1;
    int res = CallMethodTrap(localMethodT3L, a, b, c, fserver);
    CHECK(res == 0);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localTrap4)
{
    a = 24;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    int res = CallMethodTrap(localMethodT4L, a, b, c, d, fserver);
    CHECK(res == 0);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localTrap5)
{
    a = 25;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    int res = CallMethodTrap(localMethodT5L, a, b, c, d, e, fserver);
    CHECK(res == 0);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localTrap6)
{
    a = 26;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    int res = CallMethodTrap(localMethodT6L, a, b, c, d, e, f, fserver);
    CHECK(res == 0);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localTrap7)
{
    a = 27;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    int res = CallMethodTrap(localMethodT7L, a, b, c, d, e, f, g, fserver);
    CHECK(res == 0);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localTrap8)
{
    a = 28;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    h = g + 1;
    int res = CallMethodTrap(localMethodT8L, a, b, c, d, e, f, g, h, fserver);
    CHECK(res == 0);
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localTrap9)
{
    a = 29;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    h = g + 1;
    i = h + 1;
    int res = CallMethodTrap(localMethodT9L, a, b, c, d, e, f, g, h, i, fserver);
    CHECK(res == 0);
    CHECK(sIvokedMethod == a + 1);
}

// Calling methods through TRAP. The method will leave.
TEST(TestFuncServer, localTrapWithLeave)
{
    int res = CallMethodTrap(localLeavingMethodTL, fserver);
    CHECK(res == -31);
    CHECK(sIvokedMethod == 31);
}

TEST(TestFuncServer, localTrapWithLeave1)
{
    a = 31;
    int res = CallMethodTrap(localLeavingMethodT1L, a, fserver);
    CHECK(res == -(a + 1));
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localTrapWithLeave2)
{
    a = 32;
    b = a + 1;
    int res = CallMethodTrap(localLeavingMethodT2L, a, b, fserver);
    CHECK(res == -(a + 1));
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localTrapWithLeave3)
{
    a = 33;
    b = a + 1;
    c = b + 1;
    int res = CallMethodTrap(localLeavingMethodT3L, a, b, c, fserver);
    CHECK(res == -(a + 1));
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localTrapWithLeave4)
{
    a = 34;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    int res = CallMethodTrap(localLeavingMethodT4L, a, b, c, d, fserver);
    CHECK(res == -(a + 1));
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localTrapWithLeave5)
{
    a = 35;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    int res = CallMethodTrap(localLeavingMethodT5L, a, b, c, d, e, fserver);
    CHECK(res == -(a + 1));
    CHECK(sIvokedMethod == a + 1);
}


TEST(TestFuncServer, localTrapWithLeave6)
{
    a = 36;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    int res = CallMethodTrap(localLeavingMethodT6L, a, b, c, d, e, f, fserver);
    CHECK(res == -(a + 1));
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localTrapWithLeave7)
{
    a = 37;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    int res = CallMethodTrap(localLeavingMethodT7L, a, b, c, d, e, f, g, fserver);
    CHECK(res == -(a + 1));
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localTrapWithLeave8)
{
    a = 38;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    h = g + 1;
    int res = CallMethodTrap(localLeavingMethodT8L, a, b, c, d, e, f, g, h, fserver);
    CHECK(res == -(a + 1));
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, localTrapWithLeave9)
{
    a = 39;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    h = g + 1;
    i = h + 1;
    int res = CallMethodTrap(localLeavingMethodT9L, a, b, c, d, e, f, g, h, i, fserver);
    CHECK(res == -(a + 1));
    CHECK(sIvokedMethod == a + 1);
}


// Testing non-const void methods from a class.

TEST(TestFuncServer, classVoid0)
{
    CallMethod( clazz, TestClass::classMethod, fserver );
    CHECK(sIvokedMethod == 41);
}

TEST(TestFuncServer, classVoid1)
{
    a = 41;
    CallMethod( clazz, TestClass::classMethod1, a, fserver );
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, classVoid2)
{
    a = 42;
    b = a + 1;
    CallMethod( clazz, TestClass::classMethod2, a, b, fserver );
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, classVoid3)
{
    a = 43;
    b = a + 1;
    c = b + 1;
    CallMethod( clazz, TestClass::classMethod3, a, b, c, fserver );
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, classVoid4)
{
    a = 44;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    CallMethod( clazz, TestClass::classMethod4, a, b, c, d, fserver );
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, classVoid5)
{
    a = 45;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    CallMethod( clazz, TestClass::classMethod5, a, b, c, d, e, fserver );
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, classVoid6)
{
    a = 46;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    CallMethod( clazz, TestClass::classMethod6, a, b, c, d, e, f, fserver );
    CHECK(sIvokedMethod == a + 1);
}

TEST(TestFuncServer, classVoid7)
{
    a = 47;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    CallMethod( clazz, TestClass::classMethod7, a, b, c, d, e, f, g, fserver );
    CHECK(sIvokedMethod == a + 1);
}

// Testing non-const methods from a class that returns a value.

TEST(TestFuncServer, classReturn0)
{
    int res;
    CallMethod(res, clazz, TestClass::classMethodR, fserver );
    CHECK(sIvokedMethod == 51);
    CHECK(res == 51);

}
TEST(TestFuncServer, classReturn1)
{
    a = 51;
    int res;
    CallMethod(res, clazz, TestClass::classMethodR1, a, fserver );
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);

}
TEST(TestFuncServer, classReturn2)
{
    a = 52;
    b = a + 1;
    int res;
    CallMethod(res, clazz, TestClass::classMethodR2, a, b, fserver );
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);

}
TEST(TestFuncServer, classReturn3)
{
    a = 53;
    b = a + 1;
    c = b + 1;
    int res;
    CallMethod(res, clazz, TestClass::classMethodR3, a, b, c, fserver );
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);

}
TEST(TestFuncServer, classReturn4)
{
    a = 54;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    int res;
    CallMethod(res, clazz, TestClass::classMethodR4, a, b, c, d, fserver );
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);

}
TEST(TestFuncServer, classReturn5)
{
    a = 55;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    int res;
    CallMethod(res, clazz, TestClass::classMethodR5, a, b, c, d, e, fserver );
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);

}
TEST(TestFuncServer, classReturn6)
{
    a = 56;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    int res;
    CallMethod(res, clazz, TestClass::classMethodR6, a, b, c, d, e, f, fserver );
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);

}
TEST(TestFuncServer, classReturn7)
{
    a = 57;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    int res;
    CallMethod(res, clazz, TestClass::classMethodR7, a, b, c, d, e, f, g, fserver );
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);

}

// Testing non-const void leaving methods from a class. Leave is not happening.

TEST(TestFuncServer, classVoidL0)
{
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodL, fserver ));
    CHECK(sIvokedMethod == 61);
    CHECK(res == 0);

}
TEST(TestFuncServer, classVoidL1)
{
    a = 61;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodL1, a, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == 0);

}
TEST(TestFuncServer, classVoidL2)
{
    a = 62;
    b = a + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodL2, a, b, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == 0);

}
TEST(TestFuncServer, classVoidL3)
{
    a = 63;
    b = a + 1;
    c = b + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodL3, a, b, c, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == 0);

}
TEST(TestFuncServer, classVoidL4)
{
    a = 64;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodL4, a, b, c, d, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == 0);

}
TEST(TestFuncServer, classVoidL5)
{
    a = 65;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodL5, a, b, c, d, e, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == 0);

}
TEST(TestFuncServer, classVoidL6)
{
    a = 66;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodL6, a, b, c, d, e, f, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == 0);

}
TEST(TestFuncServer, classVoidL7)
{
    a = 67;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodL7, a, b, c, d, e, f, g, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == 0);

}

// Testing non-const void leaving methods from a class. Leave is happening.
TEST(TestFuncServer, classVoidLeavingL0)
{
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodWithLeaveL, fserver ));
    CHECK(sIvokedMethod == 71);
    CHECK(res == -71);

}
TEST(TestFuncServer, classVoidLeavingL1)
{
    a = 71;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodWithLeave1L, a, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == -(a + 1));

}
TEST(TestFuncServer, classVoidLeavingL2)
{
    a = 72;
    b = a + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodWithLeave2L, a, b, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == -(a + 1));

}
TEST(TestFuncServer, classVoidLeavingL3)
{
    a = 73;
    b = a + 1;
    c = b + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodWithLeave3L, a, b, c, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == -(a + 1));

}
TEST(TestFuncServer, classVoidLeavingL4)
{
    a = 74;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodWithLeave4L, a, b, c, d, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == -(a + 1));

}
TEST(TestFuncServer, classVoidLeavingL5)
{
    a = 75;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodWithLeave5L, a, b, c, d, e, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == -(a + 1));

}
TEST(TestFuncServer, classVoidLeavingL6)
{
    a = 76;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodWithLeave6L, a, b, c, d, e, f, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == -(a + 1));

}
TEST(TestFuncServer, classVoidLeavingL7)
{
    a = 77;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodWithLeave7L, a, b, c, d, e, f, g, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == -(a + 1));

}

// Testing non-const value returning leaving methods from a class. Leave is not happening.

TEST(TestFuncServer, classReturnL0)
{
    int res;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodRL, fserver ));
    CHECK(sIvokedMethod == 81);
    CHECK(res == 81);
    CHECK(leaveRes == 0);
}
TEST(TestFuncServer, classReturnL1)
{
    a = 81;
    int res;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodR1L, a, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);
    CHECK(leaveRes == 0);
}
TEST(TestFuncServer, classReturnL2)
{
    a = 82;
    b = a + 1;
    int res;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodR2L, a, b, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);
    CHECK(leaveRes == 0);
}
TEST(TestFuncServer, classReturnL3)
{
    a = 83;
    b = a + 1;
    c = b + 1;
    int res;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodR3L, a, b, c, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);
    CHECK(leaveRes == 0);
}
TEST(TestFuncServer, classReturnL4)
{
    a = 84;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    int res;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodR4L, a, b, c, d, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);
    CHECK(leaveRes == 0);
}
TEST(TestFuncServer, classReturnL5)
{
    a = 85;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    int res;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodR5L, a, b, c, d, e, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);
    CHECK(leaveRes == 0);
}
TEST(TestFuncServer, classReturnL6)
{
    a = 86;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    int res;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodR6L, a, b, c, d, e, f, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);
    CHECK(leaveRes == 0);
}
TEST(TestFuncServer, classReturnL7)
{
    a = 87;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    int res;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodR7L, a, b, c, d, e, f, g, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);
    CHECK(leaveRes == 0);
}

// Testing non-const value returning leaving methods from a class. Leave is happening.


TEST(TestFuncServer, classReturnLeaveL0)
{
    int res=123;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodWithLeaveRL, fserver ));
    CHECK(sIvokedMethod == 91);
    CHECK(leaveRes == -91);
    CHECK(res == 123);
}
TEST(TestFuncServer, classReturnLeaveL1)
{
    a = 91;
    int res=123;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodWithLeaveR1L, a, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(leaveRes == -(a + 1));
    CHECK(res == 123);
}
TEST(TestFuncServer, classReturnLeaveL2)
{
    a = 92;
    b = a + 1;
    int res=123;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodWithLeaveR2L, a, b, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(leaveRes == -(a + 1));
    CHECK(res == 123);
}
TEST(TestFuncServer, classReturnLeaveL3)
{
    a = 93;
    b = a + 1;
    c = b + 1;
    int res=123;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodWithLeaveR3L, a, b, c, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(leaveRes == -(a + 1));
    CHECK(res == 123);
}
TEST(TestFuncServer, classReturnLeaveL4)
{
    a = 94;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    int res=123;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodWithLeaveR4L, a, b, c, d, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(leaveRes == -(a + 1));
    CHECK(res == 123);
}
TEST(TestFuncServer, classReturnLeaveL5)
{
    a = 95;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    int res=123;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodWithLeaveR5L, a, b, c, d, e, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(leaveRes == -(a + 1));
    CHECK(res == 123);
}
TEST(TestFuncServer, classReturnLeaveL6)
{
    a = 96;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    int res=123;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodWithLeaveR6L, a, b, c, d, e, f, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(leaveRes == -(a + 1));
    CHECK(res == 123);
}
TEST(TestFuncServer, classReturnLeaveL7)
{
    a = 97;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    int res=123;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodWithLeaveR7L, a, b, c, d, e, f, g, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(leaveRes == -(a + 1));
    CHECK(res == 123);
}

// Testing const void methods from a class.

TEST(TestFuncServer, classVoidC0)
{
    CallMethod( clazz, TestClass::classMethodC, fserver );
    CHECK(sIvokedMethod == 101);

}
TEST(TestFuncServer, classVoidC1)
{
    a = 101;
    CallMethod( clazz, TestClass::classMethodC1, a, fserver );
    CHECK(sIvokedMethod == a + 1);

}
TEST(TestFuncServer, classVoidC2)
{
    a = 102;
    b = a + 1;
    CallMethod( clazz, TestClass::classMethodC2, a, b, fserver );
    CHECK(sIvokedMethod == a + 1);

}
TEST(TestFuncServer, classVoidC3)
{
    a = 103;
    b = a + 1;
    c = b + 1;
    CallMethod( clazz, TestClass::classMethodC3, a, b, c, fserver );
    CHECK(sIvokedMethod == a + 1);

}
TEST(TestFuncServer, classVoidC4)
{
    a = 104;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    CallMethod( clazz, TestClass::classMethodC4, a, b, c, d, fserver );
    CHECK(sIvokedMethod == a + 1);

}
TEST(TestFuncServer, classVoidC5)
{
    a = 105;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    CallMethod( clazz, TestClass::classMethodC5, a, b, c, d, e, fserver );
    CHECK(sIvokedMethod == a + 1);

}
TEST(TestFuncServer, classVoidC6)
{
    a = 106;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    CallMethod( clazz, TestClass::classMethodC6, a, b, c, d, e, f, fserver );
    CHECK(sIvokedMethod == a + 1);

}
TEST(TestFuncServer, classVoidC7)
{
    a = 107;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    CallMethod( clazz, TestClass::classMethodC7, a, b, c, d, e, f, g, fserver );
    CHECK(sIvokedMethod == a + 1);

}

// Testing const methods from a class retruning a value.

TEST(TestFuncServer, classReturnC0)
{
    int res;
    CallMethod(res, clazz, TestClass::classMethodRC, fserver );
    CHECK(sIvokedMethod == 111);
    CHECK(res == 111);

}
TEST(TestFuncServer, classReturnC1)
{
    a = 111;
    int res;
    CallMethod(res, clazz, TestClass::classMethodRC1, a, fserver );
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);

}
TEST(TestFuncServer, classReturnC2)
{
    a = 112;
    b = a + 1;
    int res;
    CallMethod(res, clazz, TestClass::classMethodRC2, a, b, fserver );
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);

}
TEST(TestFuncServer, classReturnC3)
{
    a = 113;
    b = a + 1;
    c = b + 1;
    int res;
    CallMethod(res, clazz, TestClass::classMethodRC3, a, b, c, fserver );
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);

}
TEST(TestFuncServer, classReturnC4)
{
    a = 114;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    int res;
    CallMethod(res, clazz, TestClass::classMethodRC4, a, b, c, d, fserver );
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);

}
TEST(TestFuncServer, classReturnC5)
{
    a = 115;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    int res;
    CallMethod(res, clazz, TestClass::classMethodRC5, a, b, c, d, e, fserver );
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);

}
TEST(TestFuncServer, classReturnC6)
{
    a = 116;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    int res;
    CallMethod(res, clazz, TestClass::classMethodRC6, a, b, c, d, e, f, fserver );
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);

}
TEST(TestFuncServer, classReturnC7)
{
    a = 117;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    int res;
    CallMethod(res, clazz, TestClass::classMethodRC7, a, b, c, d, e, f, g, fserver );
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);

}

// Testing const leaving void methods from a class. No leave.


TEST(TestFuncServer, classVoidCL0)
{
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodCL, fserver ));
    CHECK(sIvokedMethod == 121);
    CHECK(res == 0);

}
TEST(TestFuncServer, classVoidCL1)
{
    a = 121;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodC1L, a, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == 0);

}
TEST(TestFuncServer, classVoidCL2)
{
    a = 122;
    b = a + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodC2L, a, b, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == 0);

}
TEST(TestFuncServer, classVoidCL3)
{
    a = 123;
    b = a + 1;
    c = b + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodC3L, a, b, c, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == 0);

}
TEST(TestFuncServer, classVoidCL4)
{
    a = 124;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodC4L, a, b, c, d, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == 0);

}
TEST(TestFuncServer, classVoidCL5)
{
    a = 125;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodC5L, a, b, c, d, e, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == 0);

}
TEST(TestFuncServer, classVoidCL6)
{
    a = 126;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodC6L, a, b, c, d, e, f, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == 0);

}
TEST(TestFuncServer, classVoidCL7)
{
    a = 127;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodC7L, a, b, c, d, e, f, g, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == 0);

}



// Testing const leaving void methods from a class. Leaving.


TEST(TestFuncServer, classVoidLeavingCL0)
{
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodWithLeaveCL, fserver ));
    CHECK(sIvokedMethod == 131);
    CHECK(res == -131);

}
TEST(TestFuncServer, classVoidLeavingCL1)
{
    a = 131;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodWithLeaveC1L, a, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == -(a + 1));

}
TEST(TestFuncServer, classVoidLeavingCL2)
{
    a = 132;
    b = a + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodWithLeaveC2L, a, b, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == -(a + 1));

}
TEST(TestFuncServer, classVoidLeavingCL3)
{
    a = 133;
    b = a + 1;
    c = b + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodWithLeaveC3L, a, b, c, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == -(a + 1));

}
TEST(TestFuncServer, classVoidLeavingCL4)
{
    a = 134;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodWithLeaveC4L, a, b, c, d, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == -(a + 1));

}
TEST(TestFuncServer, classVoidLeavingCL5)
{
    a = 135;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodWithLeaveC5L, a, b, c, d, e, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == -(a + 1));

}
TEST(TestFuncServer, classVoidLeavingCL6)
{
    a = 136;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodWithLeaveC6L, a, b, c, d, e, f, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == -(a + 1));

}
TEST(TestFuncServer, classVoidLeavingCL7)
{
    a = 137;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    TRAPD(res, CallMethodL(clazz, TestClass::classMethodWithLeaveC7L, a, b, c, d, e, f, g, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == -(a + 1));

}



// Testing const leaving value returning methods from a class. Not leaving.
TEST(TestFuncServer, classReturnLC0)
{
    int res;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodRCL, fserver ));
    CHECK(sIvokedMethod == 141);
    CHECK(res == 141);
    CHECK(leaveRes == 0);
}
TEST(TestFuncServer, classReturnLC1)
{
    a = 141;
    int res;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodRC1L, a, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);
    CHECK(leaveRes == 0);
}
TEST(TestFuncServer, classReturnLC2)
{
    a = 142;
    b = a + 1;
    int res;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodRC2L, a, b, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);
    CHECK(leaveRes == 0);
}
TEST(TestFuncServer, classReturnLC3)
{
    a = 143;
    b = a + 1;
    c = b + 1;
    int res;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodRC3L, a, b, c, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);
    CHECK(leaveRes == 0);
}
TEST(TestFuncServer, classReturnLC4)
{
    a = 144;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    int res;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodRC4L, a, b, c, d, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);
    CHECK(leaveRes == 0);
}
TEST(TestFuncServer, classReturnLC5)
{
    a = 145;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    int res;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodRC5L, a, b, c, d, e, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);
    CHECK(leaveRes == 0);
}
TEST(TestFuncServer, classReturnLC6)
{
    a = 146;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    int res;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodRC6L, a, b, c, d, e, f, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);
    CHECK(leaveRes == 0);
}
TEST(TestFuncServer, classReturnLC7)
{
    a = 147;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    int res;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodRC7L, a, b, c, d, e, f, g, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(res == a + 1);
    CHECK(leaveRes == 0);
}

// Testing const leaving value returning methods from a class. Leaving.

TEST(TestFuncServer, classReturnLeaveCL0)
{
    int res=456;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodWithLeaveRCL, fserver ));
    CHECK(sIvokedMethod == 151);
    CHECK(leaveRes == -151);
    CHECK(res == 456);
}
TEST(TestFuncServer, classReturnLeaveCL1)
{
    a = 151;
    int res=456;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodWithLeaveRC1L, a, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(leaveRes == -(a + 1));
    CHECK(res == 456);
}
TEST(TestFuncServer, classReturnLeaveCL2)
{
    a = 152;
    b = a + 1;
    int res=456;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodWithLeaveRC2L, a, b, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(leaveRes == -(a + 1));
    CHECK(res == 456);
}
TEST(TestFuncServer, classReturnLeaveCL3)
{
    a = 153;
    b = a + 1;
    c = b + 1;
    int res=456;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodWithLeaveRC3L, a, b, c, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(leaveRes == -(a + 1));
    CHECK(res == 456);
}
TEST(TestFuncServer, classReturnLeaveCL4)
{
    a = 154;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    int res=456;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodWithLeaveRC4L, a, b, c, d, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(leaveRes == -(a + 1));
    CHECK(res == 456);
}
TEST(TestFuncServer, classReturnLeaveCL5)
{
    a = 155;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    int res=456;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodWithLeaveRC5L, a, b, c, d, e, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(leaveRes == -(a + 1));
    CHECK(res == 456);
}
TEST(TestFuncServer, classReturnLeaveCL6)
{
    a = 156;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    int res=456;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodWithLeaveRC6L, a, b, c, d, e, f, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(leaveRes == -(a + 1));
    CHECK(res == 456);
}
TEST(TestFuncServer, classReturnLeaveCL7)
{
    a = 157;
    b = a + 1;
    c = b + 1;
    d = c + 1;
    e = d + 1;
    f = e + 1;
    g = f + 1;
    int res=456;
    TRAPD(leaveRes, CallMethodL(res, clazz, TestClass::classMethodWithLeaveRC7L, a, b, c, d, e, f, g, fserver ));
    CHECK(sIvokedMethod == a + 1);
    CHECK(leaveRes == -(a + 1));
    CHECK(res == 456);
}

