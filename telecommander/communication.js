/**
 * Module dependencies
 **/
var EventEmitter = require('events').EventEmitter;
var util = require('util');
var async = require('async');
var serialport = require('serialport');
var log = require('node-logging');

/**
 * Export the constructor
 **/
module.exports = Communication;

/**
 * Inherits from EventEmitter
 **/
util.inherits(Communication, EventEmitter);

/**
 * Pug constructor
 * Connect to the serialport and bind
 *
 * Options :
 *  - baudrate (default: 9600) Need change for bluetooth connection (115200)
 **/
function Communication(options) {
  // Inherit EventEmitter
  EventEmitter.call(this);

  // Options
  options = options || {};
  this.baudrate = options.baudrate || 9600;

  // Auto connect
  var self = this;
  this.connect(function (err, serial) {
    if (err) {
      throw new Error(err);
    } else {
      self.serial = serial;
      self.emit('ready');
    }
  });
}

/**
 * Connect Pug
 *
 * @callback {Error} err
 * @callback {serialport.SerialPort} found
 * @api private
 **/
Communication.prototype.connect = function (callback) {
  log.inf('Attempting to find Pug ...')
  var self = this;
  var err = null,
    serialCandidate, timeOutId;

  // List & test serials ports availables
  serialport.list(function (err, ports) {

    async.eachSeries(ports, function (port, next) {
      // Try to open serial connection
      log.dbg('[' + port.comName + '] Try');
      serialCandidate = new serialport.SerialPort(
        port.comName, {
        baudrate: self.baudrate,
        parser: serialport.parsers.readline('\n')
      });

      // Timeout (before because we must clear it if we find something)
      timeOutId = setTimeout(function () {
        log.dbg('[' + port.comName + '] Timeout');
        next(); // Next port (TimeOut)
      }, 500);

      // Serial connection open
      serialCandidate.on('open', function () {
        log.dbg('[' + port.comName + '] Open');
        // Send a ping
        serialCandidate.write('V\n');

        // Receive a pong ?
        serialCandidate.on('data', function (data) {
          log.dbg('[' + port.comName + '] Data : ' + data.toString());
          if (/PUG/i.test(data.toString())) {
            clearTimeout(timeOutId);
            log.inf('Pug is connected on ' + port.comName);
            next(serialCandidate); // Finish !
          } else {
            clearTimeout(timeOutId);
            log.dbg("[" + port.comName + "] I'm not Pug");
            next(); // Next port
          }
        });
      });

    }, function (found) {
      if (!found) {
        err = new Error('Pug not found');
      }
      // Remove ping/pong listeners
      if (serialCandidate)
        serialCandidate.removeAllListeners();

      // Send result into a callback
      callback(err, found);
    });
  });
}

/**
 * Send a command
 *
 * @param {String} command
 * @api private
 **/
Communication.prototype.write = function (command) {
  this.serial.write(command + '\n');
  log.dbg('Send: ' + command);
}