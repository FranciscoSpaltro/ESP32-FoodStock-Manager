#include "Parser.h"

bool parseCommand(String message, Command &cmd){
    if(message.isEmpty())
        return false;
        
    message.trim();
    message.toLowerCase();

    while(message.charAt(0) == ' '){
        message.remove(0, 1);
    }

    // Extraer el primer carácter para la acción (a, q, c)
    switch(message.charAt(0)){
        case 'a': cmd.action = ADD; break;
        case 'q': cmd.action = REMOVE; break;
        case 'c': cmd.action = QUERY; break;
        default: return false;
    }

    if (message.length() == 1 &&cmd.action == QUERY) {
        cmd.quantity = 0;
        cmd.description[0] = '\0';
        cmd.location = ALL; // Default location for query without location
        return true;
    }

    // "a 1 manteca heladera"
    // "a 1 mantequilla de mani heladera"
    int first_space = message.indexOf(' ');
    int last_space = message.lastIndexOf(' ');
    if(first_space == -1 || last_space == -1)
        return false;

    String location = message.substring(last_space + 1);
    
    if(location.equals("heladera")) {
        cmd.location = FRIDGE;
    } else if(location.equals("freezer")) {
        cmd.location = FREEZER;
    } else if(location.equals("despensa")) {
        cmd.location = PANTRY;
    } else if(location.equals("frutas")) {
        cmd.location = FRUIT;
    } else if(location.equals("verduras")) {
        cmd.location = VEGGIES;
    } else if(location.equals("otros")) {
        cmd.location = OTHER;
    } else {
        return false;
    }

    if(cmd.action == QUERY){
        cmd.description[0] = '\0';
        cmd.quantity = 0;
        return true;
    }

    if(first_space == last_space)
        return false;

    // La cantidad son los digitos entre el primer espacio y el segundo espacio
    int second_space = message.indexOf(' ', first_space + 1);
    while(second_space < last_space && message.charAt(second_space + 1) == ' '){
        second_space = message.indexOf(' ', second_space + 1);
    }

    if(second_space == -1 || second_space >= last_space)
        return false;

    String quantity_str = message.substring(first_space + 1, second_space);
    cmd.quantity = quantity_str.toFloat();

    // La descripción es el resto del mensaje entre el segundo espacio y el último espacio
    String description = message.substring(second_space + 1, last_space);
    description.trim();

    if(description.length() >= sizeof(cmd.description))
        description = description.substring(0, sizeof(cmd.description) - 1);

    // Quito espacios repetidos que pueda haber en el medio de la descripción
    String cleaned = "";
    bool last_was_space = false;
    for (size_t i = 0; i < description.length(); i++) {
        char c = description.charAt(i);
        if (c == ' ') {
            if (!last_was_space) { cleaned += c; last_was_space = true; }
        } else {
            cleaned += c; last_was_space = false;
        }
    }
    cleaned.toCharArray(cmd.description, sizeof(cmd.description));

    return true;
}

const char* actionToString(ItemAction a) {
    switch(a) {
        case ADD:    return "ADD";
        case REMOVE: return "REMOVE";
        case QUERY:  return "QUERY";
        default:     return "UNKNOWN";
    }
}

const char* locationToString(StoreLoc l) {
    switch(l) {
        case FRIDGE:  return "heladera";
        case FREEZER: return "freezer";
        case PANTRY:  return "despensa";
        case FRUIT:   return "frutas";
        case VEGGIES: return "verduras";
        case OTHER:   return "otros";
        case ALL:     return "all";
        default:      return "UNKNOWN";
    }
}