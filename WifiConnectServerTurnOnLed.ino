#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "Steve'sWIFI";
const char* password = "ji7ewg1k";

ESP8266WebServer server(80);

const int ledPin = 2;
int ledState = LOW;

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>ESP8266 Web Server</h1>";
  html += "<p>LED is currently " + String(ledState == HIGH ? "On" : "Off") + "</p>";
  html += "<p><a href='/toggle'>Toggle LED</a></p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleToggle() {
  ledState = (ledState == HIGH) ? LOW : HIGH;
  digitalWrite(ledPin, ledState);
  handleRoot();
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

  server.begin();
  Serial.println("HTTP server started");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();
}
