#define LED 13
#define BUTTON 7

#define BRIGHTNESS 128

/** 入力文字に関する設定 */ 
#define MAX_CHAR 100 

/** モールス信号に関する設定 */
#define INPUT_S 1           //短点
#define INPUT_L 2           //長点
#define INPUT_INTERVAL 200  //一単語の入力の間隔
#define INPUT_NEXT_INTERVAL 600
#define INPUT_SPACE_INTERVAL 1400
#define INPUT_MAX 8         //一単語に置ける最大入力数
       

//モールス信号の定義
const int SIGNAL_A[] = {1,2,0,0};
const int SIGNAL_B[] = {2,1,1,1};
const int SIGNAL_C[] = {2,1,2,1};
const int SIGNAL_D[] = {2,1,1,0};
const int SIGNAL_E[] = {1,0,0,0};
const int SIGNAL_F[] = {1,1,2,1};
const int SIGNAL_G[] = {2,2,1,2};
const int SIGNAL_H[] = {1,1,1,1};
const int SIGNAL_I[] = {1,1,0,0};
const int SIGNAL_J[] = {1,2,2,2};
const int SIGNAL_K[] = {2,1,2,0};
const int SIGNAL_L[] = {1,2,1,1};
const int SIGNAL_M[] = {2,2,0,0};
const int SIGNAL_N[] = {2,1,0,0};
const int SIGNAL_O[] = {2,2,2,0};
const int SIGNAL_P[] = {1,2,2,1};
const int SIGNAL_Q[] = {2,2,1,2};
const int SIGNAL_R[] = {1,2,1,0};
const int SIGNAL_S[] = {1,1,1,0};
const int SIGNAL_T[] = {2,0,0,0};
const int SIGNAL_U[] = {1,1,2,0};
const int SIGNAL_V[] = {1,1,1,2};
const int SIGNAL_W[] = {1,2,2,0};
const int SIGNAL_X[] = {2,1,1,2};
const int SIGNAL_Y[] = {2,1,2,2};
const int SIGNAL_Z[] = {2,2,1,1};

int val = 0;
int old_val = 0;
int state = 0;

// 入力モード
int input_mode = 0;
int mode_change_state = 0;
int mode_timer = 0;

String output = "input:";
int output_count = 0;

int signals[INPUT_MAX] = {}; //モールス信号の数値
int signal_count = 0;
int signal_timer = 0;
int signal_space_state = 0;

int signal_state = 0; // 0:on_signal , 1:off_signal
int signal_state_old = 0;
int signal_start_time = 0;


void setup(){
  pinMode(LED,OUTPUT);
  pinMode(BUTTON,INPUT);
  Serial.begin(9600);
  //printSignal();
}
/**
*
1.入力モードON
1) 入力開始
  - タイマー計測開始
  - ボタン状態を保存する。
  - ボタン ON 継続時間最大[TIME]で短信号（トン） [TIME]を越えた超信号場合(ツー)　 継続[MODE_TIME]で入力モード終了
  - ボタン OFF 短信号 継続時間 [TIME]×３異常の場合は単語終了 [TIME]×7の場合はスペースを空ける。
  - 合計で８文字分 [TIME]×8で一文字

2.入力モードOFF
  - ボタンON 継続[MODE_TIME]で入力モード開始
  -出力
*/
void loop(){
  val = digitalRead(BUTTON);
  mode_change();
  
  if(input_mode){
    mores_signal();
  }
  
  if(val == HIGH){
    digitalWrite(LED, BRIGHTNESS);
  }else{
    digitalWrite(LED,LOW);
  }

  old_val = val;
}



