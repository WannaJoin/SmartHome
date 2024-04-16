const WebSocket = require('ws');
const fs = require("fs");

const espAquaIp = "10.8.0.185"

let waterTemp = [];
loadDataFiles();


//        MAIN WEBSOCKET CODE

const wss = new WebSocket.Server({ port: 8080 });

wss.on('listening', () => {
  const address = wss.address();
  console.log(`WebSocket server is listening on ${address.address}:${address.port}`);
});

wss.on('connection', function connection(ws, req) {
  //Log the IP of the connection for further verification
  const deviceIp = extractIPv4(req.socket.remoteAddress);
  console.log(`Client connected from IP: ${deviceIp}`);

  ws.on('message', (message) => {    
    // Handle the message received
    let parsedMessage = parseMessage(message);
    if (deviceIp === "espAquaIp"){
      savewaterTemp(parsedMessage);
    }
  });

  ws.on('close', function close() {
    console.log('Client disconnected');
  });
});

////////////////////


//    HANDLERS AND OTHER FUNCTIONS

function loadDataFiles(){
  //Load the temperature list to the global list
  fs.readFile("data/waterTemp.json", (error, data) => {
    if (data != 0){
      waterTemp = JSON.parse(data);
    }
  });
}
  
function parseMessage(message){
  //Safely convert the message received and return it
  let parsedMessage;
  try {
    parsedMessage = JSON.parse(message);
  } catch (error) {
    console.error('Error parsing JSON message:', error);
    return;
  }
  return parsedMessage;
}

function savewaterTemp(push){
  //Push the value to the current list and log it.
  waterTemp.push(push);
  // console.log("New updated list: \n");
  // waterTemp.forEach((x) => console.log(x));
  // console.log("\n\n");

  fs.writeFile("data/waterTemp.json", JSON.stringify(waterTemp), (error) => {
    if (error) {
      console.error(error);
      throw error;
    }
  });
};

function extractIPv4(ip) {
  if (ip.includes('::ffff:')) {
    return ip.split('::ffff:')[1];
  } else {
    return ip;
  }
}