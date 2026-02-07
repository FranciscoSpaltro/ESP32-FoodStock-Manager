#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "credentials.h"
#include "Parser.h"
#include "NetworkHandler.h"

// Clientes y Bot
WiFiClientSecure clientBot;
UniversalTelegramBot bot(BOT_TOKEN, clientBot);

unsigned long lastTimeBotRan;
const unsigned long botRequestDelay = 2000; // Polling cada 2 segundos

void ejecutarComando(String chat_id, String line) {
    Command cmd;

    // Proceso el comando usando el parser
    if (parseCommand(line, cmd)) {
        
        // Feedback visual para el usuario en Telegram
        bot.sendMessage(chat_id, "🔄 Procesando: `" + line + "`...", "Markdown");

        // Creo el payload (el paquete a enviar)
        String payload = createJsonPayload(cmd);

        // 3. Envio a GOOGLE y capturo la respuesta real
        String jsonResponse = sendToGoogleSheets(payload);

        // Proceso la respuesta de la nube
        JsonDocument responseDoc;
        DeserializationError error = deserializeJson(responseDoc, jsonResponse);

        if (!error && responseDoc["status"] == "OK") {
            
            // CASO A: Es una consulta (Individual o ALL)
            if (cmd.action == QUERY) {
                String titulo = (cmd.location == ALL) ? "GENERAL" : locationToString(cmd.location);
                String listado = "📦 *Inventario " + titulo + "*:\n";
                
                JsonArray data = responseDoc["data"];
                
                if (data.size() == 0) {
                    listado += "_(Vacío)_";
                } else {
                    for (JsonObject item : data) {
                        String nombre = item["i"].as<String>();
                        String cant = item["q"].as<String>();
                        
                        listado += "• " + nombre + ": " + cant;
                        
                        // Si es consulta ALL, Google manda la clave "l" (location)
                        if (item.containsKey("l")) {
                            listado += " _(" + item["l"].as<String>() + ")_";
                        }
                        listado += "\n";
                    }
                }
                bot.sendMessage(chat_id, listado, "Markdown");
            } 
            // CASO B: Fue un ADD o REMOVE exitoso
            else {
                bot.sendMessage(chat_id, "✅ Operación exitosa: `" + line + "`", "Markdown");
            }

        } else {
            // Error de comunicación o error reportado por el Script de Google
            String msgError = responseDoc.containsKey("msg") ? responseDoc["msg"].as<String>() : "Respuesta inválida";
            bot.sendMessage(chat_id, "❌ Error: " + msgError, "");
        }
    } else {
        // El parser no pudo entender la línea
        String helpMsg = "❓ No entendí: '" + line + "'\n";
        helpMsg += "Formatos válidos:\n";
        helpMsg += "- 'a 5 leche heladera'\n";
        helpMsg += "- 'q 2 pollo freezer'\n";
        helpMsg += "- 'c heladera' (consulta una hoja)\n";
        helpMsg += "- 'c' (consulta todo)";
        bot.sendMessage(chat_id, helpMsg, "");
    }
}

void handleNewMessages(int numNewMessages) {
    for (int i = 0; i < numNewMessages; i++) {
        // 1. Obtengo la identificación del remitente y la valido
        String chat_id = String(bot.messages[i].chat_id);
        if (chat_id != CHAT_ID) {
        bot.sendMessage(chat_id, "⚠️ Acceso denegado. Usuario no autorizado.", "");
        continue;
        }

        // 2. Obtengo el texto del mensaje
        String fulltext = bot.messages[i].text;

        int start = 0;
        int end = fulltext.indexOf('\n');

        while(end != -1){
                String line = fulltext.substring(start, end);
                line.trim(); // Protección más \n y espacios al principio/final
                if(line.length() > 0) {
                    ejecutarComando(chat_id, line);
                }
                start = end + 1;
                end = fulltext.indexOf('\n', start);
            }
        String lastline = fulltext.substring(start);
        lastline.trim(); // Elimino espacios al principio y al final
        if(lastline.length() > 0)
            ejecutarComando(chat_id, lastline);
    }
}

void setup() {
    Serial.begin(115200);
    
    // Conexión WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n¡WiFi Conectado!");

    // Configurar certificado de Telegram para su verificación SSL
    clientBot.setCACert(TELEGRAM_ROOT_CA);

    // Verificación del token de autenticación del bot
    if(bot.getMe()) {
        Serial.println("¡Bot conectado exitosamente!");
    } else {
        Serial.println("Error al conectar el bot. Revisa el token y la conexión.");
    }

    bot.sendMessage(CHAT_ID, "🤖 Bot iniciado y listo para recibir comandos.", "");
}

void loop() {
    // Cada cierto tiempo, verifico si hay mensajes nuevos
    if (millis() > lastTimeBotRan + botRequestDelay) {
        // getUpadtes lleva la identificación del primer mensaje a recibir
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

        while(numNewMessages) {
            handleNewMessages(numNewMessages);
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }
        lastTimeBotRan = millis();
    }
}