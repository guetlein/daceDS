package eu.fau.cs7.daceDS.Translators;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.stream.Collectors;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.xpath.XPath;
import javax.xml.xpath.XPathConstants;
import javax.xml.xpath.XPathExpression;
import javax.xml.xpath.XPathExpressionException;
import javax.xml.xpath.XPathFactory;

import org.apache.log4j.Logger;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import eu.fau.cs7.daceDS.Component.DomainHelper;
import eu.fau.cs7.daceDS.datamodel.Macro;
import eu.fau.cs7.daceDS.datamodel.Meso;

public class DomainHelperTraffic extends DomainHelper {

	static Logger logger = Logger.getLogger("DomainHelperTraffic");

	List<String> outgoingLinks = new ArrayList<String>();
	
	XPath xpath;
	private Document mesoMap = null;
	private Document domainReference = null;
	private Document microMap = null;
	
	public DomainHelperTraffic(List<TrafficReference> references, List<CharSequence> responsibilities){
		
		this.setExternalResponsibilities(responsibilities.stream()
	                .map(String::valueOf)
	                .collect(Collectors.toList())); //nodes
		
	
	}
	
	
	boolean isResponsible(Meso meso) {
		//more complicated example for numbers, here we can use directly the embedded info
		return internalResponsibilities.contains(meso.getLink().toString());
	}	
	boolean isResponsible(Macro macro) {
		//we might not now the macro "linkid" but only start end desintation node, therefore comparison looks like this:
		String ml = macro.getLinkID().toString();
		String mls = getMacroStartNode(ml);
		String mld = getMacroDestinationNode(ml);
		
		for (String internalResponsibility : internalResponsibilities) {
			
			String from = getMacroStartNode(internalResponsibility);
			String to = getMacroDestinationNode(internalResponsibility);
			
			if ( from.equals(mls) && to.equals(mld)) {
				return true;
			}
		}
		return false;
	}
	
	private String getMacroDestinationNode(String link) {
		String[] tmp = {"-1"};
		
		try {
			tmp = link.split("_");
			if(tmp.length==3) {
				return tmp[2];
				}
				else if (tmp.length==2) {
					return tmp[1];
				}
				else {
				System.out.println("failed to to getMacroStartNode for "+link);
				return "";
			}
		} catch (Exception e) {
			System.out.println("tried to getMacroDestinationNode for "+link);
			System.out.println(tmp);
			e.printStackTrace();
		}
		return "";
		
	}



	private String getMacroStartNode(String link) {
		String[] tmp = {"-1"};
		
		try {
			tmp = link.split("_");
			if(tmp.length==3) {
				return tmp[1];
				}
				else if (tmp.length==2) {
					return tmp[0];
				}
				else {
				System.out.println("failed to to getMacroStartNode for "+link);
				return "";
			}
		} catch (Exception e) {
			System.out.println("tried to getMacroStartNode for "+link);
			System.out.println(tmp);
			e.printStackTrace();
		}
		return "";
	}



	public boolean isResponsibleExternal(String node) {
		return getExternalResponsibilities().contains(node);
	}
	
	/*
	 * LayerReference ist der layer interne ort andem ndms gesendet und empfangen werden
	 */
	String getLayerReference(Meso meso) {
		//more complicated example for numbers, here we can use directly the embedded info
		return "link."+meso.getLink().toString();
	}	


	public List<String> getOutgoingLinks() {
		return outgoingLinks;
	}
	

	
	
