# DMD_Home
Automação residencial com base na plataforma OpenHab por meio do MQTT.

Alterado arquivo 
C:\Users\DallasNB\.platformio\packages\framework-arduinoespressif8266\libraries\ESP8266WiFi\src\include\ClientContext.h

linha 85 - incluida linha > tcp_abort(_pcb);

            tcp_arg(_pcb, NULL);
            tcp_sent(_pcb, NULL);
            tcp_recv(_pcb, NULL);
            tcp_err(_pcb, NULL);
            tcp_poll(_pcb, NULL, 0);
            err = tcp_close(_pcb);
            //DMD
            tcp_abort(_pcb); //Custom modification
            //DMD
            if(err != ERR_OK) {
                DEBUGV(":tc err %d\r\n", (int) err);
                tcp_abort(_pcb);
                err = ERR_ABRT;
            }