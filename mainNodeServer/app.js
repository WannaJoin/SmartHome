const WebSocket = require('ws');

const wss = new WebSocket.Server({ port: 8080 });

const tempSaveArr = [];

wss.on('connection', function connection(ws) {

  ws.on('message', function incoming(message) {    
    let parsedMessage;
    try {
      parsedMessage = JSON.parse(message);
    } catch (error) {
      console.error('Error parsing JSON message:', error);
      return;}
      
  });

  ws.on('close', function close() {
    console.log('Client disconnected');
  });
});