void mores_signal(){
  //ボタンの状態が変更
  if(val == HIGH && old_val == LOW){
    signal_state = 0;
   int signal_duration = millis() - signal_start_time;
   input_signal(signal_duration);  
   signal_start_time = millis();
   
  }else if(val == LOW && old_val == HIGH){
   signal_state = 1;
   int signal_duration = millis() - signal_start_time;
   input_signal(signal_duration);  
    signal_start_time = millis();
  }
}
void input_signal(int signal_duration){

  if(signal_state == 1){
    //Serial.println(signal_duration);
    if(signal_duration < INPUT_INTERVAL ){

     // Serial.println(".");
     addSignal(INPUT_S);
    }else{
      
      //Serial.println("_");
      addSignal(INPUT_L);
    }
  }else{
    if(signal_duration < INPUT_NEXT_INTERVAL){
      //何もしない
    }else if(signal_duration < INPUT_SPACE_INTERVAL ){
  
     //次の文字へ
      //Serial.println("next");
      addCharFromInputSignal();

    }else {
     //空白文字を挿入。
     //Serial.println("next");
     if(signal_space_state == 0){
     addCharFromInputSignal();
     //Serial.println("space");
     addSpace();
     signal_space_state = 1;
     }
    }
  }
}
void mode_change(){
  if(val == LOW){
    return;
  }
  if(val == HIGH && old_val == LOW){
    mode_timer = millis();
    mode_change_state = 0;
    delay(10);
  }
  
  if(val == HIGH && old_val == HIGH && mode_change_state == 0){
    int current_time = millis();
    if((current_time - mode_timer) > 3000 ){
      input_mode = 1 - input_mode;
      mode_change_state = 1;
     Serial.println(input_mode);
      if(input_mode == 1){
        Serial.println("mode : input");
      }else{
        Serial.println("--finish input--");
        Serial.println(output);
        output = "input:";
      }

    }
  }
}



void addSignal(const int input){
  signals[signal_count] = input;
  signal_count++;
  if(signal_count == INPUT_MAX){
     addCharFromInputSignal();
    signal_count = 0;
  }  
}


//配列を比較する
boolean equalArrays(int a[],const int b[]){
   int i;
   for( i = 0;i < 4;i++){
     if(a[i] != b[i]){
       return false;
     }
   }
   return true;
}

void addSpace(){
  output += " ";
}

//現在の信号データから文字列を追加する。
void addCharFromInputSignal(){

  //printSignal();
  if(equalArrays(signals,SIGNAL_A)){
    output += "a";
  }else if(equalArrays(signals,SIGNAL_B)){
    output += "b";
  }else if(equalArrays(signals,SIGNAL_C)){
    output += "c";
  }else if(equalArrays(signals,SIGNAL_D)){
    output += "d";
  }else if(equalArrays(signals,SIGNAL_E)){
    output += "e";
  }else if(equalArrays(signals,SIGNAL_F)){
    output += "f";
  }else if(equalArrays(signals,SIGNAL_G)){
    output += "g";
  }else if(equalArrays(signals,SIGNAL_H)){
    output += "h";
  }else if(equalArrays(signals,SIGNAL_I)){
    output += "i";
  }else if(equalArrays(signals,SIGNAL_J)){
    output += "j";
  }else if(equalArrays(signals,SIGNAL_K)){
    output += "k";
  }else if(equalArrays(signals,SIGNAL_L)){
    output += "l";
  }else if(equalArrays(signals,SIGNAL_M)){
    output += "m";
  }else if(equalArrays(signals,SIGNAL_N)){
    output += "n";
  }else if(equalArrays(signals,SIGNAL_O)){
    output += "o";
  }else if(equalArrays(signals,SIGNAL_P)){
    output += "p";
  }else if(equalArrays(signals,SIGNAL_Q)){
    output += "q";
  }else if(equalArrays(signals,SIGNAL_R)){
    output += "r";
  }else if(equalArrays(signals,SIGNAL_S)){
    output += "s";
  }else if(equalArrays(signals,SIGNAL_T)){
    output += "t";
  }else if(equalArrays(signals,SIGNAL_U)){
    output += "u";
  }else if(equalArrays(signals,SIGNAL_V)){
    output += "v";
  }else if(equalArrays(signals,SIGNAL_W)){
    output += "w";
  }else if(equalArrays(signals,SIGNAL_X)){
    output += "x";
  }else if(equalArrays(signals,SIGNAL_Y)){
    output += "y";
  }else if(equalArrays(signals,SIGNAL_Z)){
    output += "z";
  }
  Serial.println(output);
  signal_count = 0;
  signal_space_state = 0;
  clearSignals();
}

void printSignal(){
    Serial.println("--------signal---------");
  int i;
  for(i = 0;i < INPUT_MAX;i++){
    Serial.println(signals[i]);
  }
  Serial.println("-----------------------");
}

void clearSignals(){
    int i;
  for(i = 0;i < INPUT_MAX;i++){
    signals[i] = 0;
  }
}
