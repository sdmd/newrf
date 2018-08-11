/*
 * This example sketch shows how to manually 
 * configure a client node via RF24Mesh,
 * and send data to the master node and 
 * receive data from master node.
 */
 
#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <SPI.h>
#include <EEPROM.h>
 
/**** Configure the nrf24l01 CE and CS pins ****/
RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);
 
/** User Configuration per node: nodeID **/
#define nodeID 42
 
// Payload to MASTER
struct payload_from_master {
  unsigned long counter;
  bool showLed;
};
 
// Payload from SLAVE
struct payload_from_slave {
  uint8_t nodeId;
  uint32_t timer;
  bool ledShown;
};
 
bool showLed;
uint32_t sleepTimer;
 
void setup() {
  pinMode(5, OUTPUT);
  Serial.begin(115200);
  // Set the nodeID manually
  mesh.setNodeID(nodeID);
  // Connect to the mesh
  Serial.println(F("Connecting to the mesh..."));
  mesh.begin();
}
 
void loop() {
  mesh.update();
 
  //// Send to the master node every two seconds - BEGIN
  if (millis() - sleepTimer >= 2000) {
    sleepTimer = millis();
    payload_from_slave payload = {nodeID, sleepTimer, showLed};
 
    // Send an 'M' type message containing the current millis()
    if (!mesh.write(&payload, 'M', sizeof(payload))) {
    // If a write fails, check connectivity to the mesh network
      if ( ! mesh.checkConnection() ) {
      //refresh the network address
      Serial.println("Renewing Address");
      mesh.renewAddress();
    } else {
      Serial.println("Send fail, Test OK");
    }
  } else {
    Serial.print("Send OK: ");
    Serial.println(payload.timer);
  }
}
//// Send to the master node every two seconds - END
 
  //// Receive a message from master if available - START
  while (network.available()) {
    RF24NetworkHeader header;
    payload_from_master payload;
    network.read(header, &payload, sizeof(payload));
    Serial.print("Received packet #");
    Serial.print(payload.counter);
    Serial.print(" Show led=");
    Serial.println(payload.showLed);
 
    showLed = payload.showLed;
 
    if (payload.showLed) {
      digitalWrite(5, HIGH);
    }
    else {
      digitalWrite(5, LOW);
    }
  }
  //// Receive a message from master if available - START
}
