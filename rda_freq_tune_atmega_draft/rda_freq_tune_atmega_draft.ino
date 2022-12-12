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

const int pot_pin = A0;
int freq_checkpoints[8] = {0,0,0,0,0,0,0,0};
int voltage_checkpoints[8] = {0,0,0,0,0,0,0,0};
int voltage_inc = 127;
long rds_elapsed = millis();
RDA5807 rx;

void setup()
{
    Wire.begin();
    rx.setup();
    rx.setVolume(15);
    delay(500);
    rx.setFrequency(10650); // It is the frequency you want to select in MHz multiplied by 100.
    delay(200);
    rx.setRDS(true); // Enables SDR
    detectTuning(A0);
}

void detectTuning(int pot_pin)
{
  rx.setFrequency(10650);
  delay(200);
  while(1)
  {
    downwardRun(pot_pin);
    upwardRun(pot_pin);
  }
}

void generateCheckpoints(freq_boundary, extreme, trend)
{
  //generate frequency checkpoints
  if(trend==0)
    for(int i=0; i<8; i++)
      freq_checkpoints[i] = freq_boundary - i*10;
  else if(trend==1)
    for(int i=0; i<8; i++)
      freq_checkpoints[i] = freq_boundary + i*10;

  //generate voltage checkpoints
  int extreme1 = extreme;
  int extreme2 = 1023-extreme;
  int sign = 1;
  if(extreme2<extreme1) sign=-1;
  for(int i=0; i<8;i++)
    voltage_checkpoints[i] = extreme+i*sign*voltage_inc;
}

void waitPotExtreme()
{
  int pot_read = analogRead(pot_pin);
  int extreme = 0;
  while(1)
  {
    if(pot_read == 0)
    {
      extreme = 0;
      break;
    }
    if(pot_read == 1023)
    {
      extreme = 1023;
      break;
    }
    pot_read = analogRead(pot_pin);
  }
  return extreme;
}

void getVoltageCPIndex(int pot_pin)
{
  int pot_read = analogRead(pot_pin);
  for(int i=0; i<8; i++)
  {
    voltage = voltage_checkpoints[i];
  }
}


void downwardRun(int pot_pin)
{
  int freq_upper = 10650;

  //wait till potentiometer at one of the extremes 0 and 1023
  int extreme = waitPotExtreme();

  //loop till downward run reaches below 87 MHz
  generateCheckpoints(freq_upper, extreme, 0);
  int curr_index = 0;
  while(1)
  {
    //if the potentiometer reaches other extreme, drop 0.8 MHz lower, change extreme, generate checkpoints, skip the current iteration
    if(analogRead(pot_pin)==1023-extreme)
    {
      freq_upper -= 80;
      extreme = 1023-extreme;
      generateCheckpoints(freq_upper, extreme, 0);
      continue;
    }
    
    int index = getVoltageCPIndex(pot_pin);
    
    // if frequency less than 87MHz, break
    if(frequency_checkpoints[index]<8700) break;

    // check whether 
    if(index>curr_index || index<curr_index)
    {
      curr_index=index;
      rx.setFrequency(frequency_checkpoints[curr_index]);
      delay(200);
    }
  }
}
