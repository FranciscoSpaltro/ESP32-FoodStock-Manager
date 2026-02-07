#include "NetworkHandler.h"
#include "credentials.h"

String createJsonPayload(Command cmd) {
    JsonDocument doc;   // Guarda un documento JSON en memoria

    // Ejemplo de uso: doc["answer"] = 42; -> el documento contiene {"answer":42}
    doc["action"] = actionToString(cmd.action);
    doc["qty"]    = cmd.quantity;
    doc["item"]   = cmd.description;
    doc["loc"]    = locationToString(cmd.location);

    String output;
    serializeJson(doc, output); // Produce el string JSON a partir del documento
    
    return output;
}

String sendToGoogleSheets(String payload) {
    WiFiClientSecure client;

    // Configuración del certificado de Google para la verificación SSL
    client.setCACert(GOOGLE_ROOT_CA);
    client.setTimeout(15000);

    HTTPClient http;
    http.setTimeout(15000);

    // Configuración inicial del HTTPClient con la URL del Web App
    http.begin(client, WEB_APP_URL);
    
    // Captura de la cabecera 'Location' para manejar el redireccionamiento 302
    const char * headerKeys[] = {"Location"};
    http.collectHeaders(headerKeys, 1);

    // Configuración para no seguir automáticamente los redireccionamientos
    http.setFollowRedirects(HTTPC_DISABLE_FOLLOW_REDIRECTS);
    
    // Content-Type es una cabecera estándar HTTP (RFC 7231) que sirve para describir el tipo de medio (Media Type) del cuerpo del mensaje
    // application/json es el estándar IANA para JavaScript Object Notation
    http.addHeader("Content-Type", "application/json");

    // Envio el JSON al Web App usando POST
    int httpResponseCode = http.POST(payload);
    
    // Manejo del redireccionamiento 302
    if (httpResponseCode == 302) {
        String newURL = http.header("Location"); 
        http.end(); 

        Serial.print("Redirigiendo a: ");
        Serial.println(newURL);
        
        if (newURL.length() > 0) {
            http.begin(client, newURL);
            httpResponseCode = http.GET(); 
        }
    }

    // Verificación final

    String response = http.getString();
    http.end();
    //Serial.println("Respuesta Servidor: " + response);
    
    // Si no obtuve un 200 OK, devuelvo un JSON vacío para que el bot sepa que hubo un error
    if (httpResponseCode != 200) {
        return "{}"; 
    }

    return response;
}

bool processCloudUpdate(Command cmd){
    return sendToGoogleSheets(createJsonPayload(cmd));
}