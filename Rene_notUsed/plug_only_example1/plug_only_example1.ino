// This is only others examples to control wireless plugs
// Does not work! It is only an example from http://funksteckdose.net/mit-microcontrollern-steuern/
//
// SEND CODE VIA 434 MHz Transmitter
//

boolean sendCode(char code[]){
    for(short z = 0; z<7; z++){ //wiederhole den Code 7x
      for(short i = 0; i<12; i++){ //ein Code besteht aus 12bits
        sendByte(code[i]);
      }
    sendByte('x'); //da der code immer mit x/sync abschliesst, brauchen wir den nicht im code und haengen es autisch immer hinten ran.
    }  
  return true;
}

void sendByte(char i) { //Diese Funktion soll 0,1 oder x senden koennen. Wir speichern die gewuenschte Ausgabe in der Variabel i
    switch(i){
      case '0':{
        digitalWrite(tx434,HIGH);
        wait(1); //da die Pausen x*350us lang sind, machen wir daraus eine Funktion
        digitalWrite(tx434,LOW);
        wait(3);
        digitalWrite(tx434,HIGH);
        wait(3);
        digitalWrite(tx434,LOW);
        wait(1);
        return;
      }
      case '1':{ //Der Code fuer '1'
        digitalWrite(tx434,HIGH);
        wait(1);
        digitalWrite(tx434,LOW);
        wait(3);
        digitalWrite(tx434,HIGH);
        wait(1);
        digitalWrite(tx434,LOW);
        wait(3);
        return;
      }
      case 'x':{ //Der Code fuer x(sync)
        digitalWrite(tx434,HIGH);
        wait(1);
        digitalWrite(tx434,LOW);
        wait(31);
      }
    }
}
 
void wait(int x) {
  delayMicroseconds(x*350); //warte x*350us
}
