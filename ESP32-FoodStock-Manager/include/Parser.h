#ifndef PARSER_H
#define PARSER_H

#include <Arduino.h>

enum ItemAction { ADD, REMOVE, QUERY, UNKNOWN };
enum StoreLoc { FRIDGE, FREEZER, PANTRY, FRUIT, VEGGIES, OTHER, ALL};   // ALL solo para QUERY

struct Command {
    ItemAction action;
    float quantity;
    char description[32];
    StoreLoc location;
};

bool parseCommand(String message, Command &cmd);
const char* actionToString(ItemAction a);
const char* locationToString(StoreLoc l);

#endif