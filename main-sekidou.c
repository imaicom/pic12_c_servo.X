#include <xc.h>         /* XC8 General Include File */
#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */
#include <pic.h>

// CONFIG1
#pragma config FOSC = INTOSC    // INTOSC= 内部クロックを使用し、CLKINピン(RA5)はデジタルピンとする
#pragma config WDTE = OFF       // ウォッチドッグタイマを無効にする
#pragma config PWRTE = ON       // ON = 電源ONから64ms後にプログラムを開始する
#pragma config MCLRE = OFF      // ピンはデジタル入力ピン(RA3)として機能する
#pragma config CP = OFF         // プログラムメモリーの保護はしない
#pragma config CPD = OFF        // データメモリーの保護はしない
#pragma config BOREN = ON       // 電源電圧降下常時監視機能は有効とする
#pragma config CLKOUTEN = OFF   // CLKOUTピンは無効、RA4ピンとして使用する
#pragma config IESO = ON        // 内部から外部クロックへの切替えで起動を行う
#pragma config FCMEN = OFF      // 外部クロックの監視はしない

// CONFIG2
#pragma config WRT = OFF        // 2KWのフラッシュメモリ自己書き込み保護ビットを保護しない
#pragma config PLLEN = ON       // 4xPLLを動作させる(クロックを32MHzで動作させる場合に使用)
#pragma config STVREN = ON      // スタックがオーバフローやアンダーフローしたらリセットを行う
#pragma config BORV = LO        // リセット電圧(VBOR)のトリップポイントを低(1.9V)に設定する
#pragma config LVP = OFF         // 低電圧プログラミングしない

#define MHz 000000
#define _XTAL_FREQ 32MHz

// PICKIT3 Setting
//
// Program Options
//   Erase All Before Program [*]
//   Program Speed [2(fastest)]
//   Enable Low Voltage Programming [ ]
//   Programming Method [Apply Vdd before Vpp]
//
// Power
//   Power target circuit from PICkit3 [*]
//   Voltage Level [4.5]

static void Delay_ms(unsigned int DELAY_CNT) {
    for (unsigned int i = 0; i < DELAY_CNT; i++) {
        __delay_ms(1);
    }
}

void main(void) {
    
    int width = 0;
    int max_width = 0;
    int pulse = 0;
    int pulseComp = 0;

 //   OSCCON  = 0b01110000; // 内部クロックを32MHzとする
 //   OSCCON  = 0b01111010; // 内部クロックを16MHzとする
    OSCCON = 0b01110010; // 内部クロックは8MHzとする PLLEN = ONにて4倍速動作指定済みで結果32MHzとする
    ANSELA = 0b00000000; // アナログAN0は未使用、すべてデジタルI/Oに割当
    TRISA  = 0b00001111; // RA3は入力専用, リミットスイッチはRA2,RA1,RA0を使用
    OPTION_REG = 0b00000000; // bit7=0 WPUx ラッチの値に応じて弱プルアップを有効
    WPUA = 0b00000111;  // RA2,RA1,RA0弱プルアップ 10K程度
    
    Delay_ms(1); // 設定が安定するまで待つ

//    RA2 = 0; RA1 = 0;
    RA5 = 0; RA4 = 0;
    
    Delay_ms(100);
    
    while(1) {
        
        if((width > 0)&&(RA3 == 0)) {
            max_width = (width - 250)*3;
            if(max_width < 0) max_width = 0;
            width = 0;
        };
        
        if((width == 0)&&(RA3 == 1)) {
            while(RA3) width++;
        };

        pulse++; if (pulse > 40) pulse = 0;

        if((5 <= max_width)&&(max_width < 973)) {
            
            if((  5 <= max_width)&&(max_width < 250)) {RA5 = 0; RA4 = 1;}; // normal rotation
            if((250 <= max_width)&&(max_width < 450)) {RA5 = 0; RA4 = 1;if(pulse > 38) {RA5 = 0; RA4 = 0;};}; // normal rotation
                
            if((450 <= max_width)&&(max_width < 550)) {RA5 = 1; RA4 = 1;}; // lock
            
            if((550 <= max_width)&&(max_width < 750)) {RA5 = 1; RA4 = 0;if(pulse > 38) {RA5 = 0; RA4 = 0;};}; // reverse rotation
            if((750 <= max_width)&&(max_width < 973)) {RA5 = 1; RA4 = 0;}; // reverse rotation
            
//            if(pulse > pulseComp) {RA5 = 1; RA4 = 1;};
                   
        } else {RA5 = 0; RA4 = 0;}; // free // if((5 <= max_width)&&(max_width < 973))
        
    };  //  while(1)

} // main
