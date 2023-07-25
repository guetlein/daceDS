/*******************************************************************************
 * Copyright 2022 Moritz Gütlein
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

package eu.fau.cs7.daceDS.Demo;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import javax.imageio.ImageIO;
import javax.swing.JFrame;
import javax.swing.JPanel;
import org.apache.kafka.clients.consumer.ConsumerRecord;
import eu.fau.cs7.daceDS.Component.InteractionHandler;
import eu.fau.cs7.daceDS.Component.Simulator;
import eu.fau.cs7.daceDS.datamodel.DemoRGB3;
 

public class RGB3Simulator extends Simulator<DemoRGB3> {
 
    private static final int NUM_PIXELS = 8;
	private static JFrame frame;
	private static JPanel pane;
	private static int pixelSize = 200;
	private static int width = 800;
	private static int height = 10;
	private static int bigHeight = 100;
	private static double alpha = 254.0;
	private Thread t;
	private InteractionHandler interactionHandler;
	private double fadeStep;
	private int seed;
	private int saveStep;
	private static ArrayList<BufferedImage> images = new ArrayList<BufferedImage>();
	private static ArrayList<BufferedImage> bigImages = new ArrayList<BufferedImage>();
	private static Color[] pixels;
	
	private final int plotElements = 100;
	private final int bigPlotElements = 10;
	private int bigSaveStep;
	

	protected static final boolean RGB3 = true;
	

	private static int c = 0;
	public RGB3Simulator(String scenarioID, String instanceID, String provisionPrePattern, String provisionPostPattern, String demoScenario) {        
		super(scenarioID, instanceID, provisionPrePattern, provisionPostPattern, demoScenario);
    }
 

 
    public static void main(String[] args) {
		if(args.length < 2) {
			System.out.println("Usage exe scenarioID simulatorID [demo]");
			return;
		}
		String scenarioID = "gs_proj4_3bit3";//args[0];
		String instanceID = args[1];
		String demoScenario = "";

		if(args.length == 3) {
			demoScenario = "daceDS/Scenario/Demo/Projection_sine.sce";
		}

		String provisionPrePattern = "container\\.";
		String provisionPostPattern = "\\.variables";

		pixels = new Color[NUM_PIXELS];
		
		RGB3Simulator instance = new RGB3Simulator(scenarioID, instanceID, provisionPrePattern,provisionPostPattern, demoScenario);
		instance.run();
		
		
//		instance.scenario = Utils.getDemoScenario(demoScenario);
//		instance.simulator = Utils.getBBFromScenario( instanceID, instance.scenario );
//		instance.runSimple();
		
		
		//small
		int heightCurr = 0;
        BufferedImage concatImage = new BufferedImage(width, height*images.size(), BufferedImage.TYPE_INT_RGB);
        Graphics2D g2d = concatImage.createGraphics();
        for(int j = 0; j < images.size(); j++) {
            g2d.drawImage(images.get(j), 0, heightCurr, null);
            heightCurr += height;
        }

        //big
		heightCurr = 0;
        BufferedImage bigConcatImage = new BufferedImage(width, bigHeight*bigImages.size(), BufferedImage.TYPE_INT_RGB);
        g2d = bigConcatImage.createGraphics();
        for(int j = 0; j < bigImages.size(); j++) {
            g2d.drawImage(bigImages.get(j), 0, heightCurr, null);
            heightCurr += bigHeight;//bigImages.get(j).getHeight();
        }

        
        
		try {
			ImageIO.write(concatImage,"png", new File(scenarioID+"_"+instanceID+".png"));
			ImageIO.write(bigConcatImage,"png", new File(scenarioID+"_"+instanceID+"_big.png"));
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

    }

    
    public void init() {
		super.init();
		interactionHandler = new InteractionHandler(scenario, simulator, false);
		interactionHandler.init();
		

    }
    
	@Override
	protected void processInteraction(ConsumerRecord r) {
		ConsumerRecord<String,String> msg = r;
		log("got interaction setGrid: "+msg.value());
		String[] values = msg.value().split(",");
		
		if(values[0].equals("+")) {
//			increaseTemperature(Integer.valueOf(values[1]));
			for(int x=0;x<NUM_PIXELS;x++) {
				increaseTemperature(x);
			}
			
		}
		if(values[0].equals("-")) {
//			decreaseTemperature(Integer.valueOf(values[1]));	
			for(int x=0;x<NUM_PIXELS;x++) {
				decreaseTemperature(x);
			}
		}

	}

    public static void repaint() {
    	if(pane != null)
    	pane.repaint();
    }

    public static void increaseTemperature(int pixel) {
    	Color c = pixels[pixel];
    	int r = c.getRed();
    	int g = c.getGreen();
    	int b = c.getBlue();
    	
    	if(r==254) {
    		r=255;
    	} else if(r<254) {
    		r+=2;
    	}    	
		if(g>0) {
			g-=1;
		}    	
		if(b>0) {
			b-=1;
		}
		    	
    	System.out.println(r+","+g+","+b);
		Color c2 = new Color(r,g,b);
		pixels[pixel] = c2;
    }

    public static void decreaseTemperature(int pixel) {
    	Color c = pixels[pixel];
    	int r = c.getRed();
    	int g = c.getGreen();
    	int b = c.getBlue();
    	
    	if(r==1) {
    		r=0;
    	} else if(r>1) {
    		r-=2;
    	}    	
		if(g<255) {
			g+=1;
		}    	
		if(b<255) {
			b+=1;
		}
		    	
    	System.out.println(r+","+g+","+b);
		Color c2 = new Color(r,g,b);
		pixels[pixel] = c2;
    }




	@Override
	public void preLoopEvent() {
		
		seed = (int) scenario.getExecution().getRandomSeed();
		fadeStep = 255.0/scenario.getSimulationEnd();
		saveStep = (int) (scenario.getSimulationEnd()/plotElements);
		bigSaveStep = (int) (scenario.getSimulationEnd()/bigPlotElements);
		System.out.println("savestep="+saveStep+",fadeStep="+fadeStep);
		
		//assign random colors 
		//todo: get this "picture" from input
		for(int x=0;x<NUM_PIXELS;x++) {
//			int r = 0;
//			int g = 0;
//			int b = 0;
			int r = Utils.pseudoRand(seed+x*7,255);
			int g = Utils.pseudoRand(x*333, 255);
			int b = Utils.pseudoRand(x+seed, 255);
			Color c = new Color(r,g,b);
			pixels[x] = c;
		}
		
        pixelSize = width/NUM_PIXELS;
        
        pane = new JPanel() {
            @Override
            protected void paintComponent(Graphics g) {
                super.paintComponent(g);
                Graphics2D g2d = (Graphics2D) g;

                
                int rSum = 0;
                int gSum = 0;
                int bSum = 0;
                for(int x=0;x<NUM_PIXELS;x++) {
                		Color c = pixels[x];
                		int r = c.getRed();
                		int gr = c.getGreen();
                		int b = c.getBlue();
                		int a = (int) alpha;
                		
                		//3bit?
                		if(RGB3) {
                			r = (int) (255 * Math.round((double)c.getRed()/255.0));
                			gr = (int) (255*Math.round((double)c.getGreen()/255.0));
                			b = (int) (255*Math.round((double)c.getBlue()/255.0));
                		}
                		Color ca = new Color(r,gr,b,a);
//                		System.out.println(ca.toString());
//                    	Rectangle2D rect = new Rectangle2D.Float(x*pixelSize, 0, pixelSize, 4*height/5);
                    	Rectangle2D rect = new Rectangle2D.Float(x*pixelSize, 0, pixelSize, bigHeight);
                    	g2d.setColor(ca);
                    	g2d.fill(rect);
                    	
                    	//add color to avg color
                    	rSum+=ca.getRed();
                    	gSum+=ca.getGreen();
                    	bSum+=ca.getBlue();
                    	

                    	//put label on it
                    	
                    	Font font = new Font("Arial", Font.PLAIN, 36);
                        FontMetrics metrics = g.getFontMetrics(font);
                        String text = ""+x;
                        int tx = (int) (rect.getX() + (rect.getWidth() - metrics.stringWidth(text)) / 2);
                        int ty = (int) (rect.getY() + ((rect.getHeight() - metrics.getHeight()) / 2) + metrics.getAscent());
                    	g2d.setColor(Color.black);
                    	if(ca.getBlue()+ca.getGreen()+ca.getRed() < 40)
                    		g2d.setColor(Color.white);
                        g2d.setFont(font);
                        g2d.setRenderingHint(
                                RenderingHints.KEY_TEXT_ANTIALIASING,
                                RenderingHints.VALUE_TEXT_ANTIALIAS_ON);
//                        g2d.drawString(text, tx, ty);
                }  

//                //draw an average color at the bottom
//                Color c = new Color((int)Math.round(rSum/NUM_PIXELS),(int)Math.round(gSum/NUM_PIXELS),(int)Math.round(bSum/NUM_PIXELS),(int)Math.round(alpha));
//                System.out.println(c.toString() + "alpa="+Math.round(alpha));
//            	g2d.setColor(c);
//            	Rectangle2D rect = new Rectangle2D.Float(0, 4*height/5, width, 1*height/5);
//            	g2d.fill(rect);
//            	
//            	//put label on it
//            	
//            	Font font = new Font("Arial", Font.PLAIN, 12);
//                FontMetrics metrics = g.getFontMetrics(font);
//                String text = "Simulation time: "+TimeSync.getLocaltime();
//                int tx = (int) (rect.getX() + (rect.getWidth() - metrics.stringWidth(text)) / 2);
//                int ty = (int) (rect.getY() + ((rect.getHeight() - metrics.getHeight()) / 2) + metrics.getAscent());
//            	g2d.setColor(Color.black);
//            	if(c.getBlue()+c.getGreen()+c.getRed() < 40)
//            		g2d.setColor(Color.white);
//                g2d.setFont(font);
//                g2d.setRenderingHint(
//                        RenderingHints.KEY_TEXT_ANTIALIASING,
//                        RenderingHints.VALUE_TEXT_ANTIALIAS_ON);
//                g2d.drawString(text, 10, ty);
                
            }

            @Override
            public Dimension getPreferredSize() {
                return new Dimension(width, bigHeight);
            }
        };
        
		Runnable r =  new Runnable() {
            @Override
            public void run() {
                frame = new JFrame("Color Simulator");
                frame.getContentPane().add(pane);
                frame.pack();
                frame.setVisible(true);
                frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            }
        };
        t = new Thread(r);
        t.start();
        
        //wait until done
        while(pane==null) {
	        try {
				Thread.sleep(50);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
        }
	}



	@Override
	protected void preStepEvent(int t) {
		// TODO Auto-generated method stub
		
	}



	@Override
	protected void stepEvent(int t) {

//        alpha -=fadeStep;
//        alpha = (alpha >= 0) ? alpha : 0;
//        alpha = (alpha <= 254) ? alpha : 254;
//		System.out.println(t+" alpha "+Math.round(alpha));
    	RGB3Simulator.repaint();
    	try {
			Thread.sleep(5);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
 
	}



	@Override
	protected void processStepEvent(int t) {

    	interactionHandler.processBuffer(t, this);
		
	}



	@Override
	protected void postStepEvent(int t) {
		// TODO Auto-generated method stub


		if(t%saveStep==0) {
			savePaint(t);
		}		
		
		if(t%bigSaveStep==0) {
			bigSavePaint(t);
		}
		
		
	}

    public void savePaint(int time)
    {

        try
        {
            BufferedImage image = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
            Graphics2D graphics2D = image.createGraphics();
            pane.paint(graphics2D);
            images.add(image);
            
        }
        catch(Exception exception)
        {
            //code
        }
    }
    public void bigSavePaint(int time)
    {

        try
        {
            BufferedImage image = new BufferedImage(width, bigHeight, BufferedImage.TYPE_INT_RGB);
            Graphics2D graphics2D = image.createGraphics();
            pane.paint(graphics2D);
            bigImages.add(image);
            
        }
        catch(Exception exception)
        {
            //code
        }
    }



	@Override
	protected void postLoopEvent() {
		// TODO Auto-generated method stub
		
	}



	@Override
	protected void preInit() {
		// TODO Auto-generated method stub
		
	}



	@Override
	protected void processMessage(ConsumerRecord r) {
		// TODO Auto-generated method stub
		
	}

}
