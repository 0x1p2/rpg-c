#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "headers/soul_create.h"
#include "headers/items.h"
#include "headers/menus.h"
#include "headers/combat.h"

void item_load(objects_t *objs)
{
	struct item *item;	// Will shorten the nested structs
	FILE *fd;		// File descriptor
	char ch, string[128], *tokk, *ptr;
	long id;		// Item ID.
	int n;			// Used for iteration of string.

	n = id = 0;

	fd = fopen("data/items.in", "r");	// Open the file
	while((ch = fgetc(fd)) != EOF)		// While it isn't the end of the file...
	{
		string[n] = ch;		// Assign the character to the string.

		if(ch == '\n')		// if end of line...
		{
			string[n] = '\0';	// Terminate the string
			n = -1;			//  Actually this is n = 0 (check end of while)

			if(strlen(string) > 0)	// Helps against double new lines.
			{
				tokk = strtok(string, ":;");	// tokk is a token.
				id = strtol(tokk, &ptr, 10);	// Assign the id (tokk) to id.
			}

			if(id == 1)
				item = &objs->bandaid;	// Set the item to be bandaid.
			else if(id == 2)
				item = &objs->arrow;	// Set the item to be arrow.
			else if(id == 3)
				item = &objs->reagent;	// Set the item to be reagent.

			if(id != 0 && id < 4)
			{
				item->id = id;		// Set item id to the id gained.

				tokk = strtok(NULL, ":;");
				strncpy(item->name, tokk, 7);	// Assign the name.

				tokk = strtok(NULL, ":;");
				item->stock = strtol(tokk, &ptr, 10);	// Assign the maximum stock.

				tokk = strtok(NULL, ":;");
				item->cost = strtof(tokk, &ptr);	// Assign the cost per item.
			}
		}

		n++;	// Next iteration; if n was set to -1 it will now be 0;
	}

	fclose(fd);	// close the file.
}


void item_init(item_t *item, char *name, int amount)
{	/* This is just a test function to create items. */
	strncpy(item->name, name, sizeof(item->name));	// Set name passed.
	item->amount = amount;	// Set item amount
	item->cost   = 15.00;	// Set item cost
}


void item_use(soul_t *ptr, char *name)
{
	int amount;
	int oh;		// Overheal

	if(!strncmp(name, "bandaid", 7))
	{
		ptr->item = &ptr->objs.bandaid;		// Set players main item to bandaid. 
		printf("\n  Amount to use [%d]: ", ptr->item->amount);	
		scanf("%d", &amount);
		getchar();

		if(amount != 0 && ptr->item->amount != 0)
		{
			if(amount > ptr->item->amount)		// If item request is greater than have.
				amount = ptr->item->amount;	//  set to the maximum owned.

			int hp_tmp;		// hold a temp hp.
			hp_tmp = ptr->hp;	// set tge temp max.
			oh = ((((hp_tmp / 6) * amount) + ptr->hp_c) - ptr->hp);	// Check the overheal amount.

			ptr->hp_c = (((hp_tmp / 6) * amount) + ptr->hp_c);	// Actual changes to hp.
			if(ptr->hp_c > ptr->hp)		// If players current hp is greater than max
				ptr->hp_c = ptr->hp;	//  set current to max
			else
				oh = 0;			// Assign overheal to none.

			if(stat_check(ptr, 1)) {
				printf("   [" BRED "+" RESET "]  Strength increased from %d to ", ptr->stats.strength);
				ptr->stats.strength += 1;		// Stat gain suceeded.
				printf("%d.\n", ptr->stats.strength);	// Print the stat gain.
			}

			ptr->item->amount -= amount;	// Lower the amount by 1.
			printf("   Used %d %s(s) to get to %d health (%d overheal) (%d left).\n",
					amount, name, ptr->hp_c, oh, ptr->item->amount);
		} else
			printf("   Not enough bandaids to perform that action.");

	}
}


int item_consume(soul_t *ptr)
{
	struct item *consumable;	// Used to point to the proper consumable.
	int n = 0;			// Return status.

	if(ptr->stats.cls == 'a')	// if Archer
		consumable = &ptr->objs.arrow;		// Assigns consumable to arrows.
	else if(ptr->stats.cls == 'm')	// if Mage
		consumable = &ptr->objs.reagent;	// Assigns consumable to reagents

	if(ptr->stats.cls == 'a' || ptr->stats.cls == 'm')	// Cut down lines by restating if either.
	{
		if(consumable->amount == 0)	// If no more consumable for the class; return 0;
			n = 0;			//  Which means attack won't be made.
		else {
			consumable->amount -= 1;	// Reduce amount of consumable.
			n = 1;				// Return 1.
		}
	} else
		n = 1;		// Consumable not required so continue.

	return n;
}

