// 모듈 추출
var http = require ('http');
var express = require ('express');
var bodyParser = require('body-parser');
var cookieParser = require("cookie-parser");

function setRouter(app, handle){
    for(var ix in handle){
        switch (handle[ix].method){
            case 'get' : app.get(handle[ix].path, handle[ix].callback);
                break;
            case 'post' : app.post(handle[ix].path, handle[ix].callback);
                break;
            case 'put' : app.put(handle[ix].path, handle[ix].callback);
                break;
            case 'delete' : app.delete(handle[ix].path, handle[ix].callback);
                break;

        }
    }
}   

// 서버 시작함수
function serverStart(handle){

    // 서버 생성
    var app = express();

    // 미들웨어 설정.
    app.use(bodyParser.urlencoded({extended : false}));
    app.use(cookieParser());
    // 라우터 설정.
    setRouter(app, handle);

    // 포트 설정.
    var port = process.env.port || 8000;

    // 서버 실행.
    http.createServer(app).listen(port);
    console.log('서버 실행중');
}

module.exports={
    serverStart : serverStart
}