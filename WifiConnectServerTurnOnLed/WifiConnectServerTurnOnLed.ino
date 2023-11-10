#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h> 
#include <FS.h> // Include the SPIFFS library to save data to flash
#include <ArduinoJson.h> // Include the ArduinoJSON library
#include <string>

const char* ssid = "Steve'sWIFI";
const char* password = "ji7ewg1k";

int bac1 = 1728;
int bac2 = 1786;
int bac3 = 2074;
int bac4 = 2612;
int bac5 = 2919;
int bac6 = 3015;

int muc1 = 50;
int muc2 = 100;
int muc3 = 200;
int muc4 = 300;
int muc5 = 400;
int muc6 = 900;


IPAddress subnet(255, 255, 0, 0);			            // Subnet Mask
IPAddress gateway(192, 168, 1, 1);			            // Default Gateway
IPAddress local_IP(192, 168, 1, 184);			        // Static IP Address for ESP8266

ESP8266WebServer server(80);

const int ledPin = 2;
int ledState = LOW;
int sensorData[] = {3, 7, 2, 5, 8, 4};
int secondaryData[] = {3, 10, 12, 17, 25, 29};
int dataLength = sizeof(sensorData)/ sizeof(sensorData[0]);

String GetDataString(int inputData[], String key){
    
  String dataString = key + ": [";
  for (int i = 0; i < dataLength; i++) {
    dataString += String(inputData[i]);
    if (i < dataLength -1) {
      dataString += ", ";
    }
  }
  dataString += "],";
  Serial.println(dataLength);
  return dataString;
}

String GetDataString(char* inputData[], String key){
    
  String dataString = key + ": [";
  for (int i = 0; i < dataLength; i++) {
    dataString += String(inputData[i]);
    if (i < dataLength -1) {
      dataString += ", ";
    }
  }
  dataString += "],";
  Serial.println(dataLength);
  return dataString;
}

int getLastElementAsInt(const String &input) {
  // Check if the input string is empty
  if (input.length() == 0) {
    return -1; // or any default value or error code
  }

  // Get the last character from the input string
  char lastChar = input.charAt(input.length() - 1);

  // Check if the last character is a digit
  if (isdigit(lastChar)) {
    // Convert the last character to an integer
    int lastElement = lastChar - '0';
    return lastElement;
  } else {
    return -1; // or any default value or error code
  }
}

// Save Json
void saveJsonData() {
  if (SPIFFS.begin()) {
    // Create the JSON object with the specified structure
    StaticJsonDocument<2048> jsonData;
    JsonObject dataObj = jsonData.createNestedObject("data");
    
    JsonObject day1 = dataObj.createNestedObject("day1");
    day1["key1"] = "value1";
    day1["key2"] = "value2";
    day1["key3"] = "value3";

    JsonObject day2 = dataObj.createNestedObject("day2");
    day2["key1"] = "value1";
    day2["key2"] = "value2";
    day2["key3"] = "value3";

    // Open the file for writing
    File file = SPIFFS.open("/data.json", "w");
    if (!file) {
      Serial.println("Failed to open file for writing");
      return;
    }

    // Serialize the JSON data and write it to the file
    if (serializeJson(jsonData, file) == 0) {
      Serial.println("Failed to write to file");
    }

    // Close the file
    file.close();
    Serial.println("Data saved to data.json");
  } else {
    Serial.println("Failed to mount SPIFFS filesystem");
  }
}

void readJsonData() {
  if (SPIFFS.begin()) {
    // Open the file for reading
    File file = SPIFFS.open("/data.json", "r");
    if (!file) {
      Serial.println("Failed to open file for reading");
      return;
    }

    // Get the file size
    size_t fileSize = file.size();

    // Allocate a buffer to hold the JSON data
    std::unique_ptr<char[]> buffer(new char[fileSize]);

    // Read the file into the buffer
    file.readBytes(buffer.get(), fileSize);

    // Parse the JSON data
    DynamicJsonDocument jsonData(2048); // Adjust the size as needed
    DeserializationError error = deserializeJson(jsonData, buffer.get());

    if (error) {
      Serial.print("JSON parsing error: ");
      Serial.println(error.c_str());
    } else {
      // Successfully parsed JSON
      Serial.println("Data from data.json:");

      // Access JSON data, for example:
      const char* key1 = jsonData["data"]["day1"]["key1"];
      const char* key2 = jsonData["data"]["day1"]["key2"];
      const char* key3 = jsonData["data"]["day1"]["key3"];

      Serial.print("key1: ");
      Serial.println(key1);
      Serial.print("key2: ");
      Serial.println(key2);
      Serial.print("key3: ");
      Serial.println(key3);
    }

    // Close the file
    file.close();
  } else {
    Serial.println("Failed to mount SPIFFS filesystem");
  }
}



