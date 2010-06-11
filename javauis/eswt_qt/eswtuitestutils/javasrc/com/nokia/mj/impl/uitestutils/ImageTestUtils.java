package com.nokia.mj.impl.uitestutils;

import org.eclipse.swt.internal.qt.graphics.Image;
import java.util.Arrays;

/**
 * Image utils for verifying image pixels against mask
 * plus some other helpers for verifying image properties. 
 * 
 * NOTE. Currently only CGFX images are supported however eSWT images 
 * can be verified by using org.eclipse.swt.internal.qt.graphics.Image internal_getImage()
 * to obtain wrapped CGFX image.
 * 
 * Usage example
 * 
 * create a mask from mask data and then mask by giving data and scanlenght.
 * <code>
 * int[] maskData = {
 *   	0,0,0,0,0,0,0,0,0,
 * 		0,0,0,0,1,0,0,0,0,
 * 		0,0,0,0,1,0,0,0,0,
 * 		0,0,0,0,1,0,0,0,0,
 * 		0,0,0,0,1,0,0,0,0,
 * 		0,0,0,0,1,0,0,0,0,
 * 		0,0,0,0,1,0,0,0,0,
 * 		0,0,0,0,0,0,0,0,0
 * };
 * 
 * Mask mask = new Mask(maskData, 9);
 * </code>
 * 
 * Then image pixels can be validated with ImageTestUtils. That is where there is 1 in mask it is
 * compared against maskArgb and all where there is 0 in mask it is compared against bgArgb.
 * <code>
 * ImageTestUtils.validatePixels(imageToValidate, mask, maskArgb, bgArgb); 
 * </code>
 * 
 * @author sampkaar
 *
 */
public class ImageTestUtils {

	/** 
	 * This pixel is rendering target image background color.
	 */
	public static final int IMAGE_BACKGROUND_COLOR = 0;

	/** 
	 * This pixel is foreground color set to GC.
	 */
	public static final int GC_FOREGROUND_COLOR = 1;

	/**
	 * This pixel is either foreground or image background color.
	 * Used for example in drawRoundRect and drawEllipse tests.
	 */
	public static final int FOREGROUND_OR_IMAGE_BACKGROUND_COLOR = 2;
	
	/** 
	 * This pixel is background color set to GC.
	 * Used for example in fillGradientRect test.
	 */
	public static final int GC_BACKGROUND_COLOR = 3;

	/**
	 * This pixel is neither foreground nor gc background color. 
	 * Used for example in fillGradientRect test. 
	 */
	public static final int NEITHER_FOREGROUND_NOR_GC_BACKGROUND_COLOR = 4;

	/** 
	 * This pixel can be ignored.
	 * Used for example in fillGradientRect test.
	 */
	public static final int IGNORE_COLOR = 5;
	
	/**
	 * Validates image dimensions against reference width and height.
	 * Prints possible problems to System.out.
	 * 
	 * @param img - The image to check
	 * @param refWidth - The reference width
	 * @param refHeight - The reference height
	 */
	public static final boolean validateImageDimensions(Image img, int refWidth, int refHeight) {
		if ((refWidth != img.getWidth()) || (refHeight != img.getHeight())) {
			System.out.println("Image size should be ("+refWidth+","+refHeight+"), "+
							   "but is ("+img.getWidth()+","+img.getHeight()+")");
			return false;
		}
		return true;
	}
	
	/**
	 * Validates image pixels which are outside specified rectangle ingoreRect against given reference color.
	 * 
	 * @param ignoreRect The area that is to be ingnored
	 * @param img Image to validate
	 * @param argb The reference color
	 * @return true if all pixels outside ingoreRect have reference color orherwise false 
	 */
	public static final boolean validatePixels(Rect ignoreRect, Image img, int argb) {
		final int width = img.getWidth();
		final int height = img.getHeight();
		final int pixels =  width * height;
		final int scanlength = img.getWidth();
		
		// data array for image data
		int[] imageData = new int[pixels];
		
		// get data for whole image
		img.getRGB(imageData, 0, width, 0, 0, width, height);

		// calculate start index
		int start = scanlength*ignoreRect.y();
		for(int y = 0; y<ignoreRect.height();y++) {
			Arrays.fill(imageData, start+ignoreRect.x(), start+ignoreRect.width()+ignoreRect.x(), argb);
			// move start to next line
			start += scanlength;
		}
		
		// validate array
		for(int i=0; i < pixels; i++) {
			if(!(imageData[i] == argb)) {
				return false;
			}
		}
		return true;
	}
	
