#ifndef NETWORK_HANDLER_H
#define NETWORK_HANDLER_H

#include <Arduino.h>
#include "Parser.h"
#include "ArduinoJson.h"
#include "HTTPClient.h"
#include "WiFiClientSecure.h"

/**
 * @brief Convierte el struct Command a un String en formato JSON.
 * @param cmd Estructura con los datos del comando.
 * @return String con el JSON formateado.
 */
String createJsonPayload(Command cmd);

/**
 * @brief Envía el payload JSON a la Web App de Google Sheets.
 * @param payload El string JSON a enviar.
 * @return true si el servidor respondió con éxito (HTTP 200/302), false de lo contrario.
 */
String sendToGoogleSheets(String payload);

/**
 * @brief Función de conveniencia que envuelve el proceso completo.
 * @param cmd Estructura procesada por el parser.
 * @return true si todo el proceso fue exitoso.
 */
bool processCloudUpdate(Command cmd);

#endif