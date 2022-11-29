// TECLA 1
#define teclaComedor1 2

// TECLA 2
#define teclaComedor2 3

// ESTADOS ANTERIORES DE LAS TECLAS COMBINADAS DOBLES
int beforeStateTeclaComedor1 = 0;
int beforeStateTeclaComedor2 = 0;


void setup() {
  pinMode(teclaComedor1 , INPUT_PULLUP);
  pinMode(teclaComedor2 , INPUT_PULLUP);

  pinMode(8, OUTPUT);
  
  Serial.begin(115200);                           // set serial communication baudrate 

  beforeStateTeclaComedor1 = digitalRead(teclaComedor1);
  beforeStateTeclaComedor2 = digitalRead(teclaComedor2);
  
}

void loop() {
  // Ponemos a escuchar
  digitalWrite(8, LOW);
  
  // Tecla comedor 1
  
  int teclaComedor1Value = digitalRead(teclaComedor1);
 
  if(teclaComedor1Value != beforeStateTeclaComedor1){
     beforeStateTeclaComedor1 = digitalRead(teclaComedor1);
     // Ponemos a escuchar
     digitalWrite(8, HIGH);
     
     Serial.println('B');                // Send 
     Serial.flush();  
        
   }

  // Tecla comedor 2
  
  int teclaComedor2Value = digitalRead(teclaComedor2);
 
  if(teclaComedor2Value != beforeStateTeclaComedor2){
     beforeStateTeclaComedor2 = digitalRead(teclaComedor2);
     
     digitalWrite(8, HIGH);

     Serial.println('D');                // Send 
     Serial.flush();  
                   
   }
  
   delay(50);
}