// Web design
void handleRoot() {

  String html = "<html><head><meta charset='UTF-8'></head><body>";
  html += "<style>h1 {text-align: center;}</style>";
  html += "<h1> BẢNG THỐNG KÊ CÔNG SUẤT SỬ DỤNG ĐIỆN </h1>";
  html += "<p> TEST trạng thái BUILDIN LED : " + String(ledState == HIGH ? "On" : "Off") + "</p>";
  html += "<p><a href='/toggle'>Bật/Tắt LED</a></p>";
  html += "<canvas id='histogramChart' ></canvas>";

  // Add text field and button
  html += "<div>";
  html += "<input type='text' id='dataInput' placeholder='Nhập số liệu'>";
  html += "<button onclick='addData()'>Xác nhận</button>";
  html += "</div>";

  // Thêm trường nhập và xử lý sự kiện vào form

  html += "<form>";
  html += "<label for='level1'>Muc 1 tu 0 - </label>";
  html += "<input type='text' id='level1' onfocus='textFieldFocus()' onblur='textFieldBlur(this.id)' >";
  html += "<p id='status'>Status: Not Focused</p>";
  html += "</form>";

html += "<table style='width: 100%; border: 1px solid black;'>";
html += "<tr>";
html += "<td style='text-align: center;'>Mức</td>";
html += "<td style='text-align: center;'>Từ (Kwh)</td>";
html += "<td style='text-align: center;'>Tới (Kwh)</td>";
html += "<td style='text-align: center;'>Giá điện (Nghin dong)</td>";
html += "<td style='text-align: center;'>Công suất sử dụng(Kwh)</td>";
html += "<td style='text-align: center;'>Đơn giá(Nghin dong)</td>";
html += "</tr>";
//Muc 1
html += "<tr>";
html += "<td style='text-align: center;'>Muc 1</td>";
html += "<td style='text-align: center;'>0</td>";
html += "<td style='text-align: center;'><input type='text' id='to1' placeholder='" + String(muc1) + "'></td>";
html += "<td style='text-align: center;'><input type='text' id='gia1'  placeholder='" + String(bac1) + "'></td>";
html += "<td style='text-align: center;'>50</td>";
html += "<td style='text-align: center;'>50</td>";
html += "</tr>";
//Muc 2
html += "<tr>";
html += "<td style='text-align: center;'>Muc 2</td>";
html += "<td style='text-align: center;'>" + String(muc1 + 1) + "</td>";
html += "<td style='text-align: center;'><input type='text' id='to2' placeholder='" + String(muc2) + "'></td>";
html += "<td style='text-align: center;'><input type='text' id='gia2' placeholder='" + String(bac2) + "'></td>";
html += "<td style='text-align: center;'>50</td>";
html += "<td style='text-align: center;'>50</td>";
html += "</tr>";
//Muc 3
html += "<tr>";
html += "<td style='text-align: center;'>Muc 3</td>";
html += "<td style='text-align: center;'>" + String(muc2 + 1) + "</td>";
html += "<td style='text-align: center;'><input type='text' id='to3' placeholder='" + String(muc3) + "'></td>";
html += "<td style='text-align: center;'><input type='text' id='gia3' placeholder='" + String(bac3) + "'></td>";
html += "<td style='text-align: center;'>50</td>";
html += "<td style='text-align: center;'>50</td>";
html += "</tr>";
//Muc 4
html += "<tr>";
html += "<td style='text-align: center;'>Muc 4</td>";
html += "<td style='text-align: center;'>" + String(muc3 + 1) + "</td>";
html += "<td style='text-align: center;'><input type='text' id='to4' placeholder='" + String(muc4) + "'></td>";
html += "<td style='text-align: center;'><input type='text' id='gia4' placeholder='" + String(bac4) + "'></td>";
html += "<td style='text-align: center;'>50</td>";
html += "<td style='text-align: center;'>50</td>";
html += "</tr>";
//Muc 5
html += "<tr>";
html += "<td style='text-align: center;'>Muc 5</td>";
html += "<td style='text-align: center;'>" + String(muc4 + 1) + "</td>";
html += "<td style='text-align: center;'><input type='text' id='to5' placeholder='" + String(muc5) + "'></td>";
html += "<td style='text-align: center;'><input type='text' id='gia5' placeholder='" + String(bac5) + "'></td>";
html += "<td style='text-align: center;'>50</td>";
html += "<td style='text-align: center;'>50</td>";
html += "</tr>";
//Muc 6
html += "<tr>";
html += "<td style='text-align: center;'>Muc 6</td>";
html += "<td style='text-align: center;'>" + String(muc5 + 1) + "</td>";
html += "<td style='text-align: center;'><input type='text' id='to6' placeholder='" + String(muc6) + "'></td>";
html += "<td style='text-align: center;'><input type='text' id='gia6' placeholder='" + String(bac6) + "'></td>";
html += "<td style='text-align: center;'>50</td>";
html += "<td style='text-align: center;'>50</td>";
html += "</tr>";
//Tong so
html += "<tr>";
html += "<td style='text-align: center;'> </td>";
html += "<td style='text-align: center;'>  </td>";
html += "<td style='text-align: center;'>  </td>";
html += "<td style='text-align: center;'>  </td>";
html += "<td style='text-align: center;'> Tong </td>";
html += "<td style='text-align: center;'> 500 </td>";
html += "</tr>";
html += "</table>";




  // OnFocus text field
  html += "<script>";
  html += "function textFieldFocus() {";
  html += "  document.getElementById('status').innerText = 'Status: Focused';";
  html += "}";
  html += "function textFieldBlur(id) {";
  //html += "  var numericValue = getLastElementAsInt(id);";
  html += "  document.getElementById('status').innerText = 'Status: Not Focused' + id;";
  // html += "  var textField = document.getElementById('level'+id);";
  // html += "  var inputValue = textField.value;";
  // html += "  document.getElementById('status').innerText = 'Result: ' + inputValue;";

  html += "}";
  html += "</script>";

  //Remove Non digit
  html += "<script>";
  html += "function allowDigits(inputField) {";
  html += "  inputField.value = inputField.value.replace(/\\D/g, '');";
  html += "}";
  html += "</script>";

  html += "<script src='https://cdnjs.cloudflare.com/ajax/libs/Chart.js/3.7.0/chart.min.js'></script>";
  html += "<script>";
  html += "var ctx = document.getElementById('histogramChart').getContext('2d');";
  html += "var data = {";
  html += "  labels: ['Ngày 1', 'Ngày 2', 'Ngày 3', 'Ngày 4', 'Ngày 5', 'Ngày 6'],";
  html += "  datasets: [";
  html += "    {";
  html += "      type: 'bar',";
  html += "      label: 'Công suất hằng ngày(kWh)',";
  html += GetDataString(sensorData, "data");
  html += "      backgroundColor: 'rgba(75, 192, 192, 0.2)',";
  html += "      borderColor: 'rgba(75, 192, 192, 1)',";
  html += "      borderWidth: 1";
  html += "    },";
  html += "    {";
  html += "      type: 'line',";
  html += "      label: 'Tổng lượng dùng từ đầu tháng(kWh)',";
  html += GetDataString(secondaryData, "data");
  html += "      borderColor: 'rgba(255, 99, 132, 1)',";
  html += "      yAxisID: 'secondaryYAxis'";
  html += "    }";
  html += "  ]";
  html += "};";

  // JavaScript function to add data from the text field
  html += "function addData() {";
  html += "  var newData = parseFloat(document.getElementById('dataInput').value);";
  html += "  if (!isNaN(newData)) {";
  html += "    data.datasets[1].data.push(newData);"; // Assuming 'data' is the secondary dataset
  html += "    myChart.update();";
  html += "  }";
  html += "}";

  html += "var myChart = new Chart(ctx, {";
  html += "  type: 'bar',";
  html += "  data: data,";
  html += "  options: {";
  html += "    scales: {";
  html += "      y: {";
  html += "        beginAtZero: true,";
  html += "        title: {";
  html += "          display: true,";
  html += "          text: 'Công suất hằng ngày(kWh)',";
  //html += "          fontSize: 36";
  html += "        }";
  html += "      },";
  html += "      secondaryYAxis: {";
  html += "        position: 'right',";
  html += "        beginAtZero: true,";
  html += "        title: {";
  html += "          display: true,";
  html += "          text: 'Tổng lượng dùng từ đầu tháng(kWh)'";
  html += "        }";
  html += "      }";
  html += "    }";
  html += "  }";
  html += "});";
  html += "</script>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}



void handleToggle() {
  ledState = (ledState == HIGH) ? LOW : HIGH;
  digitalWrite(ledPin, ledState);
  handleRoot();
}

void setup() {
  // if (WiFi.config(local_IP, gateway, subnet)) {
  //   Serial.println("Static IP Configured");
  // }
  // else {
  //   Serial.println("Static IP Configuration Failed");
  // }
  Serial.begin(115200);
  WiFi.persistent(false);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
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
  if (!MDNS.begin("esp8266")) {             // Start the mDNS responder for esp8266.local
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");
  MDNS.addService("http", "tcp", 80);

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount SPIFFS filesystem");
    return;
  }

  // Save JSON data to a file
  //saveJsonData();
  //readJsonData();
}

void loop() {
  MDNS.update();
  server.handleClient();
}
