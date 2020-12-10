const serialport = require('serialport')
const serial_options={ baudRate: 9600 }; //Ange ditt systems baudrate
const serial_port="COM7"; //Ange dutt systems COM-port
const myPort = new serialport(serial_port, serial_options);

const mqtt= require('mqtt');
/* Nån som vet vad clientId är till för? Jag skrev bara in nåt*/
const client = mqtt.connect("mqtt://130.239.163.210", {clientId:"", username:"mikro2", password:"mAkRo"});

/*En global variabel jag använde för att testa funktionen*/

let i = 1510;

/*Funktionsdeklarion*/ 

const sendThis = function(){
    client.on('message', (topic, message) =>
    {
        if(topic === 'grupp4/direction/remote_1')
        {
            myPort.write(message);
        };
    })

    /*Öka värdet på i, återställ till 1510 vid 2010*/
    /*Detta är var bara ett test för att verifiera att det funkade*/

    i += 100;
    if(i === 2010) i = 1510;
    
    /* `${}`, gör om varibeln den omsluter till en sträng, vilket publishfunktionen vill ha*/
    client.publish('grupp4/direction/remote_1', `${i}`);

}

/*Program start*/
/*Från rad börjar programmet sin "main-loop"*/

/*Börja prenumerera på den data du vill ha*/ 
client.on('connect', () =>
{
    client.subscribe('grupp4/direction/remote_1')
    /*prenumerar på mer
    ...
    ... */
})

/*Ange hur ofta vi ska hoppa in i funktionen som skickar
och tar emot data via MQTT, i detta fall 3000ms*/
const updateTimer1 = setInterval(sendThis, 3000);
