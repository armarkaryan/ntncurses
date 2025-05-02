#ifndef NTTYPES_H
#define NTTYPES_H

/*!	\brief	Standard vector library */
#include <vector>
/*!	\brief	Standard string library */
#include <string>

namespace nt {

//
struct Image {
	std::vector<std::string> img;
	unsigned int width;
	unsigned int height;
};

// Цветовые пары (текст/фон)
enum ColorPair {
	DEFAULT = 1,
	RED_TEXT,
	GREEN_TEXT,
	BLUE_TEXT,
	YELLOW_TEXT,
	CYAN_TEXT,
	MAGENTA_TEXT,
	WHITE_TEXT,
	CUSTOM
};

}
// End of header guard
#endif // NTTYPES_H
