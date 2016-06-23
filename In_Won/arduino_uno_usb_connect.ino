int number = 0;
void setup(){
     Serial.begin(9600);
       pinMode(13, OUTPUT);
   
}

void loop(){
  int incomingBytes;
   if (Serial.available() > 0)
   {
       //Serial.print("character recieved: ");
       //Serial.println(number, DEC);
       incomingBytes = Serial.read();
       
       if(incomingBytes == '1'){
        Serial.print("sending...");
       digitalWrite(13, HIGH);
      delay(1000);
          }
   
      else{
        digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
        }
   }
}

