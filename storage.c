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
    
    // Open the vault ONCE
    int fd = open(DB_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    // fd fails show error and exit
    if(fd == -1) print_error(ERNO8, CRITICAL);

    // Write the Header
    struct DatabaseHeader db_header;
    db_header.num_decks = decks_counter;
    db_header.num_cards = cards_counter;
    write(fd, &db_header, sizeof(struct DatabaseHeader));

    // Write the Data 
    for(int i = 0; i < db->capacity; i++)
    {
        struct Deck *current = db->decks[i];
        while(current != NULL)
        {
            // Prepare and write the StoredDeck
            struct StoredDeck flat_deck;
            strncpy(flat_deck.key, current->key, KEY_LENGTH - 1);
            flat_deck.key[KEY_LENGTH - 1] = '\0';
            write(fd, &flat_deck, sizeof(struct StoredDeck));

            // Traverse the cards for this deck and write them
            struct LegendsCard *current_card = current->cards_head;
            while(current_card != NULL)
            {
                struct StoredCard flat_card;
                // Copy the Foreign Key so the card remembers who it belongs to!
                strncpy(flat_card.deck_key, current->key, KEY_LENGTH - 1);
                flat_card.deck_key[KEY_LENGTH - 1] = '\0';
                
                // Copy the card's name
                strncpy(flat_card.name, current_card->name, NAME_LENGTH - 1);
                flat_card.name[NAME_LENGTH - 1] = '\0';
                
                // Copy the raw stats
                flat_card.magicka_cost = current_card->magicka_cost;
                flat_card.attack = current_card->attack;
                flat_card.defence = current_card->defence;

                // Write the card to disk
                write(fd, &flat_card, sizeof(struct StoredCard));

                current_card = current_card->next;
            }

            current = current->next;
        }
    }

    // Lock the vault
    close(fd);
    DB_SAVE_OK;
}

/**
 * @brief Load the database file. If the database file is present
 * use malloc to allocate the in-memory space needed, otheriwise
 * call the db_create function to calloc a brand new instance.
 * @return void
 */
//struct Database *db_load(void){}
