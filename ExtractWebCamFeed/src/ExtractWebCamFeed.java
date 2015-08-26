

import java.net.*;
import java.awt.image.BufferedImage;
import java.io.*;
import java.util.*;

import javax.imageio.ImageIO;
import javax.media.MediaLocator;

import com.bric.qt.io.JPEGMovWriter;

/**
 * 
 * Class to extract the public web cam feed  from either of the two  <br>
 * public web cameras at Iowa State University : West Lawns Web Cam & Memorial Union Web Cam <br>
 * Usage :  ExtractWebCamPoC <Web Cam Code> <Output File Path> <Image count> <br>
 * Web Cam Code: To select the web cam feed to extract <br>
 * &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;			1 : West Lawns Web Cam <br>
 * &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;			2 : Memorial Union Web Cam <br>
 * 
 * Output File Path : absolute path of the output file. It should be .mov <br>
 * 
 * Image Count : Defines length of the video. Basically the number of images that must be stitched together 
 * in the video. <br>
 * 
 * Format of MJPEG Stream is as follows :<br>
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;					--myboundary<br> 
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;					Content-Type: image/jpeg<br>
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;					Content-Length: 36232<br>
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;						&lt;newline&gt;<br>
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;					&lt;binary Data starts&gt;<br>
 * 
 * @author <a href="mailto:abhineet@iastate.edu">Abhineet Sharma</a> 
 * @author <a href="mailto:ghosh@iastate.edu">Sayantani Ghosh</a>
 * 
 */
public class ExtractWebCamFeed {
	
	/**
	 * Stores URL for West Lawns Live Web Cam 
	 */
	private static String urlWestLawns= "http://129.186.176.245/axis-cgi/mjpg/video.cgi?resolution=4CIF";
	/**
	 * Stores URL for Memorial Union Public Web Cam
	 */
	private static String urlMU= "http://129.186.47.239/axis-cgi/mjpg/video.cgi?resolution=4CIF";
	
	/**
	 * JPEG Movie Writer object : Stitches JPEG images into a mov file
	 */
	private static JPEGMovWriter objWriter = null;
	
	/**
	 * Image Width and Height : Hard Coded from the Web Cam's live feed
	 */
	private static final int IMAGE_HEIGHT = 480;
	private static final int IMAGE_WIDTH = 704;

	/**
	 * Maintains the count of images
	 */
	private static int imageCount = 0;
	//private static String imageName = "output/testImage";

	/**
	 * Private method to extract the image content length. 
	 * @param dis Reference to the http data stream object  
	 * @return the image length ( length in bytes that constitute the JPEG image )
	 */
	private static int readLine(DataInputStream dis){
		int len = -1;
		try{
			boolean end = false;
			String lineEnd = "\n"; //assumes that the end of the line is marked with this
			byte[] lineEndBytes = lineEnd.getBytes();
			byte[] byteBuf = new byte[lineEndBytes.length];
			ByteArrayOutputStream baos = new ByteArrayOutputStream();
			while(!end){
				dis.read(byteBuf,0,lineEndBytes.length);
				baos.write(byteBuf);
				String t = new String(byteBuf);
				//System.out.print(t); //uncomment if you want to see what the lines actually look like
				if(t.equals(lineEnd)) end=true;
			}
			String data = new String(baos.toByteArray());
			if(data.contains("Content-Length: ")){
				int index = data.indexOf("Content-Length: ") +"Content-Length: ".length();
				if(index >= 0){
					len = Integer.parseInt(data.substring(index, data.length() -2));
				}
			}
		}catch(Exception e){
			System.err.println("Error Processing the data input Stream : " +e.getMessage());
			return -1;
		}
		return len;

	}

	/**
	 * To skip some lines from the data stream to filter out the ascii part.
	 * @param noOfLines Number of lines to skip
	 * @param dis Reference to data stream object
	 * @return returns the length of image to be read.
	 */
	private static int skipLines(int noOfLines, DataInputStream dis){
		int len = -1;
		for(int i = 0; i < noOfLines; i++){
			int templen = readLine(dis);
			if(templen > 0){
				len = templen;
			}
		}
		return len;
	}
	
