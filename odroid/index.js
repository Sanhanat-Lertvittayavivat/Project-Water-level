const XMLHttpRequest = require('xhr2');
const express = require("express");
const app = express();
const lineNotify = require('line-notify-nodejs')('Rz6mjV6NgVby5f8X3jOyX66x2jxPooimPbuA5tGjcNH');


var a = 10;

function SendTP(water) {
  
  fetch(`https://api.thingspeak.com/update?api_key=W9X6HZ2SEW1UOUUW&field1=${water}`).then(() => console.log('Send Complete.'))

}





app.use(express.json())

const notify = (message) => {
    lineNotify.notify({
        message,
    }).then(() => {
        console.log('send completed!');
    });
}

app.post("/line", (req, res) => {
    notify(req.body.text);
    res.send({ message: "OK" });
})

app.post("/water", (req, res) => {
    SendTP(req.body.text);
    res.send({ message: "OK" });
})

app.listen(3000, () => {
    console.log("Server is running...")
})
