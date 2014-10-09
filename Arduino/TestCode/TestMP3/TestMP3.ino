/*****************************************************************************/
//	Function: control the seeedstudo Grove MP3 player
//      Hardware: Grove - Serial MP3 Player
/*******************************************************************************/
#include <SoftwareSerial.h>
const int NUMReadings = 10;
const int NUMChannels = 4;
SoftwareSerial mp3(2, 3);//modify this with the connector you are using.

int index[NUMChannels];
int total[NUMChannels];
int average[NUMChannels];

int c1[NUMReadings];
int c2[NUMReadings];
int c3[NUMReadings];
int c4[NUMReadings];
int c5[NUMReadings];

void setup()
{
	Serial.begin(115200); 
        //initialMP3();
        sampleInitial();
}
void loop()
{
  sampleADC(0,c1);
  sampleADC(1,c2);
  sampleADC(2,c3);
  sampleADC(3,c4);
  Serial.println(sampleADC(3,c4));
}
//ADC
void sampleInitial()
{
  for (int i=0;i<NUMChannels;i++) {
    index[i] = 0;
    total[i] = 0;
    average[i] = 0;
  }
  for (int i=0;i<NUMReadings;i++) {
    c1[i] = 0;
    c2[i] = 0;
    c3[i] = 0;
    c4[i] = 0;
    
  }
}
int sampleADC(int channel,int temp[])
{
  int indexlocal = index[channel];
  int totallocal = total[channel];
  int reading;
  int averagelocal;
  
  totallocal= totallocal - temp[indexlocal];         
  // read from the sensor: 
  reading = analogRead(channel); 
  temp[indexlocal]  = reading;
  // add the reading to the total:
  totallocal= totallocal + reading;  
  // advance to the next position in the array:  
  indexlocal = indexlocal + 1;                    

  // if we're at the end of the array...
  if (indexlocal >= NUMReadings)              
    indexlocal = 0;                           
  // calculate the average:
  averagelocal = totallocal / NUMReadings; 
  index[channel] = indexlocal;
  total[channel] = totallocal;
  average[channel] = averagelocal;
  return averagelocal;
}
//MP3
void testMP3()
{
  SetMusicPlay(00,01);
  delay(100);
  SetVolume(0x0E);
  delay(5000);
  SetMusicPlay(00,02);
  delay(100);
  SetVolume(0x0E);
  delay(5000);
  SetMusicPlay(00,03);
  delay(100);
  SetVolume(0x0E);
  delay(5000);
}
void initialMP3()
{
	mp3.begin(9600);
	delay(100);
        if (true ==SetPlayMode(0x01))
          Serial.println("Set The Play Mode to 0x01, Single Loop Mode.");
        else
          Serial.println("Playmode Set Error");
        PauseOnOffCurrentMusic();  
        delay(1000); 
        SetPlayMode(0x01);
        delay(1000);      
}
//Set the music index to play, the index is decided by the input sequence
//of the music;
//hbyte: the high byte of the index;
//lbyte: the low byte of the index;
boolean SetMusicPlay(uint8_t hbyte,uint8_t lbyte)
{
	mp3.write(0x7E);
	mp3.write(0x04);
	mp3.write(0xA0);
	mp3.write(hbyte);
	mp3.write(lbyte);
	mp3.write(0x7E);
        delay(10);
	 while(mp3.available())
	{
	if (0xA0==mp3.read())
	return true;
	else 
	return false;
        }
}
// Pause on/off  the current music
boolean PauseOnOffCurrentMusic(void)
{
   mp3.write(0x7E);
   mp3.write(0x02);
   mp3.write(0xA3);
   mp3.write(0x7E);
   delay(10);
   while(mp3.available())
   {
     if (0xA3==mp3.read())
       return true;
     else 
       return false;
   }
}

//Set the volume, the range is 0x00 to 0x1F
boolean SetVolume(uint8_t volume)
{
   mp3.write(0x7E);
   mp3.write(0x03);
   mp3.write(0xA7);
   mp3.write(volume);
   mp3.write(0x7E);
   delay(10);
   while(mp3.available())
   {
     if (0xA7==mp3.read())
       return true;
     else 
       return false;
   }
}

boolean SetPlayMode(uint8_t playmode)
{
  if (((playmode==0x00)|(playmode==0x01)|(playmode==0x02)|(playmode==0x03))==false)
  {
    Serial.println("PlayMode Parameter Error! ");
    return false;
  }
   mp3.write(0x7E);
   mp3.write(0x03);
   mp3.write(0xA9);
   mp3.write(playmode);
   mp3.write(0x7E);
   delay(10);
   while(mp3.available())
   {
     if (0xA9==mp3.read())
       return true;
     else 
       return false;
   }
}

