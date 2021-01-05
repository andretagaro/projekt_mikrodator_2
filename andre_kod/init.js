load('api_timer.js');
load('api_uart.js');
load('api_sys.js');
load('api_mqtt.js');

let uartNo = 1;

UART.setConfig(uartNo, {
  baudRate: 9600,
  esp32: {
    gpio: {
      rx: 16,
      tx: 17,
    },
  },
});

/*Subscribe, handler = UART.write*/

MQTT.sub('grupp4/direction/remote_1', function(conn, topic, msg) {
  UART.write(
    uartNo,
    msg
  );
}, null);

/*UART receive, handler = mqtt.pub*/

UART.setDispatcher(uartNo, function(uartNo) {
  let dataAmount = UART.readAvail(uartNo);
  if (dataAmount > 0)
  {
    let receivedData = UART.read(uartNo);
    MQTT.pub('grupp4/range/car_1', receivedData, 1);
  }
}, null);

UART.setRxEnabled(uartNo, true);


