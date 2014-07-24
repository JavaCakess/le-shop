#include "screen.h"

/* The selected options for
 * menus and such. */
size_t selected_item;
size_t selected_option;

int last_key;

/* What the user is currently focused on. */
typedef enum
{
	SHOP_LIST,
	MENU
} Status;

Status status = SHOP_LIST;

/* A string that gets updated after buying
 * something. */
char money_status[26];

void screen_init(void)
{
	initscr();
	keypad(stdscr, TRUE);

	/* Check if the user's terminal supports color. */
	if (has_colors() == FALSE)
	{
		/* Get rid of the window, print a message
		 * and then exit eith error code (1). */
		screen_destroy();
		printf("Whoopsies. Your terminal doesn't support color.\n");
		exit(1);
	}

	/* .. otherwise start color and initialize
	 * our color pairs. */
	start_color();
	
	init_pair(1, COLOR_BLACK, COLOR_GREEN);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);

	/* Zero out money_status. */
	memset(money_status, 0, 26);
}

void screen_destroy(void)
{
	endwin();
}

void render(void)
{
	/* The height (y) and width (x) of the window. */
	size_t y, x;
	getmaxyx(stdscr, y, x);

	/* A string that will be used to temporarily
	 * hold data. */
	char str[x + 1];
	/* Counter variable. */
	size_t i;

	/* Print a nice heading. */
	mvprintw(0, 0, "SHOP ITEMS");
	mvprintw(0, 31, "INVENTORY");
	memset(str, '_', x);
	mvprintw(1, 0,"%.*s\n", x, str);

	/* -= Render the Shop List =- */
	for (i = 0; i < shop_item_count(); i++)
	{
		writestr_item(str, shop_item_at(i));

		if (selected_item == i)
		{
			if (status == SHOP_LIST)
				attron(COLOR_PAIR(1));
			else
				attron(COLOR_PAIR(2));
		}
		
		mvprintw(2 + i, 0, "%s\n", str);

		attroff(COLOR_PAIR(1));
		attroff(COLOR_PAIR(2));
	}

	if (status == MENU)
	{
		if (last_key == KEY_ENTER)
		{
			status = SHOP_LIST;
		}
	}
	/* -= End Rendering Shop List -= */


	/* -= Render The Inventory -= */
	for (i = 0; i < Vector_len(user_inventory); i++)
	{
		invitem_t citem = Vector_at(user_inventory, invitem_t, i);
		mvprintw(2 + i, 31, "%s  x%i", citem.item.name, citem.count);
	}
	/* -= End Rendering Inventory -= */

	/* Dashes to seperate parts of the screen. */
	memset(str, '-', x);
	mvprintw(7, 0, "%.*s", x, str);
	
	/* Output formatted money string to str. */
	writestr_money(str, user_money);

	/* Print the money out in yellow! */
	mvprintw(8, 0, "Balance: ");
	attron(COLOR_PAIR(3));
	printw("$%s", str);	
	attroff(COLOR_PAIR(3));

	/* Print the money status string. */
	mvprintw(8, 20, "(%s)", money_status);	

	/* The description of the item. This will be
	 * shifted over 2 spaces and printed in yellow. */

	mvprintw(9, 0, "Description:");
	attron(COLOR_PAIR(3));
	mvprintw(10, 2, "%s", shop_item_at(selected_item).desc);
	attroff(COLOR_PAIR(3));

	memset(str, '-', x);
	mvprintw(14, 0, "%.*s", x, str);

	/* Some nice instructions for the user. */
	mvprintw(15, 0, "B - buy, Q - quit");
}

void input(void)
{
	if (last_key == KEY_DOWN)	/* MOVE SELECTION DOWN */
	{
		if (status == SHOP_LIST)
		{
			if (selected_item < shop_item_count() - 1)
				selected_item++;
		}
	}
	else if (last_key == KEY_UP) /* MOVE SELECTION UP */
	{
		if (status == SHOP_LIST)
		{
			if (selected_item > 0)
				selected_item--;
		}
	}
	else if (last_key == 'b')	/* BUY ITEM */
	{
		status = SHOP_LIST;
		if (user_money < shop_item_at(selected_item).price)
		{
			sprintf(money_status, "Not enough money.");
		}
		else
		{
			user_money -= shop_item_at(selected_item).price;
			char money_str[20];
			writestr_money(money_str, shop_item_at(selected_item).price);
			user_add_item(shop_item_at(selected_item));
			sprintf(money_status, "-$%s", money_str);
		}
	}
}

