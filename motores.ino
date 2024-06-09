//Motores 
const int R1D = 2;
const int R1E = 3;
const int R2D = 4;
const int R2E = 5;


void setup() {
  //OUTPUT 
      pinMode(R1D, OUTPUT);  
      pinMode(R1E, OUTPUT); 
      pinMode(R2D, OUTPUT); 
      pinMode(R2E, OUTPUT);  
} 

void loop() {
  
Parou();
Frente();
Parou();
Atras();

}

void Frente()
{
  digitalWrite(R1E, LOW);
  digitalWrite(R2E, LOW);
  digitalWrite(R1D, HIGH);
  digitalWrite(R2D, HIGH);
  delay(2000);
}

void Atras()
{
  digitalWrite(R1D, LOW);
  digitalWrite(R2D, LOW);
  digitalWrite(R1E, HIGH);
  digitalWrite(R2E, HIGH);
  delay(2000);
}


void Parou(){
  digitalWrite(R1D, LOW);
  digitalWrite(R2E, LOW);
  digitalWrite(R2D, LOW);
  digitalWrite(R1E, LOW);
  delay(2000);
}
