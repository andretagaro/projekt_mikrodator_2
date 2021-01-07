const serialport = require('serialport')
const serial_options={ baudRate: 9600 };
const serial_port="COM5"; 
const myPort = new serialport(serial_port, serial_options);

const mqtt= require('mqtt');
const client = mqtt.connect("mqtt://130.239.163.210", {clientId:"", username:"mikro2", password:"mAkRo"});

let Readline = serialport.parsers.Readline;
let parser = new Readline();
myPort.pipe(parser);

/*Funktionsdeklarioner*/ 

const publishThis = function(data) {

    console.log(`Data is ${data}`);
    client.publish("grupp4/range/remote_2", data);


}

parser.on('data', publishThis);