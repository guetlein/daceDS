/*******************************************************************************
 * Copyright 2021 Moritz Gütlein
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations under
 * the License.
 ******************************************************************************/

 package eu.fau.cs7.daceDS.VisumWrapper;

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
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import eu.fau.cs7.daceDS.Component.DomainHelper;
import eu.fau.cs7.daceDS.datamodel.Macro;
import eu.fau.cs7.daceDS.datamodel.Meso;

public class DomainHelperTraffic extends DomainHelper {

	static Logger logger = Logger.getLogger("DomainHelperTraffic");

	List<String> outgoingLinks = new ArrayList<String>();
	
	XPath xpath;
	private Document mesoMap;
	
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
	
	
	//that means all links that have a resp node as destination
	public void setInternalResponsibilitiesForMeso() {
		internalResponsibilities.clear();
		for(String node : getExternalResponsibilities()) {
			try {
				XPathExpression expr = xpath.compile(".//link[@to='"+node+"']");
				NodeList nl = (NodeList) expr.evaluate(mesoMap, XPathConstants.NODESET);

				for (int i = 0; i < nl.getLength(); i++) {
					String nodeID = nl.item(i).getAttributes().getNamedItem("id").getNodeValue();
					if(!internalResponsibilities.contains(nodeID)) {
						internalResponsibilities.add(nodeID);
					}
				}
			} catch (XPathExpressionException e) {
				e.printStackTrace();
			}
		}

		System.out.println("responsible for "+Arrays.toString(internalResponsibilities.toArray()));
	}
	
	public void setInternalResponsibilitiesForMacro() {
		internalResponsibilities.clear();
		for(String node : getExternalResponsibilities()) {
			try {
				XPathExpression expr = xpath.compile(".//link[@to='"+node+"']");
				NodeList nl = (NodeList) expr.evaluate(mesoMap, XPathConstants.NODESET);

				for (int i = 0; i < nl.getLength(); i++) {
					String nodeID = nl.item(i).getAttributes().getNamedItem("id").getNodeValue();
					if(!internalResponsibilities.contains(nodeID)) {
						internalResponsibilities.add(mesoLink2MacroLink(nodeID));
					}
				}
			} catch (XPathExpressionException e) {
				e.printStackTrace();
			}
		}

		System.out.println("responsible for "+Arrays.toString(internalResponsibilities.toArray()));
	}
	
	
	
	
	public List<CharSequence> getMesoRouteForMacroRoute(List<Integer> routeNodes, String startLinkMeso) {
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
	public String macroLink2MesoLink(String fromNode, String toNode) {
		String[] tmp = {"-1"};
		
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
		return "";
	}
	


	public String mesoLink2MacroLink(String id) {
		try {
			XPathExpression expr = xpath.compile(".//link[@id='"+id+"']");
			NodeList nl = (NodeList) expr.evaluate(mesoMap, XPathConstants.NODESET);
			if(nl.getLength()==0) { return "";}

			String from = nl.item(0).getAttributes().getNamedItem("from").getNodeValue();
			String to = nl.item(0).getAttributes().getNamedItem("to").getNodeValue();
			return from+"_"+to;

		} catch (Exception e) {
			System.out.println("tried to getMacroLinkForMesoLink for "+id);
			e.printStackTrace();
		}
		return "";
	}


	String getLayerReference(Macro macro) {
		//more complicated example for numbers, here we can use directly the embedded info
		return "link."+macro.getLinkID().toString();
	}	
	

}