	/**
	 * Validates that there is at least one pixel with foreground color inside the specified rectangle
	 * and that image pixels which are outside specified rectangle has background color.
	 * 
	 * @param rect The area that is to be checked
	 * @param img Image to validate
	 * @param fgColor The color that must exist inside the rect
	 * @param bgColor The color which must be outside the rect
	 * @return true if all pixels outside rect have background color and at least one pixel inside the rect has foreground color 
	 *         orherwise false 
	 */
	public static final boolean validatePixels(Rect rect, Image img, int fgColor, int bgColor) {
		final int width = img.getWidth();
		final int height = img.getHeight();
		final int pixels =  width * height;
		final int scanlength = img.getWidth();
		boolean foundFgColor = false;
				
		// data array for image data
		int[] imageData = new int[pixels];
		
		// get data for whole image
		img.getRGB(imageData, 0, width, 0, 0, width, height);

		Rect imageRect = new Rect(0,0,width, height);
		rect = rect.intersection(imageRect);

		// Check that foreground color was found inside the rect
		foundFgColor = isAtleastOnePixelWithRefColor(img, rect, fgColor);		
		if(!foundFgColor){
			return false;
		}
		
		int start = scanlength*rect.y();
		// Fill the rect with background color
		for(int y = 0; y<rect.height();y++) {
			Arrays.fill(imageData, start+rect.x(), start+rect.width()+rect.x(), bgColor);
			// move start to next line
			start += scanlength;
		}
		
		// validate array, now all pixels should have background color
		for(int i=0; i < pixels; i++) {
			if(!(imageData[i] == bgColor)) {
				return false;
			}
		}
		return true;
	}

	/**
	 * Validates that there is at least one pixel with foreground color inside the specified rectangle
	 * and that image pixels which are outside specified rectangle has background color.
	 * 
	 * @param rect The area that is to be checked
	 * @param img Image to validate
	 * @param fgColor The color that must exist inside the rect
	 * @param textRectColor The background color of the rect 
	 * @param bgColor The color which must be outside the rect
	 * @param textRectColorOn true if textRectColor must exist inside the rect, false if textRectColor shouldn't be found inside the rect 
	 * @return true if all pixels outside rect have background color and at least one pixel inside the rect has foreground color 
	 *         orherwise false 
	 */
	public static final boolean validatePixels(Rect rect, Image img, int fgColor, int textRectColor, int bgColor, boolean textRectColorOn) {

		if(textRectColorOn){
			// Check that text background color was found inside the rect
			if(!isAtleastOnePixelWithRefColor(img, rect, textRectColor)){
				return false;
			}
		} else {
			// Check that text background color is not found inside the rect		
			if(isAtleastOnePixelWithRefColor(img, rect, textRectColor)){
				return false;
			}
		}
		
		return validatePixels(rect, img, fgColor, bgColor);
	}

