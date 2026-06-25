#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "../include/memory.h"
#include "../include/symbol_table.h"

#define TABLE_SIZE 257

/**
 * @brief Symbol parameters struct
 * 
 */
struct _SymbolParameters {
    SymbolDataType type;
    SymbolKind kind;
    SymbolParameters* next;
    int amount;
};

/**
 * @brief Symbol entry struct
 *
 */
struct _SymbolEntry {
    const char* name;
    SymbolDataType type;
    SymbolKind kind;
    SymbolEntry* next;
    SymbolParameters* parameters;
    int position;
    int amount;
};

/**
 * @brief Symbol table struct
 *
 */
struct _SymbolTable {
    SymbolEntry* table[TABLE_SIZE];
    int size;
};

/**
 * @brief Symbol scope (stack of symbol tables) struct
 *
 */
struct _SymbolScope {
    SymbolTable* symbol_table;
    SymbolScope* next;
    int position;
};

// -------------------- Symbol Parameters -------------------- //

/**
 * @brief Create symbol parameters for symbol function
 * 
 * @param type Symbol data type
 * @param kind Symbol kind
 * @return SymbolParameters* New symbol parameters object
 */
SymbolParameters* symbol_parameters_create(SymbolDataType type, SymbolKind kind) {
    SymbolParameters* parameters = (SymbolParameters*) allocate_memory(sizeof(SymbolParameters));

    *parameters = (SymbolParameters){
        .type = type,
        .kind = kind,
        .next = NULL,
        .amount = 1,
    };

    return parameters;
}

/**
 * @brief Delete symbol parameters object
 * 
 * @param parameters Symbol parameters to be deleted
 */
void symbol_parameters_delete(SymbolParameters* parameters) {
    while (parameters != NULL) {
        SymbolParameters* next = parameters->next;
        free_memory(parameters);
        parameters = next;
    }
}

/**
 * @brief Add a new parameter in symbol parameter object
 * 
 * @param parameters Symbol parameter object
 * @param type The data type of the new parameter
 * @param kind The kind of the new parameter
 */
void symbol_parameters_add_parameter(SymbolParameters* parameters, SymbolDataType type, SymbolKind kind) {
    if (parameters == NULL) {
        return;
    }

    parameters->amount++;

    while (parameters->next != NULL) {
        parameters = parameters->next;
    }

    parameters->next = symbol_parameters_create(type, kind);
}

/**
 * @brief Check if two symbol parameters are equal
 * 
 * @param parameters First symbol parameters
 * @param other Second symbol parameters
 * @return int Position of parameter error
 */
int symbol_parameters_check(SymbolParameters* parameters, SymbolParameters* other) {
    if (parameters == NULL || other == NULL) {
        return -1;
    }

    if (parameters->amount != other->amount) {
        return -2;
    } 

    int position = 0;

    while (parameters->next != NULL) {
        position++;

        if (parameters->kind != other->kind || parameters->type != other->type) {
            return position;
        }

        parameters = parameters->next;
        other = other->next;
    }

    return 0;
}

// -------------------- Symbol Entry -------------------- //

/**
 * @brief Create a new symbol entry
 * 
 * @param name Symbol name
 * @param type Symbol type (integer or character)
 * @param kind Symbol kind
 * @param parameters Symbol parameters (only for function symbol kind)
 * @param position Position in relation the first symbol
 * @param amount Vector size (only for vector symbol kind)
 * @return SymbolEntry* New symbol entry
 */
SymbolEntry* symbol_entry_create(const char* name, SymbolDataType type, SymbolKind kind, SymbolParameters* parameters, int position, int amount) {
    SymbolEntry* symbol = (SymbolEntry*) allocate_memory(sizeof(SymbolEntry));

    *symbol = (SymbolEntry){
        .name = name,
        .type = type,
        .kind = kind,
        .parameters = parameters,
        .position = position,
        .amount = amount,
        .next = NULL,
    };

    return symbol;
}

/**
 * @brief Delete a symbol entry
 * 
 * @param symbol Symbol entry to be deleted
 */
void symbol_entry_delete(SymbolEntry* symbol) {
    if (symbol == NULL) {
        return;
    }

    symbol_parameters_delete(symbol);
    free_memory(symbol);
}

/**
 * @brief Get symbol data type
 * 
 * @param symbol Symbol
 * @return SymbolDataType Data type of the symbol
 */
SymbolDataType symbol_entry_get_data_type(SymbolEntry* symbol) {
    if (symbol == NULL) {
        return SYMBOL_NOTYPE;
    }

    return symbol->type;
}

