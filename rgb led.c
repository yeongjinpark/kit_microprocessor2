/*
 * GccApplication62.c
 *
 * Created: 2021-04-20 오후 2:06:19
 *  Author: dudwl
 */ 


//#include <avr/io.h>
//#define F_CPU 16000000UL
//#include <util/delay.h>
//#define delaytime 1000
//
//int main(void)
//{
	//DDRB= _BV(5) | _BV(6) | _BV(7);
	//
    //while(1)
    //{
        //for(unsigned char i=0; i<8; i++){
			//PORTB=i<<5;
			//_delay_ms(delaytime);	
			//}
    //}
//}



////RGB LED-PWM제어 - 0~255까지 밝기 조절
//#include <avr/io.h>
//#define F_CPU 16000000UL
//#include <util/delay.h>
//
//#define  OCR1_R OCR1A
//#define  OCR1_G OCR1B
//#define  OCR1_B OCR1C
//
//unsigned char redV;             //빨간 LED아날로그 값 설정 (0~255)
//unsigned char greenV;			//초록 LED아날로그 값 설정 (0~255)
//unsigned char blueV;			//파란 LED아날로그 값 설정 (0~255)
//
//#define delaytime 30
//
//void set_color(unsigned char red, unsigned char green, unsigned char blue){
	//OCR1_R=red;
	//OCR1_G= green;
	//OCR1_B=blue;
//}
//
//void timer_init(){
	////fast PWM 8bit
	////WGM13(0), WGM12(1), WGM11(0), WGM10(1)
	////COM1A1/COM1B1/COM1C1: 1, COM1A0/COM1B0/COM1C0: 0
	//TCCR1A |= _BV(WGM10) | _BV(COM1A1) |_BV(COM1B1) | _BV(COM1C1);
	//TCCR1B |= _BV(WGM12) | _BV(CS11);
	//set_color(0,0,0);
//}
//
//int main(void){
	//DDRB= _BV(5) | _BV(6) | _BV(7);
	//timer_init();
	//
	//while(1){
		////빨간 led밝기 조절
		//greenV=0;
		//blueV=0;
		//for(redV =0; redV <255; redV +=5){
			////0~255까지 값을 5씩 늘리면서 점점밝게 led켜기
			//set_color(redV,greenV,blueV);
			//_delay_ms(delaytime);
		//}
		//
		//for(redV=255; redV >0; redV -=5){
			////255~0까지 값을 5씩 줄이면서 점점 어둡게 led켜기
			//set_color(redV,greenV,blueV);
			//_delay_ms(delaytime);
		//}
		////
		//////초록 led밝기 조절
				////redV=0;
				////blueV=0;
				////for(greenV =0; greenV <=255; greenV +=5){
					//////0~255까지 값을 5씩 늘리면서 점점밝게 led켜기
					////set_color(redV,greenV,blueV);
					////_delay_ms(delaytime);
				////}
				////
				////for(greenV=255; greenV >=0; greenV -=5){
					//////255~0까지 값을 5씩 줄이면서 점점 어둡게 led켜기
					////set_color(redV,greenV,blueV);
					////_delay_ms(delaytime);
				////}
				//
		//////파란 led밝기 조절
		////redV=0;
		////greenV=0;
		////for(blueV =0; blueV <=255; blueV +=5){
			//////0~255까지 값을 5씩 늘리면서 점점밝게 led켜기
			////set_color(redV,greenV,blueV);
			////_delay_ms(delaytime);
		////}
		////
		////for(blueV=255; blueV >=0; blueV -=5){
			//////255~0까지 값을 5씩 줄이면서 점점 어둡게 led켜기
			////set_color(redV,greenV,blueV);
			////_delay_ms(delaytime);
		////}		
//
	//}
//}



////RGB LED - PWM 제어 - RGB컬러 값 랜덤 생성
#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define  OCR1_R OCR1A
#define  OCR1_G OCR1B
#define  OCR1_B OCR1C

unsigned char redV;             //빨간 LED아날로그 값 설정 (0~255)
unsigned char greenV;			//초록 LED아날로그 값 설정 (0~255)
unsigned char blueV;			//파란 LED아날로그 값 설정 (0~255)

#define delaytime 1000

void set_color(unsigned char red, unsigned char green, unsigned char blue){
	OCR1_R=red;
	OCR1_G= green;
	OCR1_B=blue;
}

void timer_init(){
//fast PWM 8bit
//WGM13(0), WGM12(1), WGM11(0), WGM10(1)
//COM1A1/COM1B1/COM1C1: 1, COM1A0/COM1B0/COM1C0: 0
TCCR1A |= _BV(WGM10) | _BV(COM1A1) |_BV(COM1B1) | _BV(COM1C1);
TCCR1B |= _BV(WGM12) | _BV(CS11);
set_color(0,0,0);
}

int main(void){
	DDRB= _BV(5) | _BV(6) | _BV(7);
	timer_init();
	
	
	while(1){
		redV=rand() %256;
		greenV=rand() %256;
		blueV=rand() %256;
		set_color(redV,greenV,blueV);
		_delay_ms(delaytime);
		
	}
}