	/**
	 * Validates that there is at least one pixel with foreground color inside the specified rectangles
	 * and that image pixels which are outside specified rectangles has background color.
	 * 
	 * @param rects The areas that is to be checked
	 * @param img Image to validate
	 * @param fgColor The color that must exist inside the rects
	 * @param bgColor The color which must be outside the rects
	 * @return true if all pixels outside rects have background color and at least one pixel inside the rects has foreground color 
	 *         orherwise false 
	 */
	public static final boolean validatePixels(Rect[] rects, Image img, int fgColor, int bgColor) {
		final int width = img.getWidth();
		final int height = img.getHeight();
		final int pixels =  width * height;
		final int scanlength = img.getWidth();
		boolean foundFgColor = false;
		final int rectCount = rects.length;

		Rect imageRect = new Rect(0,0,width, height);
		// Check that foreground color is found inside each rect
		for(int i=0; i<rectCount; i++){
			rects[i] = rects[i].intersection(imageRect);
			foundFgColor = isAtleastOnePixelWithRefColor(img, rects[i], fgColor);		
			if(!foundFgColor){
				return false;
			}
		}
		
		// Check that background color is found outside the rects

		// data array for image data
		int[] imageData = new int[pixels];
		// get data for whole image
		img.getRGB(imageData, 0, width, 0, 0, width, height);

		int start;
		// Fill the rects with background color
		for(int i=0; i<rectCount; i++){
			start = scanlength*rects[i].y();
			for(int y = 0; y<rects[i].height();y++) {
				Arrays.fill(imageData, start+rects[i].x(), start+rects[i].width()+rects[i].x(), bgColor);
				// move start to next line
				start += scanlength;
			}
		}
		
		// validate array, now all pixels should have background color
		for(int i=0; i < pixels; i++) {
			if(!(imageData[i] == bgColor)) {
				return false;
			}
		}
		return true;
	}

	/**
	 * Checks is there at least one pixel with reference color inside the specified rectangle in Image.
	 * 
	 * @param rect The area that is to be checked
	 * @param refColor The color that must exist inside the rect
	 * @return true if at least one pixel inside the rect has reference color orherwise false 
	 */
	public static final boolean isAtleastOnePixelWithRefColor(Image img, Rect rect, int refColor) {
		
		final int width = img.getWidth();
		final int height = img.getHeight();
		final int pixels =  width * height;
		final int scanlength = img.getWidth();

		// data array for image data
		int[] imageData = new int[pixels];
		
		// get data for whole image
		img.getRGB(imageData, 0, width, 0, 0, width, height);

		// calculate start index
		int start = scanlength*rect.y();

		// Check that is at least one pixel has foreground color inside the rect
		for(int y = 0; y<rect.height();y++) {
			for(int i=start+rect.x(); i < start+rect.width()+rect.x(); i++) {			
				if((imageData[i] == refColor)) {
					return true;
				}
			}
			// move start to next line
			start += scanlength;
		}	
		return false;
	}
	
	/**
	 * Validates that every pixel in given image has
	 * given referense color value argb. 
	 * 
	 * @param img - The image to check
	 * @param argb - The referece color to compare
	 */
	public static final boolean validatePixels(Image img, int argb) {
		
		final int width = img.getWidth();
		final int height = img.getHeight();
		final int pixels =  width * height;
		
		// get color components from reference
		final int refAlpha 	= (argb >> 24) 	& 0xff;
		final int refRed   	= (argb >> 16) 	& 0xff;
		final int refGreen 	= (argb >> 8) 	& 0xff;
		final int refBlue 	= (argb) 		& 0xff;
		
		// data array for image data
		int[] imageData = new int[pixels];
		
		// get data for whole image
		img.getRGB(imageData, 0, width, 0, 0, width, height);
	
		// check each pixel in image, break if pixel does not match
		for(int i=0; i < pixels; i++) {
			if (refAlpha != ((imageData[i] >> 24) & 0xff)) return false;
			if (refRed 	 != ((imageData[i] >> 16) & 0xff)) return false; 
			if (refGreen != ((imageData[i] >> 8 ) & 0xff)) return false;
			if (refBlue  != ( imageData[i]        & 0xff)) return false;
		}
		return true;
	}
	
	/**
	 * Validates that given pixels are equal to 
	 * reference pixel color (argb) and background color in given rgb data
	 * 
	 * @param rgbdata - The image data to be checked
	 * @param mask - the reference mask
	 * @param maskArgb - The reference color for pixels with value 1 in mask
	 * @param bgArgb - The reference color for pixels with value 0 in mask
	 * 
	 */
	public static final boolean validatePixels(int[] rgbdata, Mask mask, int maskArgb, int bgArgb) {
		return doCheckPixels(rgbdata, mask, maskArgb, bgArgb, 0, false);
	}
	
