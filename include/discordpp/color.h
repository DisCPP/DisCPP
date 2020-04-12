#ifndef DISCORDPP_COLOR_H
#define DISCORDPP_COLOR_H

namespace discord {
	class Color {
	public:
		Color() = default;
		Color(int red, int green, int blue) : red(red), green(green), blue(blue) {
			/**
			 * @brief Constructor to convert an rgb color to a discord::Color
			 *
			 * ```cpp
			 *      discord::Color color(255, 255, 255); // White
			 * ```
			 *
			 * @param[in] red Red value
			 * @param[in] green Green value
			 * @param[in] blue Blue value
			 *
			 */

			color_hex = (red << 16) + (green << 8) + (blue);
		}

		Color(int color_hex) : color_hex(color_hex) {
			/**
			 * @brief Constructor to go from a hex color value to a discord::Color
			 *
			 * ```cpp
			 *      discord::Color color(0xffffff); // White
			 * ```
			 *
			 * @param[in] color_hex The combined hex value
			 */

			red = (color_hex & 0xFF000000) >> 24;
			green = (color_hex & 0x00FF0000) >> 16;
			blue = (color_hex & 0x0000FF00) >> 8;
		}

		int color_hex; // Combined RGB value

		int red;
		int green;
		int blue;
	public:
	};
}

#endif