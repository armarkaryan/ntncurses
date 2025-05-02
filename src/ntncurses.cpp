/*
Сделай правильную потокобезопасную обертку функции addch библтотеки ncurses.
Фунуция должна быть обернута в классе NTNCurses.
Пример:
void NTNCurses::print(const std::string& msg) {
	enqueue([msg]() {
		printw("%s", msg.c_str());
		refresh();
	});
}
Выравнивание сделай отступами.
*/
/*
Сделай правильную потокобезопасную обертку функции waddch, mvaddch, mvwaddch библтотеки ncurses.
Фунуция должна быть обернута в классе NTNCurses.
Пример:
void NTNCurses::print(const std::string& msg) {
	enqueue([msg]() {
		printw("%s", msg.c_str());
		refresh();
	});
}
Выравнивание сделай отступами.
*/

/*
init_color
bkgd ?
init_pair

attron
attroff
resizeterm ?
*/
#include "ntncurses.h"

// ctor
NTNCurses::NTNCurses(){
	// Инициализация ncurses в отдельном потоке
	ncurses_thread = std::thread([this]() {
		initscr();
		cbreak();
		noecho();
		keypad(stdscr, TRUE);
		curs_set(0);

	//timeout(0);
	start_color();
	use_default_colors();

	// Проверка поддержки RGB (256 или truecolor)
	_supports_rgb = (can_change_color() && COLORS >= 256);

	// Инициализация стандартных цветов
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_GREEN, COLOR_BLACK);
	init_pair(4, COLOR_BLUE, COLOR_BLACK);
	init_pair(5, COLOR_YELLOW, COLOR_BLACK);
	init_pair(6, COLOR_CYAN, COLOR_BLACK);
	init_pair(7, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(8, COLOR_WHITE, COLOR_BLACK);
	init_pair(9, COLOR_WHITE, COLOR_BLACK);

	attron(COLOR_PAIR(3));
	printw("Thread 1: %d\n", 34);
	attroff(COLOR_PAIR(3));

		while (true) {
			std::function<void()> task;
			{
				std::unique_lock<std::mutex> lock(this->_mtx);
				this->cv.wait(lock, [this]() {
					return !this->tasks.empty() || this->stop_requested;
				});

				if (this->stop_requested && this->tasks.empty()) {
					break;
				}

				task = std::move(this->tasks.front());
				this->tasks.pop();
			}

			task();
		}

		endwin();
	});
}

// dtor
NTNCurses::~NTNCurses() {
	{
		std::lock_guard<std::mutex> lock(_mtx);
		stop_requested = true;
	}
	cv.notify_one();
	ncurses_thread.join();
}

//
template<typename F>
void NTNCurses::enqueue(F&& f) {
	{
		std::lock_guard<std::mutex> lock(_mtx);
		tasks.emplace(std::forward<F>(f));
	}
	cv.notify_one();
}
    
// Методы
//
void NTNCurses::clear() {
	enqueue([this]() {
		clear();
		refresh();
	});
}

// Проверить, поддерживается ли RGB
bool NTNCurses::isRgbSupported() const {
	return _supports_rgb;
}

// Установка аттрибутов (цвет текста, цвет фона)
void NTNCurses::setColor(short color, short bg_color) {
	short pairNum = nt::CUSTOM;
	_color = color;	// ?
	_bg_color = bg_color; // ?
	enqueue([pairNum, color, bg_color]() {
		init_pair(pairNum, color, bg_color);
		refresh();
	});
}

// Установить RGB-цвет текста и фона (если поддерживается)
bool NTNCurses::setColorRgb(short r_text, short g_text, short b_text,
					 short r_bg, short g_bg, short b_bg) {
	if (!_supports_rgb) return false;
	short color = 100;
	short colorBg = 101;
	enqueue([color, r_text, g_text, b_text, colorBg, r_bg, g_bg, b_bg]() {
		// Создаем новый цвет в палитре
		init_color(color, r_text * 1000 / 255, g_text * 1000 / 255, b_text * 1000 / 255);
		init_color(colorBg, r_bg * 1000 / 255, g_bg * 1000 / 255, b_bg * 1000 / 255);
		// Связываем цветовую пару
		init_pair(nt::CUSTOM, color, colorBg);
		refresh();
	});
	return true;
}

//
int NTNCurses::setAttrOn(int attrs) {
	try {
		enqueue([attrs]() {
			int result = attron(attrs);
			if (result == ERR) {
				throw std::runtime_error("setAttrOn failed");
			}
			refresh();
		});
			return OK;
		} catch (...) {
			return ERR;
	}
}

//
int NTNCurses::setAttrOff(int attrs) {
	try {
		enqueue([attrs]() {
			int result = attroff(attrs);
			if (result == ERR) {
				throw std::runtime_error("setAttrOff failed");
			}
			refresh();
		});
			return OK;
		} catch (...) {
			return ERR;
	}
}

// Базовая версия addch
int NTNCurses::addCh(chtype ch) {
	try {
		enqueue([ch]() {
			int result = addch(ch);
			if (result == ERR) {
				throw std::runtime_error("addCh failed");
			}
			refresh();
		});
			return OK;
		} catch (...) {
			return ERR;
	}
}

// Версия addch для окон (window)
int NTNCurses::waddCh(WINDOW* win, chtype ch) {
	try {
		enqueue([win, ch]() {
			int result = waddch(win, ch);
			if (result == ERR) {
				throw std::runtime_error("waddch failed");
			}
			wrefresh(win);
		});
			return OK;
		} catch (...) {
			return ERR;
	}
}

// Версия addch с перемещением курсора
int NTNCurses::mvaddCh(int y, int x, chtype ch) {
	try {
		enqueue([y, x, ch]() {
			int result = mvaddch(y, x, ch);
			if (result == ERR) {
				throw std::runtime_error("mvaddch failed");
			}
			refresh();
		});
		return OK;
	} catch (...) {
		return ERR;
	}
}

// Комбинированная версия addch (окно + перемещение)
int NTNCurses::mvwaddCh(WINDOW* win, int y, int x, chtype ch) {
	try {
		enqueue([win, y, x, ch]() {
			int result = mvwaddch(win, y, x, ch);
			if (result == ERR) {
				throw std::runtime_error("mvaddch failed");
			}
			wrefresh(win);
		});
		return OK;
	} catch (...) {
		return ERR;
	}
}

//
void NTNCurses::print(const std::string& msg) {
	enqueue([msg]() {
		printw("%s", msg.c_str());
		refresh();
	});
}

//
int NTNCurses::getKey() {
	int ch = 0;
	std::mutex m;
	std::condition_variable cv;
	bool done = false;
        
	enqueue([&]() {
		ch = getch();
		{
			std::lock_guard<std::mutex> lock(m);
			done = true;
		}
		cv.notify_one();
	});
        
	std::unique_lock<std::mutex> lock(m);
	cv.wait(lock, [&]() { return done; });

	return ch;
}