	/**
	 * Validates that given pixels are equal to 
	 * reference pixel color (argb) and background color in given rgb data
	 * 
	 * @param rgbdata - The image data to be checked
	 * @param dataOffset The offset from beginning of rgbdata where image data starts
	 * @param dataLength The lenght of image data after offset
	 * @param mask - the reference mask
	 * @param maskArgb - The reference color for pixels with value 1 in mask
	 * @param bgArgb - The reference color for pixels with value 0 in mask
	 * 
	 */
	public static final boolean validatePixels(int[] rgbdata, int dataOffset, int dataLength, Mask mask, int maskArgb, int bgArgb) {
		
		// create array for data 
		int[] data = new int[dataLength];
		
		// copy from rgbdata to data
		for(int i=dataOffset; i < rgbdata.length; i++) {
			data[i-dataOffset] = rgbdata[i];
		}
		
		//System.out.println("\n mask size: " + mask.getData().length+", dataSize: " + rgbdata.length);
		return doCheckPixels(data, mask, maskArgb, bgArgb, 0, false);
	}
	
	/**
	 * Validates that given pixels are equal to 
	 * reference pixel color (argb) and background color in image
	 * 
	 * @param img - The image to check
	 * @param mask - the reference mask
	 * @param maskArgb - The reference color for pixels with value 1 in mask
	 * @param bgArgb - The reference color for pixels with value 0 in mask
	 * 
	 */
	public static final boolean validatePixels(Image img, Mask mask, int maskArgb, int bgArgb) {
		
		// data array for image data
		int[] imageData = new int[img.getWidth()*img.getHeight()];
		
		// get data for whole image
		img.getRGB(imageData, 0, img.getWidth(), 0, 0, img.getWidth(), img.getHeight());
		
		// check pixels
		return doCheckPixels(imageData, mask, maskArgb, bgArgb, 0, false);
	}
	
	/**
	 * Validates that given pixels are equal to 
	 * reference pixel color (argb) and background color in image
	 * 
	 * @param img - The image to check
	 * @param mask - the reference mask
	 * @param maskArgb - The reference color for pixels with value 1 in mask
	 * @param bgArgb - The reference color for pixels with value 0 in mask
	 * @param ingoreAlpha If true alpha value is not validated
	 * @return true if mask and image match otherwise false
	 */
	public static final boolean validatePixels(Image img, Mask mask, int maskArgb, int bgArgb, boolean ignoreAlpha) {
		
		// data array for image data
		int[] imageData = new int[img.getWidth()*img.getHeight()];
		
		// get data for whole image
		img.getRGB(imageData, 0, img.getWidth(), 0, 0, img.getWidth(), img.getHeight());
		
		// check pixels
		return doCheckPixels(imageData, mask, maskArgb, bgArgb, 0, ignoreAlpha);
	}

	/**
	 * Validates that given pixels are equal to 
	 * foreground color (argb), image background color and gc background color in image
	 * 
	 * @param img - The image to check
	 * @param mask - the reference mask
	 * @param fgArgb - The reference color for pixels with value 1 GC_FOREGROUND_COLOR in mask
	 * @param gcBgArgb - The reference color for pixels with value 3 GC_BACKGROUND_COLOR in mask
	 * @param imgBgArgb - The reference color for pixels with value 0 IMAGE_BACKGROUND_COLOR in mask
	 * @param ingoreAlpha If true alpha value is not validated
	 * @return true if mask and image match otherwise false
	 */
	public static final boolean validatePixels(Image img, Mask mask, int fgArgb, int gcBgArgb, int imgBgArgb, boolean ignoreAlpha) {
		
		// data array for image data
		int[] imageData = new int[img.getWidth()*img.getHeight()];
		
		// get data for whole image
		img.getRGB(imageData, 0, img.getWidth(), 0, 0, img.getWidth(), img.getHeight());
		
		// check pixels
		return doCheckPixels(imageData, mask, fgArgb, imgBgArgb, gcBgArgb, ignoreAlpha);
	}

