
#include <RFM69OOK.h>
#include <RFM69.h>
#include <SPI.h>
boolean RFM69OOK_DEBUG = true;      // Activate RFM69OOK Debug function
RFM69OOK switchKaku;                // Create a RFMOOK instance with default parametres
 #define NODEID      1              // Dummy node address
 #define NETWORKID   100            // Dummy network address
 #define FREQUENCY   RF69_433MHZ    // Match this with the version of your Moteino! (for KAKU only 433MHz is supported
 RFM69 radio;
void setup() {
  Serial.begin(115200);
  //Initialize serial and wait for port to open: 
  while (!Serial) {
     ; // wait for serial port to connect. Needed for Leonardo only
  }
  radio.initialize(FREQUENCY,NODEID,NETWORKID);       // Default RFM FSK initialisation

}
void loop() {
  switchKaku.setOokParams(250,5,10);        // Configure the specific parametres for new kaku (symbol period 250us, repeat 5 times with delaty of 10ms)
  switchKaku.sendKakuNew(radio, 1332798,  16,  true, 0, 0); //  Send command ON via RFM69 radio instance from House Code 1332798 to unit 16 no group, no dim
  delay (5000);
  switchKaku.setOokParams(300, 8,15);        // Configure the specific parametres for old kaku (symbol period 250us, repeat 8 times with delaty of 15ms)
  switchKaku.sendKakuOld(radio, 'D', 16, false); // Send command OFF via RFM69 radio instance from House Code 'D' to unit 16
  delay (5000);
  switchKaku.setOokParams(350,10,20);     // Configure the specific parametres for cogex kaku (symbol period 350us, repeat 10 times with delaty of 20ms)
  switchKaku.sendKakuCogex(radio, 12, 1, true); // Send command ON via RFM69 radio instance from House Code 12 to unit 11
  delay (5000);
  switchKaku.sendKakuCogex(radio, 12, 1, false); // Send command OFF via RFM69 radio instance from House Code 12 to unit 11
  delay(5000);
}
