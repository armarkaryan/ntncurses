#include <vector>
#include <string>
#include <functional>
#include <mutex>
#include <ncurses.h>
#include <queue>
#include <thread>
#include <condition_variable>
#include <stdexcept>

#include "nttypes.h"

class NTNCurses {
	std::mutex _mtx;
    std::condition_variable cv;
    std::queue<std::function<void()>> tasks;
    bool stop_requested = false;
    std::thread ncurses_thread;

	short _color;				// Текущий цвет фона
	short _bg_color;			// Текущий цвет фона
	bool _supports_rgb;			// Флаг поддержки RGB

public:
	// ctor
    NTNCurses();
	// dtor
    ~NTNCurses();
	// enqueue
    template<typename F>
    void enqueue(F&& f);
    

	// Clear terminal
	void clear();


	// Работы с цветом
	// Проверить, поддерживается ли RGB
	bool isRgbSupported() const;
/*
	// Установить пользовательские цвета (текст/фон) в стандартной палитре
	void setColor(short text_color, short bg_color);

	// Установить пользовательские цвета (текст/фон) в стандартной палитре
	void setCustomColor(short text_color, short bg_color);

	// Установить RGB-цвет текста и фона (если поддерживается)
	bool setRgbColor(short r_text, short g_text, short b_text,
					 short r_bg, short g_bg, short b_bg);

	// Залить весь терминал цветом фона (стандартные цвета)
	void fillBackground(short bg_color);

	// Залить весь терминал RGB-цветом фона (если поддерживается)
	bool fillBackgroundRgb(short r, short g, short b);
*/

	// Установка аттрибутов (цвет текста, цвет фона)
	void setColor(short color, short bgColor);

	// Установить RGB-цвет текста и фона (если поддерживается)
	bool setColorRgb(short r_text, short g_text, short b_text,
					short r_bg, short g_bg, short b_bg);

	//
	int setAttrOn(int attrs);

	//
	int setAttrOff(int attrs);

	// Вывод символов
	// Базовая версия addch
	int addCh(chtype ch);

	// Версия addch для окон (window)
	int waddCh(WINDOW* win, chtype ch);

	// Версия addch с перемещением курсора
	int mvaddCh(int y, int x, chtype ch);

	// Комбинированная версия addch (окно + перемещение)
	int mvwaddCh(WINDOW* win, int y, int x, chtype ch);


	// Работа со строками
    void print(const std::string& msg);
    

	// Работа с устройствами ввода
	// Считываем нажатую клавишу
    int getKey();  // Переименуйте, чтобы избежать конфликта с ncurses::getch()
};