	/*
	 * method for checking pixels against mask
	 * The imageData and the data in mask must be same size.  
	 */
	private static final boolean doCheckPixels(int[] imageData, Mask mask, int fgColorArgb, int imgBgColorArgb, int gcBgColorArgb, boolean ignoreAlpha ) {
		
		// get color components for GC foreground color
		final int fgAlpha 	= (fgColorArgb >> 24) 	& 0xff;
		final int fgRed   	= (fgColorArgb >> 16) 	& 0xff;
		final int fgGreen 	= (fgColorArgb >> 8) 	& 0xff;
		final int fgBlue 	= (fgColorArgb) 		& 0xff;
		
		// get color components for image background color
		final int imgBgAlpha 	= (imgBgColorArgb  >> 24) 	& 0xff;
		final int imgBgRed   	= (imgBgColorArgb  >> 16) 	& 0xff;
		final int imgBgGreen 	= (imgBgColorArgb  >> 8) 	& 0xff;
		final int imgBgBlue 	= (imgBgColorArgb) 			& 0xff;
		
		// get color components for GC background color
		int gcBgAlpha 	= 0;
		int gcBgRed   	= 0;
		int gcBgGreen 	= 0;
		int gcBgBlue 	= 0;
		if (gcBgColorArgb != 0){ // GC Background color is defined
			gcBgAlpha 	= (gcBgColorArgb  >> 24) 	& 0xff;
			gcBgRed   	= (gcBgColorArgb  >> 16) 	& 0xff;
			gcBgGreen 	= (gcBgColorArgb  >> 8) 	& 0xff;
			gcBgBlue 	= (gcBgColorArgb) 			& 0xff;
		}
		
		// get mask data
		int[] maskdata = mask.getData();
		
		// verify background & pixels
		for(int i=0; i < imageData.length; i++) {

			// if mask pixel is GC_FOREGROUND_COLOR (1), check against fgArgb
			if(maskdata[i] == GC_FOREGROUND_COLOR) {	
				if (!ignoreAlpha) {
				    if (fgAlpha != ((imageData[i] >> 24) & 0xff)) return false;
				}
				if (fgRed	!= ((imageData[i] >> 16) & 0xff)) return false; 
				if (fgGreen != ((imageData[i] >> 8 ) & 0xff)) return false;
				if (fgBlue  != ( imageData[i]        & 0xff)) return false;
			} 
			// if mask pixel is FOREGROUND_OR_IMAGE_BACKGROUND_COLOR (2), this pixel is either 
			// foreground or image background color. 
			// Used for example in drawRoundRect and drawEllipse tests.
			else if(maskdata[i] == FOREGROUND_OR_IMAGE_BACKGROUND_COLOR) {	
				if (!ignoreAlpha) {
					if (fgAlpha 	!= ((imageData[i] >> 24) & 0xff) && 
						imgBgAlpha 	!= ((imageData[i] >> 24) & 0xff)) return false;
				}				
				if (fgRed 	 	!= ((imageData[i] >> 16) & 0xff) && 
					imgBgRed 	!= ((imageData[i] >> 16) & 0xff)) return false; 

				if (fgGreen 	!= ((imageData[i] >> 8 ) & 0xff) && 
					imgBgGreen 	!= ((imageData[i] >> 8 ) & 0xff)) return false;
	
				if (fgBlue  	!= ( imageData[i]        & 0xff) && 
					imgBgBlue	!= ( imageData[i]         & 0xff)) return false;
			} 
			// if mask pixel is GC_BACKGROUND_COLOR (3), this pixel is background color set to GC. 
			// Used for example in fillGradientRect test.
			else if(maskdata[i] == GC_BACKGROUND_COLOR) {	
				if (!ignoreAlpha) {
					if (gcBgAlpha != ((imageData[i] >> 24) & 0xff)) return false;
				}
				if (gcBgRed 	!= ((imageData[i] >> 16) & 0xff)) return false; 
				if (gcBgGreen 	!= ((imageData[i] >> 8 ) & 0xff)) return false;
				if (gcBgBlue  	!= ( imageData[i]        & 0xff)) return false;
			} 
			// if mask pixel is NEITHER_FOREGROUND_NOR_GC_BACKGROUND_COLOR (4), this pixel is neither 
			// foreground nor gc background color. 
			// Used for example in fillGradientRect test.
			else if(maskdata[i] == NEITHER_FOREGROUND_NOR_GC_BACKGROUND_COLOR) {	
				if (!ignoreAlpha) {
					if (fgAlpha != gcBgAlpha){
						if (fgAlpha 	== ((imageData[i] >> 24) & 0xff) || 
							gcBgAlpha 	== ((imageData[i] >> 24) & 0xff)) return false;
					}
				}
				
				if (fgRed != gcBgRed){
					if (fgRed 	 	== ((imageData[i] >> 16) & 0xff) || 
						gcBgRed  	== ((imageData[i] >> 16) & 0xff)) return false; 
				}
				if (fgGreen != gcBgGreen){
					if (fgGreen 	== ((imageData[i] >> 8 ) & 0xff) || 
						gcBgGreen	== ((imageData[i] >> 8 ) & 0xff)) return false;
				}
				if (fgBlue != gcBgBlue){
					if (fgBlue  	== ( imageData[i]        & 0xff) || 
						gcBgBlue 	== ( imageData[i]         & 0xff)) return false;
				}
			} 
			else if(maskdata[i] == IGNORE_COLOR) {	
				// skip the pixel
			} 
			// if mask pixel is IMAGE_BACKGROUND_COLOR (1), check against imgBg
			else  if(maskdata[i] == IMAGE_BACKGROUND_COLOR) {
				if (!ignoreAlpha) {
				    if (imgBgAlpha != ((imageData[i] >> 24) & 0xff)) return false;
				}
				if (imgBgRed 	!= ((imageData[i] >> 16) & 0xff)) return false; 
				if (imgBgGreen 	!= ((imageData[i] >> 8 ) & 0xff)) return false;
				if (imgBgBlue 	!= ( imageData[i]         & 0xff)) return false;
			}
			// If mask pixel is something else that's error
			else {
				return false;
			}
		}
		return true;
	}
	
