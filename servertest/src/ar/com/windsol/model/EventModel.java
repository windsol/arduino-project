package ar.com.windsol.model;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicLong;

import ar.com.windsol.data.Event;

public class EventModel {
	
	static Map<Long, List<Event>> events = new HashMap<Long, List<Event>>();
	
	static AtomicLong sequence = new AtomicLong();
	
	public EventModel(){
	}
	
	public synchronized void addEvent(Event e){
		if ( !events.containsKey(1L)){
			events.put(1L, new ArrayList<Event>());
		}
		e.setId(sequence.incrementAndGet());
		events.get(1L).add(e);
	}
	
	public List<Event> getEvents(List<Long> originIds) {
		
		if ( originIds == null || originIds.isEmpty()){
			return getAll();
		}
		
		List<Event> ev = new ArrayList<Event>();
		for ( Long id : originIds){
			if ( events.containsKey(id)){
				List<Event> e = events.get(id); 
				ev.addAll(e);
			}
		}
		
		return ev;
	}
	
	public List<Event> getAll() {
		
		List<Event> ev = new ArrayList<Event>();
		for ( Long id : events.keySet()){
			List<Event> e = events.get(id);
			ev.addAll(e);
		}
		
		return ev;
	}

}
