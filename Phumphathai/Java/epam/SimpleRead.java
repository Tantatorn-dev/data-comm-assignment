package com.epam;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Enumeration;

import java.util.concurrent.TimeUnit;

import javax.comm.CommPortIdentifier;
import javax.comm.PortInUseException;
import javax.comm.SerialPort;
import javax.comm.UnsupportedCommOperationException;

public class SimpleRead {
	private static final  char[]COMMAND = {'*', 'R', 'D', 'Y', '*'};
	private static final int WIDTH = 160; //640;
    private static final int HEIGHT = 120; //480;
    	
    private static CommPortIdentifier portId;
    InputStream inputStream;
	OutputStream outputStream;
    SerialPort serialPort;

    public static void main(String[] args) {
    	 Enumeration portList = CommPortIdentifier.getPortIdentifiers();

        while (portList.hasMoreElements()) {
        	portId = (CommPortIdentifier) portList.nextElement();
            if (portId.getPortType() == CommPortIdentifier.PORT_SERIAL) {
            	System.out.println("Port name: " + portId.getName());
                if (portId.getName().equals("COM3")) {
                	SimpleRead reader = new SimpleRead();
                }
            }
        }
    }

    public SimpleRead() {
       	int[][]rgb = new int[HEIGHT][WIDTH];
       	int[][]rgb2 = new int[WIDTH][HEIGHT];
    	
    	try {
            serialPort = (SerialPort) portId.open("SimpleReadApp", 1000);
            inputStream = serialPort.getInputStream();
			outputStream = serialPort.getOutputStream();

            serialPort.setSerialPortParams(1000000,
                SerialPort.DATABITS_8,
                SerialPort.STOPBITS_1,
                SerialPort.PARITY_NONE);

        	int counter = 0;
			
			byte[] b = {'h'};

        	while(true) {
        		System.out.println("Looking for image");
				
				
        		while(!isImageStart(inputStream, 0)){};
        	
	        	System.out.println("Found image: " + counter);
	        	
	        	for (int y = 0; y < HEIGHT; y++) {
	        		for (int x = 0; x < WIDTH; x++) {
		       			int temp = read(inputStream);
		    			rgb[y][x] = ((temp&0xFF) << 16) | ((temp&0xFF) << 8) | (temp&0xFF);
	        		}
	        	}
				
				System.out.println("--");
	        	
	        	for (int y = 0; y < HEIGHT; y++) {
		        	for (int x = 0; x < WIDTH; x++) {
		        		rgb2[x][y]=rgb[y][x];
		        	}	        		
	        	}
	        	
		        BMP bmp = new BMP();
	      		bmp.saveBMP("c:/out/" + (counter++) + ".bmp", rgb2);
	      		
	      		System.out.println("Saved image: " + counter);
				
				TimeUnit.MILLISECONDS.sleep(10);
				outputStream.write(b);
        	}
		} catch (Exception e) {
			e.printStackTrace();
		}
    }
    
    private int read(InputStream inputStream) throws IOException {
    	int temp = (char) inputStream.read();
		if (temp == -1) {
			throw new  IllegalStateException("Exit");
		}
		return temp;
    }
    	
    private boolean isImageStart(InputStream inputStream, int index) throws IOException {
    	if (index < COMMAND.length) {
    		if (COMMAND[index] == read(inputStream)) {
    			return isImageStart(inputStream, ++index);
    		} else {
    			return false;
    		}
    	}
    	return true;
    }
}