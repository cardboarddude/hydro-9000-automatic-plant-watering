
#ifndef ScreenMenu_h
#define ScreenMenu_h

#include "Arduino.h"
#include "../Screen/Screen.h"

class ScreenMenu : public Screen {
    private:
        static const unsigned char SELECTOR_HEIGHT PROGMEM = 8, SELECTOR_WIDTH = 8, SELECTOR_PADDING_RIGHT = 2;
        static const unsigned char MENU_ITEMS_MAX_SIZE PROGMEM = 7;
        unsigned char itemNameCount = 0, itemCount = 0;
        
	public: 
        unsigned char selectedItemIndex = 0;

		DisplayText itemDisplayNames[MENU_ITEMS_MAX_SIZE];
        Screen* items[MENU_ITEMS_MAX_SIZE];
		
        ScreenMenu();
		ScreenMenu(unsigned char id, String title);
		ScreenMenu(unsigned char id, String title, String itemDisplayNames[], unsigned char displayNameCount);
		void selectItem(int menuItemIndex);
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
        void addItem(String displayName, Screen& screen);
        Screen& getSelectedItem();
        String getSelectedItemDisplayName();
};

#endif