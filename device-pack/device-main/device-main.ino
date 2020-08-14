#include <ArduinoHttpClient.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h>

//MAC address
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// Set the static address values incase automatic connection doesnt work
IPAddress ip(86, 136, 198, 250);
IPAddress jsonT(172, 64, 109, 25);
IPAddress myDns(86, 136, 198, 2);

char serverAddress[] = "benmotts.com"; // server address
int port = 80;


EthernetClient EthClient;
HttpClient client = HttpClient(EthClient, serverAddress, port);

void setup()
{
  Serial.begin(9600);
  Serial.println("Serial Started");
  // start the Ethernet connection:

  Serial.println("Initialize Ethernet with DHCP:");
  if (!Ethernet.begin(mac))
  {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware)
    {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true)
      {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF)
    {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  }
  else
  {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }

  //Start built in LED
  pinMode(LED_BUILTIN, OUTPUT);
}
// the loop function runs over and over again forever
void loop()
{
  String response = "";
  if (sendRequest(response))
    if (getJsonResponse(response))
      digitalWrite(LED_BUILTIN, HIGH);
    else digitalWrite(LED_BUILTIN, LOW);
  delay(5000);
}

bool sendRequest(String & response) {
  Serial.println("Making request");
  client.get("/phpfunctions/main.php?id=1");

  int statusCode = client.responseStatusCode();
  Serial.print("Status Code: ");
  Serial.println(statusCode);
  if (statusCode >= 0) {
    response = client.responseBody();
    Serial.print("Response: ");
    Serial.println(response);
    Serial.println("Wait 5 secs");
    return true;
  }
  delay(5000);
  client.stop();
  return false;
}

const bool getJsonResponse(const String & response) {
  const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2);
  DynamicJsonDocument doc(capacity);
  Serial.print("Doc capacity: ");
  Serial.println(doc.capacity());

  DeserializationError error = deserializeJson(doc, response);
  if (error) {
    Serial.println("Deserialization Error");
    Serial.println(error.c_str());
    return false; 
  }
  else  {
    Serial.println("Deserialization Success");
    return (int) doc["val"];
  }
}

void testGet() {
  if (EthClient.connect(serverAddress, 80)) {
    Serial.println("Connected");
    EthClient.println("GET /phpfunctions/main.php?id=1 HTTP/1.0");
    EthClient.println("Host: benmotts.com");
    EthClient.println();
  } else {
    Serial.println("Connection failed");
    Serial.println();

  }

  while (EthClient.connected() && !EthClient.available()) delay(1);
  while (EthClient.connected() || EthClient.available()) {
    char c = EthClient.read();
    Serial.print(c);
  }
  Serial.println();
  Serial.println("Disconnecting");
  Serial.println("=============");
  Serial.println();
  EthClient.stop();
}
