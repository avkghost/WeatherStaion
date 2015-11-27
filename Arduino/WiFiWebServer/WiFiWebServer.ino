#include <Adafruit_CC3000.h>
#include <SPI.h>
#include <DHT.h>
#include <MG.h>

#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10

#define WLAN_SSID       "APNAME"   // cannot be longer than 32 characters!
#define WLAN_PASS       "APPASS"
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define LISTEN_PORT           80      // What TCP port to listen on for connections.
#define MAX_ACTION            10      // Maximum length of the HTTP action that can be parsed.  
#define MAX_PATH              64      // Maximum length of the HTTP request path that can be parsed.
#define BUFFER_SIZE           MAX_ACTION + MAX_PATH + 20
#define TIMEOUT_MS            500

#define DHTTYPE DHT11
#define DHTPIN 7

#define MGTYPE MG811
#define MGPIN 0

Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                         SPI_CLOCK_DIVIDER); // you can change this clock speed

Adafruit_CC3000_Server httpServer(LISTEN_PORT);

// Initialize temperture sensor
DHT dht(DHTPIN, DHTTYPE);

// Initialize CO2 sensor
MG mg(MGPIN, MGTYPE);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Initialise the module
  Serial.println(F("\nInitializing..."));

  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while (1);
  }

  Serial.print(F("\nAttempting to connect to ")); Serial.println(WLAN_SSID);
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while (1);
  }

  Serial.println(F("Connected!"));

  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  {
    delay(100); // ToDo: Insert a DHCP timeout!
  }

  // Display the IP address DNS, Gateway, etc.
  while (! displayConnectionDetails()) {
    delay(1000);
  }

  // Start listening for connections
  httpServer.begin();

  Serial.println(F("Listening for connections..."));

  dht.begin();
  mg.begin();

}

void loop()
{
  // put your main code here, to run repeatedly:

  Adafruit_CC3000_ClientRef client = httpServer.available();
  if (client) {

    Serial.println("new client");

    while (client.connected()) {

      if (client.available()) {

        char c = client.read();
        Serial.write(c);

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply

        if (c == '\n') {
          
          int CO2Level = analogRead(MGPIN);
          float alco = 0;
          //mg.readCO2();
          CO2Level = map(CO2Level, 0, 1023, 350, 10000); // CO2 mapping
//          CO2Level = map(CO2Level, 512, 820, 25, 500); // Alco

          if (CO2Level < 0) {
            CO2Level = 0;
          } else {
            alco = CO2Level / 1000.;
          }

          float humidity = dht.readHumidity();
          float temperature = dht.readTemperature();

          // send a standard http response header
          client.fastrprintln(F("HTTP/1.1 200 OK"));
          client.fastrprintln(F("Content-Type: text/plain"));
          client.fastrprintln(F("Connection: close"));  // the connection will be closed after completion of the response
          client.fastrprintln(F("Refresh: 5"));  // refresh the page automatically every 5 sec
          client.fastrprintln(F(""));
          client.fastrprintln(F("Room: 1\n"));

          if (isnan(humidity) || isnan(temperature)) {

            const char *error = "Failed to read from DHT sensor!";
            client.fastrprintln(error);
            Serial.println(error);

          } else {

            // Compute heat index in Celsius (isFahreheit = false)
            float heatIndex = dht.computeHeatIndex(temperature, humidity, false);

            client.fastrprint(F("Humidity: "));
            client.print(humidity);
            client.fastrprint(F(" %\n"));

            client.fastrprint("Temperature: ");
            client.print(temperature);
            client.fastrprint(" *C\n");

            client.fastrprint(F("Heat index: "));
            client.print(heatIndex);
            client.fastrprint(F(" *C\n"));

          }

          client.fastrprint(F("Sensor value: "));
//          client.print(CO2Level);
          client.print(alco);
          client.fastrprint(F("ppm\n"));

          client.fastrprintln(F("\n"));

          break;
        }
        
      }
    }

    // give the web browser time to receive the data
    delay(100);

    // close the connection:
    Serial.println("client disconnected");
    client.close();
  }

}

// Tries to read the IP address and other connection details
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;

  if (!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}

