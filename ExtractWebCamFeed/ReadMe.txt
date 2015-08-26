WebCamFeedExtractor
-----------------------
	A software utility that reads a public webcam feed, an MJPEG ( Motion - JPEG ) format and converts it into videos that can be saved on a local hard drive. This serves as data acquisition software for our main project - 'The Eye'. It uses two libraries 
		1. JMF : Java Media Framework ( to process the JPEGs)
		2. MovWriter : To join all the JPEGs into a video
		
	This one is specifically designed to extract the public webcam feeds at Iowa State University as it is tightly coupled with the format and not tested on other feeds. But it should work with any MJPEG format with slight modifications, if not without any.
	
