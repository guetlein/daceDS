/**
 * Autogenerated by Avro
 *
 * DO NOT EDIT DIRECTLY
 */
package eu.fau.cs7.daceDS.datamodel;

import org.apache.avro.generic.GenericArray;
import org.apache.avro.specific.SpecificData;
import org.apache.avro.util.Utf8;
import org.apache.avro.message.BinaryMessageEncoder;
import org.apache.avro.message.BinaryMessageDecoder;
import org.apache.avro.message.SchemaStore;

@org.apache.avro.specific.AvroGenerated
public class MacroPath extends org.apache.avro.specific.SpecificRecordBase implements org.apache.avro.specific.SpecificRecord {
  private static final long serialVersionUID = 7615396972897005838L;
  public static final org.apache.avro.Schema SCHEMA$ = new org.apache.avro.Schema.Parser().parse("{\"type\":\"record\",\"name\":\"MacroPath\",\"namespace\":\"eu.fau.cs7.daceDS.datamodel\",\"fields\":[{\"name\":\"fromZone\",\"type\":\"int\"},{\"name\":\"toZone\",\"type\":\"int\"},{\"name\":\"pathID\",\"type\":\"int\"},{\"name\":\"flow\",\"type\":\"double\"},{\"name\":\"totalFlow\",\"type\":\"double\"},{\"name\":\"nodes\",\"type\":{\"type\":\"array\",\"items\":\"long\"}}]}");
  public static org.apache.avro.Schema getClassSchema() { return SCHEMA$; }

  private static SpecificData MODEL$ = new SpecificData();

  private static final BinaryMessageEncoder<MacroPath> ENCODER =
      new BinaryMessageEncoder<MacroPath>(MODEL$, SCHEMA$);

  private static final BinaryMessageDecoder<MacroPath> DECODER =
      new BinaryMessageDecoder<MacroPath>(MODEL$, SCHEMA$);

  /**
   * Return the BinaryMessageEncoder instance used by this class.
   * @return the message encoder used by this class
   */
  public static BinaryMessageEncoder<MacroPath> getEncoder() {
    return ENCODER;
  }

  /**
   * Return the BinaryMessageDecoder instance used by this class.
   * @return the message decoder used by this class
   */
  public static BinaryMessageDecoder<MacroPath> getDecoder() {
    return DECODER;
  }

  /**
   * Create a new BinaryMessageDecoder instance for this class that uses the specified {@link SchemaStore}.
   * @param resolver a {@link SchemaStore} used to find schemas by fingerprint
   * @return a BinaryMessageDecoder instance for this class backed by the given SchemaStore
   */
  public static BinaryMessageDecoder<MacroPath> createDecoder(SchemaStore resolver) {
    return new BinaryMessageDecoder<MacroPath>(MODEL$, SCHEMA$, resolver);
  }

  /**
   * Serializes this MacroPath to a ByteBuffer.
   * @return a buffer holding the serialized data for this instance
   * @throws java.io.IOException if this instance could not be serialized
   */
  public java.nio.ByteBuffer toByteBuffer() throws java.io.IOException {
    return ENCODER.encode(this);
  }

  /**
   * Deserializes a MacroPath from a ByteBuffer.
   * @param b a byte buffer holding serialized data for an instance of this class
   * @return a MacroPath instance decoded from the given buffer
   * @throws java.io.IOException if the given bytes could not be deserialized into an instance of this class
   */
  public static MacroPath fromByteBuffer(
      java.nio.ByteBuffer b) throws java.io.IOException {
    return DECODER.decode(b);
  }

   private int fromZone;
   private int toZone;
   private int pathID;
   private double flow;
   private double totalFlow;
   private java.util.List<java.lang.Long> nodes;

  /**
   * Default constructor.  Note that this does not initialize fields
   * to their default values from the schema.  If that is desired then
   * one should use <code>newBuilder()</code>.
   */
  public MacroPath() {}

  /**
   * All-args constructor.
   * @param fromZone The new value for fromZone
   * @param toZone The new value for toZone
   * @param pathID The new value for pathID
   * @param flow The new value for flow
   * @param totalFlow The new value for totalFlow
   * @param nodes The new value for nodes
   */
  public MacroPath(java.lang.Integer fromZone, java.lang.Integer toZone, java.lang.Integer pathID, java.lang.Double flow, java.lang.Double totalFlow, java.util.List<java.lang.Long> nodes) {
    this.fromZone = fromZone;
    this.toZone = toZone;
    this.pathID = pathID;
    this.flow = flow;
    this.totalFlow = totalFlow;
    this.nodes = nodes;
  }