	/**
	 * Reads the embedded JPEG image by reading bytes specified by length from the Http data stream
	 * @param dis Reference to HTTP data stream object
	 * @param length the length of bytes to read from the data stream
	 */
	private static  void readJPG(DataInputStream dis, int length){  
		try{
			byte[] arrBytes= new byte[length];
			for(int i= 0; i<length;i++){
				arrBytes[i] = dis.readByte();
			}
			BufferedImage img = ImageIO.read(new ByteArrayInputStream(arrBytes));
			objWriter.addFrame(1.0f/4.0f, img, 1.0f);
			imageCount++;
			// Printing Image count for fun so that you don't get bored. :-P
			System.out.print(imageCount +"\t");
			if(imageCount % 10 == 0){
				System.out.println("");
			}
		}catch(Exception e){e.printStackTrace();}
	}

	/**
	 * Print Usage of the Class.
	 */
	private static void printUsage(){
		System.err.println("USAGE : java -jar ExtractWebCamFeed <Web Cam Code> <Output File Path> <Image Count>");
		System.err.println("Web Cam Code : \n\t1 : West Lawns Camera");
		System.err.println("\t2 : Memorial Union Camera Camera");
		System.err.println("Output File Path : to store the path of the file ( must be .mov)");
		System.err.println("Image Count : Specifies the length of the video ");
	}
	
	/**
	 * Our very own main method
	 * @param args 3 arguments : <web cam code> <output file Path> <image count>
	 */
	public static void main(String[] args) {
		System.out.println("Arguments : ");
		for(String arg : args){
			System.out.println("\t" +arg);
		}
		System.out.println("------------------");
		String outputFilePath = null;
		int imageLength, webCamCode;
		if(args.length != 3){
			printUsage();
			return;
		}else{
			try{
				webCamCode = Integer.parseInt(args[0]);
				if(webCamCode != 1 && webCamCode != 2){
					throw new IllegalArgumentException("Web cam code must be either 1 or 2");
				}else{
					//Good as of now. Go Ahead
				}
				
				outputFilePath = args[1];
				if(!outputFilePath.endsWith(".mov")){
					throw new IllegalArgumentException("Output file must end with .mov");
				}
				imageLength = Integer.parseInt(args[2]);
				
				if(imageLength <= 0){
					throw new IllegalArgumentException("Image length must be non zero & positive");
				}else{
					//Good as of now. Go Ahead
				}
				
			}catch(Exception e){
				System.err.println("Invalid Arguments Passed. Error : " +e.getMessage());
				printUsage();
				return;
			}
		}
		
		try {
			File outputFile = new File(outputFilePath);
			objWriter = new JPEGMovWriter(outputFile);
			System.out.println("Writing File to : " +outputFile.getAbsolutePath());
		} catch (IOException e) { 
			System.err.println("Error Creating file : " +e.getMessage());
			return;
		}
		while(true){
			try {
				if(imageCount >= imageLength){
					break;
				}
				URL webCamURL = null;
				switch(webCamCode){
				case 1:
					webCamURL = new URL(urlWestLawns);
					break;
				case 2:
					webCamURL = new URL(urlMU);
					break;
				default:
					//Code should never enter here
				}
				/*
				 * Format seems to be : 
						--myboundary
						Content-Type: image/jpeg
						Content-Length: 36232
						<newline>
						<binary Data starts>
				 * 
				 */
				HttpURLConnection webCamURLConn = (HttpURLConnection)webCamURL.openConnection();
				//BufferedReader in = new BufferedReader(new InputStreamReader(webCamURLConn.getInputStream()));
				BufferedInputStream in = new BufferedInputStream(webCamURLConn.getInputStream());
				DataInputStream dis = new DataInputStream(in);
				int length = skipLines(4,dis);
				readJPG(dis, length);
				webCamURLConn.disconnect();
				
				
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		Vector<String> inputFiles = new Vector<String>();

		try {

			for(String str : inputFiles){
				objWriter.addFrame(1.0f/30.0f, new File(str));
			}
			objWriter.close(true);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}

