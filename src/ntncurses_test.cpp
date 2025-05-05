#include "ntncurses.h"
int main() {
	NTNCurses ntncurses;
/*
    std::thread worker1([&]() {
		for (int i = 0; i < 10; ++i) {
			ntncurses.print("Thread 1: " + std::to_string(i) + "\n");
			//printw("Thread 1: %d\n", i);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
    
    std::thread worker2([&]() {
		for (int i = 0; i < 10; ++i) {
			ntncurses.print("	Thread 2: " + std::to_string(i) + "\n");
			//printw("	Thread 2: %d\n", i);
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
    });
*/

	std::thread worker3([&]() {
		for (int i = 0; i < 20; ++i) {
			ntncurses.addCh('X' | A_BOLD | A_REVERSE);
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	});

	std::thread worker4([&]() {
		for (int i = 0; i < 20; ++i) {
			ntncurses.addCh('O' | A_BOLD | A_REVERSE);
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	});

	std::thread worker5([&]() {
		for (int i = 0; i < 20; ++i) {
			ntncurses.setColorRgb(255, 255, 0, 50, 50, 100);
			ntncurses.setAttrOn(COLOR_PAIR(nt::CUSTOM));
			//ntncurses.mvaddCh(i, i, ' ' | A_REVERSE);
			//ntncurses.mvaddCh(i, i, ' ' | ACS_CKBOARD);
			ntncurses.mvaddCh(i, i, ' ' | A_BOLD | A_REVERSE | ACS_BULLET);
			ntncurses.setAttrOff(COLOR_PAIR(nt::CUSTOM));
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	});

	//worker1.join();
	//worker2.join();
	worker3.join();
	worker4.join();
	worker5.join();

	ntncurses.print("     Press Space to exit...");
/*
	for (int i = 0; i < 20; ++i) {
			ntncurses.setColorRgb(255, 255, 0, 50, 50, 100);
			ntncurses.setAttrOn(COLOR_PAIR(nt::CUSTOM));
			//ntncurses.mvaddCh(i, i, ' ' | A_REVERSE);
			//ntncurses.mvaddCh(i, i, ' ' | ACS_CKBOARD);
			ntncurses.mvaddCh(i, i, ' ' | A_BOLD | A_REVERSE | ACS_BULLET);
			ntncurses.setAttrOff(COLOR_PAIR(nt::CUSTOM));
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
*/
	int ch;
	while((ch = ntncurses.getKey()) != ' ') {
		ntncurses.addCh(ch);
	}
    return 0;
}
