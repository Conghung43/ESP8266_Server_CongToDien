#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h> 
const char* ssid = "Steve'sWIFI";
const char* password = "ji7ewg1k";

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
  Serial.begin(115200);
  //WiFi.persistent( false );
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
}

void loop() {
  MDNS.update();
  server.handleClient();
}
