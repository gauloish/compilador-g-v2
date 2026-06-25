#include <stdbool.h>

#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

typedef enum _SymbolDataType SymbolDataType;
typedef enum _SymbolKind SymbolKind;

typedef struct _SymbolParameters SymbolParameters;
typedef struct _SymbolEntry SymbolEntry;
typedef struct _SymbolTable SymbolTable;
typedef struct _SymbolScope SymbolScope;

SymbolParameters* symbol_parameters_create(SymbolDataType, SymbolKind);
void symbol_parameters_delete(SymbolParameters*);
void symbol_parameters_add_parameter(SymbolParameters*, SymbolDataType, SymbolKind);
int symbol_parameters_check(SymbolParameters*, SymbolParameters*);

SymbolEntry* symbol_entry_create(const char*, SymbolDataType, int);
void symbol_entry_delete(SymbolEntry*);
const char* symbol_entry_get_name(SymbolEntry*);
SymbolDataType symbol_entry_get_data_type(SymbolEntry*);
int symbol_entry_get_position(SymbolEntry*);

SymbolTable* symbol_table_create(void);
void symbol_table_delete(SymbolTable*);
void symbol_table_add_symbol(SymbolTable*, const char*, SymbolDataType, int);
bool symbol_table_check_symbol(SymbolTable*, const char*);
SymbolEntry* symbol_table_get_symbol(SymbolTable*, const char*);
int symbol_table_get_size(SymbolTable*);

SymbolScope* symbol_scope_create(void);
void symbol_scope_delete(SymbolScope*);
SymbolScope* symbol_scope_push_scope(SymbolScope*);
SymbolScope* symbol_scope_pop_scope(SymbolScope*);
void symbol_scope_add_symbol(SymbolScope*, const char*, SymbolDataType);
bool symbol_scope_check_symbol(SymbolScope*, const char*, bool);
SymbolEntry* symbol_scope_get_symbol(SymbolScope*, const char*, bool);
int symbol_scope_get_size(SymbolScope*);

enum _SymbolDataType {
    SYMBOL_INTEGER,
    SYMBOL_CHARACTER,
    SYMBOL_NOTYPE,
};

enum _SymbolKind {
    SYMBOL_VARIABLE,
    SYMBOL_VECTOR,
    SYMBOL_FUNCTION,
};

#endif /* __SYMBOL_TABLE_H__ */