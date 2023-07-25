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
import java.lang.reflect.ParameterizedType;

import eu.fau.cs7.daceDS.datamodel.DemoDouble;
import eu.fau.cs7.daceDS.datamodel.DemoInteger;

public class TestClass<A,B> {

	Class<A> classA;

	public static void main(String[] args) throws InstantiationException, IllegalAccessException {
		TestClass<DemoInteger,DemoDouble> t = new TestClass<DemoInteger,DemoDouble>();

		DemoInteger i = new DemoInteger();
		DemoDouble d = new DemoDouble();
		
		t.bla();
		
//		System.out.println(t.isOfTypeA(d));
//		i.getClass().equals(A.class);
	}
	
	void bla() {
		Class<A> persistentClass = (Class<A>)((ParameterizedType)getClass().getGenericSuperclass()).getActualTypeArguments()[0];
	}

	boolean isOfTypeA(Object o) throws InstantiationException, IllegalAccessException {
		A a = classA.newInstance();
		return a.getClass().equals(o.getClass());
//		return c.isInstance(o);
	}
	
//	o.getClass().equals(MicroList.class);
}