/**
 * @brief Get symbol name
 * 
 * @param symbol Symbol
 * @return const char* Name of the symbol
 */
const char* symbol_entry_get_name(SymbolEntry* symbol) {
    if (symbol == NULL) {
        return NULL;
    }

    return symbol->name;
}

/**
 * @brief Get symbol kind
 * 
 * @param symbol Symbol
 * @return Symbol kind
 */
SymbolKind symbol_entry_get_kind(SymbolEntry* symbol) {
    if (symbol == NULL) {
        return NULL;
    }

    return symbol->kind;
}

/**
 * @brief Get symbol parameters
 * 
 * @param symbol Symbol
 * @return Symbol parameters
 */
SymbolParameters* symbol_entry_get_parameters(SymbolEntry* symbol) {
    if (symbol == NULL) {
        return NULL;
    }

    return symbol->parameters;
}

/**
 * @brief Get symbol position
 * 
 * @param symbol Symbol
 * @return int Symbol position
 */
int symbol_entry_get_position(SymbolEntry* symbol) {
    if (symbol == NULL) {
        return 0;
    }

    return symbol->position;
}

/**
 * @brief Get symbol amount
 * 
 * @param symbol Symbol
 * @return int Symbol amount
 */
int symbol_entry_get_amount(SymbolEntry* symbol) {
    if (symbol == NULL) {
        return 0;
    }

    return symbol->amount;
}
// -------------------- Symbol Table -------------------- //

/**
 * @brief Create a new symbol table
 * 
 * @return SymbolTable* New symbol table
 */
SymbolTable* symbol_table_create(void) {
    SymbolTable* symbol_table = (SymbolTable*) allocate_memory(sizeof(SymbolTable));

    for (int i = 0; i < TABLE_SIZE; i++) {
        symbol_table->table[i] = NULL;
    }

    symbol_table->size = 0;

    return symbol_table;
}

/**
 * @brief Delete a symbol table
 * 
 * @param symbol_table Symbol table to be deleted
 */
void symbol_table_delete(SymbolTable* symbol_table) {
    if (symbol_table == NULL) {
        return;
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        SymbolEntry* symbol = symbol_table->table[i];

        while (symbol != NULL) {
            SymbolEntry* next = symbol->next;
            symbol_entry_delete(symbol);
            symbol = next;
        }
    }

    free_memory(symbol_table);
}

/**
 * @brief Compute hash of a string via FNV-1a algorithm
 * 
 * @param string String to be hashed
 * @return int Hash of the string
 */
int compute_hash(const char* string) {
    uint64_t hash = 1469598103934665603ULL;

    while (*string) {
        hash ^= (unsigned char)(*string++);
        hash *= 1099511628211ULL;
    }

    return hash % TABLE_SIZE;
}

/**
 * @brief Add a symbol in symbol table
 * 
 * @param symbol_table Symbol table where symbol will be added
 * @param name Name of the symbol
 * @param symbol Symbol entry
 */
void symbol_table_add_symbol(SymbolTable* symbol_table, SymbolEntry* symbol) {
    if (symbol_table == NULL || symbol == NULL) {
        return;
    }

    int i = compute_hash(symbol->name);

    symbol->next = symbol_table->table[i];
    symbol_table->table[i] = symbol;
    symbol_table->size++;
}

/**
 * @brief Check if a name is present in the symbol table
 * 
 * @param symbol_table Symbol table
 * @param name Name to be checked
 * @return true If name is present in the symbol table
 * @return false If not
 */
bool symbol_table_check_symbol(SymbolTable* symbol_table, const char* name) {
    if (symbol_table == NULL || name == NULL) {
        return false;
    }

    int i = compute_hash(name);
    SymbolEntry* symbol = symbol_table->table[i];

    while (symbol != NULL) {
        if (strcmp(name, symbol->name) == 0) {
            return true;
        }

        symbol = symbol->next;
    }

    return false;
}

/**
 * @brief Get the symbol entry object from symbol table by name
 * 
 * @param symbol_table Symbol table
 * @param name Name of the symbol
 * @return SymbolEntry* Symbol entry object
 */
SymbolEntry* symbol_table_get_symbol(SymbolTable* symbol_table, const char* name) {
    if (symbol_table == NULL || name == NULL) {
        return NULL;
    }

    int i = compute_hash(name);
    SymbolEntry* symbol = symbol_table->table[i];

    while (symbol != NULL) {
        if (strcmp(name, symbol->name) == 0) {
            return symbol;
        }

        symbol = symbol->next;
    }

    return NULL;
}

/**
 * @brief Get symbol table size
 * 
 * @param symbol_table Symbol table
 * @return int Symbol table size
 */