  public org.apache.avro.specific.SpecificData getSpecificData() { return MODEL$; }
  public org.apache.avro.Schema getSchema() { return SCHEMA$; }
  // Used by DatumWriter.  Applications should not call.
  public java.lang.Object get(int field$) {
    switch (field$) {
    case 0: return fromZone;
    case 1: return toZone;
    case 2: return pathID;
    case 3: return flow;
    case 4: return totalFlow;
    case 5: return nodes;
    default: throw new IndexOutOfBoundsException("Invalid index: " + field$);
    }
  }

  // Used by DatumReader.  Applications should not call.
  @SuppressWarnings(value="unchecked")
  public void put(int field$, java.lang.Object value$) {
    switch (field$) {
    case 0: fromZone = (java.lang.Integer)value$; break;
    case 1: toZone = (java.lang.Integer)value$; break;
    case 2: pathID = (java.lang.Integer)value$; break;
    case 3: flow = (java.lang.Double)value$; break;
    case 4: totalFlow = (java.lang.Double)value$; break;
    case 5: nodes = (java.util.List<java.lang.Long>)value$; break;
    default: throw new IndexOutOfBoundsException("Invalid index: " + field$);
    }
  }

  /**
   * Gets the value of the 'fromZone' field.
   * @return The value of the 'fromZone' field.
   */
  public int getFromZone() {
    return fromZone;
  }


  /**
   * Sets the value of the 'fromZone' field.
   * @param value the value to set.
   */
  public void setFromZone(int value) {
    this.fromZone = value;
  }

  /**
   * Gets the value of the 'toZone' field.
   * @return The value of the 'toZone' field.
   */
  public int getToZone() {
    return toZone;
  }


  /**
   * Sets the value of the 'toZone' field.
   * @param value the value to set.
   */
  public void setToZone(int value) {
    this.toZone = value;
  }

  /**
   * Gets the value of the 'pathID' field.
   * @return The value of the 'pathID' field.
   */
  public int getPathID() {
    return pathID;
  }


  /**
   * Sets the value of the 'pathID' field.
   * @param value the value to set.
   */
  public void setPathID(int value) {
    this.pathID = value;
  }

  /**
   * Gets the value of the 'flow' field.
   * @return The value of the 'flow' field.
   */
  public double getFlow() {
    return flow;
  }


  /**
   * Sets the value of the 'flow' field.
   * @param value the value to set.
   */
  public void setFlow(double value) {
    this.flow = value;
  }

  /**
   * Gets the value of the 'totalFlow' field.
   * @return The value of the 'totalFlow' field.
   */
  public double getTotalFlow() {
    return totalFlow;
  }


  /**
   * Sets the value of the 'totalFlow' field.
   * @param value the value to set.
   */
  public void setTotalFlow(double value) {
    this.totalFlow = value;
  }

  /**
   * Gets the value of the 'nodes' field.
   * @return The value of the 'nodes' field.
   */
  public java.util.List<java.lang.Long> getNodes() {
    return nodes;
  }


  /**
   * Sets the value of the 'nodes' field.
   * @param value the value to set.
   */
  public void setNodes(java.util.List<java.lang.Long> value) {
    this.nodes = value;
  }

  /**
   * Creates a new MacroPath RecordBuilder.
   * @return A new MacroPath RecordBuilder
   */
  public static eu.fau.cs7.daceDS.datamodel.MacroPath.Builder newBuilder() {
    return new eu.fau.cs7.daceDS.datamodel.MacroPath.Builder();
  }

  /**
   * Creates a new MacroPath RecordBuilder by copying an existing Builder.
   * @param other The existing builder to copy.
   * @return A new MacroPath RecordBuilder
   */
  public static eu.fau.cs7.daceDS.datamodel.MacroPath.Builder newBuilder(eu.fau.cs7.daceDS.datamodel.MacroPath.Builder other) {
    if (other == null) {
      return new eu.fau.cs7.daceDS.datamodel.MacroPath.Builder();
    } else {
      return new eu.fau.cs7.daceDS.datamodel.MacroPath.Builder(other);
    }
  }

  /**
   * Creates a new MacroPath RecordBuilder by copying an existing MacroPath instance.
   * @param other The existing instance to copy.
   * @return A new MacroPath RecordBuilder
   */
  public static eu.fau.cs7.daceDS.datamodel.MacroPath.Builder newBuilder(eu.fau.cs7.daceDS.datamodel.MacroPath other) {
    if (other == null) {
      return new eu.fau.cs7.daceDS.datamodel.MacroPath.Builder();
    } else {
      return new eu.fau.cs7.daceDS.datamodel.MacroPath.Builder(other);
    }
  }

