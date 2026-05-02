#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grimoire.h"

// Interface ######################################################################

/**
 * Shows the welcome menu to the user 
 * @return void
 */
void user_menu(void)
{
    printf("#####################################\n");
    printf("# GRIMOIRE Initialised              #\n");
    printf("# Type: HELP to learn about cmds    #\n");
    printf("# Type: EXIT to quit the process    #\n");
    printf("#####################################\n");
}
/**
 * @brief Parses, validates and maps the first two input keywords to decided
 * which query will be executed:
 * 0 add deck query 
 * 1 add card query
 * 3 select query
 * -1 error
 * @param struct Database pointer
 * @param char cmd1 The first input keyword
 * @param char cmd2 The second input keyword
 * @return int
 */
int parse_command(char *cmd1, char *cmd2)
{
    if((strcmp(cmd1, "ADD") == 0) && (strcmp(cmd2, "DECK") == 0))
    {
        return 0;
    }
    else if((strcmp(cmd1, "ADD") == 0) && (strcmp(cmd2, "CARD") == 0))
    {
        return 1;
    }
    else if((strcmp(cmd1, "SELECT") == 0) && (strcmp(cmd2, "CARDS") == 0))
    {
        return 2;
    }
    else
    {
        return -1;
    }
}

/**
 * @brief Internally calls the add_deck method
 * @param struct Database pointer
 * @param char deck_key The name/key of the deck
 * @return void
 */
void parse_add_deck(struct Database *db, char *deck_key)
{
    add_deck(db, deck_key);
    DECK_ADDED_MSG(deck_key);
}

/**
 * @brief Parses, validates the query and internally calls the add_card function
 * @param struct Database pointer
 * @param char card_name The name of the card 
 * @param char cmd_val The VAL keyword from the user's query
 * @param char mgk The magicka cost of the card 
 * @param char atk The attack power of the card
 * @param char def The defence power of the card
 * @param char cmd_to The TO keyword from the user's query
 * @param char deck_name The name of the deck
 * @return void
 */
void parse_add_card(struct Database *db, char *card_name, char *cmd_val, char *mgk, char *atk, char*def, char *cmd_to, char *deck_name)
{
    int is_valid = 1;
    int mgk_int = atoi(mgk);
    int atk_int = atoi(atk);
    int def_int = atoi(def);

    if((strcmp(cmd_val, "VAL") != 0) || (strcmp(cmd_to, "TO") != 0)) is_valid--;
    

    if(is_valid)
    {
        add_card(db, deck_name, card_name, mgk_int, atk_int, def_int);
    }
    else
    {
        print_error(INVALID_QUERY_CARD, STANDARD);
    }
}

/**
 * @brief Parses, validates the query and internally calls the print_deck function
 * @param struct Database pointer
 * @param char cmd_from The command FROM from the user's query 
 * @param char deck_name The given deck name from the user's query
 * @return void
 */
void parse_select(struct Database *db, char *cmd_from, char *deck_name)
{
    int is_valid = 1;
    if((strcmp(cmd_from, "FROM") != 0)) is_valid--;
    
    if(is_valid)
    {
        print_deck(db, deck_name, print_cards);
    }
    else
    {
        print_error(INVALID_QUERY_SELECT, STANDARD);
    }
}

/**
 * @brief Handles the incoming buffer and the logic for the commands
 * execution flow.
 * @param struct Database pointer
 * @param char cmd The buffer read 
 * @param int cmd_type The type of the command
 * @return void
 */
void handle_command(struct Database *db, char *cmd, int cmd_type)
{
    switch(cmd_type)
    {
        case 0:
            // save to disk on exit
            db_save(db);
            // free the heap 
            db_free(db);

            printf("🫡|See you next time\n");
            exit(1);
        case 1:
            SEPARATOR;
            printf("📖|GRIMOIRE Help\n\n");
            printf("Supported operations:\n");
            printf("1 - Adding a new deck\n");
            printf("\t ADD DECK '<deck_name>'\n");
            printf("\t I.E: ADD DECK 'Neutral Control'\n");
            printf("2 - Adding a new card\n");
            printf("\t VAL(magicka_cost, attack, defence)\n");
            printf("\t ADD CARD '<card_name>' VAL(0,1,1) TO '<deck_name>'\n");
            printf("\t I.E: ADD CARD 'Adoring Fan' VAL(2,3,1) TO 'Neutral Control'\n");
            printf("3 - Get a cards stored in a deck\n");
            printf("\t SELECT CARDS FROM '<deck_name>'\n");
            printf("\t SELECT CARDS FROM 'Neutral Control'\n");
            SEPARATOR;
            break;
        case 2:
        {
            // get the first two input to detect the user command intensions
            char *cmd1 = strtok(cmd, " ");
            char *cmd2 = strtok(NULL, " ");
            // get the mapped query result after parsing the first two keywords
            int mapped_result = parse_command(cmd1, cmd2);

            // parse the command 
            switch(mapped_result)
            {
                case 0: // Add Deck
                {
                    char *deck_key = strtok(NULL, "'");
                    // parse and add the new deck 
                    parse_add_deck(db, deck_key);
                    break;
                }
                case 1: // Add Card
                {
                    char *card_name = strtok(NULL, "'");
                    char *val_kw = strtok(NULL, " (");
                    char *mag_str = strtok(NULL, ",");
                    char *atk_str = strtok(NULL, ",");
                    char *def_str = strtok(NULL, ") ");
                    char *to_kw = strtok(NULL, " '");
                    char *dest_deck = strtok(NULL, "'");
                    // parse cmds and add new card
                    parse_add_card(db, card_name, val_kw, mag_str, atk_str, def_str, to_kw, dest_deck);
                    break;
                }
                case 2: // Select Cards
                {
                    char *from = strtok(NULL, " ");
                    char *deck_name = strtok(NULL, "'");
                    // parse and print the entire deck
                    parse_select(db, from, deck_name);
                    break;
                }
                default:
                    print_error(INVALID_QUERY_DECK, STANDARD);
                    break;
            }
            break;
        }
        default:
            print_error(ERNO7, STANDARD);
    }
}

/**
 * @brief Program Loop - Read user commands
 * @param struct Database pointer 
 * @return void
 */
void get_user_input(struct Database *db)
{
    char buffer[256];
    
    while(1)
    {
        printf("grimoire> ");

        if(fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            buffer[strcspn(buffer, "\n")] = '\0';
            
            if(strcmp(buffer, "EXIT") == 0 || strcmp(buffer, "exit") == 0)
            {
                handle_command(db, buffer, EXIT);
            }
            else if(strcmp(buffer, "HELP") == 0 || strcmp(buffer, "help") == 0)
            {
                handle_command(db, buffer, HELP);
            }
            else
            {
                handle_command(db, buffer, QUERY);
            }
        }
    }
}

// MAIN ########################################################################### 

int main(void)
{
    // show menu
    user_menu();
    // init GRIMOIRE
    struct Database *db = init_db();
    // Input loop
    get_user_input(db);
    
    return 0;
}
