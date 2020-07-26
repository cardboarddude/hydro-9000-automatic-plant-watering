
#ifndef SelectMenu_h
#define SelectMenu_h

#include "Arduino.h"
#include <Adafruit_SSD1306.h>
#include <vector>

typedef void (*VoidFunction)(void);

class SelectMenu {
    private:
        int TITLE_TEXT_SIZE = 2;
        int ITEM_TEXT_SIZE = 1;
        int TITLE_START_X = 0, TITLE_START_Y = 0;
        int ITEM_START_X = 10, ITEM_START_Y = 20, ITEM_HEIGHT = 10;
        int SELECTED_ITEM_INDICATOR_START_X = 0, SELECTED_ITEM_INDICATOR_HEIGHT = 8, SELECTED_ITEM_INDICATOR_WIDTH = 8;
        
	public: 
        Adafruit_SSD1306* display;
        int selectedItemIndex = 0;

        String title;
        SelectMenu* parentMenu;
		std::vector<String> itemDisplayNames;
        std::vector<SelectMenu*> childMenus;
        std::vector<VoidFunction> actions;

		
        SelectMenu();
		SelectMenu(String, Adafruit_SSD1306&);
		SelectMenu(String, std::vector<String>&, Adafruit_SSD1306&);
		void selectItem(int);
		void selectNextItem();
		void selectPreviousItem();
        void doDisplay();
        void addItem(String);
        void addItemAction(String, VoidFunction);
        void addItemSubMenu(String, SelectMenu&);
        bool isSubMenuSelected();
        bool isActionSelected();
        SelectMenu& getSelectedSubMenu();
        VoidFunction getSelectedAction();
        String getSelectedItemDisplayName();
};

#endif