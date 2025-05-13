const int motor1pin1 = 2;
const int motor1pin2 = 3;

const int motor2pin1 = 4;
const int motor2pin2 = 5;

//#define ENA_PIN 9
#define PWM_PIN 7

void setup() {
  // put your setup code here, to run once:
   pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1,   OUTPUT);
  pinMode(motor2pin2, OUTPUT);

  //(Optional)
  //pinMode(ENA_PIN,  OUTPUT); 
  pinMode(PWM_PIN, OUTPUT);
  //(Optional)
}

void loop() {
   // put your main code here, to run repeatedly:

  //Controlling speed (0   = off and 255 = max speed):     
  //(Optional)
  //analogWrite(ENA_PIN, 100); //ENA   pin
  analogWrite(PWM_PIN, 100); //ENB pin
  //(Optional)
  
  digitalWrite(motor1pin1,   HIGH);
  digitalWrite(motor1pin2, LOW);

  digitalWrite(motor2pin1, HIGH);
   digitalWrite(motor2pin2, LOW);
  delay(3000);

  digitalWrite(motor1pin1,   LOW);
  digitalWrite(motor1pin2, HIGH);

  digitalWrite(motor2pin1, LOW);
   digitalWrite(motor2pin2, HIGH);
  delay(3000);
}
