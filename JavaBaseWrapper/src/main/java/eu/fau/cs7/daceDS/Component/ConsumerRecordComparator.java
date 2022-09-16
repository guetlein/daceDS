package eu.fau.cs7.daceDS.Component;

import java.util.Comparator;

import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.kafka.common.header.Header;

public class ConsumerRecordComparator implements Comparator<ConsumerRecord> {

	//prototypical! might need adjustments for the middleware and other implementations
	public int compare(ConsumerRecord a, ConsumerRecord b) {
		if(getTime(a)!=getTime(b)) {
			return (int) (getTime(a)-getTime(b));
		}
		if(getEpoch(a)!=getEpoch(b)) {
			return getEpoch(a)-getEpoch(b);
		}
		if(!a.topic().equals(b.topic())) {
			return a.topic().hashCode()-b.topic().hashCode();
		}
		if(!a.key().equals(b.key())) {
			return a.key().hashCode()-a.key().hashCode();
		}
		if(getSender(a)!=getSender(b)) {
			return getSender(a).hashCode()-getSender(b).hashCode();
		}
		return 0;
	}
	public long getTime(ConsumerRecord record) {
		Header h = record.headers().lastHeader("time");
		if(h== null) {
			return -1;
		}
		return ScenarioUtils.bytesToLong(h.value());
	}

	public int getEpoch(ConsumerRecord record) {		
		Header h = record.headers().lastHeader("epoch");
		if(h== null) {
			return 0;
		}
		return ScenarioUtils.bytesToInt(h.value());
	}
	public String getSender(ConsumerRecord record) {		
		Header h = record.headers().lastHeader("sender");
		if(h== null) {
			return "";
		}
		return h.value().toString();
	}
}