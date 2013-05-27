#define ledRotPin 43
#define ledBlauPin 41
#define ledGruenPin 39

void ledInit(){
  pinMode     (ledRotPin, OUTPUT);
  digitalWrite(ledRotPin, LOW);
  pinMode     (ledBlauPin, OUTPUT);
  digitalWrite(ledBlauPin, LOW);
  pinMode     (ledGruenPin, OUTPUT);
  digitalWrite(ledGruenPin, LOW);
}

void ledDoEvents(){
    switch (EthState) {
    case 0: 
  digitalWrite(ledRotPin, LOW);
  digitalWrite(ledBlauPin, LOW);
  digitalWrite(ledGruenPin, LOW);
      break; 
    case 1: 
  digitalWrite(ledRotPin, LOW);
  digitalWrite(ledBlauPin, HIGH);
  digitalWrite(ledGruenPin, LOW);
      break;
    case 2: 
  digitalWrite(ledRotPin, LOW);
  digitalWrite(ledBlauPin, LOW);
  digitalWrite(ledGruenPin, HIGH);
      break;
    case 3: 
  digitalWrite(ledRotPin, HIGH);
  digitalWrite(ledBlauPin, LOW);
  digitalWrite(ledGruenPin, LOW);
      break;
 }
}



