  //Projeto PlanDuino por Diego Bengozzi de Brito
  #include <Wire.h> //biblioteca para o relogio I2C
  #include <RTClib.h>
  #include <LiquidCrystal.h>
  #include <EEPROM.h> // incluir a biblioteca 
  
  LiquidCrystal lcd(12, 11, 5, 4, 3, 2);                           
  RTC_DS1307 RTC;
  const byte letters[8][8]= {
     { 0xe, 0x11, 0x10, 0x10, 0x15, 0xe, 0x8, 0x0 }, // ccedil....0
     { 0x8, 0x14, 0x8, 0x3, 0x4, 0x4, 0x3, 0x0 }, // celsius......1
     { 0x0, 0x4, 0x8, 0x1f, 0x8, 0x4, 0x0, 0x0 }, // arrow left...2
     { 0x0, 0x4, 0x2, 0x1f, 0x2, 0x4, 0x0, 0x0 }, // arrow right..3
     { 0x0, 0x4, 0x4, 0x15, 0xe, 0x4, 0x0, 0x0 }, // arrow down...4
     { 0x0, 0x4, 0xe, 0x15, 0x4, 0x4, 0x0, 0x0 }, // arrow up.....5
     { 0x8, 0x14, 0x14, 0x8, 0x0, 0x5, 0x6, 0x5 }, // ok..........6
     { 0x0, 0xa, 0xa, 0x0, 0x11, 0x11, 0xe, 0x0 }, // smile.......7
     //www.analisandobits.com/bitmap-char
  };
  
  
  const byte alerta = 9;// saida para sistema de alerta
  
  const byte chds = 8;//teste na protobord CHave Direito Superior
  const byte chdi = 6;//CHave Direito Inferior  
  const byte ches = 13;//CHave Esquerda Superior
  const byte chei = 10;//CHave Esqueda Inferior
  
  const byte botao = 7;//botão do painel para passa as telas
  const int b2 = A0; //botão adicionar
  const int b3 = A1; //botão decramentar
  
  const int pistao = A2; //'botao' q determina se a plantadeira esta erguida ou abaixada
  byte estadoPistao;//variavel informa se a plantadeira esta esguida ou abaixada
  
  byte telaInicio = 0;      //VARIAVEIS
  boolean altTela = false;  
  unsigned long distanciaTotal =0;
  unsigned long sementeTotal = 0;
  float hectare = 0;
  float alqueire = 0;
  float kmh = 0;
  float sementeLinear = 0;
  float metroLinearTotal = 0;
  float parcialSementeLinear = 0;
  float parcialDistanciaLinear = 0;
  int test1;
  int test2;
  int test3;
  int test4;  
  byte numeroLinha = EEPROM.read(0);
  byte espacamento = EEPROM.read(1);
  byte discoEnd = EEPROM.read(2);
  byte discoSemente[] = {18, 28, 45, 78, 90, 100};
  byte velocidadeMaxima = EEPROM.read(3);  
  //int notas[] = { 524, 588, 660, 699, 785, 881, 989 }; // possivelmente usadas no sistema de alerta de erros e falhas
  
  
  void setup(){
    for (int i = 0; i < 8; i++) {//carrega o LCD com os novos caracteres
      lcd.createChar(i, (uint8_t*)letters[i]);
    }
    pinMode(alerta, OUTPUT);    //INICIALIZACAO DA PORTA
    pinMode(chds, INPUT);
    pinMode(chdi, INPUT);
    pinMode(ches, INPUT);
    pinMode(chei, INPUT);
    pinMode(botao, INPUT);
    pinMode(b2, INPUT);
    pinMode(b3, INPUT);
    pinMode(pistao, INPUT);    
    Wire.begin();
    RTC.begin();
    lcd.begin(16, 2);
    ajusteRTC();    
    telaApresentacao();    
  }  
  void loop(){ 
    
    unsigned long tempoFinal = millis() + 1000;

    test1 = 0;
    test2 = 0;
    test3 = 0;
    test4 = 0;

    
    byte estadoAnteriorChds = digitalRead(chds);
    byte estadoAnteriorChdi = digitalRead(chdi);
    byte estadoAnteriorChes = digitalRead(ches);
    byte estadoAnteriorChei = digitalRead(chei);
    byte estadoAnteriorBotao = digitalRead(botao);
    byte estadoAnteriorB2 = digitalRead(b2);
    byte estadoAnteriorB3 = digitalRead(b3);  
    estadoPistao = digitalRead(pistao);
    
    while(tempoFinal > millis()){

      
      byte estadoAtualChds = digitalRead(chds);
      byte estadoAtualChdi = digitalRead(chdi);
      byte estadoAtualChes = digitalRead(ches);
      byte estadoAtualChei = digitalRead(chei);
      
      byte estadoAtualBotao = digitalRead(botao);
      byte estadoAtualB2 = digitalRead(b2);
      byte estadoAtualB3 = digitalRead(b3);
      
  
      if(estadoAtualChds != estadoAnteriorChds){ // teste de pulsos da chave optica
        test1++;
        estadoAnteriorChds = estadoAtualChds;
      }
     
      if(estadoAtualChdi != estadoAnteriorChdi){ 
        test2++;
        estadoAnteriorChdi = estadoAtualChdi;
      } 
  
      if(estadoAtualChes != estadoAnteriorChes){ 
        test3++;
        estadoAnteriorChes = estadoAtualChes;
      }
      
      if(estadoAtualChei != estadoAnteriorChei){ 
        test4++;
        estadoAnteriorChei = estadoAtualChei;
      } 
      
      //Botao central - confirmacao/altera tela de configuracao/paraliza alarmes
      if(estadoAtualBotao != estadoAnteriorBotao && estadoAtualBotao == HIGH ){
        switch(telaInicio){
           case 0:
             break;
           case 1:             
             break;
           case 2:             
             break;
           case 3:
             break;
           case 4:
             break;
           case 5:
             telaInicio++;
             break;
           case 6:
             telaInicio++;
             break;
           case 7:
             telaInicio++;
             break;
           case 8:
             telaInicio++;
             break;
           case 9:
             telaInicio++;        
             break;            
           case 10:       
             break; 
           case 11:
             if(altTela == false)telaInicio = 0;
             else telaInicio = 12;       
             break; 
           case 12:
             telaInicio = 1;       
             break;             
           case 13:
             //if(altTela == false)telaInicio = 0;
             //else telaInicio = 12;
             telaInicio = 12;   
             break;             
           case 14:
             telaInicio = 11;
             break;
           case 15:
             telaInicio = 11;
             break;            
        }
        estadoAnteriorBotao = estadoAtualBotao;
      } 
      
      //Botao adicionar/acresentar/lado direito/avancar tela
       if(estadoAtualB2 != estadoAnteriorB2 && estadoAtualB2 == HIGH ){
         switch(telaInicio){
           case 0:
             telaInicio++;
             break;
           case 1:
             telaInicio++;
             break;
           case 2:
             telaInicio++;
             break;
           case 3:
             telaInicio++;
             break;
           case 4:
             telaInicio++;
             break;
           case 5:
             break;           
           case 6:
             if(numeroLinha < 32){
               numeroLinha++;
             }
             break;
           case 7:
             if(espacamento < 100){
               espacamento++;
             }
             break;
           case 8:
             if(discoEnd < sizeof(discoSemente)-1){
               discoEnd++;
             }
             break;
           case 9:
             if(velocidadeMaxima < 30){
               velocidadeMaxima++;
             }
             break; 
           case 10:
             EEPROM.write(0, numeroLinha);
             EEPROM.write(1, espacamento);
             EEPROM.write(2, discoEnd);
             EEPROM.write(3,  velocidadeMaxima);
             telaInicio = 0;
             break;
             
           case 11:
             telaInicio = 0;
             break;
           case 12:       
             telaInicio = 1;
             break;             
           case 13:       
             //telaInicio = 0;
             break;             
           case 14:       
             //telaInicio = 0;
             break;
           case 15:
             //telaInicio = 0;       
             break; 

            
         }
        estadoAnteriorB2 = estadoAtualB2;
      }
      
      //Botao decrementar/lado esquerdo/voltar tela
      if(estadoAtualB3 != estadoAnteriorB3 && estadoAtualB3 == HIGH ){
        switch(telaInicio){
           case 0:
             break;
           case 1:
             if(altTela == false)telaInicio = 0;
             else telaInicio = 12;
             break;
           case 2:
             telaInicio--;
             break;
           case 3:
             telaInicio--;
             break;
           case 4:
             telaInicio--;
             break;
           case 5:
             telaInicio--;
             break;
           case 6:
             if(numeroLinha > 2){
               numeroLinha--;
             }
             break;
           case 7:
             if(espacamento > 40){
               espacamento--;
             }            
             break;
           case 8:
             if(discoEnd > 0){
               discoEnd--;
             }
             break;
             
           case 9:
             if(velocidadeMaxima > 1){
               velocidadeMaxima--;
             }
             break; 
           case 10:
             numeroLinha = EEPROM.read(0);
             espacamento = EEPROM.read(1);
             discoEnd = EEPROM.read(2);
             velocidadeMaxima = EEPROM .read(3);
             telaInicio = 0;
             break;
           case 11:       
             break; 
           case 12:       
             break;             
           case 13:       
             break;             
           case 14:       
             break;             
        }
        estadoAnteriorB3 = estadoAtualB3;
      } 
    }//fim while
                     /*********************************************************************************************************
                      *TESTE PRINCIPAIS E REGRAS DE NEGOCIO.
                      ********************************************************************************************************/
                      
    //PISTAO ABAIXADO / MODO PLANTIO    
    if(estadoPistao == LOW){      
      if(altTela == true){// alterna Tranporte para Plantando
        telaInicio = 0;
        altTela = false;
      }    
      distanciaTotal = distanciaTotal + ((test1 + test3) / 2);//calculo media distacia percorrida
      sementeTotal = sementeTotal + ((test2 + test4) / 2);
      parcialDistanciaLinear = parcialDistanciaLinear + ((test1 + test3) / 2);
      parcialSementeLinear = parcialSementeLinear + ((test2 + test4) / 2);      
      float valorPorcento = test1/100*50;          
      if(((test1 - test3 > valorPorcento)||(test3 - test1 > valorPorcento))&& telaInicio != 11){
        //diferença maior q 25% de velocidade
        telaInicio = 15;
      } 
      if((test1 == 0 || test3 == 0) && telaInicio != 11 ){
        //sensores de velocidade parados
        telaInicio = 14;         
      }
      if((test2 == 0 || test4 == 0) && telaInicio != 11){
        //sensores de semente parados        
        telaInicio = 14;
      }      
      if((test1 > 0 && test2 > 0 && test3 > 0 && test4 > 0) && telaInicio == 11){//caso a plantadeira esteja parada, ao movimentar-se entra no modo plantando
        telaInicio = 0;
      }        
      kmh = ((2.5/20)*((test1 + test3) / 2))*3.6;          //calculo de km/h com base em SEGUNDOS
      if(kmh >  velocidadeMaxima){                         //teste de velocidade alta
        alert(1);
        telaInicio = 3;        
      }else if(kmh > 0 && kmh <= velocidadeMaxima){ 
        alert(0);
      }          
    }
    
    //Pistao Erguido  - Modo Trasporte *********************************************************************************************************
    if(estadoPistao == HIGH){     
      if(test1 > 0 || test2 > 0 || test3 > 0 || test4 > 0 ){
      //Erro movimento dos sensores com a plantadeira erguida
      telaInicio = 13;
      }
      if(altTela == false){//alterna plantado para parado
        telaInicio = 12;
        altTela = true;
      } 
    }
    
    if(parcialDistanciaLinear > 50){
      sementeLinear = (parcialSementeLinear/12.0/5.0*discoSemente[discoEnd])/((2.5/20) * parcialDistanciaLinear);
      parcialSementeLinear = 0;
      parcialDistanciaLinear = 0;
    }
      
    metroLinearTotal = ((2.5/20) * distanciaTotal);
    // 2,5 metros percorridos com 20 pulsos    
    hectare = (( metroLinearTotal*(numeroLinha * espacamento/100) ) / 10000 );
    alqueire = (( metroLinearTotal*(numeroLinha * espacamento/100) ) / 24200 );
    
    telaLCD(telaInicio);
  }
  //fim loop **************************************************************************************************************************************
  
  void telaLCD(int aux){ //Metodo de escola da TELA
    lcd.clear(); 
    switch(aux){
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("    Plantando  ");
        lcd.setCursor(15, 0);
        lcd.write(3);
        lcd.setCursor(0, 1);
        lcd.print(alqueire, 1);
        lcd.print("  ");
        lcd.print(sementeLinear, 1);
        lcd.print("  ");
        lcd.print(kmh, 1); 
        alert(0);       
        break;
        
      case 1:
        lcd.setCursor(0, 0);
        lcd.write(2);
        lcd.print(" Hect. - Alq.");
        lcd.setCursor(15, 0);
        lcd.write(3); 
        lcd.setCursor(0, 1);     
        lcd.print(hectare, 3);       
        lcd.setCursor(8, 1);
        lcd.print(alqueire, 3);
        break;

      case 2:
        lcd.setCursor(0, 0);
        lcd.write(2);
        lcd.print("Semente Metro");
        lcd.setCursor(15, 0);
        lcd.write(3);
        lcd.setCursor(0, 1);
        lcd.print("L:");
        lcd.print(sementeLinear, 2);
        lcd.setCursor(7, 1);
        lcd.print("T:"); 
        lcd.print(sementeTotal/12.0/5.0*discoSemente[discoEnd]*numeroLinha, 0);

        break;

      case 3:
        lcd.setCursor(0, 0);
        lcd.write(2);
        lcd.print("Vel.Max:");
        lcd.print(velocidadeMaxima);
        lcd.print("km/h");
        lcd.setCursor(15, 0);
        lcd.write(3);
        lcd.setCursor(2, 1);
        lcd.print(kmh, 1);
        if(kmh > velocidadeMaxima){
          lcd.print("  Alta");
        }else{ 
          lcd.print("  Normal");
        }
        break;   
        
      case 4: //tela de informacao da data e hora
        lcd.setCursor(0, 0);
        lcd.write(2);
        hora();
        lcd.setCursor(15, 0);
        lcd.write(3);
        lcd.setCursor(0, 1);
        data();
        alert(0);
        break;
             
      case 5:
        lcd.setCursor(0, 0);
        lcd.write(2);
        lcd.print(" CONFIGURACAO  ");      
        lcd.setCursor(0, 1);
        lcd.print("       OK       ");
        break;
     
      case 6:
        lcd.setCursor(0, 0);
        lcd.print("Numero de Linhas");
        lcd.setCursor(0, 1);
        lcd.print("-     ");
        lcd.print(numeroLinha);
        lcd.setCursor(15, 1);
        lcd.print("+");
        break;
        
      case 7:
         lcd.setCursor(0, 0);
         lcd.print("Espacamento - Cm");
         lcd.setCursor(0, 1);
         lcd.print("-     ");
         lcd.print(espacamento);
         lcd.setCursor(15, 1);
         lcd.print("+");
         break;
  
      case 8:
         lcd.setCursor(0, 0);
         lcd.print("Nun. Furos Disco");
         lcd.setCursor(0, 1);
         lcd.print("-     ");
         lcd.print(discoSemente[discoEnd]);
         lcd.setCursor(15, 1);
         lcd.print("+");
         break;

     case 9:
       lcd.setCursor(0, 0);
       lcd.print("Veloc. Maxima");
       lcd.setCursor(0, 1);
       lcd.print("-     ");
       lcd.print(velocidadeMaxima);
       lcd.setCursor(15, 1);
       lcd.print("+");
       break;
       
      case 10:
        lcd.setCursor(0, 0);
        lcd.print("     SALVAR?    ");
        lcd.setCursor(0, 1);
        lcd.print("NAO          SIM");
        break;
        
      case 11:
        lcd.setCursor(0, 0);
        lcd.print("     Parado     ");
        lcd.setCursor(0, 1);
        lcd.print("       OK       "); 
        alert(0);       
        break;
        
      case 12:
        lcd.setCursor(0, 0);
        lcd.print("   Transporte   ");
        lcd.setCursor(15, 0);
        lcd.write(3);
        lcd.setCursor(0, 1);
        lcd.print("       OK       ");
        alert(0);        
        break;
        
      case 13:
        lcd.setCursor(0, 0);
        lcd.print("     Alerta     ");
        lcd.setCursor(0, 1);
        lcd.print("       OK       ");
        alert(1);        
        break;
      
      case 14:
        lcd.setCursor(0, 0);
        lcd.print("  Falha / Erro  ");
        lcd.setCursor(0, 1);
        lcd.print("       OK       ");
        alert(1);        
        break;
      
      case 15:
        lcd.setCursor(0, 0);
        lcd.print("Diferenca A != B");
        lcd.setCursor(0, 1);
        lcd.print("       OK       ");
        alert(1);        
        break;
        
    }
  }
  void alert(byte n){ //chamado quando algo estiver errado...saida para disposistivos
    switch(n){
      case 0:
        digitalWrite(alerta, LOW);
        break;
      case 1:
        digitalWrite(alerta, !digitalRead(alerta));
        break;
    }
//    for (int i = 0; i < 7; i++) {
//      tone(alerta, notas[i], 1000);
//      delay(1000);
//    }
//    delay(1000);
  }
     
  void telaApresentacao(){ // Tela de Inicio do Sistema
    do{
      lcd.setCursor(0,0);   
      lcd.write("Monitor de plantio PLANDUINO!");
      lcd.setCursor(0,1);
      lcd.print("   Lin:");
      lcd.print(numeroLinha);
      lcd.print(" -Esp:");
      lcd.print(espacamento);
      lcd.print(" -Fur:");
      lcd.print(discoSemente[discoEnd]);
      lcd.scrollDisplayLeft();
      telaInicio++;
      delay(400);
    }while(telaInicio < 19);
    lcd.clear();
    telaInicio = 0;
  }
  
  void data(){ // Exibe a Data na Tela
     DateTime now = RTC.now();
     lcd.print(" Data:");
     lcd.print(now.day());
     lcd.print("/");
     lcd.print(now.month());
     lcd.print("/");
     lcd.print(now.year());
  }
  
  void hora(){ // Exibe a hora na tela
    DateTime now = RTC.now();
    lcd.print("Hora:");
    lcd.print(now.hour());
    lcd.print(":");
    lcd.print(now.minute());
    lcd.print(":");
    lcd.print(now.second());
  }
  
  void ajusteRTC(){ //Se o RTC nao estive executando ele configura a hora da compilacao
      if (!RTC.isrunning()) { //encaso da hora estiver errada somente tirar o ponto de explamacao e compilar
          RTC.adjust(DateTime(__DATE__, __TIME__));
      }
  }
