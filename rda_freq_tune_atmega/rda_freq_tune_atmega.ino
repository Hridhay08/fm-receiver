/*
   Test and validation of RDA5807 on ESP32 board.
    
   ATTENTION:  
   Please, avoid using the computer connected to the mains during testing. Used just the battery of your computer. 
   This sketch was tested on ATmega328 based board. If you are not using a ATmega328, please check the pins of your board. 
  | RDA5807    | Function              |ESP LOLIN32 WEMOS (GPIO) |
  |-----------| ----------------------|-------------------------|
  | SDA/SDIO  |   SDIO                |   21 (SDA / GPIO21)     |
  | CLK/CLOCK |   SCLK                |   22 (SCL / GPIO22)     |
   By Ricardo Lima Caratti, 2020.
*/

#include <RDA5807.h>
#define MAX_DELAY_RDS 40   // 40ms - polling method

const int pot = A0;
int freq_checkpoints[7] = {9100, 9350, 9500, 9750, 10050, 10210, 10640};
int voltage_checkpoints[8] = {0, 146, 292, 438, 584, 730, 876, 1024};
long rds_elapsed = millis();
RDA5807 rx;

void setup()
{
    Wire.begin();
    rx.setup();
    rx.setVolume(15);
    delay(500);
    rx.setRDS(true); // Enables SDR
    detectTuning(pot);
}

void detectTuning(int pot_pin)
{
  int curr_index = getVoltageIndex(pot_pin);
  rx.setFrequency(freq_checkpoints[curr_index-1]);
  delay(200);
  while(1)
  {
    int index = getVoltageIndex(pot_pin);
    if(index!=curr_index)
    {
      curr_index = index;
      rx.setFrequency(freq_checkpoints[curr_index-1]);
      delay(200);
    }
  }
}

int getVoltageIndex(int pot_pin)
{
  int pot_read = analogRead(pot_pin);
  int index = 1;
  for(int i=1; i<8; i++)
  {
    if(voltage_checkpoints[i-1]<=pot_read && pot_read<voltage_checkpoints[i])
    {
      index=i;
      break;
    }
  }
  return index;
}
