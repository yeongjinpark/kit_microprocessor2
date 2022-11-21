

//mission
#include<avr/io.h>
#include<stdio.h>     //printf를 사용하기 위한 include
#define F_CPU 16000000
#include <util/delay.h>

//각도 조절을 위한 카운트 값 
#define ANGLE_MINUS90  1400   //39999 * 0.035 = 1400

#define ANGLE_ZERO  3000          //39999 * 0.075 = 3000

#define ANGLE_PLUS90  4600          //39999 * 0.115 = 4600


static int putchar0(char c, FILE *stream);         //1char송신

static FILE mystdout =FDEV_SETUP_STREAM(putchar0, NULL, _FDEV_SETUP_WRITE);


int putchar0(char c, FILE *stream)
{

if(c=='\n')
putchar0('\r',stream);                   //1charater 수신하는 함수

while(!(UCSR0A & (1<<UDRE0)));          //UCSR0A 5번 비트 = UDRE0
UDR0=c;                              //1charater 전송
return 0;
}

char getchar0()                        //1charater 수신하는 함수
{
while(!(UCSR0A & (1<<RXC0)));          //UCSR0A 7번 비트= RXC
return(UDR0);                          //1charater 수신
}

void init_uart()
{
UBRR0H=0;               //12번 비트가 의미를 가짐
UBRR0L=8;               // 16Mhz, 115200baud

UCSR0B=0x18;             //Receive(RX) 및 Transmit(TX) Enable
UCSR0C=0x06;              //UATR 모드, 8비트 데이터, No 패리티, 1 스탑비트
}

int main()
{
char c;
int i;

	DDRB |=(1<<PB5);         //모터 제어 신호 연결-> OC1A(PB5)
	
	//16비트 타이머/카운터 사용(OC1A 출력 핀 연결) IF(8비트 타이머/카운터로 연결 시 카운트 횟수가 8비트로 표현 불가)
	//모드 14, 고속 PWM모드 설정
	TCCR1A |=(1<<WGM11) | (1<<COM1A1);  //COM1A=10으로 설정
	
	//BPTTOM값에 도달 시 OC1A핀 출력 HIGH로 전환
	//비교일치가 발생할 경우 OC1A핀 출력 LOW로 전환
	//CS1= 101-분주율8로 설정 
	TCCR1B |=(1<<WGM12) | (1<<WGM13) | (1<<CS11); 
	
	//TOP값을 ICR1 레지스터에 설정
	//비교 일치값: OCRLA레지스터
	
	ICR1=39999;   //카운트 TOP값=(16M)/(50*8) - 1= 39999

init_uart();
stdout=&mystdout;


while(1)
{
printf("//Servo Motor Control Program \n1. -90 \n2. 0 \n3. 90 \n4.Exit \n");
printf("Select : "); //해당문구 출력
c=getchar0();  //1character을 받아서
printf("%c\n",c); //원래 chatacter를 돌려보냄

c=c-'0';             //아스키코드를 숫자로 변환

if(c>=1 && c<=4)        //1이상 4이하일때 아래 조건 출력
{
if(c==1)                  //c가 1일때
{
printf("\nServo motor angle -90\n\n");    //해당문구출력
while(1){                                 //루프 생성
for (i=0; i<50; i++){
OCR1A=ANGLE_MINUS90;                  //-90도
}

break;                        //루프 탈출
}
}


//////////////////////////////////////////////
if(c==2)                    //c가 2일 경우
{
printf("\nServo motor angle 0\n");
while(1)                     //루프생성
{
for (i=0; i<50; i++){
	OCR1A=ANGLE_ZERO;                //0도 

}
break;                 //루프 탈출
}

}


/////////////////////////////////////
if(c==3)                             //c가 3인 경우
{
printf("\nServo motor angle 90\n");
while(1)                                 //루프 생성
{
for (i=0; i<50; i++){
	OCR1A=ANGLE_PLUS90;                       //90도
}


break;                           //루프 탈출
}

}

if(c==4){                              //c가 4인경우
printf("Program ended");              //해당문구출력
break;                                //루프 탈출 후 프로그램 종료
}

}
}
}