/*******************************************************************************
 * Copyright (c) 2008 - 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/

#include "swtjavabufferdrawer.h"
#include "eswt.h"
#include "eswtgraphics.h"
#include "swtcolor.h"
#ifdef _DEBUG
#include "swtdisplay.h" // for debug heap and related macros
#endif //_DEBUG

// Codes for all different bufferable GC operations
static const int OP_DRAWARC               = 1;
static const int OP_DRAWFOCUS             = 2;
static const int OP_DRAWIMAGE             = 3;
static const int OP_DRAWLINE              = 4;
static const int OP_DRAWOVAL              = 5;
static const int OP_DRAWPOINT             = 6;
static const int OP_DRAWPOLYGON           = 7;
static const int OP_DRAWPOLYLINE          = 8;
static const int OP_DRAWRECTANGLE         = 9;
static const int OP_DRAWROUNDRECTANGLE    = 10;
static const int OP_DRAWTEXT              = 11;
static const int OP_FILLARC               = 12;
static const int OP_FILLGRADIENTRECTANGLE = 13;
static const int OP_FILLOVAL              = 14;
static const int OP_FILLPOLYGON           = 15;
static const int OP_FILLRECTANGLE         = 16;
static const int OP_FILLROUNDRECTANGLE    = 17;
static const int OP_SETALPHA              = 18;
static const int OP_SETBACKGROUND         = 19;
static const int OP_SETCLIPPING           = 20;
static const int OP_CANCELCLIPPING        = 21;
static const int OP_SETFONT               = 22;
static const int OP_SETFOREGROUND         = 23;
static const int OP_SETLINESTYLE          = 24;
static const int OP_SETLINEWIDTH          = 25;
static const int OP_SETXORMODE            = 26;
static const int OP_COPYAREA              = 27;
static const int OP_DRAWM2GIMAGE          = 28;
static const int OP_COPYAREATOIMAGE       = 29;
// To be used for debugging, enable writing this flag in the Java code and then
// enable the check in the code below.
//static const int OP_END = 2147483647;


CSwtJavaBufferDrawer::CSwtJavaBufferDrawer(MSwtDisplay& aDisplay) : iDisplay(aDisplay)
{
}


CSwtJavaBufferDrawer::~CSwtJavaBufferDrawer()
{
}

void CSwtJavaBufferDrawer::Draw(MSwtGc* aGc, TInt* aIntParams, TInt aIntCount, HBufC* aStrParams, RWindow* aWindow, const TRect& aDrawRect)
{
    if (!aGc || !aIntParams)
    {
        return;
    }

    TInt i(0);   // Integer parameter array index
    TInt s(0);   // String parameter array index
    const TInt strLength(aStrParams ? aStrParams->Length() : 0);
    while ((i < aIntCount) && (s <= strLength))
    {
        switch (aIntParams[ i++ ])
        {
        case OP_DRAWARC:
        {
            TInt& x          = aIntParams[ i++ ];
            TInt& y          = aIntParams[ i++ ];
            TInt& width      = aIntParams[ i++ ];
            TInt& height     = aIntParams[ i++ ];
            TInt& startAngle = aIntParams[ i++ ];
            TInt& arcAngle   = aIntParams[ i++ ];

            TPoint* point = reinterpret_cast<TPoint*>(&x);
            TSize* size   = reinterpret_cast<TSize*>(&width);

            aGc->DrawArc(TRect(*point, *size), startAngle, arcAngle);
        }
        break;
        case OP_DRAWFOCUS:
        {
            TInt& x      = aIntParams[ i++ ];
            TInt& y      = aIntParams[ i++ ];
            TInt& width  = aIntParams[ i++ ];
            TInt& height = aIntParams[ i++ ];

            TPoint* point = reinterpret_cast<TPoint*>(&x);
            TSize* size   = reinterpret_cast<TSize*>(&width);

            aGc->DrawFocusRect(TRect(*point, *size), iDisplay.UiUtils());
        }
        break;
        case OP_DRAWIMAGE:
        {
            MSwtImage* image = reinterpret_cast<MSwtImage*>(aIntParams[ i++ ]);
            TInt& srcX       = aIntParams[ i++ ];
            TInt& srcY       = aIntParams[ i++ ];
            TInt& srcWidth   = aIntParams[ i++ ];
            TInt& srcHeight  = aIntParams[ i++ ];
            TInt& destX      = aIntParams[ i++ ];
            TInt& destY      = aIntParams[ i++ ];
            TInt& destWidth  = aIntParams[ i++ ];
            TInt& destHeight = aIntParams[ i++ ];

            TPoint* srcPos   = reinterpret_cast<TPoint*>(&srcX);
            TSize* srcSize   = reinterpret_cast<TSize*>(&srcWidth);
            TPoint* destPos  = reinterpret_cast<TPoint*>(&destX);
            TSize*  destSize = reinterpret_cast<TSize*>(&destWidth);

            aGc->DrawImage(*image, *srcPos, *srcSize, *destPos, *destSize);
        }
        break;
        case OP_DRAWLINE:
        {
            TInt& x1 = aIntParams[ i++ ];
            TInt& y1 = aIntParams[ i++ ];
            TInt& x2 = aIntParams[ i++ ];
            TInt& y2 = aIntParams[ i++ ];

            TPoint* p1 = reinterpret_cast<TPoint*>(&x1);
            TPoint* p2 = reinterpret_cast<TPoint*>(&x2);

            aGc->DrawLine(*p1, *p2);
        }
        break;
        case OP_DRAWOVAL:
        {
            TInt& x      = aIntParams[ i++ ];
            TInt& y      = aIntParams[ i++ ];
            TInt& width  = aIntParams[ i++ ];
            TInt& height = aIntParams[ i++ ];

            TPoint* point = reinterpret_cast<TPoint*>(&x);
            TSize* size   = reinterpret_cast<TSize*>(&width);

            aGc->DrawOval(TRect(*point, *size));
        }
        break;
        case OP_DRAWPOINT:
        {
            TInt& x = aIntParams[ i++ ];
            TInt& y = aIntParams[ i++ ];

            TPoint* point = reinterpret_cast<TPoint*>(&x);

            aGc->DrawPoint(*point);
        }
        break;
        case OP_DRAWPOLYGON:
        {
            TInt& arrayLength = aIntParams[ i++ ];
            aGc->DrawPolygon(reinterpret_cast<TPoint*>(&aIntParams[ i ]), arrayLength/2);
            i += arrayLength;
        }
        break;
        case OP_DRAWPOLYLINE:
        {
            TInt& arrayLength = aIntParams[ i++ ];
            aGc->DrawPolyline(reinterpret_cast<TPoint*>(&aIntParams[ i ]), arrayLength/2);
            i += arrayLength;
        }
        break;
        case OP_DRAWRECTANGLE:
        {
            TInt& x      = aIntParams[ i++ ];
            TInt& y      = aIntParams[ i++ ];
            TInt& width  = aIntParams[ i++ ];
            TInt& height = aIntParams[ i++ ];

            TPoint* point = reinterpret_cast<TPoint*>(&x);
            TSize* size   = reinterpret_cast<TSize*>(&width);

            aGc->DrawRectangle(TRect(*point, *size));
        }
        break;
        case OP_DRAWROUNDRECTANGLE:
        {
            TInt& x         = aIntParams[ i++ ];
            TInt& y         = aIntParams[ i++ ];
            TInt& width     = aIntParams[ i++ ];
            TInt& height    = aIntParams[ i++ ];
            TInt& arcWidth  = aIntParams[ i++ ];
            TInt& arcHeight = aIntParams[ i++ ];

            TPoint* point  = reinterpret_cast<TPoint*>(&x);
            TSize* size    = reinterpret_cast<TSize*>(&width);
            TSize* arcSize = reinterpret_cast<TSize*>(&arcWidth);

            aGc->DrawRoundRectangle(TRect(*point, *size), *arcSize);
        }
        break;
        case OP_DRAWTEXT:
        {
            TInt& stringLength = aIntParams[ i++ ];
            TInt& x            = aIntParams[ i++ ];
            TInt& y            = aIntParams[ i++ ];
            TInt& flags        = aIntParams[ i++ ];

            const TPoint* position = reinterpret_cast<const TPoint*>(&x);

            ASSERT(strLength -  s >= 0);
#ifdef ESWT_NATIVE_UITHREAD_OWN_HEAP
            RHeap* debugHeap = User::SwitchHeap(static_cast<CSwtDisplay&>(iDisplay).iJavaHeap);
#endif //ESWT_NATIVE_UITHREAD_OWN_HEAP
            TPtr text = aStrParams->Des().MidTPtr(s, stringLength);
#ifdef ESWT_NATIVE_UITHREAD_OWN_HEAP
            User::SwitchHeap(debugHeap);
#endif //ESWT_NATIVE_UITHREAD_OWN_HEAP
            TRAP_IGNORE(aGc->DrawTextL(text, *position, flags));
            s += stringLength;
        }
        break;
        case OP_FILLARC:
        {
            TInt& x          = aIntParams[ i++ ];
            TInt& y          = aIntParams[ i++ ];
            TInt& width      = aIntParams[ i++ ];
            TInt& height     = aIntParams[ i++ ];
            TInt& startAngle = aIntParams[ i++ ];
            TInt& arcAngle   = aIntParams[ i++ ];

            TPoint* point = reinterpret_cast<TPoint*>(&x);
            TSize* size   = reinterpret_cast<TSize*>(&width);

            aGc->FillArc(TRect(*point, *size), startAngle, arcAngle);
        }
        break;
        case OP_FILLGRADIENTRECTANGLE:
        {
            TInt& x        = aIntParams[ i++ ];
            TInt& y        = aIntParams[ i++ ];
            TInt& width    = aIntParams[ i++ ];
            TInt& height   = aIntParams[ i++ ];
            TInt& vertical = aIntParams[ i++ ];

            TPoint* point = reinterpret_cast<TPoint*>(&x);
            TSize* size   = reinterpret_cast<TSize*>(&width);

            TBool invertGradient(EFalse);
            if ((width < 0) || (height < 0))
            {
                invertGradient = ETrue;
            }

            aGc->FillGradientRectangle(TRect(*point, *size), vertical, invertGradient);
        }
        break;
        case OP_FILLOVAL:
        {
            TInt& x      = aIntParams[ i++ ];
            TInt& y      = aIntParams[ i++ ];
            TInt& width  = aIntParams[ i++ ];
            TInt& height = aIntParams[ i++ ];

            TPoint* point = reinterpret_cast<TPoint*>(&x);
            TSize* size   = reinterpret_cast<TSize*>(&width);

            aGc->FillOval(TRect(*point, *size));
        }
        break;
        case OP_FILLPOLYGON:
        {
            TInt& arrayLength = aIntParams[ i++ ];
            aGc->FillPolygon(reinterpret_cast<TPoint*>(&aIntParams[ i ]), arrayLength/2);
            i += arrayLength;
        }
        break;
        case OP_FILLRECTANGLE:
        {
            TInt& x      = aIntParams[ i++ ];
            TInt& y      = aIntParams[ i++ ];
            TInt& width  = aIntParams[ i++ ];
            TInt& height = aIntParams[ i++ ];

            TPoint* point = reinterpret_cast<TPoint*>(&x);
            TSize* size   = reinterpret_cast<TSize*>(&width);

            aGc->FillRectangle(TRect(*point, *size));
        }
        break;
        case OP_FILLROUNDRECTANGLE:
        {
            TInt& x         = aIntParams[ i++ ];
            TInt& y         = aIntParams[ i++ ];
            TInt& width     = aIntParams[ i++ ];
            TInt& height    = aIntParams[ i++ ];
            TInt& arcWidth  = aIntParams[ i++ ];
            TInt& arcHeight = aIntParams[ i++ ];

            TPoint* point  = reinterpret_cast<TPoint*>(&x);
            TSize* size    = reinterpret_cast<TSize*>(&width);
            TSize* arcSize = reinterpret_cast<TSize*>(&arcWidth);

            aGc->FillRoundRectangle(TRect(*point, *size), *arcSize);
        }
        break;
        case OP_SETALPHA:
        {
            TInt& alpha = aIntParams[ i++ ];
            aGc->SetAlpha(alpha);
        }
        break;
        case OP_SETBACKGROUND:
        {
            TRgb desiredColor((aIntParams[i] & 0x00ff0000) >> 16,
                              (aIntParams[i] & 0x0000ff00) >> 8,
                              (aIntParams[i] & 0x000000ff),
                              0xff // alpha, fully opaque
                             );
            TRgb deviceColor(
                CSwtColor::RgbValue(*(aGc->BitmapContext().Device()),
                                    desiredColor));
            aGc->SetBrushRgbValue(deviceColor);
            ++i;
        }
        break;
        case OP_SETCLIPPING:
        {
            TInt& x      = aIntParams[ i++ ];
            TInt& y      = aIntParams[ i++ ];
            TInt& width  = aIntParams[ i++ ];
            TInt& height = aIntParams[ i++ ];

            TPoint* point = reinterpret_cast<TPoint*>(&x);
            TSize* size   = reinterpret_cast<TSize*>(&width);

            aGc->SetClippingRect(TRect(*point, *size));
        }
        break;
        case OP_CANCELCLIPPING:
        {
            aGc->CancelClipping();
        }
        break;
        case OP_SETFONT:
        {
            // Font can be NULL
            MSwtFont* font = reinterpret_cast<MSwtFont*>(aIntParams[ i++ ]);
            aGc->SetFont(font);
        }
        break;
        case OP_SETFOREGROUND:
        {
            TRgb desiredColor((aIntParams[i] & 0x00ff0000) >> 16,
                              (aIntParams[i] & 0x0000ff00) >> 8,
                              (aIntParams[i] & 0x000000ff),
                              0xff // alpha, fully opaque
                             );
            TRgb deviceColor(
                CSwtColor::RgbValue(*(aGc->BitmapContext().Device()),
                                    desiredColor));
            aGc->SetPenRgbValue(deviceColor);
            ++i;
        }
        break;
        case OP_SETLINESTYLE:
        {
            TInt& lineStyle = aIntParams[ i++ ];
            TRAP_IGNORE(aGc->SetLineStyleL(static_cast<TSwtLineStyle>(lineStyle)));
        }
        break;
        case OP_SETLINEWIDTH:
        {
            TInt& lineWidth = aIntParams[ i++ ];
            aGc->SetLineWidth(lineWidth);
        }
        break;
        case OP_SETXORMODE:
        {
            TInt& xorMode = aIntParams[ i++ ];
            aGc->SetXORMode(xorMode);
        }
        break;
        case OP_COPYAREA:
        {
            TInt& srcX       = aIntParams[ i++ ];
            TInt& srcY       = aIntParams[ i++ ];
            TInt& width      = aIntParams[ i++ ];
            TInt& height     = aIntParams[ i++ ];
            TInt& destX      = aIntParams[ i++ ];
            TInt& destY      = aIntParams[ i++ ];

            TPoint* pointSrc = reinterpret_cast<TPoint*>(&srcX);
            TSize* size   = reinterpret_cast<TSize*>(&width);
            TPoint* pointDest = reinterpret_cast<TPoint*>(&destX);

            (void)aWindow;    // Just to suppress a potential warning 
            (void)aDrawRect;  // Just to suppress a potential warning
#ifdef RD_JAVA_NGA_ENABLED
#ifdef RD_JAVA_UI_ALFDRAWER_ENABLED
            aWindow->EndRedraw();
            TInt result = KErrAbort;
            TRAPD(err,result = aGc->FinishOperationL());

            if (err != KErrNone || result != KErrNone)
            {
                CCoeEnv::Static()->WsSession().Flush();
                CCoeEnv::Static()->WsSession().Finish();
            }
            aWindow->BeginRedraw(aDrawRect);
#endif // RD_JAVA_UI_ALFDRAWER_ENABLED
#endif // RD_JAVA_NGA_ENABLED

            aGc->CopyArea(TRect(*pointSrc, *size), *pointDest);
        }
        break;
        case OP_COPYAREATOIMAGE:
        {
            MSwtImage* image = reinterpret_cast<MSwtImage*>(aIntParams[ i++ ]);
            TInt& srcX       = aIntParams[ i++ ];;
            TInt& srcY       = aIntParams[ i++ ];

            TPoint pos(srcX, srcY);

            (void)aWindow;    // Just to suppress a potential warning 
            (void)aDrawRect;  // Just to suppress a potential warning
#ifdef RD_JAVA_NGA_ENABLED
#ifdef RD_JAVA_UI_ALFDRAWER_ENABLED
            aWindow->EndRedraw();
            TInt result = KErrAbort;
            TRAPD(err,result = aGc->FinishOperationL());

            if (err != KErrNone || result != KErrNone)
            {
                CCoeEnv::Static()->WsSession().Flush();
                CCoeEnv::Static()->WsSession().Finish();
            }
            aWindow->BeginRedraw(aDrawRect);
#endif // RD_JAVA_UI_ALFDRAWER_ENABLED
#endif // RD_JAVA_NGA_ENABLED
            aGc->CopyAreaL(*image, pos);
        }
        break;
        case OP_DRAWM2GIMAGE:
        {
            TInt bitmapHandles[4];
            bitmapHandles[0]      = aIntParams[ i++ ];
            bitmapHandles[1]      = aIntParams[ i++ ];
            bitmapHandles[2]      = aIntParams[ i++ ];
            bitmapHandles[3]      = aIntParams[ i++ ];
            TInt& srcX            = aIntParams[ i++ ];
            TInt& srcY            = aIntParams[ i++ ];
            TInt& srcWidth        = aIntParams[ i++ ];
            TInt& srcHeight       = aIntParams[ i++ ];
            TInt& destX           = aIntParams[ i++ ];
            TInt& destY           = aIntParams[ i++ ];
            TInt& destWidth       = aIntParams[ i++ ];
            TInt& destHeight      = aIntParams[ i++ ];
            TInt& useNativeClear  = aIntParams[ i++ ];

            TPoint* srcPos   = reinterpret_cast<TPoint*>(&srcX);
            TSize* srcSize   = reinterpret_cast<TSize*>(&srcWidth);
            TPoint* destPos  = reinterpret_cast<TPoint*>(&destX);
            TSize*  destSize = reinterpret_cast<TSize*>(&destWidth);

            TRAP_IGNORE(aGc->DrawM2GImageL(bitmapHandles, *srcPos, *srcSize, *destPos, *destSize, useNativeClear));
        }
        break;
        default:
            // Abort drawing, buffer is messed up
            ASSERT(EFalse);
            break;
        }
        // If Java code is putting this flag in to separate the parameters for different
        // operations then it can be verified here for debug purposes.
        //ASSERT(aIntParams[i++] == OP_END);
    }
    // All parameters must have been consumed
    ASSERT((i == aIntCount) && (s == strLength));
}
