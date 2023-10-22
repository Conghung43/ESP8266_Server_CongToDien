#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "Steve'sWIFI";
const char* password = "ji7ewg1k";

ESP8266WebServer server(80);

const int ledPin = 2;
int ledState = LOW;
int data[] = {3, 7, 2, 5, 8, 4};
int secondaryData[] = {3, 10, 12, 17, 25, 29};

void handleRoot() {
  String html = "<html><head><meta charset='UTF-8'></head><body>";
  html += "<style>h1 {text-align: center;}</style>";
  html += "<h1> BẢNG THỐNG KÊ CÔNG SUẤT SỬ DỤNG ĐIỆN </h1>";
  html += "<p> TEST trạng thái BUILDIN LED : " + String(ledState == HIGH ? "On" : "Off") + "</p>";
  html += "<p><a href='/toggle'>Bật/Tắt LED</a></p>";
  html += "<canvas id='histogramChart' width='400' height='200'></canvas>";
  html += "<script src='https://cdnjs.cloudflare.com/ajax/libs/Chart.js/3.7.0/chart.min.js'></script>";
  html += "<script>";
  html += "var ctx = document.getElementById('histogramChart').getContext('2d');";
  html += "var data = {";
  html += "  labels: ['Ngày 1', 'Ngày 2', 'Ngày 3', 'Ngày 4', 'Ngày 5', 'Ngày 6'],";
  html += "  datasets: [";
  html += "    {";
  html += "      type: 'bar',";
  html += "      label: 'Công suất hằng ngày(kWh)',";
  html += "      data: " + String("[") + data[0] + ", " + data[1] + ", " + data[2] + ", " + data[3] + ", " + data[4] + ", " + data[5] + "],";
  html += "      backgroundColor: 'rgba(75, 192, 192, 0.2)',";
  html += "      borderColor: 'rgba(75, 192, 192, 1)',";
  html += "      borderWidth: 1";
  html += "    },";
  html += "    {";
  html += "      type: 'line',";
  html += "      label: 'Tổng lượng dùng từ đầu tháng(kWh)',";
  html += "      data: " + String("[") + secondaryData[0] + ", " + secondaryData[1] + ", " + secondaryData[2] + ", " + secondaryData[3] + ", " + secondaryData[4] + ", " + secondaryData[5] + "],";
  html += "      borderColor: 'rgba(255, 99, 132, 1)',";
  html += "      yAxisID: 'secondaryYAxis'";
  html += "    }";
  html += "  ]";
  html += "};";
  html += "var myChart = new Chart(ctx, {";
  html += "  type: 'bar',";
  html += "  data: data,";
  html += "  options: {";
  html += "    scales: {";
  html += "      y: {";
  html += "        beginAtZero: true,";
  html += "        title: {";
  html += "          display: true,";
  html += "          text: 'Công suất hằng ngày(kWh)'";
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
    delay(20);
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