	/**
	 * prints black & white image to console.
	 * black is printed as 1 and white as 0.
	 */
	public static final void print(Image img) {

		final int width = img.getWidth();
		final int height = img.getHeight();
		final int pixels =  img.getWidth() * img.getHeight();
		
		// data array for image data
		int[] imageData = new int[pixels];
		
		// get data for whole image
		img.getRGB(imageData, 0, width, 0, 0, width, height);
		
		String output = "Image("+img.getWidth()+","+img.getHeight()+"):\n";
		int y = 0;
		for(int i = 0; i < imageData.length; i++) {
			
			int a = ((imageData[i] >> 24) & 0xff);
			int r = ((imageData[i] >> 16) & 0xff); 
			int b = ((imageData[i] >> 8 ) & 0xff);
			int g = ( imageData[i] & 0xff);
			
			if (a == 255 && r == 0 && g == 0 && b == 0) {
				output += "1,";
			} else if (a == 255 && r == 255 && g == 255 && b == 255) {
				output += "0,";
			}
			else {
				output += "("+a+","+r+","+b+","+g+")";
			}
			
			if(i==((y*width) + (width-1))) {
				output += "\n";
				y++;
			}
		}
		System.out.println(output);
	}
	
	public static final void print(int[] array, int scanlength) {
		final int width = scanlength;
		String output = "";
		
		int y = 0;
		for(int i = 0; i < array.length; i++) {
			
			int a = ((array[i] >> 24) & 0xff);
			int r = ((array[i] >> 16) & 0xff); 
			int b = ((array[i] >> 8 ) & 0xff);
			int g = ( array[i] & 0xff);
			
			if (a == 255 && r == 0 && g == 0 && b == 0) {
				output += "1,";
			} else if (a == 255 && r == 255 && g == 255 && b == 255) {
				output += "0,";
			}
			else {
				output += "x,";
			}
			
			if(i==((y*width) + (width-1))) {
				output += "\n";
				y++;
			}
		}
		System.out.println(output);
	}

	private int getIndexForPoint(int scanlength, int x, int y) {
		return y*scanlength+x;
	}
	
	
}
