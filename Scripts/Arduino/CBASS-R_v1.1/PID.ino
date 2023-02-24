


void RampSet()
{

  // ***** TEMP CONTROLLERS *****
  //initialize the variables ***** 

  NegIfRamping = -1;
  T1SetPoint = RAMP1_START_TEMP;
  T2SetPoint = RAMP2_START_TEMP;
  T3SetPoint = RAMP3_START_TEMP;
  T4SetPoint = RAMP4_START_TEMP;
  T1Correction = TANK1_TEMP_CORRECTION;  // adding temp probe correction values
  T2Correction = TANK2_TEMP_CORRECTION;  // adding temp probe correction values
  T3Correction = TANK3_TEMP_CORRECTION;  // adding temp probe correction values
  T4Correction = TANK4_TEMP_CORRECTION;  // adding temp probe correction values
  ChillOffset = CHILLER_OFFSET;  //the offset below the set temp when the chiller kicks in
}



void ShowRampInfo()
{
  Serial.print("The ramp1 temp:\t");
  Serial.println (T1SetPoint);
  Serial.print("The ramp2 temp:\t");
  Serial.println (T2SetPoint);
  Serial.print("The ramp3 temp:\t");
  Serial.println (T3SetPoint);
  Serial.print("The ramp4 temp:\t");
  Serial.println (T4SetPoint);
}


void PIDinit()
{
  //tell the PID to range between 0 and the full window size
  //turn the PID on
  T1_PID.SetMode(AUTOMATIC);
  T2_PID.SetMode(AUTOMATIC);
  T3_PID.SetMode(AUTOMATIC);
  T4_PID.SetMode(AUTOMATIC);
  T1_PID.SetOutputLimits(-TPCwindow, TPCwindow);//cooling range = -TPCwindow->0,  heating range = 0->TPCwindow
  T1_PID.SetTunings(kp, ki, kd);
  T2_PID.SetOutputLimits(-TPCwindow, TPCwindow);//cooling range = -TPCwindow->0,  heating range = 0->TPCwindow
  T2_PID.SetTunings(kp, ki, kd);
  T3_PID.SetOutputLimits(-TPCwindow, TPCwindow);//cooling range = -TPCwindow->0,  heating range = 0->TPCwindow
  T3_PID.SetTunings(kp, ki, kd);
  T4_PID.SetOutputLimits(-TPCwindow, TPCwindow);//cooling range = -TPCwindow->0,  heating range = 0->TPCwindow
  T4_PID.SetTunings(kp, ki, kd);
}
