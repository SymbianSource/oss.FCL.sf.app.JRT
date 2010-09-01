/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/

#ifndef SWTEDWIN_H
#define SWTEDWIN_H

class CEikEdwin;
class CLafEdwinCustomDrawBase;
class CSwtEdwinCustomDrawer;
class CEikFloatingPointEditor;
class CAknIntegerEdwin;

/**
 * Class extending CEikEdwin so it allows custom clipping.
 *
 * This class is needed to prevent edwin painting outside of
 * its parent control.
 */
NONSHARABLE_CLASS(CSwtEdwin)
        : public CEikEdwin
{
public:
    CSwtEdwin();

    /**
     * Sets custom clipping rectangle.
     */
    void SetClippingRect(const TRect& aRect);

// From CEikEdwin
protected:
    CLafEdwinCustomDrawBase* CreateCustomDrawL();

private:
    /**
     * Custom drawer.
     */
    CSwtEdwinCustomDrawer* iClippingCustomDrawer;
};


/**
 * Class extending CEikFloatingPointEditor so it allows custom clipping.
 *
 * This class is needed to prevent edwin painting outside of
 * its parent control.
 */
NONSHARABLE_CLASS(CSwtFloatingPointEdwin)
        : public CEikFloatingPointEditor
{
public:
    CSwtFloatingPointEdwin();

    /**
     * Override needed for readonly editor.
     */
    TCoeInputCapabilities InputCapabilities() const;

    /**
     * Sets custom clipping rectangle.
     */
    void SetClippingRect(const TRect& aRect);

// From CEikEdwin
protected:
    CLafEdwinCustomDrawBase* CreateCustomDrawL();

private:
    /**
     * Custom drawer.
     */
    CSwtEdwinCustomDrawer* iClippingCustomDrawer;
};


/**
 * Class extending CAknIntegerEdwin so it allows custom clipping.
 *
 * This class is needed to prevent edwin painting outside of
 * its parent control.
 */
NONSHARABLE_CLASS(CSwtIntegerEdwin)
        : public CAknIntegerEdwin
{
public:
    CSwtIntegerEdwin();

    /**
     * Sets custom clipping rectangle.
     */
    void SetClippingRect(const TRect& aRect);

// From CEikEdwin
protected:
    CLafEdwinCustomDrawBase* CreateCustomDrawL();

private:
    /**
     * Custom drawer.
     */
    CSwtEdwinCustomDrawer* iClippingCustomDrawer;
};

#endif // SWTEDWIN_H
