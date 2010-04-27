/*******************************************************************************
 * Copyright (c) 2000, 2005 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Lynne Kues (IBM Corp) - modified to reflect eSWT API subset
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.swt.widgets;


import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.ControlListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.ercp.swt.expanded.internal.OS;
import org.eclipse.ercp.swt.mobile.Command;
import java.util.Vector;


/**
 * Instances of this class allow the user to select a font
 * from all available fonts in the system.
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>(none)</dd>
 * <dt><b>Events:</b></dt>
 * <dd>(none)</dd>
 * </dl>
 * <p>
 * IMPORTANT: This class is intended to be subclassed <em>only</em>
 * within the SWT implementation.
 * </p>
 */
public class FontDialog extends Dialog
{
    // Constants
    private final int[] HEIGHTS = {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 24, 36, 48, 72};
    private final String BOLD = "B";
    private final String ITALIC = "I";

    // Components
    private Shell shell;
    private Combo faceCombo;
    private Combo heightCombo;
    private Label previewLabel;
    private Button weightButton;
    private Button styleButton;
    private Button colorButton;
    private Composite row4;
    Button lsk;
    Button rsk;

    // Component data
    private String title = "";
    private String stringOk = "";
    private String stringCancel = "";
    private Font previewFont;
    private Color previewColor;
    private Image colorButtonImg;
    private Font weightButtonFont;
    private Font styleButtonFont;
    private FontData defFontData;

    // Result data
    private FontData resFontData = null;
    private RGB resRgb = null;
    private boolean resRgbChanged = false;

    // Flags
    private boolean isOpen = false;

    // Platform fonts
    private String[] platformFonts;

    // Listeners
    private Listener settingsListener;
    private Listener skListener;

    /**
     * Constructs a new instance of this class given its parent
     * and a style value describing its behavior and appearance.
     * <p>
     * The style value is either one of the style constants defined in
     * class <code>SWT</code> which is applicable to instances of this
     * class, or must be built by <em>bitwise OR</em>'ing together
     * (that is, using the <code>int</code> "|" operator) two or more
     * of those <code>SWT</code> style constants. The class description
     * lists the style constants that are applicable to the class.
     * Style bits are also inherited from superclasses.
     * </p>
     *
     * @param parent a shell which will be the parent of the new instance
     * @param style the style of dialog to construct
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     */
    public FontDialog(Shell parent)
    {
        this(parent, SWT.NONE);
    }

    /**
     * Constructs a new instance of this class given only its parent.
     *
     * @param parent a shell which will be the parent of the new instance
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     */
    public FontDialog(Shell parent, int style)
    {
        super(parent, style);
        checkSubclass();
        stringOk = OS.FontDialog_GetCommandOkTitle();
        stringCancel = OS.FontDialog_GetCommandCancelTitle();
        platformFonts = OS.FontDialog_GetFontNames(true);
        defFontData = parent.getDisplay().getSystemFont().getFontData()[0];
    }

    /**
     * Returns a FontData set describing the font that was
     * selected in the dialog, or null if none is available.
     *
     * @return the FontData for the selected font, or null
     * @since 2.1.1
     */
    public FontData[] getFontList()
    {
        if (resFontData == null)
        {
            return null;
        }
        else
        {
            return new FontData[] {resFontData};
        }
    }

    /**
     * Returns the currently selected color in the receiver.
     *
     * @return the RGB value for the selected color, may be null
     *
     * @see PaletteData#getRGBs
     *
     * @since 2.1
     */
    public RGB getRGB()
    {
        return resRgb;
    }

