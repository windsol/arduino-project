package ar.com.windsol.data;

public class Event {

	// eventid=1,productid=1,type=1,
	
	private boolean processed;
	private Long id;
	private String productId;
	private String destination;
	// deliver product
	private String eventType = "1";
	
	public Event(Long id, String productId) {
		this.id = id;
		this.productId = productId;
	}
	
	public Event(String productId) {
		this(null, productId);
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

	
	
	public String getProductId() {
		return productId;
	}

	public void setProductId(String productId) {
		this.productId = productId;
	}

	public String getDestination() {
		return destination;
	}

	public void setDestination(String destination) {
		this.destination = destination;
	}
	
	

	public boolean isProcessed() {
		return processed;
	}

	public void setProcessed(boolean processed) {
		this.processed = processed;
	}

	@Override
	public String toString() {
		return "eventid=" + id + ",productid=" + productId 
				+ ",dest=" + destination 
				+ ",type=" + eventType;
	}
}
