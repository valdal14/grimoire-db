#ifndef GRIMOIRE_H
#define GRIMOIRE_H

#include <stddef.h> 

#define DB_NAME "grimoirestore.db"
#define MAX_STORABLE_DECK 10

// String length
#define KEY_LENGTH 32
#define NAME_LENGTH 64

// Object Type
#define DB 0
#define DECK 1
#define CARD 2
// Error Messages
#define CRITICAL 0
#define STANDARD 1
#define ERNO1 "Could not allocate Database"
#define ERNO2 "Could not allocate space for the decks"
#define ERNO3 "Could not allocate space for the card"
#define ERNO4 "Unsupported object type"
#define ERNO5 "Deck's key name cannot be longer than 32 characters"
#define ERNO6 "Card's name cannot be longer than 64 characters"
#define ERR_NO_DECK(deck_key) fprintf(stderr, "[ERROR] A card cannot be added to non-existing deck: %s\n", deck_key)
#define EMPTY_DECK(deck_key) fprintf(stderr, "[ERROR] The deck '%s' does not have card stored in it\n", deck_key)
#define NO_DECK_FOUND(deck_key) fprintf(stderr, "[ERROR] Could not find a deck named '%s'\n", deck_key);
// UI Messages 
#define CARD_ADDED_OK(card_name, deck_key) printf("Successfully added card '%s' to deck '%s'\n", card_name, deck_key)
#define PRINT_CARD(card_name, mgk, att, def) printf("'%s': Magicka = %d, Attack = %d, Defence = %d\n", card_name, mgk, att, def)
#define PRINT_DECK_STATS(card_count) printf("This deck has %d card/s stored in it\n", card_count)
#define SEPARATOR printf("------------------------------------------------------------\n");
#define NO_CARDS(deck_key) printf("Deck '%s' does not have cards stored in it\n", deck_key)

struct LegendsCard
{
    char *name;
    int magicka_cost;
    int attack;
    int defence;
    // Linked list of cards within the deck
    struct LegendsCard *next; 
};

struct Deck 
{
    // The hash table key (e.g., "aggro_mage")
    char *key;           
    // 1-to-N: Pointer to the first card
    struct LegendsCard *cards_head; 
    // For Hash Table collision resolution
    struct Deck *next;
};

struct Database 
{
    // The Hash Table spine
    struct Deck **decks; 
    int capacity;
};

struct DatabaseHeader 
{
    size_t num_decks;
    size_t num_cards;
};

struct StoredDeck 
{
    char key[KEY_LENGTH];
};

struct StoredCard 
{
    // THE FOREIGN KEY: Links card to its deck
    char deck_key[KEY_LENGTH]; 
    char name[NAME_LENGTH];
    int magicka_cost;
    int attack;
    int defence;
};

/**
 * @brief Initializes an empty database in RAM with default capacities.
 * @return A pointer to the newly allocated Database struct.
 */
struct Database *init_db(void);

/**
 * @brief Load the database file. If the database file is present
 * use malloc to allocate the in-memory space needed, otheriwise
 * call the db_create function to calloc a brand new instance.
 * @return void
 */
struct Database *db_load(void);

/**
 * @brief Performs a Full Flush, overwriting 'grimoirestore.db' with the current RAM state.
 * @param db Pointer to the active Database instance.
 * @return void
 */
void db_save(struct Database *db);

/**
 * @brief Safely frees all memory associated with the database (arrays and the struct).
 * @param db Pointer to the active Database instance.
 * @return void
 */
void db_free(struct Database *db);

/** 
 * @brief Initializes an empty deck and set its name
 * @param struct Database db pointer 
 * @param char deck_name The key (also acts as deck name) 
 * @return void
 */
void add_deck(struct Database *db, char *deck_name);

/**
 * @brief Creates a new card
 * @param struct Database db pointer
 * @param deck_key pointer The FK that points to the correct deck
 * @param char name pointer The name of the card
 * @param int magicka_cost How much does this card costs to play
 * @param int attack The attack power 
 * @param int defence The defence power
 * @return void
 */
void add_card(struct Database *db, char *deck_key, char *name, int magicka_cost, int attack, int defence);

/**
 * @brief Generates an integer hash for a given string key.
 * @param key The string to hash.
 * @param capacity The total number of buckets in the Hash Table.
 * @return unsigned int The computed bucket index.
 */
unsigned int hash_key(char *key, int capacity);

/**
 * @brief Prints a specific deck and all the cards it contains.
 * @param db Pointer to the active Database instance.
 * @param deck_key The string key of the deck to print.
 * @param void print Callback 
 * @return void
 */
void print_deck(struct Database *db, char *deck_key, void(*print)(struct Deck *deck));

#endif