    /**
     * Makes the dialog visible and brings it to the front
     * of the display.
     *
     * @return a FontData object describing the font that was selected,
     *         or null if the dialog was cancelled or an error occurred
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the dialog has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the dialog</li>
     * </ul>
     */
    public FontData open()
    {
        // Check thread access
        if (!parent.getDisplay().isValidThread())
        {
            error(SWT.ERROR_THREAD_INVALID_ACCESS);
        }

        // Dialog shell
        int flags = SWT.BORDER | SWT.APPLICATION_MODAL | SWT.ON_TOP;
        if (title != null && title.length() > 0)
            flags |= SWT.TITLE;
        shell = new Shell(getParent(), flags);
        shell.setText(title);
        shell.open();
        
        // Listen to skin and resolution changes
        settingsListener = new Listener()
        {
            public void handleEvent(Event ev)
            {
                if (resRgb == null || !resRgbChanged)
                {
                    resRgb = Display.getDefault().getSystemColor(SWT.COLOR_WIDGET_FOREGROUND).getRGB();
                    updatePreviewColor();
                }
                Rectangle rect = shell.internal_getDefaultBounds(); // default dialog bounds
                Rectangle screenRect = parent.getDisplay().getBounds();
                rect.height += row4.computeSize(shell.getClientArea().width, SWT.DEFAULT).y;
                if (screenRect.height > screenRect.width)
                {
                    // Bottom position for portrait orientation
                    rect.y = screenRect.height - rect.height;
                }
                else
                {
                    // Vertical middle position for landscape orientation
                    rect.y = (screenRect.height - rect.height) / 2;
                }
                
                shell.setBounds(rect);
                updatePreviewFont();
            }
        };
        shell.getDisplay().addListener(SWT.Settings, settingsListener);
        
        // Listen to softkey presses and deliver them to the buttons
        skListener = new Listener(){
            public void handleEvent(Event event) {
                switch(event.keyCode) {
                case -6: // LSK
                    isOpen = false;
                    break;
                case -7: // RSK
                    resFontData = null;
                    resRgb = null;
                    isOpen = false;
                    break;
                default:
                    break;
                }
            }};
        parent.getDisplay().addFilter(SWT.KeyDown, skListener);

        // Create default results if not already set
        if (resFontData == null)
        {
            resFontData = defFontData;
        }
        if (resRgb == null || !resRgbChanged)
        {
            resRgb = Display.getDefault().getSystemColor(SWT.COLOR_WIDGET_FOREGROUND).getRGB();
        }

        GridLayout layout0 = new GridLayout();
        layout0.marginTop = 0;
        layout0.marginBottom = 0;
        layout0.marginLeft = 0;
        layout0.marginRight = 0;
        layout0.marginWidth = 0;
        layout0.marginHeight = 0;
        layout0.horizontalSpacing = 1;
        layout0.verticalSpacing = 1;
        shell.setLayout(layout0);
        
        // Row 1
        createFaceControl(shell);

        // Row 2
        Composite row2 = new Composite(shell, 0);
        row2.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        GridLayout layout2 = new GridLayout(5, true);
        layout2.marginTop = 0;
        layout2.marginBottom = 0;
        layout2.marginLeft = 0;
        layout2.marginRight = 0;
        layout2.marginWidth = 0;
        layout2.marginHeight = 0;
        layout2.horizontalSpacing = 1;
        layout2.verticalSpacing = 1;
        row2.setLayout(layout2);
        createHeightControl(row2);
        createWeightControl(row2);
        createStyleControl(row2);
        createColorControl(row2);
        
        // Row 3
        createPreviewControl(shell);
        
        // Row 4
        row4 = new Composite(shell, 0);
        row4.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        row4.setLayout(new FillLayout(SWT.HORIZONTAL));
        
        createButtons(row4);
        
        Rectangle rect = shell.getBounds(); // default dialog bounds
        Rectangle screenRect = parent.getDisplay().getBounds();
        rect.height += row4.computeSize(shell.getClientArea().width, SWT.DEFAULT).y;
        if (screenRect.height > screenRect.width)
        {
            // Bottom position for portrait orientation
            rect.y = screenRect.height - rect.height;
        }
        else
        {
            // Vertical middle position for landscape orientation
            rect.y = (screenRect.height - rect.height) / 2;
        }
        
        shell.setBounds(rect);

        // Block till submited
        isOpen = true;
        Display display = shell.getDisplay();
        while (isOpen)
        {
            if (!display.readAndDispatch())
            {
                display.sleep();
            }
        }
        destroyControls();

        return resFontData;
    }

    /**
     * Sets a set of FontData objects describing the font to
     * be selected by default in the dialog, or null to let
     * the platform choose one.
     *
     * @param fontData the set of FontData objects to use initially, or null
     * @since 2.1.1
     */
    public void setFontList(FontData [] fontData)
    {
        if (fontData != null && fontData.length > 0)
        {
            resFontData = fontData[0];
        }
        else
        {
            resFontData = null;
        }
    }

    /**
     * Sets the receiver's selected color to be the argument.
     *
     * @param rgb the new RGB value for the selected color, may be
     *        null to let the platform to select a default when
     *        open() is called
     *
     * @see PaletteData#getRGBs
     *
     * @since 2.1
     */
    public void setRGB(RGB rgb)
    {
        resRgb = rgb;
    }

