
const serial_options = {baudRate: 9600};
const serial_port = "COM13";

const serialport = require('serialport');
let myPort = new serialport(serial_port, serial_options);


let Readline = serialport.parsers.Readline;
let parser = new Readline();
myPort.pipe(parser);

//Map callback for incomming data events on parser
myPort.on('open', showPortOpen);
parser.on('data', readSerialData);
myPort.on('close', showPortClose);
myPort.on('error', showError);


function showPortOpen() {
  console.log('port open. Data rate: ' + myPort.baudRate);
}

//Global variable to store incoming data
var read_data1 = "off";
var read_data2 = "off";

//Data från serialport (uart received)

function readSerialData(data) {
  console.log("Program ended");
  console.log('Data received: ', data)

  // First bit determine where to store data
  if (data[0] == "A") {
    read_data2 = data;  // This will be sent to one topic
  }
  if (data[0] == "B") {
    read_data1 = data;  //This will be sent to another topic
  }
}


function showPortClose() {
  console.log("Serial port closed. " + '\n' + 'Shutting down!' );
  process.exit(1);
}

function showError(error) {
  console.log('Serial port error: ' + error);
  console.log('Change current port in this script');
  console.log('Shutting down!');
  process.exit(1);
}


//Write first data to serialport
// text is globalvariable declare to receive data from mqtt
let timerVer = setInterval(sendSerialData, 900);
function sendSerialData() {

  console.log('send_serial: ' + text );
  myPort.write(text +  '\n');

}


//Second data to serialport
//// text2 is globalvariable declare to receive second data from mqtt
let timerVer1 = setInterval(sendSerialData2, 910);
function sendSerialData2() {
  console.log('send_serial2: ' + text2)
  myPort.write(text2 + '\n');
}

//************MQTT SETUP******************

// Default values if no data comes from mqtt
var text = "C";
var text2 =  "D";

//Global variable to store counting values
var count_var = 1;
var count_var2 = 1;

// Initiate mqtt setup requirement
let mqtt = require('mqtt');
let mqtt_server = "mqtt://130.239.163.210:1883";

connect_options = {
  clientId: "ok",
  username: "mikro2",
  password: "mAkRo",
  clean: true
};


let mqtt_client = mqtt.connect(mqtt_server, connect_options);
mqtt_client.on("connect", function() {
    console.log("connection: " + mqtt_client.connected)

})

  //*****************A topic to check if a car goes offline*****************
  const offline_check = function () {
       mqtt_client.publish('grupp4/range/check', count_var + '\n');
   }
   const timer_check = setInterval(offline_check, 1000);



  //******************Publish to car A***********************
 const publish_this_1 = function () {
      mqtt_client.publish('grupp4/direction/remote_2', read_data2 );
  }
  const timer_count1 = setInterval(publish_this_1, 250);



  //****************Publish to car B************************
    const publish_this = function () {
      mqtt_client.publish('grupp4/direction/remote_1', read_data1 + '\n');
  }
  const timer_count = setInterval(publish_this, 250);


  //Subscribe to multiple topics by format topic/sub_topic/#
  let topic_in = "grupp4/range/#";
  mqtt_client.subscribe(topic_in, {qos:1});

  //One connection to multiple topics
  mqtt_client.on("message", function(topic_in, message, packet) {

    //grupp4/range/check is always online when program executes
    if (topic_in == 'grupp4/range/check') {
      count_var++;
      count_var2++;

    }

    // If no new data is received after before count_var > 20
    if ((topic_in == 'grupp4/range/car_2') || (count_var > 20)) {
      console.log("topic is: " + topic_in);
      console.log("Message is: " + message);

      //Topic is offline if the statement evaluate true else send received message
      if ((message == "ANaN") || (count_var > 20)) {
        text = "C";
      }
      else {
        text = message;
      }
      count_var = 1;
    }

    // If no new data is received after 10s count_var > 20
    if ((topic_in == 'grupp4/range/car_1') || (count_var2 > 20)) {
      console.log("topic is: " + topic_in);
      console.log("Message is: " + message);

      //Topic is offline if the statement evaluate true else send received message
      if ((message == "ANaN") || (count_var2 > 20)) {
        text2 = "D";
      }
      else {
      text2 = message;
      }
      count_var2 = 1;
    }
  })


//
