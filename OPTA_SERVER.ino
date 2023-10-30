#include <SPI.h>
#include <Ethernet.h>

#include <ArduinoRS485.h> // ArduinoModbus depends on the ArduinoRS485 library
#include <ArduinoModbus.h>
// **** ETHERNET SETTING ****

byte mac[6] = {0xA8,0x61,0xA,0x50,0x1A,0xBC};
IPAddress ip(192, 168, 1, 116);
EthernetClient client;
signed long next;

void setup() {
  // pin declaration
  pinMode(LED_D0, OUTPUT);
  pinMode(LED_D1, OUTPUT);
  pinMode(LED_D2, OUTPUT);
  pinMode(LED_D3, OUTPUT);
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);

  Serial.begin(9600);
// display splash screen:
  digitalWrite(A0, LOW);                  // trigger some sort of alarm
  digitalWrite(A1, LOW);                  // for COMM FAIL
// start the Ethernet connection
  Ethernet.begin(mac, ip);
  delay(1000);
  Serial.print("local IP: ");
  Serial.println(Ethernet.localIP());
  Serial.println("");

  next = 0;
}

void loop() {

  if (((signed long)(millis() - next)) > 0)
    {
      next = millis() + 5000;
      Serial.print("Connecting to server ->");
      if (client.connect(IPAddress(192, 168, 1, 115),80))
        {
          Serial.print("-> Connected. ");
          client.println("GET HTTP/1.1");
          client.println("Connection: close");
          client.println();
          while(client.available()==0)
            {
              if (next - millis() < 0)
                goto close;
            }
          int size;
          while((size = client.available()) > 0)
            {
              String result = client.readString();
              Serial.print("Received-> ");
              //Serial.print(result);               // print the entire message for debuging
              for (int i = 42; i <= 47; i++) {      // print only the important bits (A0->A5)
                Serial.print(result.charAt(i));
              }
              // extracting each bit and activating the corresponding pin
              char val1; int val2;
              val1 = result.charAt(42);             // extract the pin value of the (remote) server
                val2 = val1 - '0';                  // conversion from 'char' to 'int'
                digitalWrite(LED_D0, val2);             // activate the (local) client respective pin
              val1 = result.charAt(43);
                val2 = val1 - '0';
                digitalWrite(LED_D1, val2);
              val1 = result.charAt(44);
                val2 = val1 - '0';
                digitalWrite(LED_D2, val2);
              val1 = result.charAt(45);
                val2 = val1 - '0';
                digitalWrite(LED_D3, val2);
              val1 = result.charAt(46);
                val2 = val1 - '0';
                digitalWrite(D0, val2);
              val1 = result.charAt(47);
                val2 = val1 - '0';
                digitalWrite(D1, val2);
            }

          close:
          //disconnect client
          Serial.println(" <-Disconnected.");
          client.stop();
        }
      else {
        Serial.println(" <-Connection failed.");
      }
    }
}