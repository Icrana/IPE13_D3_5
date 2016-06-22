int weight = 125;
int temperature = 39;
int QRE1113_Pin = 0; //connected to analog 0

void setup(){
  Serial.begin(9600);
}

void loop(){
  Serial.print("weight:");
  Serial.print(weight);
  Serial.print("g_temperature:");
  Serial.print(temperature);
  Serial.print("C_light:");
  int QRE_Value = analogRead(QRE1113_Pin);
  Serial.print(QRE_Value); 
  Serial.print(";");
  
  delay(5000);
}

