
#ifndef ScreenMenu_h
#define ScreenMenu_h

#include "Arduino.h"
#include "../Screen/Screen.h"
#include <vector>

typedef void (*VoidFunction)(void);

class ScreenMenu : public Screen {
    private:
        static const unsigned char SELECTOR_HEIGHT = 8, SELECTOR_WIDTH = 8, SELECTOR_PADDING_RIGHT = 2;
        static const unsigned char MENU_ITEMS_MAX_SIZE = 10, CHILD_MENUS_MAX_SIZE = 10, ACTIONS_MAX_SIZE = 10;
        unsigned char menuItemCount = 0, childMenuCount = 0, actionCount = 0;
        
	public: 
        static Adafruit_SSD1306* display;
        unsigned char selectedItemIndex = 0;

        ScreenMenu* parentMenu;
		DisplayText menuItems[MENU_ITEMS_MAX_SIZE];
        ScreenMenu* childMenus[CHILD_MENUS_MAX_SIZE];
        VoidFunction actions[ACTIONS_MAX_SIZE];

		
        ScreenMenu();
		ScreenMenu(String title);
		ScreenMenu(String title, std::vector<String>& itemDisplayNames);
		void selectItem(unsigned char menuItemIndex);
		void selectNextItem();
        virtual void doDisplay();
		void selectPreviousItem();
        void doDisplaySelector();
        void addItem(String menuItemName);
        unsigned int getMenuItemHeight(unsigned char index);
        // void addItemAction(String, VoidFunction);
        // void addItemSubMenu(String, ScreenMenu&);
        // bool isSubMenuSelected();
        // bool isActionSelected();
        // ScreenMenu& getSelectedSubMenu();
        // VoidFunction getSelectedAction();
        // String getSelectedItemDisplayName();
};

#endif