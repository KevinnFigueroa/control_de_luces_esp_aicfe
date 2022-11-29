// TECLA 1
#define teclaLiving 2

// ESTADOS ANTERIORES DE LAS TECLAS
int beforeStateTeclaLiving = 0;

void setup() {
  pinMode(teclaLiving , INPUT_PULLUP);

  pinMode(8, OUTPUT);

  Serial.begin(115200);                           

  beforeStateTeclaLiving = digitalRead(teclaLiving);

}

void loop() {
 // Ponemos a escuchar
  digitalWrite(8, LOW);
  
  // Tecla living 1
  
  int teclaLivingValue = digitalRead(teclaLiving);
 
  if(teclaLivingValue != beforeStateTeclaLiving){
     beforeStateTeclaLiving = digitalRead(teclaLiving);
     // Ponemos a enviar
     digitalWrite(8, HIGH);
     
     Serial.println('E');               
     Serial.flush();  
                   
   }
   
   delay(50);
}
