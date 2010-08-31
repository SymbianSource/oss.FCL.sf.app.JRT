package org.eclipse.swt.internal.extension;

import org.eclipse.swt.internal.qt.OS;

public class StringUtil {

	/**
	 * Determines if a given text right to left.
	 * It goes through characters of the text until it finds a
	 * Strong bidirectional character type as defined in the
	 * Unicode specification.
	 *
	 * @param text
	 * @return
	 */
	public static boolean isRightToLeftText(String text) {
		if (text == null || text.length() < 1)
			return false;
		// We search for the first strong bidirectional character type
		// to guess the directionality of the given text.
		for (int i = 0; i < text.length(); i++) {
			int dir = OS.QChar_direction(text.charAt(i));
			switch (dir) {
			//Strong right-to-left unicode directions
			case OS.QCHAR_DIRR://fall through
			case OS.QCHAR_DIRRLE://fall through
			case OS.QCHAR_DIRRLO://fall through
			case OS.QCHAR_DIRAL:
				return true;
			//Strong left-to-right unicode directions
			case OS.QCHAR_DIRL://fall through
			case OS.QCHAR_DIRLRE://fall through
			case OS.QCHAR_DIRLRO:
				return false;
			}
		}
		return false;

	}
}
