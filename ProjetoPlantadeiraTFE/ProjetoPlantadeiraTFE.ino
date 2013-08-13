  //Projeto PlanDuino por Diego Bengozzi de Brito
  #include <Wire.h> //biblioteca para o relogio I2C
  #include <RTClib.h>
  #include <LiquidCrystal.h>
  #include <EEPROM.h>
  #define DS1307_ADDRESS 0x68
  #include <IRremote.h>
  
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
  
  
  const byte alerta = 13;// saida para sistema de alerta
  
  const byte chds = 10;//teste na protobord CHave Direito Superior
  const byte chdi = 9;//CHave Direito Inferior
  const byte ches = 8;//CHave Esquerda Superior
  const byte chei = 7;//CHave Esqueda Inferior
  const byte pistao = 6; //'botao' q determina se a plantadeira esta erguida ou abaixada
  const int emergencia = A3; // nova chave de emergencia / porta usada para controle remoto...A3
  
  const byte b3 = A0; //botão decramentar                             
  const byte botao = A1;//botão do painel para passa as telas        
  const byte b2 = A2; //botão adicionar                               
  
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
  float maximoSementeLinear = getFloat(4); //ocupas as posicoes 4 e 5 da memoria eeprom
  byte crAtivo = EEPROM.read(6);//controle remoto ativo? 0=NAO -/- 1=SIM
  //byte variavelLivreEEPROM = EEPROM.read(7);
  
  //int notas[] = { 524, 588, 660, 699, 785, 881, 989 }; // possivelmente usadas no sistema de alerta de erros e falhas
  
  //VARIAVEIS PARA O RELOGIO   
  byte second = 0;    //0-59
  byte minute = 0;    //0-59
  byte hour = 0;      //0-23
  byte weekDay = 1;    //1-7
  byte monthDay = 1;  //1-31
  byte month = 1;     //1-12
  byte year  = 0;     //0-99
  
  IRrecv irrecv(emergencia);// CONTROLE REMOTO
  decode_results results;
  byte contR; //controleRemoto
  
  byte estadoAnteriorEmergencia;
  byte estadoAtualEmergencia;
  
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
    pinMode(emergencia , INPUT);  
    Wire.begin();
    RTC.begin();
    lcd.begin(16, 2);
    irrecv.enableIRIn(); // Start the receiver
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
    if(crAtivo == 0) estadoAnteriorEmergencia = digitalRead(emergencia);    
    estadoPistao = digitalRead(pistao);
    
    while(tempoFinal > millis()){
      
      byte estadoAtualChds = digitalRead(chds);
      byte estadoAtualChdi = digitalRead(chdi);
      byte estadoAtualChes = digitalRead(ches);
      byte estadoAtualChei = digitalRead(chei);
      
      byte estadoAtualBotao = digitalRead(botao);
      byte estadoAtualB2 = digitalRead(b2);
      byte estadoAtualB3 = digitalRead(b3);      
      if(crAtivo == 0) estadoAtualEmergencia = digitalRead(emergencia);      
  
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
      
      if(crAtivo == 1){
        if (irrecv.decode(&results)) {        
          contR = results.value;
          irrecv.resume(); // Receive the next value       
        }
        switch(contR){
          case 93:if(altTela == false)telaInicio = 0;
                   else telaInicio = 12; 
                   break;                   //power = Tela Inicio - Plantando - Transporte
          case 157:telaInicio = 5; break;   //modo = Tela Configuração
          case 29: telaInicio = 11; break;  //mute = Tela Parado
          case 221:; break;                 //  >||
          case 253:; break;                 //  |<<
          case 61:; break;                  //  >>|
          case 31:; break;                  // EQ = Botao Centro
          case 87:; break;                  // -  = Botao Decrementar
          case 111:; break;                 // +  = Botao Acrementar
          case 151:; break;                 //0
          case 103: telaInicio = 4; break;  //->~<- = Relogio
          case 79:; break;                  // u/sd
          case 207:; break;                 //1
          case 231:; break;                 //2
          case 133:; break;                 //3
          case 239:; break;                 //4
          case 199:; break;                 //5
          case 165:; break;                 //6
          case 189:; break;                 //7
          case 181:; break;                 //8
          case 173:; break;                 //9
          case 255:; break;                 //precionado
        }
      }
            
      //Botao central - confirmacao/altera tela de configuracao/paraliza alarmes
      if((estadoAtualBotao != estadoAnteriorBotao && estadoAtualBotao == LOW)|| contR == 31 ){
        switch(telaInicio){
           case 0:
             break;
           case 1:             
             break;
           case 2:
             telaInicio = 23;           
             break;
           case 3:
             telaInicio = 24;
             break;
           case 4:
             ajusteDataHoraPC();
             break;
           case 5:
             carregarDataHora();
             telaInicio = 6;
             break;
           case 6:
             telaInicio = 7;
             break;
           case 7:
             telaInicio = 8;
             break;
           case 8:
             telaInicio = 22;
             break;
           case 9:
             telaInicio = 17;        
             break;            
           case 10:
             telaInicio = 6;       
             break; 
           case 11:
             if(altTela == false)telaInicio = 0;
             else telaInicio = 12;       
             break; 
           case 12:
             telaInicio = 1;       
             break;             
           case 13:
             if(altTela == false)telaInicio = 0;
             else telaInicio = 12;
             //telaInicio = 12;   
             break;             
           case 14:
             telaInicio = 11;
             break;
           case 15:
             telaInicio = 11;
             break;
           case 16:       
             break;
           case 17:
             telaInicio = 18;       
             break;
           case 18:
             telaInicio = 19;       
             break;
           case 19:
            telaInicio = 20;       
             break;
           case 20:
             telaInicio = 21;       
             break;
           case 21:
             telaInicio = 25;       
             break;
           case 22:
             telaInicio = 9;
             break;
           case 23:
             telaInicio = 2;
             break;
           case 24:
             telaInicio = 3;
             break;          
           case 25:
             telaInicio = 10;        
             break;            
             
        }
        estadoAnteriorBotao = estadoAtualBotao;
      } 
      
      //Botao adicionar/acresentar/lado direito/avancar tela
       if((estadoAtualB2 != estadoAnteriorB2 && estadoAtualB2 == LOW) || contR == 111 ){
         switch(telaInicio){
           case 0:
             telaInicio = 1;
             break;
           case 1:
             telaInicio = 2;
             break;
           case 2:
             telaInicio = 16;
             break;
           case 3:
             telaInicio = 4;
             break;
           case 4:
             telaInicio = 5;
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
           case 10: //Salvar? SIM
             if(altTela == false){
               telaInicio = 0;
             }else telaInicio = 12;
             EEPROM.write(0, numeroLinha);
             EEPROM.write(1, espacamento);
             EEPROM.write(2, discoEnd);
             EEPROM.write(3,  velocidadeMaxima);
             setFloat(maximoSementeLinear, 4);
             EEPROM.write(6, crAtivo);
             setDateTime();             
             break;             
           case 11:
             //telaInicio = 0;
             break;
           case 12:       
             telaInicio = 1;
             break;             
           case 13:       
             break;             
           case 14:       
             break;
           case 15:      
             break;
           case 16:
             telaInicio = 3;       
             break;

           case 17:
             if(hour < 23){
               hour++;
             }       
             break;
           case 18:
             if(minute < 59 ){
               minute++;
             }       
             break;
           case 19:
             if(monthDay < 31){
               monthDay++;
             }       
             break;
           case 20:
             if(month < 12){
               month++;
             }       
             break;
           case 21:
             if(year < 99){
               year++;
             }       
             break;            
           case 22:
             if(maximoSementeLinear < 30){
               maximoSementeLinear = maximoSementeLinear + 0.25f;
             }
             break;
           case 23:       
             break;
           case 24:       
             break;          
           case 25:
             crAtivo = 1;        
             break;
         }
        estadoAnteriorB2 = estadoAtualB2;
      }
      
      //Botao decrementar/lado esquerdo/voltar tela
      if((estadoAtualB3 != estadoAnteriorB3 && estadoAtualB3 == LOW) || contR == 87 ){
        switch(telaInicio){
           case 0:
             break;
           case 1:
             if(altTela == false)telaInicio = 0;
             else telaInicio = 12;
             break;
           case 2:
             telaInicio = 1;
             break;
           case 3:
             telaInicio = 16;
             break;
           case 4:
             telaInicio = 3;
             break;
           case 5:
             telaInicio = 4;
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
           case 10://Salvar? NÃO
             numeroLinha = EEPROM.read(0);
             espacamento = EEPROM.read(1);
             discoEnd = EEPROM.read(2);
             velocidadeMaxima = EEPROM .read(3);
             maximoSementeLinear = getFloat(4);
             crAtivo = EEPROM.read(6);
             if(altTela == false)telaInicio = 0;
             else telaInicio = 12;
             break;
           case 11:       
             break; 
           case 12:       
             break;             
           case 13:       
             break;             
           case 14:       
             break;
           case 15:       
             break; 
           case 16:
             telaInicio = 2;       
             break;

           case 17:
             if(hour > 0){
               hour--;
             }       
             break;
           case 18:
             if(minute > 0 ){
               minute--;
             }       
             break;
           case 19:
             if(monthDay > 1){
               monthDay--;
             }       
             break;
           case 20:
             if(month > 1){
               month--;
             }       
             break;
           case 21:
             if(year > 13){
               year--;
             }       
             break;
           case 22:
             if(maximoSementeLinear > 1){
               maximoSementeLinear = maximoSementeLinear - 0.25f;
             }
             break;
           case 23:       
             break;
           case 24:       
             break;          
           case 25:
             crAtivo = 0;        
             break;
        }
        estadoAnteriorB3 = estadoAtualB3;
      }
       if(crAtivo == 0){ 
        if(estadoAtualEmergencia != estadoAnteriorEmergencia && estadoAtualEmergencia == LOW){
          alert(1);
          telaInicio = 13;
          estadoAnteriorEmergencia = estadoAtualEmergencia;
        }
       }
      contR = 0;
    }//fim while
                     /*******************************************************************************************************
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
        telaInicio = 4; //14        
      }
      if((test2 == 0 || test4 == 0) && telaInicio != 11){
        //sensores de semente parados        
        telaInicio = 4;
      }      
      if((test1 > 0 && test2 > 0 && test3 > 0 && test4 > 0) && telaInicio == 11){// caso a plantadeira esteja parada, ao movimentar-se entra no modo plantando
        telaInicio = 0;
      }

      
      kmh = ((2.5/20)*((test1 + test3) / 2))*3.6;          //calculo de km/h com base em SEGUNDOS
      if(kmh >  velocidadeMaxima){                         //teste de velocidade alta
        alert(1);
        telaInicio = 3;        
      }else if(kmh > 0 && kmh <= velocidadeMaxima){ 
        alert(0);
      }
      
      if(sementeLinear >  maximoSementeLinear){                         //teste de velocidade alta
        alert(1);
        telaInicio = 2;        
      }else if(sementeLinear > 0 && sementeLinear <= maximoSementeLinear){ 
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
    
    
      if(parcialDistanciaLinear > 50){//calcula o numero de sementes por metro linear a cada determinado periodo pra realizar a media/maior precisao
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
  
  void telaLCD(int aux){ //Metodo de escolha da TELA
    lcd.clear(); 
    switch(aux){
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("    PLANTANDO  ");
        lcd.setCursor(15, 0);
        lcd.write(3);
        lcd.setCursor(0, 1);
        lcd.print(alqueire, 1);
        lcd.print(" : ");
        lcd.print(sementeLinear, 1);
        lcd.print(" : ");
        lcd.print(kmh, 1); 
        alert(0);       
        break;
        
      case 1:
        lcd.setCursor(0, 0);
        lcd.write(2);
        lcd.print(" HECT. - ALQ.");
        lcd.setCursor(15, 0);
        lcd.write(3); 
        lcd.setCursor(0, 1);     
        lcd.print(hectare, 3);       
        lcd.setCursor(8, 1);
        lcd.print(alqueire, 3);
        alert(0);
        break;

      case 2:
        lcd.setCursor(0, 0);
        lcd.write(2);
        lcd.print("SEM.MET.LINEAR");
        lcd.setCursor(15, 0);
        lcd.write(3);
        lcd.setCursor(0, 1);
        lcd.print(sementeLinear, 2);
        lcd.setCursor(5, 1);
        lcd.write(4);
        if(sementeLinear > maximoSementeLinear){
          lcd.print(" ALTA");
        }else{ 
          lcd.print(" NORMAL");
        }
        break;

      case 3:
        lcd.setCursor(0, 0);
        lcd.write(2);
        lcd.print(" VELOCIDADE ");
        lcd.write(4);
        lcd.setCursor(15, 0);
        lcd.write(3);
        lcd.setCursor(0, 1);
        lcd.print(kmh, 1);
        lcd.print(" Km/h");
        if(kmh > velocidadeMaxima){
          lcd.print(" ALTA");
        }else{ 
          lcd.print(" NORMAL");
        }
        break;   
        
      case 4:
        getDataHora();
        alert(0);
        break;
      
        
      case 5:
        lcd.setCursor(0, 0);
        lcd.write(2);
        lcd.print(" CONFIGURACAO  ");      
        lcd.setCursor(7, 1);
        lcd.print("OK");
        alert(0);
        break;
     
      case 6:
        lcd.setCursor(0, 0);
        lcd.print("NUMERO DE LINHAS");
        lcd.setCursor(0, 1);
        lcd.print("-");
        lcd.setCursor(5, 1);
        lcd.write(4);
        lcd.setCursor(7, 1);
        lcd.print(numeroLinha);
        lcd.setCursor(15, 1);
        lcd.print("+");
        alert(0);
        break;
        
      case 7:
         lcd.setCursor(0, 0);
         lcd.print("ESPACAMENTO - Cm");
         lcd.setCursor(0, 1);
         lcd.print("-");
         lcd.setCursor(5, 1);
         lcd.write(4);
         lcd.setCursor(7, 1);
         lcd.print(espacamento);
         lcd.setCursor(15, 1);
         lcd.print("+");
         alert(0);
         break;
  
      case 8:
         lcd.setCursor(0, 0);
         lcd.print("NUN. FUROS DISCO");
         lcd.setCursor(0, 1);
         lcd.print("-");
         lcd.setCursor(5, 1);
         lcd.write(4);
         lcd.setCursor(7, 1);
         lcd.print(discoSemente[discoEnd]);
         lcd.setCursor(15, 1);
         lcd.print("+");
         alert(0);
         break;

     case 9:
       lcd.setCursor(1, 0);
       lcd.print("VELOC. MAXIMA");
       lcd.setCursor(0, 1);
       lcd.print("-");
       lcd.setCursor(5, 1);
       lcd.write(4);
       lcd.setCursor(7, 1);
       lcd.print(velocidadeMaxima);
       lcd.setCursor(15, 1);
       lcd.print("+");
       alert(0);
       break;
       
      case 10:
        lcd.setCursor(5, 0);
        lcd.print("SALVAR?");
        lcd.setCursor(0, 1);
        lcd.print("NAO");
        lcd.setCursor(7, 1);
        lcd.write(5);
        lcd.setCursor(13, 1);
        lcd.print("SIM");
        alert(0);
        break;
        
      case 11:
        lcd.setCursor(5, 0);
        lcd.print("PARADO");
        lcd.setCursor(7, 1);
        lcd.print("OK"); 
        alert(0);       
        break;
        
      case 12:
        lcd.setCursor(3, 0);
        lcd.print("TRANSPORTE");
        lcd.setCursor(15, 0);
        lcd.write(3);
        lcd.setCursor(7, 1);
        lcd.print("OK");
        alert(0);        
        break;
        
      case 13:
        lcd.setCursor(5, 0);
        lcd.print("ALERTA");
        lcd.setCursor(7, 1);
        lcd.print("OK");
        alert(1);        
        break;
      
      case 14:
        lcd.setCursor(2, 0);
        lcd.print("FALHA / ERRO");
        lcd.setCursor(7, 1);
        lcd.print("OK");
        alert(1);        
        break;
      
      case 15:
        lcd.setCursor(0, 0);
        lcd.print("DIFERENCA A != B");
        lcd.setCursor(7, 1);
        lcd.print("OK");
        alert(1);        
        break;

      case 16:
        lcd.setCursor(0, 0);
        lcd.write(2);
        lcd.print("POPUL. FINAL");
        lcd.setCursor(15, 0);
        lcd.write(3);
        lcd.setCursor(0, 1);
        lcd.print("ALQ: "); 
        lcd.print((sementeTotal/12.0/5.0*discoSemente[discoEnd]*numeroLinha), 2);
        alert(0);
        break;
        
      case 17:
        lcd.setCursor(0, 0);
        lcd.print("RELOGIO : HORA");
        lcd.setCursor(0, 1);
        lcd.print("-");
        lcd.setCursor(5, 1);
        lcd.write(4);
        lcd.setCursor(7, 1);
        lcd.print(hour);
        lcd.setCursor(15, 1);
        lcd.print("+");
        alert(0);
        break; 
        
      case 18:
        lcd.setCursor(0, 0);
        lcd.print("RELOGIO : MINUTO");
        lcd.setCursor(0, 1);
        lcd.print("-");
        lcd.setCursor(5, 1);
        lcd.write(4);
        lcd.setCursor(7, 1);
        lcd.print(minute);
        lcd.setCursor(15, 1);
        lcd.print("+");
        alert(0);
        break;
        
      case 19:
        lcd.setCursor(0, 0);
        lcd.print("RELOGIO : DIA");
        lcd.setCursor(0, 1);
        lcd.print("-");
        lcd.setCursor(5, 1);
        lcd.write(4);
        lcd.setCursor(7, 1);
        lcd.print(monthDay);
        lcd.setCursor(15, 1);
        lcd.print("+");
        alert(0);
        break;
        
      case 20:
        lcd.setCursor(0, 0);
        lcd.print("RELOGIO : MES");
        lcd.setCursor(0, 1);
        lcd.print("-");
        lcd.setCursor(5, 1);
        lcd.write(4);
        lcd.setCursor(7, 1);
        lcd.print(month);
        lcd.setCursor(15, 1);
        lcd.print("+");
        alert(0);
        break; 
        
      case 21:
        lcd.setCursor(0, 0);
        lcd.print("RELOGIO : ANO");
        lcd.setCursor(0, 1);
        lcd.print("-");
        lcd.setCursor(5, 1);
        lcd.write(4);
        lcd.setCursor(7, 1);
        lcd.print(year + 2000);
        lcd.setCursor(15, 1);
        lcd.print("+");
        alert(0);
        break;
        
       case 22:
        lcd.setCursor(0, 0);
        lcd.print("MAX. SEM. LINEAR");
        lcd.setCursor(0, 1);
        lcd.print("-");
        lcd.setCursor(3, 1);
        lcd.write(4);
        lcd.setCursor(5, 1);
        lcd.print(maximoSementeLinear, 2);
        lcd.setCursor(15, 1);
        lcd.print("+");
        alert(0);
        break;
      
      case 23: //somente visualizacao
        lcd.setCursor(0, 0);
        lcd.print("MAX. SEM. LINEAR");
        lcd.setCursor(3, 1);
        lcd.write(5);
        lcd.setCursor(5, 1);
        lcd.print(maximoSementeLinear, 2);
        alert(0);
        break;
        
      case 24:
        lcd.setCursor(1, 0);
        lcd.print("VELOC. MAXIMA");
        lcd.setCursor(5, 1);
        lcd.write(5);
        lcd.setCursor(7, 1);
        lcd.print(velocidadeMaxima);
        lcd.print(" Km/h");
        alert(0);
        break;
        
      case 25:
        lcd.setCursor(0, 0);
        lcd.print("CONTR.REMOTO.");
        lcd.print(crAtivo);
        lcd.setCursor(0, 1);
        lcd.print("DESAT");
        lcd.setCursor(7, 1);
        lcd.write(4);
        lcd.setCursor(10, 1);
        lcd.print("ATIVAR");
        if(crAtivo == 0){
          lcd.setCursor(5,1);
          lcd.write(2);
        }
        if(crAtivo == 1){
          lcd.setCursor(9,1);
          lcd.write(3);  
        }
        alert(0);
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
      case 2:
      
        break;
      case 3: //Biiiip
        digitalWrite(alerta, HIGH);
        delay(150);
        alert(0);
        break;
    }
//    for (int i = 0; i < 7; i++) {
//      tone(alerta, notas[i], 1000);
//      delay(1000);
//    }
//    delay(1000);
  }
     
  void telaApresentacao(){ // Tela de Inicio do Sistema
    alert(3);
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
      delay(300);
    }while(telaInicio < 19);
    lcd.clear();
    telaInicio = 0;
    alert(3);
  }
  
  void getDataHora(){ // Exibe a Data na Tela
     DateTime now = RTC.now();
      lcd.setCursor(0, 0);
      lcd.write(2);
      lcd.print("HORA:");
      lcd.print(now.hour());
      lcd.print(":");
      lcd.print(now.minute());
      lcd.print(":");
      lcd.print(now.second());
      
      lcd.setCursor(15, 0);
      lcd.write(3);
      lcd.setCursor(1, 1);
      lcd.print("DATA:");
      lcd.print(now.day());
      lcd.print("/");
      lcd.print(now.month());
      lcd.print("/");
      lcd.print(now.year());     
  }
  
  void setDateTime(){ //metodo para setar data e hora manualmente..o Year possui apenas 2 digitos
    byte zero = 0x00;
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.write(zero); //stop Oscillator 
    Wire.write(decToBcd(second));
    Wire.write(decToBcd(minute));
    Wire.write(decToBcd(hour));
    Wire.write(decToBcd(weekDay));
    Wire.write(decToBcd(monthDay));
    Wire.write(decToBcd(month));
    Wire.write(decToBcd(year));  
    Wire.write(zero); //start  
    Wire.endTransmission();
  }
  
  byte decToBcd(byte val){ //CONVERTE NUMERO DECIMAL PRA NUMERO BINARIO  - setdateTime();
    return ( (val/10*16) + (val%10) ); 
  }
  
  void ajusteRTC(){ //Se o RTC nao estive executando ele configura a hora da compilacao
      if (!RTC.isrunning()) { //encaso da hora estiver errada somente tirar o ponto de explamacao e compilar
          RTC.adjust(DateTime(__DATE__, __TIME__));
      }
  }
  
  void carregarDataHora(){//abastece as variaveis antes da configuração/alteração
     DateTime t = RTC.now();
     second = 00;
     minute = t.minute();   
     hour = t.hour();     
     monthDay = t.day();
     month = t.month();
     year  = t.year() - 2000;
  }
  
  void ajusteDataHoraPC(){ //altera a hora do RTC com base na ultima compilacao do codigo na IDE
          RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  
  void setFloat(float x,int p){ //X valor a ser salvo P posição na memoria(usa 2 byte) ... Seta um FLOAT na EEPROM  
    EEPROM.write (p,int(x)); 
    EEPROM.write (p+1,int((x-int(x))*100));
  }
  
  float getFloat(int p){ // Busca um Float na EEPROM
    return float(EEPROM.read(p))+ float(EEPROM.read(p+1))/100;
  }
  
  /*
  *ATENÇÃO - Revisar
  *Metodo Alerta
  *
  *
  */
