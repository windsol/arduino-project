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
		e.setDestination("1");
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
				for (Event event : e){
					if ( !event.isProcessed()){
						ev.add(event);
					}
				}
			}
		}
		
		return ev;
	}
	
	public List<Event> getAll() {
		
		List<Event> ev = new ArrayList<Event>();
		for ( Long id : events.keySet()){
			List<Event> e = events.get(id);
			for (Event event : e){
				if ( !event.isProcessed()){
					ev.add(event);
				}
			}
			
		}
		
		return ev;
	}

	public boolean remove(String origin, String eventId) {
		boolean found = false;
		if ( events.containsKey(Long.parseLong(origin))){
			List<Event> evs = events.get(Long.parseLong(origin));
			for (Event e : evs){
				if (e.getId().equals(Long.parseLong(eventId))){
					if ( !e.isProcessed()){
						e.setProcessed(true);
						found = true;
					}
					break;
				}
			}
			
		}
		
		return found;
		
	}

}