	public void initMesoMap(String file) {
		System.out.println("trying to read "+file);
		DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
		DocumentBuilder builder = null;
		try {
			builder = factory.newDocumentBuilder();
			mesoMap = builder.parse(file);
			XPathFactory xPathfactory = XPathFactory.newInstance();
			xpath = xPathfactory.newXPath();
		} catch (ParserConfigurationException e) {
			e.printStackTrace();
		} catch (SAXException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}	
	
	
	public void initDomainReference(String file) {
		System.out.println("trying to read "+file);
		DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
		DocumentBuilder builder = null;
		try {
			builder = factory.newDocumentBuilder();
			domainReference = builder.parse(file);
			XPathFactory xPathfactory = XPathFactory.newInstance();
			xpath = xPathfactory.newXPath();
		} catch (ParserConfigurationException e) {
			e.printStackTrace();
		} catch (SAXException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}	
	
	public void initMicroMap(String file) {
		System.out.println("trying to read "+file);
		DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
		DocumentBuilder builder = null;
		try {
			builder = factory.newDocumentBuilder();
			microMap = builder.parse(file);
			XPathFactory xPathfactory = XPathFactory.newInstance();
			xpath = xPathfactory.newXPath();

			System.out.println("success!");
		} catch (ParserConfigurationException e) {
			e.printStackTrace();
		} catch (SAXException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	
	//that means all links that have a resp node as destination
	public void setInternalResponsibilitiesForMeso() {
		internalResponsibilities.clear();
		
		
		//variant that works on meso map
		if(mesoMap!=null) {
			for(String node : getExternalResponsibilities()) {
				try {
					XPathExpression expr = xpath.compile(".//link[@to='"+node+"']");
					NodeList nl = (NodeList) expr.evaluate(mesoMap, XPathConstants.NODESET);
	
					for (int i = 0; i < nl.getLength(); i++) {
						String linkID = nl.item(i).getAttributes().getNamedItem("id").getNodeValue();
						if(!internalResponsibilities.contains(linkID)) {
							internalResponsibilities.add(linkID);
						}
					}
				} catch (XPathExpressionException e) {
					System.out.println(e.getLocalizedMessage());
				}
			}
		} else {
			System.out.println("No map is present");
		}
		
		
		System.out.println("responsible for "+Arrays.toString(internalResponsibilities.toArray()));
	}
	
	//assume we always have similar edge and link names
		public void setInternalResponsibilitiesForSubmicro() {
		
			setInternalResponsibilitiesForMicro();
			System.out.println("found "+internalResponsibilities.size()+" internal resps with micro method, now convert edges to submicro roads");
			
			for(int i = 0; i<internalResponsibilities.size();i++) {
				System.out.println(internalResponsibilities.get(i)+"-->"+microEdge2SubMicroRoad(internalResponsibilities.get(i)));
				internalResponsibilities.set(i, microEdge2SubMicroRoad(internalResponsibilities.get(i)));
			}
		}
		
	
	//assume we always have similar edge and link names
	public void setInternalResponsibilitiesForMicro() {
		internalResponsibilities.clear();
		

		//variant that works on meso map
		if(mesoMap!=null) {
			for(String node : getExternalResponsibilities()) {
				System.out.println("setInternalResponsibilitiesForMicro: " + node);
				try {
					XPathExpression expr = xpath.compile(".//link[@to='"+node+"']");
					NodeList nl = (NodeList) expr.evaluate(mesoMap, XPathConstants.NODESET);
	
					for (int i = 0; i < nl.getLength(); i++) {
						String linkID = nl.item(i).getAttributes().getNamedItem("id").getNodeValue();
						if(!internalResponsibilities.contains(linkID)) {
							System.out.println("adding "+linkID);
							internalResponsibilities.add(linkID);
						}
					}
				} catch (XPathExpressionException e) {
					System.out.println(e.getLocalizedMessage());
				}
			}
		}
		else if(microMap!=null) {
			for(String node : getExternalResponsibilities()) {
				System.out.println("setInternalResponsibilitiesForMicro: " + node);
				try {
					XPathExpression expr = xpath.compile(".//edge[@to='"+node+"']");
					NodeList nl = (NodeList) expr.evaluate(microMap, XPathConstants.NODESET);
	
					for (int i = 0; i < nl.getLength(); i++) {
						String linkID = nl.item(i).getAttributes().getNamedItem("id").getNodeValue();
						if(!internalResponsibilities.contains(linkID)) {
							System.out.println("adding "+linkID);
							internalResponsibilities.add(linkID);
						}
					}
				} catch (XPathExpressionException e) {
					System.out.println(e.getLocalizedMessage());
				}
			}
		}
		else {
			System.out.println("No map is present");
		}

		System.out.println("responsible for "+Arrays.toString(internalResponsibilities.toArray()));
	}
	
	
	
	
	public void setInternalResponsibilitiesForMacro() {
		internalResponsibilities.clear();
		

		//variant that works on meso map
		if(mesoMap!=null) {
			for(String node : getExternalResponsibilities()) {
				try {
					XPathExpression expr = xpath.compile(".//link[@to='"+node+"']");
					NodeList nl = (NodeList) expr.evaluate(mesoMap, XPathConstants.NODESET);
	
					for (int i = 0; i < nl.getLength(); i++) {
						String linkID = nl.item(i).getAttributes().getNamedItem("id").getNodeValue();
						if(!internalResponsibilities.contains(linkID)) {
							internalResponsibilities.add(mesoLink2MacroLink(linkID));
						}
					}
				} catch (XPathExpressionException e) {
					System.out.println(e.getLocalizedMessage());
				}
			}
		}
		else {
			System.out.println("No map is present");
		}
		
		System.out.println("responsible for "+Arrays.toString(internalResponsibilities.toArray()));
	}
	
	
	
	
	
	

	public List<CharSequence> getMacroRouteForMicroRoute(List<String> routeEdges) {
		List<CharSequence> linkRoute = new ArrayList<CharSequence>();
		linkRoute.add(getMicroStartNode(routeEdges.get(0)));
		for(String edge : routeEdges) {
			String to = getMicroDestinationNode(edge);
			linkRoute.add(to);
		}
		return linkRoute;
	}

	
	

	public List<CharSequence> getMicroRouteForMacroRoute(List<Long> routeNodes, String microEdgeStart) {
		List<CharSequence> linkRoute = new ArrayList<CharSequence>();
		boolean routeStarted = false;
		for(int i=1;i<routeNodes.size();i++) {
			String from = routeNodes.get(i-1).toString();
			String to = routeNodes.get(i).toString();
			String ml = macroLink2MicroEdge(from, to);
			//skip all links before startLink
			if(ml.equals(microEdgeStart)) {
				routeStarted = true;
			}
			if(routeStarted) {
				linkRoute.add(ml);
			}
		}
		return linkRoute;
	}

	
	public List<CharSequence> getMesoRouteForMacroRoute(List<Long> routeNodes, String startLinkMeso) {
		List<CharSequence> linkRoute = new ArrayList<CharSequence>();
		boolean routeStarted = false;
		for(int i=1;i<routeNodes.size();i++) {
			String from = routeNodes.get(i-1).toString();
			String to = routeNodes.get(i).toString();
			String ml = macroLink2MesoLink(from, to);
			//skip all links before startLink
			if(ml.equals(startLinkMeso)) {
				routeStarted = true;
			}
			if(routeStarted) {
				linkRoute.add(ml);
			}
		}
		return linkRoute;
	}
	
	
	
	

	
	public String macroLink2MesoLink(String link) {
		String[] tmp = {"-1"};
		String fromNode = "";
		String toNode = "";
		String linkid = "";
		
		try {
			tmp = link.split("_");
			if(tmp.length==3) {
			linkid= tmp[0];
			fromNode = tmp[1];
			toNode = tmp[2];
			}
			else if (tmp.length==2) {
				fromNode = tmp[0];
				toNode = tmp[1];
			}
			else {
				System.out.println("failed to to macroLink2MesoLink for "+link);
				return "";
			}
			
			return macroLink2MesoLink(fromNode,toNode);

		} catch (Exception e) {
			System.out.println("tried to macroLink2MesoLink for "+link);
			System.out.println(tmp);
			e.printStackTrace();
		}
		return "";
	}	

	
	public String macroLink2MicroEdge(String edge) {

		String d = getMacroDestinationNode(edge);
		String s = getMacroStartNode(edge);
		
		return macroLink2MicroEdge(d,s);
	}
	
	public String macroLink2MicroEdge(String d, String s) {
	
		String[] tmp = {"-1"};
		//variant that works on domain ref
		if(domainReference!=null) {
			try {			
				XPathExpression expr = xpath.compile(".//link[@from='"+s+"'][@to='"+d+"']");
				NodeList nl = (NodeList) expr.evaluate(domainReference, XPathConstants.NODESET);
				if(nl.getLength()==0) { return "";}
	
				return nl.item(0).getAttributes().getNamedItem("id").getNodeValue();
	
			} catch (Exception e) {
				System.out.println("tried to macroLink2MesoLink for "+s+ " -> "+d);
				System.out.println(tmp);
				e.printStackTrace();
			}
		}
		else {
			System.out.println("No map is present");
		}
		return "";
	}

	public String getMicroStartNode(String edge) {
		
		//variant that works on domain ref
		if(domainReference!=null) {
			try {			
				XPathExpression expr = xpath.compile(".//link[@id='"+edge+"']");
				NodeList nl = (NodeList) expr.evaluate(domainReference, XPathConstants.NODESET);
				if(nl.getLength()==0) { return "";}
	
				return nl.item(0).getAttributes().getNamedItem("from").getNodeValue();
	
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		else {
			System.out.println("No map is present");
		}
		return "";
	}
	public String getMicroDestinationNode(String edge) {
		
		//variant that works on domain ref
		if(domainReference!=null) {
			try {			
				XPathExpression expr = xpath.compile(".//link[@id='"+edge+"']");
				NodeList nl = (NodeList) expr.evaluate(domainReference, XPathConstants.NODESET);
				if(nl.getLength()==0) { return "";}
	
				return nl.item(0).getAttributes().getNamedItem("to").getNodeValue();
	
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		else {
			System.out.println("No map is present");
		}
		return "";
	}

	
	
	public String macroLink2MesoLink(String fromNode, String toNode) {
		String[] tmp = {"-1"};
		//variant that works on meso map
		if(mesoMap!=null) {
			try {			
				XPathExpression expr = xpath.compile(".//link[@from='"+fromNode+"'][@to='"+toNode+"']");
				NodeList nl = (NodeList) expr.evaluate(mesoMap, XPathConstants.NODESET);
				if(nl.getLength()==0) { return "";}
	
				return nl.item(0).getAttributes().getNamedItem("id").getNodeValue();
	
			} catch (Exception e) {
				System.out.println("tried to macroLink2MesoLink for "+fromNode+ " -> "+toNode);
				System.out.println(tmp);
				e.printStackTrace();
			}
		}
		else {
			System.out.println("No map is present");
		}
		return "";
	}
	


	public String microEdge2MacroLink(String id) {
		//variant that works on domain reference
		if(domainReference!=null) {
			try {
				XPathExpression expr = xpath.compile(".//link[@id='"+id+"']");
				NodeList nl = (NodeList) expr.evaluate(domainReference, XPathConstants.NODESET);
				if(nl.getLength()==0) { return "";}
	
				String from = nl.item(0).getAttributes().getNamedItem("from").getNodeValue();
				String to = nl.item(0).getAttributes().getNamedItem("to").getNodeValue();
				return from+"_"+to;
	
			} catch (Exception e) {
				System.out.println("tried to getMacroLinkForMicroEdge for "+id);
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		else {
			System.out.println("No map is present");
		}
		return "";
	}	
	
	
	public String mesoLink2MacroLink(String id) {
		//variant that works on meso map
		if(mesoMap!=null) {
			try {
				XPathExpression expr = xpath.compile(".//link[@id='"+id+"']");
				NodeList nl = (NodeList) expr.evaluate(mesoMap, XPathConstants.NODESET);
				if(nl.getLength()==0) { return "";}
	
				String from = nl.item(0).getAttributes().getNamedItem("from").getNodeValue();
				String to = nl.item(0).getAttributes().getNamedItem("to").getNodeValue();
				return from+"_"+to;
	
			} catch (Exception e) {
				System.out.println("tried to getMacroLinkForMesoLink for "+id);
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		else if(domainReference!=null) {
			try {
				XPathExpression expr = xpath.compile(".//link[@id='"+id+"']");
				NodeList nl = (NodeList) expr.evaluate(domainReference, XPathConstants.NODESET);
				if(nl.getLength()==0) { return "";}
	
				String from = nl.item(0).getAttributes().getNamedItem("from").getNodeValue();
				String to = nl.item(0).getAttributes().getNamedItem("to").getNodeValue();
				return from+"_"+to;
	
			} catch (Exception e) {
				System.out.println("tried to getMacroLinkForMesoLink for "+id);
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		else {
			System.out.println("No map is present");
		}
		return "";
	}

	
	

	public String subMicroRoad2MicroEdge(String road) {
		//variant that works on micro map
		if(microMap!=null) {
			try {
				XPathExpression expr = xpath.compile(".//edge/lane/param[@key='origId'][@value='"+road+"']");
				NodeList nl = (NodeList) expr.evaluate(microMap, XPathConstants.NODESET);
				if(nl.getLength()==0) { return "";}
				
				Element e = (Element) nl.item(0);
				String laneid = e.getParentNode().getAttributes().getNamedItem("id").getNodeValue();
				String edgeid = e.getParentNode().getParentNode().getAttributes().getNamedItem("id").getNodeValue();
				System.out.println("edge for road "+road+" is "+edgeid);
				return edgeid;
	
			} catch (Exception e) {
				System.out.println("tried to submicroRoad2MicroEdgeLink for "+road);
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		else {
			System.out.println("No map is present");
		}
		return "";
	}
	
	

	public String microEdge2SubMicroRoad(String edge) {
		//variant that works on micro map
		System.out.println("microEdge2SubMicroRoad for "+edge);
		if(microMap!=null) {
			try {
				XPathExpression expr = xpath.compile(".//edge[@id='"+edge+"'][./lane/param[@key='origId']]");
				NodeList nl = (NodeList) expr.evaluate(microMap, XPathConstants.NODESET);
				if(nl.getLength()==0) { 
					System.out.println("element not found");
					return "";
					}
				System.out.println("found "+nl.getLength()+" elements");
				
				Element edgeXml = (Element) nl.item(0);
				Element lane = (Element) edgeXml.getElementsByTagName("lane").item(0);
				Element param = (Element) lane.getElementsByTagName("param").item(0);
				String roadid = param.getAttributes().getNamedItem("value").getNodeValue();
				System.out.println("road for edge "+edge+" is "+roadid);
				return roadid;
	
			} catch (Exception e) {
				System.out.println("tried to submicroRoad2MicroEdgeLink for "+edge);
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		else {
			System.out.println("No map is present");
		}
		return "";
	}
	
	
	
	
	/*
	 * LayerReference ist der layer interne ort andem ndms gesendet und empfangen werden
	 */
	String getLayerReference(Macro macro) {
		//more complicated example for numbers, here we can use directly the embedded info
		return "link."+macro.getLinkID().toString();
	}




	
	
//	public void calcRespAndOutLinks() {
//	internalResponsibilities.clear();
//	outgoingLinks.clear();
//	
//	//get all links
//	Collection<? extends Link> links = MatsimAPI.getLinks();
//	for(Link link : links) {
//		String startNode = link.getFromNode().getId().toString();
//		String endNode = link.getToNode().getId().toString();
//		
//		//start and end inside resp region
//		if(getExternalResponsibilities().contains(startNode) && getExternalResponsibilities().contains(endNode) ) {
//			internalResponsibilities.add(link.getId().toString());
//		} 
//		// or if incoming link
//		else if(getExternalResponsibilities().contains(endNode)) {
//			internalResponsibilities.add(link.getId().toString());
//		}
//		// if start inside and end outside: outgoing link
//		else if(getExternalResponsibilities().contains(startNode)) {
//			outgoingLinks.add(link.getId().toString());
//		} 
//		// else: we don't care
//	}
//	
//	logger.info("==========================================");
//	logger.info("updated responsibilities:");
//	logger.info("rexternalResponsibilities:");
//	for(String s : getExternalResponsibilities()) {
//		logger.info(s);
//	}
//	logger.info("internalResponsibilities:");
//	for(String s : internalResponsibilities) {
//		logger.info(s);
//	}
//	logger.info("outgoing links:");
//	for(String s : outgoingLinks) {
//		logger.info(s);
//	}
//	logger.info("==========================================");
//}
//
//
//
//List<String> routeStr2List(String route){
//	ArrayList<String> l = new ArrayList<String>();
//	retur

}
