#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grimoire.h"
#define _POSIX_C_SOURCE 200809L

// Helper Functions ############################################################### 

/**
 * @brief Prints out the error message based on a given error type
 * @param char erno The pointer to the error message stored in the .h file
 * @param int error_type The type of the error to cast declared in the .h file
 * @return void
 */
void print_error(char *erno, int error_type)
{
    fprintf(stderr,"%s\n", erno);
    if(error_type == CRITICAL) exit(1);
}

/**
 * @brief Verify the allocation of a given object 
 * @param void p Pointer
 * @param int p_type The pointer type based on the Object Type declared
 * in the .h file
 * @return void
 */
void check_alloc(void *p, int p_type)
{
    switch(p_type)
    {
        case 0:
            if((struct Database *)p == NULL) print_error(ERNO1, CRITICAL);
            break;
        case 1:
            if((struct Deck *)p == NULL) print_error(ERNO2, CRITICAL);
            break;
        case 2:
            if((struct LegendsCard *)p == NULL) print_error(ERNO3, CRITICAL);
            break;
        default:
            print_error(ERNO4, CRITICAL);
            break;
    }
}

/**
 * @brief Checks the length of the user's input 
 * @param char s pointer 
 * @param int s_type The type of the string either DECK or CARD 
 * declared in the .h file 
 * @return void
 */
void check_user_input(char *s, int s_type)
{
    switch(s_type)
    {
        // Deck Key
        case 1:
            if(strlen(s) + 1 >= KEY_LENGTH) print_error(ERNO5, STANDARD);
            break;
        // Card name 
        case 2:
            if(strlen(s) + 1 >= NAME_LENGTH) print_error(ERNO6, STANDARD);
            break;
        default:
            print_error(ERNO4, CRITICAL);
            break;

    }
}

// Core Engine APIs ###############################################################

struct Database *init_db(void)
{
    struct Database *db = (struct Database *)malloc(sizeof(struct Database));
    check_alloc(db, DB);
    // set the max num of decks we can store
    db->capacity = MAX_STORABLE_DECK;
    db->decks = (struct Deck **)calloc(db->capacity, sizeof(struct Deck *));
    check_alloc(db->decks, DECK);

    return db;
}

unsigned int hash_key(char *key, int capacity)
{
    int hash = 0;
    while (*key != '\0') {
        hash = (hash + *key) % capacity;
        key++;
    }
    return hash;
}

/**
 * @brief Initializes an empty deck and set its name
 * @param db Pointer to the active Database instance
 * @param deck_name The key (also acts as deck name) 
 * @return void
 */
void add_deck(struct Database *db, char *deck_name)
{
    // check user input
    check_user_input(deck_name, DECK);
    
    // allocate space for the new deck 
    struct Deck *new_deck = (struct Deck *)calloc(1, sizeof(struct Deck));
    check_alloc(new_deck, DECK);
    
    // hash the given key 
    int index = hash_key(deck_name, db->capacity);
    printf("DECK INDEX = %d\n", index);    
    // copy the deck name into the new allocated deck  
    new_deck->key = strdup(deck_name);

    // Head Insertion (Handles both NULL and Collision)
    new_deck->next = db->decks[index]; 
    db->decks[index] = new_deck;
}

/**
 * @brief Creates a new card and adds it to the specified deck using O(1) head insertion.
 * @param struct Database db pointer
 * @param char deck_key pointer The FK that points to the correct deck
 * @param char name pointer The name of the card
 * @param int magicka_cost How much does this card costs to play
 * @param int attack The attack power 
 * @param int defence The defence power
 * @return void
 */
void add_card(struct Database *db, char *deck_key, char *name, int magicka_cost, int attack, int defence)
{
    // check user input
    check_user_input(deck_key, DECK);
    check_user_input(name, CARD);
    
    // Allocate space for the new card 
    struct LegendsCard *card = (struct LegendsCard *)calloc(1, sizeof(struct LegendsCard));
    check_alloc(card, CARD);
    
    // allocate the space for the new card's name and copy the new value 
    card->name = strdup(name);
    card->magicka_cost = magicka_cost;
    card->attack = attack;
    card->defence = defence;
    
    // hash the deck key to find its index
    int index = hash_key(deck_key, db->capacity);
    
    // Get the head of the bucket
    struct Deck *current_deck = db->decks[index];

    // Traverse the linked list of Decks in this bucket
    while(current_deck != NULL)
    {
        // Check if this deck is the one we are looking for
        if(strcmp(deck_key, current_deck->key) == 0)
        {
            // Head Insertion into current_deck
            card->next = current_deck->cards_head;
            current_deck->cards_head = card;
            
            CARD_ADDED_OK(card->name, current_deck->key);        
            return;
        }
        
        // Move to the next deck in the collision chain
        current_deck = current_deck->next;
    }

    // Deck was not found.
    ERR_NO_DECK(deck_key);
    
    // clean up the allocated memory
    free(card->name);
    free(card);
}

/**
 * @brief Searches for a deck by its key and executes a callback function on it.
 * @param db Pointer to the active Database instance.
 * @param deck_key The string key of the deck to find.
 * @param print Function pointer to the callback that handles the presentation logic.
 * @return void
 */
void print_deck(struct Database *db, char *deck_key, void(*print)(struct Deck *deck))
{
    check_user_input(deck_key, DECK);
    int index = hash_key(deck_key, db->capacity);

    struct Deck *current = db->decks[index];
        
    while(current != NULL)
    {
        // check if the deck_key matches the deck key at index 
        if(strcmp(deck_key, current->key) == 0)
        {
            // callback: print cards stored in the current deck 
            print(current);
            return; 
        }

        current = current->next;
    }
   
    NO_DECK_FOUND(deck_key);
}

/**
 * @brief Prints out all cards stats stored inside the given deck
 * @param struct Deck pointer
 * @return void
 */
void print_cards(struct Deck *deck)
{
    int card_counter = 0;
    struct LegendsCard *cards = deck->cards_head;

    while(cards != NULL)
    {
        PRINT_CARD(cards->name, cards->magicka_cost, cards->attack, cards->defence); 
        card_counter += 1;
        cards = cards->next;
    }
    
    if(card_counter > 0)
    {
        SEPARATOR;
        PRINT_DECK_STATS(card_counter);
    }
    else
    {
        SEPARATOR;
        NO_CARDS(deck->key);
    }

}

/**
 * @brief Safely frees all memory associated with the database (arrays, structs, and strings).
 * @param db Pointer to the active Database instance.
 * @return void
 */
void db_free(struct Database *db)
{
    if (db == NULL) return;

    for(int i = 0; i < db->capacity; i++)
    {
        struct Deck *current_deck = db->decks[i];
        struct Deck *next_deck;

        // Traverse the collision chain of decks
        while(current_deck != NULL)
        {
            next_deck = current_deck->next;

            struct LegendsCard *current_card = current_deck->cards_head;
            struct LegendsCard *next_card;

            // Traverse the chain of cards
            while(current_card != NULL)
            {
                next_card = current_card->next;
                
                // Free the strdup'd string
                if (current_card->name != NULL) free(current_card->name);
                free(current_card);
                
                current_card = next_card;
            }

            // Free the strdup'd deck key
            if (current_deck->key != NULL) free(current_deck->key); 
            free(current_deck);

            current_deck = next_deck;
        }
    }

    free(db->decks); 
    free(db);
}
