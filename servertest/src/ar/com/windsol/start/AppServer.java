package ar.com.windsol.start;

import org.apache.log4j.Logger;
import org.eclipse.jetty.server.Server;
import org.eclipse.jetty.webapp.WebAppContext;

public class AppServer {

	private static final String WAR_DIR = "war/";
	private static final int PORT = 9090;
	private static final Logger logger = Logger.getLogger(AppServer.class);
	
	public AppServer(){
		
	}
	
	public void start(){
		logger.debug("Starting server");
		
        Server server = new Server(PORT);
        
        WebAppContext context = new WebAppContext();
        context.setDescriptor( WAR_DIR + "/WEB-INF/web.xml");
        context.setResourceBase(WAR_DIR);
        
        context.setContextPath("/");
        context.setParentLoaderPriority(true);
 
        server.setHandler(context);
        
        try {
			server.start();

			logger.debug("Server started and listening on port " + PORT);

			server.join();
			
			
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
        
        

	}
	
}
