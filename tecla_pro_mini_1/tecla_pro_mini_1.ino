// TECLA 1
#define teclaEntrada1 2

// TECLA 2
#define teclaEntrada2 3

// TECLA 3
#define teclaEntrada3 4

// ESTADOS ANTERIORES DE LAS TECLAS COMBINADAS DOBLES
int beforeStateTeclaEntrada1 = 0;
int beforeStateTeclaEntrada2 = 0;
int beforeStateTeclaEntrada3 = 0;


void setup() {
  pinMode(teclaEntrada1 , INPUT_PULLUP);
  pinMode(teclaEntrada2 , INPUT_PULLUP);
  pinMode(teclaEntrada3 , INPUT_PULLUP);

  pinMode(8, OUTPUT);

  Serial.begin(115200);                           // set serial communication baudrate

  beforeStateTeclaEntrada1 = digitalRead(teclaEntrada1);
  beforeStateTeclaEntrada2 = digitalRead(teclaEntrada2);
  beforeStateTeclaEntrada3 = digitalRead(teclaEntrada3);

}

void loop() {
  // Ponemos a escuchar
  digitalWrite(8, LOW);

  // Tecla Simple 1

  int teclaEntrada1Value = digitalRead(teclaEntrada1);

  if (teclaEntrada1Value != beforeStateTeclaEntrada1) {
    beforeStateTeclaEntrada1 = digitalRead(teclaEntrada1);
    // Ponemos a enviar
    digitalWrite(8, HIGH);

    Serial.println('A');                // Send
    Serial.flush();

  }

  // Tecla Simple 2

  int teclaEntrada2Value = digitalRead(teclaEntrada2);

  if (teclaEntrada2Value != beforeStateTeclaEntrada2) {
    beforeStateTeclaEntrada2 = digitalRead(teclaEntrada2);
    // Ponemos a enviar
    digitalWrite(8, HIGH);

    Serial.println('B');                // Send
    Serial.flush();

  }

  // Tecla Simple 3

  int teclaEntrada3Value = digitalRead(teclaEntrada3);

  if (teclaEntrada3Value != beforeStateTeclaEntrada3) {
    beforeStateTeclaEntrada3 = digitalRead(teclaEntrada3);
    // Ponemos a enviar
    digitalWrite(8, HIGH);

    Serial.println('C');                // Send
    Serial.flush();

  }

  delay(50);
}