  /**
   * RecordBuilder for MacroPath instances.
   */
  @org.apache.avro.specific.AvroGenerated
  public static class Builder extends org.apache.avro.specific.SpecificRecordBuilderBase<MacroPath>
    implements org.apache.avro.data.RecordBuilder<MacroPath> {

    private int fromZone;
    private int toZone;
    private int pathID;
    private double flow;
    private double totalFlow;
    private java.util.List<java.lang.Long> nodes;

    /** Creates a new Builder */
    private Builder() {
      super(SCHEMA$);
    }

    /**
     * Creates a Builder by copying an existing Builder.
     * @param other The existing Builder to copy.
     */
    private Builder(eu.fau.cs7.daceDS.datamodel.MacroPath.Builder other) {
      super(other);
      if (isValidValue(fields()[0], other.fromZone)) {
        this.fromZone = data().deepCopy(fields()[0].schema(), other.fromZone);
        fieldSetFlags()[0] = other.fieldSetFlags()[0];
      }
      if (isValidValue(fields()[1], other.toZone)) {
        this.toZone = data().deepCopy(fields()[1].schema(), other.toZone);
        fieldSetFlags()[1] = other.fieldSetFlags()[1];
      }
      if (isValidValue(fields()[2], other.pathID)) {
        this.pathID = data().deepCopy(fields()[2].schema(), other.pathID);
        fieldSetFlags()[2] = other.fieldSetFlags()[2];
      }
      if (isValidValue(fields()[3], other.flow)) {
        this.flow = data().deepCopy(fields()[3].schema(), other.flow);
        fieldSetFlags()[3] = other.fieldSetFlags()[3];
      }
      if (isValidValue(fields()[4], other.totalFlow)) {
        this.totalFlow = data().deepCopy(fields()[4].schema(), other.totalFlow);
        fieldSetFlags()[4] = other.fieldSetFlags()[4];
      }
      if (isValidValue(fields()[5], other.nodes)) {
        this.nodes = data().deepCopy(fields()[5].schema(), other.nodes);
        fieldSetFlags()[5] = other.fieldSetFlags()[5];
      }
    }

    /**
     * Creates a Builder by copying an existing MacroPath instance
     * @param other The existing instance to copy.
     */
    private Builder(eu.fau.cs7.daceDS.datamodel.MacroPath other) {
      super(SCHEMA$);
      if (isValidValue(fields()[0], other.fromZone)) {
        this.fromZone = data().deepCopy(fields()[0].schema(), other.fromZone);
        fieldSetFlags()[0] = true;
      }
      if (isValidValue(fields()[1], other.toZone)) {
        this.toZone = data().deepCopy(fields()[1].schema(), other.toZone);
        fieldSetFlags()[1] = true;
      }
      if (isValidValue(fields()[2], other.pathID)) {
        this.pathID = data().deepCopy(fields()[2].schema(), other.pathID);
        fieldSetFlags()[2] = true;
      }
      if (isValidValue(fields()[3], other.flow)) {
        this.flow = data().deepCopy(fields()[3].schema(), other.flow);
        fieldSetFlags()[3] = true;
      }
      if (isValidValue(fields()[4], other.totalFlow)) {
        this.totalFlow = data().deepCopy(fields()[4].schema(), other.totalFlow);
        fieldSetFlags()[4] = true;
      }
      if (isValidValue(fields()[5], other.nodes)) {
        this.nodes = data().deepCopy(fields()[5].schema(), other.nodes);
        fieldSetFlags()[5] = true;
      }
    }

    /**
      * Gets the value of the 'fromZone' field.
      * @return The value.
      */
    public int getFromZone() {
      return fromZone;
    }


    /**
      * Sets the value of the 'fromZone' field.
      * @param value The value of 'fromZone'.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.MacroPath.Builder setFromZone(int value) {
      validate(fields()[0], value);
      this.fromZone = value;
      fieldSetFlags()[0] = true;
      return this;
    }

    /**
      * Checks whether the 'fromZone' field has been set.
      * @return True if the 'fromZone' field has been set, false otherwise.
      */
    public boolean hasFromZone() {
      return fieldSetFlags()[0];
    }


    /**
      * Clears the value of the 'fromZone' field.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.MacroPath.Builder clearFromZone() {
      fieldSetFlags()[0] = false;
      return this;
    }

    /**
      * Gets the value of the 'toZone' field.
      * @return The value.
      */
    public int getToZone() {
      return toZone;
    }


    /**
      * Sets the value of the 'toZone' field.
      * @param value The value of 'toZone'.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.MacroPath.Builder setToZone(int value) {
      validate(fields()[1], value);
      this.toZone = value;
      fieldSetFlags()[1] = true;
      return this;
    }

    /**
      * Checks whether the 'toZone' field has been set.
      * @return True if the 'toZone' field has been set, false otherwise.
      */
    public boolean hasToZone() {
      return fieldSetFlags()[1];
    }


    /**
      * Clears the value of the 'toZone' field.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.MacroPath.Builder clearToZone() {
      fieldSetFlags()[1] = false;
      return this;
    }

    /**
      * Gets the value of the 'pathID' field.
      * @return The value.
      */
    public int getPathID() {
      return pathID;
    }


    /**
      * Sets the value of the 'pathID' field.
      * @param value The value of 'pathID'.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.MacroPath.Builder setPathID(int value) {
      validate(fields()[2], value);
      this.pathID = value;
      fieldSetFlags()[2] = true;
      return this;
    }

    /**
      * Checks whether the 'pathID' field has been set.
      * @return True if the 'pathID' field has been set, false otherwise.
      */
    public boolean hasPathID() {
      return fieldSetFlags()[2];
    }


    /**
      * Clears the value of the 'pathID' field.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.MacroPath.Builder clearPathID() {
      fieldSetFlags()[2] = false;
      return this;
    }

    /**
      * Gets the value of the 'flow' field.
      * @return The value.
      */
    public double getFlow() {
      return flow;
    }


    /**
      * Sets the value of the 'flow' field.
      * @param value The value of 'flow'.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.MacroPath.Builder setFlow(double value) {
      validate(fields()[3], value);
      this.flow = value;
      fieldSetFlags()[3] = true;
      return this;
    }

    /**
      * Checks whether the 'flow' field has been set.
      * @return True if the 'flow' field has been set, false otherwise.
      */
    public boolean hasFlow() {
      return fieldSetFlags()[3];
    }


    /**
      * Clears the value of the 'flow' field.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.MacroPath.Builder clearFlow() {
      fieldSetFlags()[3] = false;
      return this;
    }

    /**
      * Gets the value of the 'totalFlow' field.
      * @return The value.
      */
    public double getTotalFlow() {
      return totalFlow;
    }


    /**
      * Sets the value of the 'totalFlow' field.
      * @param value The value of 'totalFlow'.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.MacroPath.Builder setTotalFlow(double value) {
      validate(fields()[4], value);
      this.totalFlow = value;
      fieldSetFlags()[4] = true;
      return this;
    }

    /**
      * Checks whether the 'totalFlow' field has been set.
      * @return True if the 'totalFlow' field has been set, false otherwise.
      */
    public boolean hasTotalFlow() {
      return fieldSetFlags()[4];
    }


    /**
      * Clears the value of the 'totalFlow' field.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.MacroPath.Builder clearTotalFlow() {
      fieldSetFlags()[4] = false;
      return this;
    }

    /**
      * Gets the value of the 'nodes' field.
      * @return The value.
      */
    public java.util.List<java.lang.Long> getNodes() {
      return nodes;
    }


    /**
      * Sets the value of the 'nodes' field.
      * @param value The value of 'nodes'.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.MacroPath.Builder setNodes(java.util.List<java.lang.Long> value) {
      validate(fields()[5], value);
      this.nodes = value;
      fieldSetFlags()[5] = true;
      return this;
    }

    /**
      * Checks whether the 'nodes' field has been set.
      * @return True if the 'nodes' field has been set, false otherwise.
      */
    public boolean hasNodes() {
      return fieldSetFlags()[5];
    }


    /**
      * Clears the value of the 'nodes' field.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.MacroPath.Builder clearNodes() {
      nodes = null;
      fieldSetFlags()[5] = false;
      return this;
    }

    @Override
    @SuppressWarnings("unchecked")
    public MacroPath build() {
      try {
        MacroPath record = new MacroPath();
        record.fromZone = fieldSetFlags()[0] ? this.fromZone : (java.lang.Integer) defaultValue(fields()[0]);
        record.toZone = fieldSetFlags()[1] ? this.toZone : (java.lang.Integer) defaultValue(fields()[1]);
        record.pathID = fieldSetFlags()[2] ? this.pathID : (java.lang.Integer) defaultValue(fields()[2]);
        record.flow = fieldSetFlags()[3] ? this.flow : (java.lang.Double) defaultValue(fields()[3]);
        record.totalFlow = fieldSetFlags()[4] ? this.totalFlow : (java.lang.Double) defaultValue(fields()[4]);
        record.nodes = fieldSetFlags()[5] ? this.nodes : (java.util.List<java.lang.Long>) defaultValue(fields()[5]);
        return record;
      } catch (org.apache.avro.AvroMissingFieldException e) {
        throw e;
      } catch (java.lang.Exception e) {
        throw new org.apache.avro.AvroRuntimeException(e);
      }
    }
  }

  @SuppressWarnings("unchecked")
  private static final org.apache.avro.io.DatumWriter<MacroPath>
    WRITER$ = (org.apache.avro.io.DatumWriter<MacroPath>)MODEL$.createDatumWriter(SCHEMA$);

  @Override public void writeExternal(java.io.ObjectOutput out)
    throws java.io.IOException {
    WRITER$.write(this, SpecificData.getEncoder(out));
  }

  @SuppressWarnings("unchecked")
  private static final org.apache.avro.io.DatumReader<MacroPath>
    READER$ = (org.apache.avro.io.DatumReader<MacroPath>)MODEL$.createDatumReader(SCHEMA$);

  @Override public void readExternal(java.io.ObjectInput in)
    throws java.io.IOException {
    READER$.read(this, SpecificData.getDecoder(in));
  }

  @Override protected boolean hasCustomCoders() { return true; }

  @Override public void customEncode(org.apache.avro.io.Encoder out)
    throws java.io.IOException
  {
    out.writeInt(this.fromZone);

    out.writeInt(this.toZone);

    out.writeInt(this.pathID);

    out.writeDouble(this.flow);

    out.writeDouble(this.totalFlow);

    long size0 = this.nodes.size();
    out.writeArrayStart();
    out.setItemCount(size0);
    long actualSize0 = 0;
    for (java.lang.Long e0: this.nodes) {
      actualSize0++;
      out.startItem();
      out.writeLong(e0);
    }
    out.writeArrayEnd();
    if (actualSize0 != size0)
      throw new java.util.ConcurrentModificationException("Array-size written was " + size0 + ", but element count was " + actualSize0 + ".");

  }

  @Override public void customDecode(org.apache.avro.io.ResolvingDecoder in)
    throws java.io.IOException
  {
    org.apache.avro.Schema.Field[] fieldOrder = in.readFieldOrderIfDiff();
    if (fieldOrder == null) {
      this.fromZone = in.readInt();

      this.toZone = in.readInt();

      this.pathID = in.readInt();

      this.flow = in.readDouble();

      this.totalFlow = in.readDouble();

      long size0 = in.readArrayStart();
      java.util.List<java.lang.Long> a0 = this.nodes;
      if (a0 == null) {
        a0 = new SpecificData.Array<java.lang.Long>((int)size0, SCHEMA$.getField("nodes").schema());
        this.nodes = a0;
      } else a0.clear();
      SpecificData.Array<java.lang.Long> ga0 = (a0 instanceof SpecificData.Array ? (SpecificData.Array<java.lang.Long>)a0 : null);
      for ( ; 0 < size0; size0 = in.arrayNext()) {
        for ( ; size0 != 0; size0--) {
          java.lang.Long e0 = (ga0 != null ? ga0.peek() : null);
          e0 = in.readLong();
          a0.add(e0);
        }
      }

    } else {
      for (int i = 0; i < 6; i++) {
        switch (fieldOrder[i].pos()) {
        case 0:
          this.fromZone = in.readInt();
          break;

        case 1:
          this.toZone = in.readInt();
          break;

        case 2:
          this.pathID = in.readInt();
          break;

        case 3:
          this.flow = in.readDouble();
          break;

        case 4:
          this.totalFlow = in.readDouble();
          break;

        case 5:
          long size0 = in.readArrayStart();
          java.util.List<java.lang.Long> a0 = this.nodes;
          if (a0 == null) {
            a0 = new SpecificData.Array<java.lang.Long>((int)size0, SCHEMA$.getField("nodes").schema());
            this.nodes = a0;
          } else a0.clear();
          SpecificData.Array<java.lang.Long> ga0 = (a0 instanceof SpecificData.Array ? (SpecificData.Array<java.lang.Long>)a0 : null);
          for ( ; 0 < size0; size0 = in.arrayNext()) {
            for ( ; size0 != 0; size0--) {
              java.lang.Long e0 = (ga0 != null ? ga0.peek() : null);
              e0 = in.readLong();
              a0.add(e0);
            }
          }
          break;

        default:
          throw new java.io.IOException("Corrupt ResolvingDecoder.");
        }
      }
    }
  }
}