    /**
     * Sets the receiver's text, which is the string that the
     * window manager will typically display as the receiver's
     * <em>title</em>, to the argument, which must not be null.
     *
     * @param string the new text
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the text is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setText(String string)
    {
        super.setText(string);
        title = string;
    }

    private void createButtons(final Composite parent) {
        lsk = new Button(parent, 0);
        shell.setDefaultButton(lsk);
        lsk.setText(stringOk);
        lsk.addSelectionListener(
            new SelectionListener()
        {
            public void widgetDefaultSelected(SelectionEvent ev)
            {
            }
            public void widgetSelected(SelectionEvent ev)
            {
                isOpen = false;
            }
        });
        
        rsk = new Button(parent, 0);
        rsk.setText(stringCancel);
        rsk.addSelectionListener(
            new SelectionListener()
        {
            public void widgetDefaultSelected(SelectionEvent ev)
            {
            }
            public void widgetSelected(SelectionEvent ev)
            {
                resFontData = null;
                resRgb = null;
                isOpen = false;
            }
        });
    }

    private void createColorControl(final Composite parent) {
        // Control
        colorButton = new Button(parent, SWT.PUSH);
        colorButton.addSelectionListener(
            new SelectionListener()
        {
            public void widgetDefaultSelected(SelectionEvent ev)
            {
            }
            public void widgetSelected(SelectionEvent ev)
            {
                ColorDialog dlg = new ColorDialog(parent.getShell(), SWT.NONE);
                RGB rgb = dlg.open();
                fontColorChanged(rgb);
            }
        });
        
        // Layout
        colorButton.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, false));
    }

    private void createFaceControl(final Shell parent) {
        // Control
        faceCombo = new Combo(parent, SWT.READ_ONLY);
        faceCombo.addSelectionListener(
            new SelectionListener()
        {
            public void widgetDefaultSelected(SelectionEvent ev)
            {
            }
            public void widgetSelected(SelectionEvent ev)
            {
                fontFaceChanged();
            }
        });

        // Items
        faceCombo.setItems(platformFonts);

        // Defaults
        int index = faceCombo.indexOf(resFontData.getName());
        if (index >= 0)
        {
            faceCombo.select(index);
        }

        // Layout
        faceCombo.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
    }

    private void createHeightControl(final Composite parent) {
        // Control
        heightCombo = new Combo(parent, SWT.READ_ONLY);
        heightCombo.addSelectionListener(
            new SelectionListener()
        {
            public void widgetDefaultSelected(SelectionEvent ev)
            {
            }
            public void widgetSelected(SelectionEvent ev)
            {
                fontHeightChanged();
            }
        });
        
        // Items
        String[] items = new String[HEIGHTS.length];
        for (int i = 0; i < HEIGHTS.length; i++)
        {
            items[i] = Integer.toString(HEIGHTS[i]);
        }
        heightCombo.setItems(items);

        // Defaults
        int pos = -1;
        for (int i = 0; i < HEIGHTS.length; i++)
        {
            if (resFontData.getHeight() <= HEIGHTS[i])
            {
                pos = i;
                break;
            }
        }
        if (pos >= 0)
        {
            heightCombo.select(pos);
        }
        
        // Layout
        heightCombo.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, 2, 1));
    }

    private void createStyleControl(final Composite parent) {
        // Control
        styleButton = new Button(parent, SWT.TOGGLE);
        styleButtonFont = new Font(parent.getDisplay(), 
            new FontData(defFontData.getName(), defFontData.getHeight(), SWT.ITALIC));
        styleButton.setFont(styleButtonFont);
        styleButton.setText(this.ITALIC);
        styleButton.addSelectionListener(
            new SelectionListener()
        {
            public void widgetDefaultSelected(SelectionEvent ev)
            {
            }
            public void widgetSelected(SelectionEvent ev)
            {
                fontStyleChanged();
            }
        });

        // Defaults
        if ((resFontData.getStyle() & SWT.ITALIC) != 0)
        {
            styleButton.setSelection(true);
        }
        
        // Layout
        styleButton.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, false));
    }

    private void createWeightControl(final Composite parent) {
        // Control
        weightButton = new Button(parent, SWT.TOGGLE);
        weightButtonFont = new Font(parent.getDisplay(), 
            new FontData(defFontData.getName(), defFontData.getHeight(), SWT.BOLD));
        weightButton.setFont(weightButtonFont);
        weightButton.setText(this.BOLD);
        weightButton.addSelectionListener(
            new SelectionListener()
        {
            public void widgetDefaultSelected(SelectionEvent ev)
            {
            }
            public void widgetSelected(SelectionEvent ev)
            {
                fontWeightChanged();
            }
        });

        // Defaults
        if ((resFontData.getStyle() & SWT.BOLD) != 0)
        {
            weightButton.setSelection(true);
        }
        
        // Layout
        weightButton.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, false));
    }

    private void createPreviewControl(final Shell parent) {
        previewLabel = new Label(parent, SWT.CENTER | SWT.BORDER);
        previewLabel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        updatePreviewText();
        updatePreviewFont();
        updatePreviewColor();
    }

    private void destroyControls()
    {
        shell.getDisplay().removeListener(SWT.Settings, settingsListener);
        shell.getDisplay().removeListener(SWT.KeyDown, skListener);

        shell.dispose();
        shell = null;
        
        faceCombo = null;
        heightCombo = null;
        previewLabel = null;
        weightButton = null;
        styleButton = null;
        colorButton = null;
        row4 = null;
        lsk = null;
        rsk = null;

        if (previewFont != null)
        {
            previewFont.dispose();
            previewFont = null;
        }
        if (previewColor != null)
        {
            previewColor.dispose();
            previewColor = null;
        }
        if (colorButtonImg != null)
        {
            colorButtonImg.dispose();
            colorButtonImg = null;
        }
        if (weightButtonFont != null)
        {
            weightButtonFont.dispose();
            weightButtonFont = null;
        }
        if (styleButtonFont != null)
        {
            styleButtonFont.dispose();
            styleButtonFont = null;
        }
    }

    private void fontColorChanged(RGB newRGB)
    {
        if (newRGB == null)
        {
            return;
        }
        resRgb = newRGB;
        resRgbChanged = true;
        updatePreviewColor();
    }

    private void fontFaceChanged()
    {
        int index = faceCombo.getSelectionIndex();
        if (index >= 0)
        {
            String fontFace = faceCombo.getItem(index);
            resFontData.setName(fontFace);
            updatePreviewText();
            updatePreviewFont();
        }
    }

    private void fontHeightChanged()
    {
        int index = heightCombo.getSelectionIndex();
        if (index >= 0 && index < HEIGHTS.length)
        {
            resFontData.setHeight(HEIGHTS[index]);
            updatePreviewFont();
        }
    }

    private void fontStyleChanged()
    {
        if (styleButton.getSelection())
        {
            resFontData.setStyle(resFontData.getStyle() | SWT.ITALIC);
        }
        else
        {
            resFontData.setStyle(resFontData.getStyle() & (~SWT.ITALIC));
        }
        updatePreviewFont();
    }

    private void fontWeightChanged()
    {
        if (weightButton.getSelection())
        {
            resFontData.setStyle(resFontData.getStyle() | SWT.BOLD);
        }
        else
        {
            resFontData.setStyle(resFontData.getStyle() & (~SWT.BOLD));
        }
        updatePreviewFont();
    }

    private void updatePreviewColor()
    {
        // Create new color based on result rgb
        Color color = new Color(shell.getDisplay(), resRgb);

        // Set new color to preview label
        if (previewLabel != null)
        {
            previewLabel.setForeground(color);
        }

        // Set new color to color button image
        Image img = null;
        int h = weightButton.getSize().y / 2;
        if (h <= 0)
            h = weightButton.computeSize(-1, -1).y / 2;
        img = new Image(shell.getDisplay(), h, h);
        GC gc = new GC(img);
        gc.setBackground(color);
        gc.fillRectangle(0, 0, h, h);
        gc.dispose();
        colorButton.setImage(img);

        // Replace old preview color
        if (previewColor != null)
        {
            previewColor.dispose();
        }
        previewColor = color;

        // Replace old color button image
        if (colorButtonImg != null)
        {
            colorButtonImg.dispose();
        }
        colorButtonImg = img;
    }

    private void updatePreviewFont()
    {
        Font font = new Font(shell.getDisplay(), resFontData);
        if (previewLabel != null)
        {
            previewLabel.setFont(font);
        }
        if (previewFont != null)
        {
            previewFont.dispose();
        }
        previewFont = font;
    }

    private void updatePreviewText()
    {
        if (previewLabel != null)
        {
            previewLabel.setText(resFontData.getName());
        }
    }

}
