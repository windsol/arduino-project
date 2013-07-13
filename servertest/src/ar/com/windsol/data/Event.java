package ar.com.windsol.data;

public class Event {

	private Long id;
	private String eventType;
	
	public Event(Long id, String eventType) {
		this.id = id;
		this.eventType = eventType;
	}
	
	public Event(String eventType) {
		this(null, eventType);
	}
	
	public Long getId() {
		return id;
	}
	public void setId(Long id) {
		this.id = id;
	}
	public String getEventType() {
		return eventType;
	}
	public void setEventType(String eventType) {
		this.eventType = eventType;
	}
	
	
	
}
