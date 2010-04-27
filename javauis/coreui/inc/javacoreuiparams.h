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
* Description: A class for storing the creation paramters for the core UI
*
*/

#ifndef JAVACOREUIPARAMS_H
#define JAVACOREUIPARAMS_H

#include <string>

#include "javaosheaders.h"


namespace java // codescanner::namespace
{
namespace ui // codescanner::namespace
{
enum UiOrientation // codescanner::enummembers::enumnames
{
    UNDEFINED,  // The default, the UI is rotated normally.
    PORTRAIT,   // User wants to keep the UI always in portrait.
    LANDSCAPE   // User wants to keep the UI always in landscape.
};

enum StartScreenMode // codescanner::enummembers::enumnames
{
    NO_START_SCREEN,       // The should be no start screeen (== suppress).
    DEFAULT_START_SCREEN,  // Show the default start screen.
    MIDLET_DEFINED_SCREEN, // MIDlet defined start screen should be shown.
    // In this case the image path must point to
    // root of the MIDlet from where can be found
    // the start screen image(s) created by the
    // Java installer
    USER_DEFINED_SCREEN    // Application defined start screen should be
    // showed. In this case the image path must point
    // to valid gif file.
};

OS_NONSHARABLE_CLASS(CoreUiParams)
{
public:

    /**
     * CoreUiParams default constructor.
     */
    CoreUiParams();

    /**
     * CoreUiParams destructor.
     */
    ~CoreUiParams();

    /**
     * CoreUiParams assign operator.
     */
    CoreUiParams& operator= (const CoreUiParams&);

    /**
     * A setter for the requested orientation of the UI.
     */
    inline void setOrientation(UiOrientation orientation);
    /**
     * A getter for the requested orientation of the UI.
     */
    inline UiOrientation getOrientation() const;

    /**
     * A setter for the requested start screen mode.
     */
    inline void setScreenMode(StartScreenMode screenMode);
    /**
     * A getter for the requested start screen mode.
     */
    inline StartScreenMode getScreenMode() const;

    /**
     * A setter for the image path.
     */
    inline void setImagePath(const std::wstring& path);
    /**
     * A getter for the image path.
     */
    inline const std::wstring& getImagePath() const;

    /**
     * A setter for the background starting info.
     */
    inline void setBackgroundStart(bool bg);
    /**
     * A getter for the background starting info.
     */
    inline bool isBackgroundStart() const;

private:

    /**
     * No copy constructor allowed
     */
    CoreUiParams(const CoreUiParams&);

private:
    UiOrientation   mOrientation;
    StartScreenMode mStartScreenMode;
    std::wstring    mPath;
    bool            mBackgroundStart;
};

} //end namespace ui
} //end namespace java


//START OF INLINE METHODS
inline
java::ui::CoreUiParams::CoreUiParams() : mOrientation(UNDEFINED),
        mStartScreenMode(NO_START_SCREEN),
        mBackgroundStart(false)
{
}

inline java::ui::CoreUiParams::~CoreUiParams()
{
}

inline
void java::ui::CoreUiParams::setOrientation(java::ui::UiOrientation orientation)
{
    mOrientation = orientation;
}

inline
java::ui::UiOrientation java::ui::CoreUiParams::getOrientation() const
{
    return mOrientation;
}

inline
void java::ui::CoreUiParams::setScreenMode(java::ui::StartScreenMode screenMode)
{
    mStartScreenMode = screenMode;
}

inline
java::ui::StartScreenMode java::ui::CoreUiParams::getScreenMode() const
{
    return mStartScreenMode;
}

inline
void java::ui::CoreUiParams::setImagePath(const std::wstring& path)
{
    mPath = path;
}

inline
const std::wstring& java::ui::CoreUiParams::getImagePath() const
{
    return mPath;
}

inline
void java::ui::CoreUiParams::setBackgroundStart(bool bg)
{
    mBackgroundStart = bg;
}

inline
bool java::ui::CoreUiParams::isBackgroundStart() const
{
    return mBackgroundStart;
}

inline
java::ui::CoreUiParams& java::ui::CoreUiParams::operator= (const CoreUiParams& cup)
{
    if (this != &cup)
    {
        mOrientation     = cup.mOrientation;
        mStartScreenMode = cup.mStartScreenMode;
        mPath            = cup.mPath;
        mBackgroundStart = cup.mBackgroundStart;
    }
    return *this;
}

#endif // JAVACOREUIPARAMS_H
