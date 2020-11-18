const express = require("express");
const bodyParser = require("body-parser");
const find_user = require('bindings')('find_user')
  
const app = express();
  
// создаем парсер для данных application/x-www-form-urlencoded
const urlencodedParser = bodyParser.urlencoded({extended: false});
 
app.get("/", urlencodedParser, function (request, response) {
    response.sendFile(__dirname + "/checkUserForm.html");
});
app.post("/", urlencodedParser, function (request, response) {
    if(!request.body) return response.sendStatus(400);
    //console.log(request.body);
    var check = find_user.findUser(request.body.userName);
    //console.log(check);
    if(check) {
        response.send('Пользователь есть');
    } else {
        response.send('Такого пользователя нет');
    }
    //response.send(`${request.body.userName}`);
}); 


// web server
var port = process.env.PORT || 3000;
app.listen(port, function () {
	console.log('listening web server');
}); 
