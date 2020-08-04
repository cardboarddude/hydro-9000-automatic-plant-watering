
#ifndef ScreenMenu_h
#define ScreenMenu_h

#include "Arduino.h"
#include "../Screen/Screen.h"
#include "../ControlPanel/ControlPanel.h"
#include <vector>

typedef void (*VoidFunction)(void);

class ScreenMenu : public Screen {
    private:
        static const unsigned char PROGMEM SELECTOR_HEIGHT = 8, SELECTOR_WIDTH = 8, SELECTOR_PADDING_RIGHT = 2;
        static const unsigned char PROGMEM MENU_ITEMS_MAX_SIZE = 10;
        unsigned char itemNameCount = 0, itemCount = 0;
        
	public: 
        unsigned char selectedItemIndex = 0;

        ScreenMenu* parentMenu;
		DisplayText itemDisplayNames[MENU_ITEMS_MAX_SIZE];
        Screen* items[MENU_ITEMS_MAX_SIZE];
		
        ScreenMenu();
		ScreenMenu(unsigned char id, String title);
		ScreenMenu(unsigned char id, String title, std::vector<String>& itemDisplayNames);
		void selectItem(unsigned char menuItemIndex);
		void selectNextItem();
        void changeSelection(int indexChange);
        virtual void doDisplay();
		void selectPreviousItem();
        void doDisplaySelector();
        void addItemName(String menuItemName);
        unsigned int getMenuItemHeight(unsigned char index);
        // void addItemAction(String, VoidFunction);
        // void addItemSubMenu(String, ScreenMenu&);
        // bool isSubMenuSelected();
        // bool isActionSelected();
        // ScreenMenu& getSelectedSubMenu();
        void addItem(Screen& action);
        Screen& getSelectedItem();
        String getSelectedItemDisplayName();
};

#endif