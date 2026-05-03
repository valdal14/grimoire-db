#include <fcntl.h> 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grimoire.h"

/**
 * @brief Helper function used to counter the number of
 * stored card for a given deck 
 * @param struct LegendsCard pointer 
 * @return size_t
 */
size_t count_cards(struct LegendsCard *cards)
{
    size_t card_counter = 0;
    
    while(cards != NULL)
    {
        cards = cards->next;
        card_counter++;
    }
    
    return card_counter;
}

void db_save(struct Database *db)
{
    size_t decks_counter = 0;
    size_t cards_counter = 0;

    // ==========================================
    // COUNT DECKS AND CARDS IN-MEMORY
    // ==========================================
    for(int i = 0; i < db->capacity; i++)
    {
        struct Deck *current = db->decks[i];
        while(current != NULL)
        {
            decks_counter++;
            if(current->cards_head != NULL)
            {
                cards_counter += count_cards(current->cards_head);
            }
            current = current->next;
        }
    }

    // If no deck is present in-memory we skip the save
    if(decks_counter == 0) return;
    SAVE_PROGRESS(decks_counter, cards_counter);

    // ==========================================
    // THE VAULT FLOW
    // ==========================================
    
	int fd = open(DB_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if(fd == -1) exit(1);

	// 1. Write Header
	struct DatabaseHeader db_header;
	db_header.num_decks = decks_counter;
	db_header.num_cards = cards_counter;
	write(fd, &db_header, sizeof(struct DatabaseHeader));

	// Write ONLY the Decks
	for(int i = 0; i < db->capacity; i++)
	{
	    struct Deck *current = db->decks[i];
	    while(current != NULL)
	    {
	        struct StoredDeck flat_deck;
	        strncpy(flat_deck.key, current->key, KEY_LENGTH - 1);
	        flat_deck.key[KEY_LENGTH - 1] = '\0';
            
	        write(fd, &flat_deck, sizeof(struct StoredDeck));
	        current = current->next;
	    }
	}

    // Write ONLY the Cards
    for(int i = 0; i < db->capacity; i++)
	{
        struct Deck *current = db->decks[i];
        while(current != NULL)
	    {
	        struct LegendsCard *current_card = current->cards_head;
	        while(current_card != NULL)
	        {
	            struct StoredCard flat_card;
	            strncpy(flat_card.deck_key, current->key, KEY_LENGTH - 1);
	            flat_card.deck_key[KEY_LENGTH - 1] = '\0';
                
	            strncpy(flat_card.name, current_card->name, NAME_LENGTH - 1);
	            flat_card.name[NAME_LENGTH - 1] = '\0';
                
	            flat_card.magicka_cost = current_card->magicka_cost;
	            flat_card.attack = current_card->attack;
	            flat_card.defence = current_card->defence;

	            write(fd, &flat_card, sizeof(struct StoredCard));
	            current_card = current_card->next;
	        }
	        
            current = current->next;
	    }
	}

	close(fd);
    DB_SAVE_OK;
}

/**
 * @brief Load the database file. If the database file is present
 * use malloc to allocate the in-memory space needed, otheriwise
 * call the db_create function to calloc a brand new instance.
 * @return void
 */
struct Database *db_load(void)
{
    // if the db file cannot be open retun the init_db
    int fd = open(DB_NAME, O_RDONLY);
    if(fd == -1) return init_db();
    
    struct DatabaseHeader db_header;
    // read the header 
    read(fd, &db_header, sizeof(struct DatabaseHeader));
    // init the db
    struct Database *db = init_db();

    printf("Decks = %zu\n", db_header.num_decks);
    printf("Cards = %zu\n", db_header.num_cards);

	// Load all Decks
	for(size_t i = 0; i < db_header.num_decks; i++)
	{
	    struct StoredDeck flat_deck;
	    read(fd, &flat_deck, sizeof(struct StoredDeck));
	    add_deck(db, flat_deck.key);
	}

	// Load all Cards
	for(size_t i = 0; i < db_header.num_cards; i++)
	{
	    struct StoredCard flat_card;
	    read(fd, &flat_card, sizeof(struct StoredCard));
	    add_card(db, flat_card.deck_key, flat_card.name, flat_card.magicka_cost, flat_card.attack, flat_card.defence);
	}

    return db;
}
