var express = require('express');
var path = require('path');
var favicon = require('serve-favicon');
var logger = require('morgan');
var cookieParser = require('cookie-parser');
var bodyParser = require('body-parser');
var qs = require('querystring');

var routes = require('./routes/index');
var users = require('./routes/users');

var app = express();

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'jade');

// uncomment after placing your favicon in /public
//app.use(favicon(path.join(__dirname, 'public', 'favicon.ico')));
app.use(logger('dev'));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', routes);
app.use('/users', users);

// catch 404 and forward to error handler
// app.use(function(req, res, next) {
//   var err = new Error('Not Found');
//   err.status = 404;
//   next(err);
// });

// error handlers

// development error handler
// will print stacktrace
if (app.get('env') === 'development') {
  app.use(function(err, req, res, next) {
    res.status(err.status || 500);
    res.render('error', {
      message: err.message,
      error: err
    });
  });
}

// production error handler
// no stacktraces leaked to user
app.use(function(err, req, res, next) {
  res.status(err.status || 500);
  res.render('error', {
    message: err.message,
    error: {}
  });
});

app.get('/', function (req, res) {
  res.send('Hello World!');
  console.log("Got a get request = " + req.url);
});

app.post('/signup', function(req,res){
  console.log("Hella wtf");
  console.log("Request body " + req.body);
  console.log(qs.stringify(req.body));
  // res.redirect('pebblejs://close#success?' + qs.stringify(req.body));
  // res.redirect('http://return_to#' + qs.stringify(req.body));
  // console.log("URL LOCATION = " + window.url.location);
  res.redirect('pebblejs://close#' + qs.stringify(req.body));

  // console.log("Original URL = " + req.originalUrl);
  // console.log("Request body " + req.body);
  // alert(req.body);

  // var str = JSON.stringify(req);
  // var fullUrl = req.protocol + '://' + req.get('host') + req.originalUrl;

  // console.log("Full URL = " + fullUrl);


  // var query = req.url.search.substring(1);
  // console.log("query = " + query);
  // var vars = query.split("&");
  // console.log("vars = " + vars)
  // var doubleTrouble = vars[0].split("=");

  // var neededURL = doubleTrouble[1];
  // console.log("needed url = " + neededURL);
  // // res.redirect('http://www.google.com?' + qs.stringify(req.body));

  // res.redirect(neededURL + '?' + qs.stringify(req.body));


  // res.render('redirect');
  return;
})


module.exports = app;
