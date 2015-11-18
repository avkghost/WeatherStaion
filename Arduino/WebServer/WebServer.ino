#include <SPI.h>
#include <Ethernet.h>
#include <DHT.h>
#include <MG.h>

#define DHTTYPE DHT11
#define DHTPIN 7

#define MGTYPE MG811
#define MGPIN 0

// MAC Address for Ethernet shield
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

// Initialize temperture sensor
DHT dht(DHTPIN, DHTTYPE);

// Initialize CO2 sensor
MG mg(MGPIN, MGTYPE);

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {

    Serial.println(F("Failed to configure Ethernet using DHCP"));
    // no point in carrying on, so do nothing forevermore:
    while (1);
	
  }

  // start the Ethernet connection and the server:
  server.begin();
  Serial.print(F("Server is at "));
  Serial.println(Ethernet.localIP());

  dht.begin();
  mg.begin();
  
}

void loop()
{
  // listen for incoming clients
  EthernetClient client = server.available();

  if (client) {

    Serial.println(F("new client"));

	// With while client connected
    while (client.connected()) {

      if (client.available()) {
        
        char c = client.read();
        Serial.write(c);

        if (c == '\n' ) {

          int CO2Level = mg.readCO2();

          // Read parameters from DHT sensor
          float humidity = dht.readHumidity();
          float temperature = dht.readTemperature();

          // send a standard http response header
          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: text/plain"));
          client.println(F("Connection: close"));  // the connection will be closed after completion of the response
          client.println(F("Refresh: 5"));  // refresh the page automatically every 5 sec
          client.println();
          client.println(F("Room: 1\n"));

          if (isnan(humidity) || isnan(temperature)) {

            const char *error = "Failed to read from DHT sensor!";
            client.println(error);
            Serial.println(error);

          } else {

            // Compute heat index in Celsius (isFahreheit = false)
            float heatIndex = dht.computeHeatIndex(temperature, humidity, false);

            client.print(F("Humidity: "));
            client.print(humidity);
            client.print(F(" %\n"));

            client.print(F("Temperature: "));
            client.print(temperature);
            client.print(F(" *C\n"));

            client.print(F("Heat index: "));
            client.print(heatIndex);
            client.print(F(" *C\n"));

          }

          client.print(F("Sensor value: "));
          client.print(CO2Level);
          client.print(F("ppm\n"));

          client.println(F("\n"));


          break;
        }
      }
    }

    // give the web browser time to receive the data
    delay(100);

    Serial.println(F("client disconnected"));
    client.stop();
    
  }
}

