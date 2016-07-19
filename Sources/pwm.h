
// º¯ÊýÉùÃ÷
void PWMGeneralInit(byte prclock,byte scla,byte sclb,byte control);
void PWMSinglePortInit(byte channel,byte clkab,byte clock,byte polarity,byte align);
void PWMSinglePortSetting(byte channel,byte period,byte duty);
void PWMEnable(byte channel);
void PWMDisable(byte channel);
void PWMConcatenatePortSetting(byte channel,word period,word duty);
void pwm_regesiterset(word duty);
void epspwm_regesiterset(word epsduty);
