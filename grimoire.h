#ifndef GRIMOIRE_H
#define GRIMOIRE_H

#include <stddef.h> 

#define DB_NAME "grimoirestore.db"

// String length
#define KEY_LENGTH 32
#define NAME_LENGTH 64

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
    struct Deck **buckets; 
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
 * @return void
 */
void print_deck(struct Database *db, char *deck_key);

#endif

