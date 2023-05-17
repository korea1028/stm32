1. Core/Inc 폴더 안에 AM2302.h 파일을 넣는다.
2. Core/Src 폴더 안에 AM2302.c 파일을 넣는다.
3. main.c 파일에 AM2302.h 파일을 include 한다.
4. timer 하나를 1us 마다 count 하도록 설정.
ex) TIM1은 APB2 timer clock을 사용, HCLK가 168Mhz일 경우 Prescaler 168 - 1, Period는 65535로 설정하면 1us 마다 count 한다. 
