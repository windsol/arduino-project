package ar.com.windsol.app.servlet;

import java.io.IOException;
import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.List;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.log4j.Logger;

import ar.com.windsol.data.Event;
import ar.com.windsol.model.EventModel;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;

public class AppServlet extends HttpServlet {

	static class Result{
		String status;

		@Override
		public String toString() {
			return "status=" + status;
		}
		
		
		
	}
	
	private static final String
	    ACTION 	= "a",   // action param 
	    ADD 	= "a",
		QUERY 	= "q",   // query for events
		UPDATE 	= "u";	 // update 
		
	private static final String 
		PARAM_EVENT = "e",
		PARAM_ID    = "id",
		PARAM_RESULT = "r",
		PARAM_MESSAGE = "m",
		RESULT_OK = "ok",
		PARAM_NG = "ng";
	
	private static final Logger logger = Logger.getLogger(AppServlet.class);
	
    private static final Gson gson;
    
    static {
        GsonBuilder builder = new GsonBuilder();
        gson = builder.create();
    }

	
	@Override
	protected void doGet(HttpServletRequest req, HttpServletResponse resp)
			throws ServletException, IOException {
		doWork(req, resp);
	}

	@Override
	protected void doPost(HttpServletRequest req, HttpServletResponse resp)
			throws ServletException, IOException {
		doWork(req, resp);
	}
	
	
	private void doWork( HttpServletRequest req, HttpServletResponse resp ) throws IOException{

		Object result = null;
		String json = "";
		try {
			result = executeAction(req);
			
			//json = gson.toJson(result);

		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			result = "error";
		}

        resp.setContentType("text/plain");
        resp.setCharacterEncoding("UTF-8");
        resp.getWriter().print(result);

        logger.debug(result);
	}
	
	private List<Long> buildParamList(String paramIds){
		if ( paramIds == null){
			return null;
		}
		String[] ids = paramIds.split(",");

		List<Long> result = new ArrayList<Long>();
		
		for (String id : ids){
		
			result.add(Long.parseLong(id));
		
		}
		
		return result;
	
	}
	
	
	private Object executeAction(HttpServletRequest req) throws Exception {
		Object result = null;
		
		String action = req.getParameter(ACTION);
		
		if ( action == null ){
			throw new Exception ("Invalid action: null");
		}
		if (action.equals(ADD)){
			result = processAddEvent(req.getParameter(PARAM_EVENT));
		}
		else if (action.equals(QUERY)){
			result = processGetEvents(req.getParameter(PARAM_ID));
		}
		else if (action.equals(UPDATE)){
			result = processUpdate(req);
		}
		else {
			throw new Exception ("Unsuported action: " + action);
		}
		
		return result;
	}
	
	private String processUpdate(HttpServletRequest req) {
		String origin = req.getParameter(PARAM_ID);
		String eventId = req.getParameter(PARAM_EVENT);
		String result = req.getParameter(PARAM_RESULT);
		logger.debug("processUpdate ID: " +origin + ", eventId: " + eventId +", result:" +result);
		
		
		boolean r = false;
		if ( RESULT_OK.equals(result)){
			r = new EventModel().remove(origin, eventId);
		}
		
		
		return r ? "ok" : "not found";
	}

	private String processGetEvents(String id) {
		logger.debug("GetEvents ID: " +id);
		StringBuilder result = new StringBuilder();
		List<Long> ids = buildParamList(id);
		
		List<Event> events = new EventModel().getEvents(ids);
		
		for (Event event : events){
			result.append(event).append(";");
		}
		String res = result.toString();
		if (res.length() > 0){
			res = res.substring(0, res.length() -1);
		}
		
		logger.debug("GetEvents result: " +res);
		return res;
	}
	
	private String processAddEvent(String event){
		logger.debug("AddEvent: " +event);
		Result r = new Result();
		
		new EventModel().addEvent(new Event(event));
		r.status = "OK";
		return r.toString();
	}



}
