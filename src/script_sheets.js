function doPost(e) {
  try {
    // Parseo los datos que envía el ESP32
    var data = JSON.parse(e.postData.contents);
    var action = data.action;    // "ADD", "REMOVE", "QUERY"
    var qty = parseFloat(data.qty);
    var itemName = data.item ? data.item.toLowerCase() : "";
    var location = data.loc.toLowerCase();

    // --- LÓGICA PARA CONSULTA (QUERY) ---
    if (action == "QUERY") {
      var responseList = [];
      
      // CASO A: Consulta Global (Iterar todas las hojas)
      if (location == "all") {
        var allSheets = ss.getSheets(); // Obtiene un array con todas las pestañas
        
        allSheets.forEach(function(sheet) {
          var sheetName = sheet.getName();
          var data = sheet.getDataRange().getValues();
          
          // Recorremos cada hoja (desde i=1 por encabezados)
          for (var i = 1; i < data.length; i++) {
            if (data[i][0] && data[i][0].toString() != "") {
              responseList.push({
                "i": data[i][0], // Item
                "q": data[i][1], // Cantidad
                "l": sheetName   // Agrego la ubicación para que el ESP32 sepa de dónde viene
              });
            }
          }
        });
      } 
      // CASO B: Consulta de una sola ubicación (Tu lógica anterior)
      else {
        // Abro la hoja correspondiente
        var ss = SpreadsheetApp.getActiveSpreadsheet();
        var sheet = ss.getSheetByName(location);
        
        // Si la hoja no existe, devolvemos error en formato JSON
        if (!sheet) {
          return ContentService.createTextOutput(JSON.stringify({"status": "ERROR", "msg": "Ubicación no encontrada"}))
                .setMimeType(ContentService.MimeType.JSON);
        }

        var sheet = ss.getSheetByName(location);
        if (!sheet) {
           return ContentService.createTextOutput(JSON.stringify({"status": "ERROR", "msg": "No existe la hoja"}))
                  .setMimeType(ContentService.MimeType.JSON);
        }
        var data = sheet.getDataRange().getValues();
        for (var i = 1; i < data.length; i++) {
          if (data[i][0] && data[i][0].toString() != "") {
            responseList.push({ "i": data[i][0], "q": data[i][1] });
          }
        }
      }

      return ContentService.createTextOutput(JSON.stringify({
        "status": "OK",
        "data": responseList
      })).setMimeType(ContentService.MimeType.JSON);
    }

    // -------------------------------------

    // Abro la hoja correspondiente
    var ss = SpreadsheetApp.getActiveSpreadsheet();
    var sheet = ss.getSheetByName(location);
    
    // Si la hoja no existe, devolvemos error en formato JSON
    if (!sheet) {
      return ContentService.createTextOutput(JSON.stringify({"status": "ERROR", "msg": "Ubicación no encontrada"}))
            .setMimeType(ContentService.MimeType.JSON);
    }

    // 3. LÓGICA PARA ADD / REMOVE
    var values = sheet.getDataRange().getValues();
    var foundRow = -1;
    
    for (var i = 0; i < values.length; i++) {
      if (values[i][0].toString().toLowerCase() == itemName) {
        foundRow = i + 1; // +1 porque los índices de fila empiezan en 1
        break;
      }
    }

    if (foundRow != -1) {
      // El producto existe, actualizo
      // values es la tabla -> foundRow - 1 es la fila (en JS sí empiezan en 0) -> [1] es la segunda columna (cantidad)
      var currentQty = parseFloat(values[foundRow - 1][1]) || 0;
      var newQty = currentQty;

      if (action == "ADD"){ 
        newQty = currentQty + qty;
        sheet.getRange(foundRow, 2).setValue(newQty); 
        sheet.getRange(foundRow, 3).setValue(new Date());
      } else if (action == "REMOVE"){
        newQty = Math.max(0, currentQty - qty);
        if (newQty <= 0) {
          sheet.deleteRow(foundRow); // Borro la fila completa si llega a 0 o menos
        } else {
          sheet.getRange(foundRow, 2).setValue(newQty); // Solo actualiza si queda resto
          sheet.getRange(foundRow, 3).setValue(new Date()); 
        }
      } 
    } else {
      // El producto no existe, lo creo (Solo si es ADD)
      if (action == "ADD") {
        sheet.appendRow([itemName, qty, new Date()]);
      }
    }

    // Respuesta exitosa estándar en JSON
    return ContentService.createTextOutput(JSON.stringify({"status": "OK"}))
           .setMimeType(ContentService.MimeType.JSON);

  } catch (error) {
    // En caso de error del script, devuelvo JSON de error
    return ContentService.createTextOutput(JSON.stringify({"status": "ERROR", "msg": error.toString()}))
           .setMimeType(ContentService.MimeType.JSON);
  }
}

function doGet(e) {
  return ContentService.createTextOutput("OK")
                       .setMimeType(ContentService.MimeType.TEXT);
}