#ifndef DISCORDPP_COLOR_H
#define DISCORDPP_COLOR_H

namespace discord {
	enum class EmbedColor {
		RED = 0xFF0000,
		DARKRED = 0x8B0000,
		GOLD = 0xFFD700,
		ORANGE = 0xFFA500,
		DARKORANGE = 0xFF8C00,
		YELLOW = 0xFFFF00,
		LIME = 0x00FF00,
		GREEN = 0x008000,
		DARKGREEN = 0x006400,
		OLIVE = 0x808000,
		CYAN = 0x00FFFF,
		TEAL = 0x008080,
		BLUE = 0x3498DB,
		DARKBLUE = 0x00008B,
		VIOLET = 0xEE82EE,
		PURPLE = 0x800080,
		PINK = 0xFFC0CB,
		HOTPINK = 0xFF69B4,
		WHITE = 0xFFFFE,
		BLACK = 0x000000,
		BROWN = 0x8B4513
	};

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

			red = (color_hex >> 16) & 0xFF;
			green = (color_hex >> 8) & 0xFF;
			blue = color_hex & 0xFF;
		}

		int color_hex; // Combined RGB value

		int red;
		int green;
		int blue;
	public:
	};
}

#endif