int symbol_table_get_size(SymbolTable* symbol_table) {
    if (symbol_table == NULL) {
        return 0;
    }

    return symbol_table->size;
}

// -------------------- Symbol Scope -------------------- //

/**
 * @brief Create a new symbol scope
 * 
 * @return SymbolScope* New symbol scope
 */
SymbolScope* symbol_scope_create(void) {
    SymbolScope* symbol_scope = (SymbolScope*) allocate_memory(sizeof(SymbolScope));

    *symbol_scope = (SymbolScope){
        .symbol_table = NULL,
        .next = NULL,
        .position = 0,
    };

    return symbol_scope;
}

/**
 * @brief Delete a symbol scope
 * 
 * @param symbol_scope Symbol scope to be deleted
 */
void symbol_scope_delete(SymbolScope* symbol_scope) {
    while (symbol_scope != NULL) {
        SymbolScope* next = symbol_scope->next;

        symbol_table_delete(symbol_scope->symbol_table);
        free_memory(symbol_scope);

        symbol_scope = next;
    }
}

/**
 * @brief Push a new scope in symbol scopes
 * 
 * @param symbol_scope Symbol scope stack to be modified
 * @return SymbolScope* New symbol scope stack
 */
SymbolScope* symbol_scope_push_scope(SymbolScope* symbol_scope) {
    SymbolTable* symbol_table = symbol_table_create();
    SymbolScope* new_symbol_scope = symbol_scope_create();

    new_symbol_scope->symbol_table = symbol_table;
    new_symbol_scope->next = symbol_scope;
    new_symbol_scope->position = symbol_scope->position;

    return new_symbol_scope;
}

/**
 * @brief Pop the top scope
 * 
 * @param symbol_scope Symbol scope stack
 * @return SymbolScope* New symbol scope stack
 */
SymbolScope* symbol_scope_pop_scope(SymbolScope* symbol_scope) {
    if (symbol_scope == NULL) {
        return NULL;
    }

    symbol_table_delete(symbol_scope->symbol_table);
    SymbolScope* next = symbol_scope->next;
    free_memory(symbol_scope);

    return next;
}

/**
 * @brief Add symbol in the current scope
 * 
 * @param symbol_scope Symbol scope stack
 * @param symbol Symbol entry
 */
void symbol_scope_add_symbol(SymbolScope* symbol_scope, SymbolEntry* symbol) {
    if (symbol_scope == NULL) {
        return;
    }

    symbol_scope->position++;

    symbol_table_add_symbol(symbol_scope->symbol_table, symbol);
}

/**
 * @brief Check if the name is in the current/all scopes
 * 
 * @param symbol_scope Scymbol scope stack
 * @param name Name of the symbol
 * @param current_scope If checking is in current or all scopes
 * @return true If name is in the respectivelly scopes
 * @return false If not
 */
bool symbol_scope_check_symbol(SymbolScope* symbol_scope, const char* name, bool current_scope) {
    if (symbol_scope == NULL) {
        return false;
    }

    if (current_scope) {
        if (symbol_table_check_symbol(symbol_scope->symbol_table, name)) {
            return true;
        }
    }
    else {
        while (symbol_scope != NULL) {
            if (symbol_table_check_symbol(symbol_scope->symbol_table, name)) {
                return true;
            }

            symbol_scope = symbol_scope->next;
        }
    }

    return false;
}

/**
 * @brief Get symbol entry object from symbol scope by the name
 * 
 * @param symbol_scope Symbol scope
 * @param name Name of the symbol
 * @param current_scope If search should be done in current or all scopes
 * @return SymbolEntry* Symbol entry object
 */
SymbolEntry* symbol_scope_get_symbol(SymbolScope* symbol_scope, const char* name, bool current_scope) {
    if (symbol_scope == NULL) {
        return NULL;
    }

    if (current_scope) {
        return symbol_table_get_symbol(symbol_scope->symbol_table, name);
    }
    else {
        while (symbol_scope != NULL) {
            SymbolEntry* symbol = symbol_table_get_symbol(symbol_scope->symbol_table, name);

            if (symbol != NULL) {
                return symbol;
            }

            symbol_scope = symbol_scope->next;
        }
    }

    return NULL;
}

/**
 * @brief Get the symbol table size of the current scope
 * 
 * @param symbol_scope Symbol scope stack
 * @return int Size of the symbol table of the current scope
 */
int symbol_scope_get_size(SymbolScope* symbol_scope) {
    if (symbol_scope == NULL) {
        return 0;
    }

    return symbol_table_get_size(symbol_scope->symbol_table);
}