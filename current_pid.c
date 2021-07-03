// Predefined Variable
//float error_list[3]; // need to be change
float target_current;
float target_vol;
float adc_vol; //this need to be change
// float P_arg;
// float I_arg;
float T_sam;

// New function defined Variable
float adc_vol; //this need to be change
float adc_current;
float error_list_vol[3]; // need to be change
float error_list_current[3]; // need to be change
int PID_vol_I_en = 1;
int PID_current_I_en = 1;
int PID_first_flag = 0;
float P_arg_vol;
float I_arg_vol;
float P_arg_current;
float I_arg_current;

void PID_vol_Integrate_limit(void);

// Need to modify
// PWM Duty initialization to 0 for now
// get_PI_signal() function need to refactory release the pwm duty change part.
// need get rid of get_PI_signal() function bondary detection. And integrate
//    path limitation.

void vol_PI_signal()
{
  // error_list_vol[1]  current error
  // error_list_vol[0]  last error
  // error_list_vol[2]  last PI signal

  float P_error = 0;
  float I_error = 0;

  error_list_vol[1] = target_vol - adc_vol;
  P_error = P_arg_vol*(error_list_vol[1] - error_list_vol[0]);
  I_error = I_arg_vol*(T_sam*error_list_vol[1]);
  error_list_vol[2] = error_list_vol[2] + P_error + PID_vol_I_en*I_error;

  error_list_vol[0] = error_list_vol[1];

  PID_vol_Integrate_limit();
  return;

}

void PID_vol_Integrate_limit(void)
{
  if (error_list_vol[2] > 195)
  {
    if (PID_first_flag < 50)
    {
      error_list_vol[2] = 20;
      PID_first_flag = 55;
    } else
    {
      error_list_vol[2] = 195;
    }
  } else if (error_list_vol[2] < 0)
  {
    error_list_vol[2] = 0;
  }

  if (error_list_vol[2] > 195 && error_list_vol[1] > 0)
  {
    PID_vol_I_en = 0;
    return;
  }
  if (error_list_vol[2] < 0 && error_list_vol[1] < 0)
  {
    PID_vol_I_en = 0;
    return;
  }

  PID_vol_I_en = 1;
  return;
}

void current_PI_signal()
{
  // error_list_current[1]  current error
  // error_list_current[0]  last error
  // error_list_current[2]  last PI signal

  float P_error = 0;
  float I_error = 0;

  error_list_current[1] = target_current - adc_current;
  P_error = P_arg_current*(error_list_current[1] - error_list_current[0]);
  I_error = I_arg_current*(T_sam*error_list_current[1]);
  error_list_current[2] = error_list_current[2] + P_error + PID_current_I_en*I_error;

  error_list_current[0] = error_list_current[1];

}

void get_PI_signal(void)
{
  vol_PI_signal();
  current_PI_signal();
  PID_integrate_limit();
}

void PID_current_Integrate_limit(void)
{

}

void PID_integrate_limit(void)
{
  if (PID_first_flag < 50)
  {
    PID_first_flag++;
  }
  PID_vol_Integrate_limit();
}

void duty_changer(void)
{
  EPwm1Regs.CMPA.half.CMPA = EPWN1_PRD-error_list[2]/100*EPWN1_PRD;
}
