/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

// Replace with your network credentials
const char* ssid     = "Nata";
const char* password = "L12q38w2";
#define SEALEVELPRESSURE_HPA (1013.25) 

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output5State = "off";
String output4State = "off";

// Assign output variables to GPIO pins
const int output5 = 15;
const int output4 = 0;
Adafruit_BME280 OutDorSensor;
Adafruit_BME280 InDorSensor;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output5, OUTPUT);
  pinMode(output4, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output5, LOW);
  digitalWrite(output4, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  
  if(!OutDorSensor.begin(0x77)){
    Serial.println("Could not find a outdor BME280 sensor, check wiring!"); 
    while (1);                                          
  
  }
  if(!InDorSensor.begin(0x76)){
    Serial.println("Could not find a indor BME280 sensor, check wiring!"); 
    while (1);                                          
  
  }
  
}

void loop(){
  WiFiClient client = server.available();               // Получаем данные, посылаемые клиентом 
 
  if (client) {                                         
    Serial.println("New Client.");                      // Отправка "Новый клиент"
    String currentLine = "";                            // Создаем строку для хранения входящих данных от клиента
    while (client.connected()) {                        // Пока есть соединение с клиентом 
      if (client.available()) {                         // Если клиент активен 
        char c = client.read();                         // Считываем посылаемую информацию в переменную "с"
        Serial.write(c);                                // Отправка в Serial port 
        header += c;
        if (c == '\n') {                                
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");          
            client.println("Content-type:text/html ");
            client.println("Connection: close");        
            client.println("Refresh: 10");              
            client.println();
            //Кнопки LED-------------------------------------------------------------
            // turns the GPIOs on and off
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 5 on");
              output5State = "on";
              digitalWrite(output5, HIGH);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("GPIO 5 off");
              output5State = "off";
              digitalWrite(output5, LOW);
            } else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 4 on");
              output4State = "on";
              digitalWrite(output4, HIGH);
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 4 off");
              output4State = "off";
              digitalWrite(output4, LOW);
            }
            

            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP8266 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 5  
            client.println("<p>GPIO 5 - State " + output5State + "</p>");
            // If the output5State is off, it displays the ON button       
            if (output5State=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 4  
            client.println("<p>GPIO 4 - State " + output4State + "</p>");
            // If the output4State is off, it displays the ON button       
            if (output4State=="off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            ///-----------------------------------------------------------------------
            //Зовнішній сенсор
            client.println("<!DOCTYPE html><html>");    
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<meta charset='UTF-8'>");   
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial;}");
            client.println("table { border-collapse: collapse; width:40%; margin-left:auto; margin-right:auto; }");
            client.println("th { padding: 12px; background-color: #0043af; color: white; }");
            client.println("tr { border: 1px solid #ddd; padding: 12px; }");
            client.println("tr:hover { background-color: #bcbcbc; }");
            client.println("td { border: none; padding: 12px; }");
            client.println(".sensor { color:white; font-weight: bold; background-color: #bcbcbc; padding: 1px; }");
            client.println("</style></head><body><h1></h1>");
            client.println("<table><tr><th>Параметр(На вулицi)</th><th>Показники</th></tr>");
            client.println("<tr><td>Температура</td><td><span class=\"sensor\">");
            client.println(OutDorSensor.readTemperature());
            client.println(" *C</span></td></tr>");    
            client.println("<tr><td>Тиск</td><td><span class=\"sensor\">");
            client.println((OutDorSensor.readPressure() / 100.0F) * 0.75006375541921);
            client.println(" mmHg</span></td></tr>");
            client.println("<tr><td>Приблизна висота над рiвнем моря</td><td><span class=\"sensor\">");
            client.println(OutDorSensor.readAltitude(SEALEVELPRESSURE_HPA));
            client.println(" m</span></td></tr>"); 
            client.println("<tr><td>Вологiсть</td><td><span class=\"sensor\">");
            client.println(OutDorSensor.readHumidity());
            client.println(" %</span></td></tr>"); 
            client.println("</body></html>");
            
            //Внутрішній сенсор сенсор
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<meta charset='UTF-8'>");   
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial;}");
            client.println("table { border-collapse: collapse; width:40%; margin-left:auto; margin-right:auto; }");
            client.println("th { padding: 12px; background-color: #0043af; color: white; }");
            client.println("tr { border: 1px solid #ddd; padding: 12px; }");
            client.println("tr:hover { background-color: #bcbcbc; }");
            client.println("td { border: none; padding: 12px; }");
            client.println(".sensor { color:white; font-weight: bold; background-color: #bcbcbc; padding: 1px; }");
            client.println("</style></head><body><h1></h1>");
            client.println("<table><tr><th>Параметр(В хаті)</th><th>Показники</th></tr>");
            client.println("<tr><td>Температура</td><td><span class=\"sensor\">");
            client.println(InDorSensor.readTemperature());
            client.println(" *C</span></td></tr>");    
            client.println("<tr><td>Тиск</td><td><span class=\"sensor\">");
            client.println((InDorSensor.readPressure() / 100.0F) * 0.75006375541921);
            client.println(" mmHg</span></td></tr>");
            client.println("<tr><td>Приблизна висота над рiвнем моря</td><td><span class=\"sensor\">");
            client.println(InDorSensor.readAltitude(SEALEVELPRESSURE_HPA));
            client.println(" m</span></td></tr>"); 
            client.println("<tr><td>Вологiсть</td><td><span class=\"sensor\">");
            client.println(InDorSensor.readHumidity());
            client.println(" %</span></td></tr>"); 
            
            client.println("</body></html>");
            client.println();
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;      
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}