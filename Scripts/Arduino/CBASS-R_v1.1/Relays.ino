


void RelaysInit()
{
  //-------( Initialize Pins so relays are inactive at reset)----
  digitalWrite(T1HeaterRelay, RELAY_OFF);
  digitalWrite(T1ChillRelay, RELAY_OFF);
  digitalWrite(T2HeaterRelay, RELAY_OFF);
  digitalWrite(T2ChillRelay, RELAY_OFF);
  digitalWrite(T3HeaterRelay, RELAY_OFF);
  digitalWrite(T3ChillRelay, RELAY_OFF);
  digitalWrite(T4HeaterRelay, RELAY_OFF);
  digitalWrite(T4ChillRelay, RELAY_OFF);
  //---( THEN set pins as Outputs )----
  pinMode(T1HeaterRelay, OUTPUT);
  pinMode(T1ChillRelay, OUTPUT);
  pinMode(T2HeaterRelay, OUTPUT);
  pinMode(T2ChillRelay, OUTPUT);
  pinMode(T3HeaterRelay, OUTPUT);
  pinMode(T3ChillRelay, OUTPUT);
  pinMode(T4HeaterRelay, OUTPUT);
  pinMode(T4ChillRelay, OUTPUT);
}
