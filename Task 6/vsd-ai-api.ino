#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>  // Include Arduino_JSON library

#define SSID "Guptasachin"
#define PASSWORD "s4@9110625515#"

#define API_URL "https://priaansh-flowise.hf.space/api/v1/prediction/3ee83624-ba26-4791-881b-e06f70082a61"
#define UART_TX_PIN 17  // TX to RISC-V
#define UART_RX_PIN 16  // RX (not used)

void setup() {
    Serial.begin(115200); // For Serial Monitor
    Serial2.begin(9600, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN); // UART for RISC-V

    WiFi.begin(SSID, PASSWORD);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi!");

    Serial.println("Enter a message to send to the API:");
}

void loop() {
    if (Serial.available() > 0) {
        String question = Serial.readStringUntil('\n'); // Read input from Serial Monitor
        question.trim(); // Remove extra spaces/newlines

        if (question.length() > 0) {
            Serial.println("Sending: " + question);
            sendToAPI(question);
        }
    }
}

void sendToAPI(const String& question) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(API_URL);
        http.addHeader("Content-Type", "application/json");

        // Create JSON string dynamically
        String payload = "{\"question\": \"" + question + "\"}";

        int httpResponseCode = http.POST(payload);

        if (httpResponseCode > 0) {
            String response = http.getString();
        
            // Parse JSON response to extract the "text" field
            String extractedText = parseJSON(response);

            // Print and send only the "text" content
            Serial.println("Beast Brain: " + extractedText);
            Serial2.println(extractedText); // Send data to RISC-V board
        } else {
            Serial.println("Error in HTTP request");
        }

        http.end();
    } else {
        Serial.println("WiFi Disconnected");
    }
}

// Function to extract "text" from JSON response
String parseJSON(const String& jsonString) {
    JSONVar jsonObj = JSON.parse(jsonString);

    // Check for errors in parsing
    if (JSON.typeof(jsonObj) == "undefined") {
        Serial.println("Parsing input failed!");
        return "ERROR";
    }

    // Extract the "text" field
    if (jsonObj.hasOwnProperty("text")) {
        return (const char*)jsonObj["text"];
    } else {
        Serial.println("Field 'text' not found in JSON!");
        return "ERROR";
    }
}
