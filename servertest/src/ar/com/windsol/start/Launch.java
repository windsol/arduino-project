package ar.com.windsol.start;

import org.apache.log4j.PropertyConfigurator;

public class Launch {

	private static final String loggerFile = "config/log4j.properties";
	
	public static void initLogging(){
		PropertyConfigurator.configure(loggerFile);
	}
		
	public static void main(String[] args) {
		initLogging();
		
		AppServer server = new AppServer();
		server.start();
	}

}
