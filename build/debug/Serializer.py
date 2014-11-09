import serial

ser = serial.Serial()

class Serializer:
	def _init_(self):
		self.baudrate = 9600
		self.comPort = None
		
	def createSerialPort(self):
		try:
			ser = serial.Serial(self.comPort, self.baudrate)
		except Exception,e: print "createSerialPort error: ", str(e)
		
	def setBaudRate(self, baudrate):
		self.baudrate = baudrate
		if (ser != None):
			ser.baudrate = baudrate
		
	def setComPort(self, comPort):
		self.comPort = comPort
		if (ser != None):
			ser.port = comPort
		
	def createConnection(self):
		try:
			ser = serial.Serial(self.comPort,self.baudrate)
		except Exception,e: print "createConnection error: ", str(e)
		
	def openConnection(self):
		ser.open()
		
	def closeConnection(self):
		ser.close()
		
	def sendData(self, b0, b1):
		try:
			print 'Sending ', hex(b0), 'and', hex(b1)
			s = str(unichr(b0))
			r = str(unichr(b1))
			ser.write(s+r)
		except Exception,e: print "sendData error: ", str(e)
			