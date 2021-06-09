#include "Energia.h"

#line 1 "C:/Users/Apoptosis/workspace_v10/Sensor_tx/Sensor_tx.ino"









#include <CC1101_MSP430.h>


void setup();
void loop();
void send_data(uint8_t Pktlen, uint8_t Rx_addr, int sensorValue);
void tx_init(uint8_t address);

#line 13
uint8_t Tx_fifo[FIFOBUFFER], Rx_fifo[FIFOBUFFER];
uint8_t My_addr, Tx_addr, Rx_addr, Pktlen, pktlen, Lqi, Rssi;
uint8_t rx_addr, sender, lqi;
int8_t rssi_dbm;




CC1101 RF;



int sensorValue = 0; 
int ack_reties = 5;  
int posValue = 0;    
int posPin = A0;     

void tx_init();
void send_data(uint8_t Pktlen, uint8_t Rx_addr, uint8_t sensorValue);


void setup()
{
  
  Serial.begin(9600);
  Serial.println();

  uint8_t add_ = 0x05;
  tx_init(add_);

  pinMode(posPin, INPUT);
}
int sensor_threshold = 100;
bool sensor_status = false;

bool senstat=TRUE;
void loop()
{
  delay(4000);
  
  
  
  posValue = analogRead(posPin);
  Serial.print("sensor:");Serial.println(posValue);

  Pktlen = 0x06;                                               
  Rx_addr = 0x02;
  send_data(Pktlen, Rx_addr, posValue);
  Rx_addr = 0x03;
  send_data(Pktlen, Rx_addr, posValue);
  Rx_addr = 0x04;
  send_data(Pktlen, Rx_addr, posValue);
  















}


void send_data(uint8_t Pktlen, uint8_t Rx_addr, int sensorValue)
{
  Tx_fifo[3] = (uint8_t)(sensorValue >> 8);
  Tx_fifo[4] = (uint8_t)(sensorValue);
  
  
  uint16_t temp_ch = 0x01;

  bool tx_status = FALSE;

  do
  {
    temp_ch += 0x32;
    if (temp_ch > 0xff)    temp_ch = 0x01;
    Tx_fifo[5] = (uint8_t)temp_ch; 

    do
    {
      tx_status = RF.send_packet(My_addr, Rx_addr, Tx_fifo, Pktlen, ack_reties);
      
    }while(!tx_status);
    delay(100);
    Serial.print("next channel:");Serial.println(temp_ch);

    RF.set_channel(temp_ch); 
    RF.receive();                        
  }while (temp_ch != 0x01);
}

void tx_init(uint8_t address)
{
                                                  
  RF.begin(My_addr);                              

  RF.sidle();                                     
  RF.set_mode(0x01);                              
  RF.set_ISM(0x02);                               
  RF.set_channel(0x01);                           
  RF.set_output_power_level(10);                  
  RF.set_myaddr(address);                         
  My_addr = address;

  RF.show_register_settings();                    
  RF.receive();                                   

  Serial.println(F("CC1101 TX Demo for MSP430")); 
}



