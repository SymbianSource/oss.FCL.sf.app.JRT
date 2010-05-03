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
package javax.microedition.lcdui.game;

/**
 * Repository for text messages used in OpenLCDUI's game package. String
 * constants in this class should begin with prefix "CLASSNAME_" where CLASSNAME
 * is the name of the class which is using the constant.
 */
final class MsgRepository {

    public static final String SPRITE_EXCEPTION_INVALID_WIDTH_HEIGHT =
        "Width and height cannot be negative";

    public static final String SPRITE_EXCEPTION_INVALID_SEQUENCE_LENGTH =
        "Invalid sequence length";

    public static final String SPRITE_EXCEPTION_INVALID_FRAME =
        "Invalid frame at ";

    public static final String SPRITE_EXCEPTION_INVALID_FRAME_INDEX =
        "Invalid frame index";

    public static final String SPRITE_EXCEPTION_INVALID_TRANSFORM =
        "Invalid transform";


    public static final String TILEDLAYER_EXCEPTION_INVALID_DIMENSIONS =
        "Invalid number of rows or columns";

    public static final String TILEDLAYER_EXCEPTION_INVALID_NUMBER_OF_COLUMNS =
        "numCols is less than zero";

    public static final String TILEDLAYER_EXCEPTION_INVALID_NUMBER_OF_ROWS =
        "numRows is less than zero";

    public static final String TILEDLAYER_EXCEPTION_INVALID_TILE_INDEX =
        "Invalid tileIndex";

    public static final String TILEDLAYER_EXCEPTION_INVALID_ANIMTILE_INDEX =
        "Invalid animatedTileIndex";


    public static final String LAYER_EXCEPTION_INVALID_TILE_WIDTH_HEIGHT =
        "Invalid tileWidth or tileHeight";



    public static final String LAYERMANAGER_EXCEPTION_LAYER_NULL =
        "Layer is null";

    public static final String LAYERMANAGER_EXCEPTION_INVALID_LAYER_INDEX =
        "Invalid layer index";

    public static final String LAYERMANAGER_EXCEPTION_INVALID_WIDTH_HEIGHT =
        "Width and height cannot be negative";


    /**
     * Private Constructor.
     */
    private MsgRepository() {
    }

}
