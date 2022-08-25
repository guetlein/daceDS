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
public class SyncMsg extends org.apache.avro.specific.SpecificRecordBase implements org.apache.avro.specific.SpecificRecord {
  private static final long serialVersionUID = -8261367994090138756L;
  public static final org.apache.avro.Schema SCHEMA$ = new org.apache.avro.Schema.Parser().parse("{\"type\":\"record\",\"name\":\"SyncMsg\",\"namespace\":\"eu.fau.cs7.daceDS.datamodel\",\"fields\":[{\"name\":\"Sender\",\"type\":\"string\"},{\"name\":\"Action\",\"type\":\"string\",\"default\":\"request\"},{\"name\":\"Time\",\"type\":\"long\"},{\"name\":\"Epoch\",\"type\":\"int\",\"default\":0},{\"name\":\"Messages\",\"type\":{\"type\":\"map\",\"values\":\"long\",\"default\":{}}}]}");
  public static org.apache.avro.Schema getClassSchema() { return SCHEMA$; }

  private static SpecificData MODEL$ = new SpecificData();

  private static final BinaryMessageEncoder<SyncMsg> ENCODER =
      new BinaryMessageEncoder<SyncMsg>(MODEL$, SCHEMA$);

  private static final BinaryMessageDecoder<SyncMsg> DECODER =
      new BinaryMessageDecoder<SyncMsg>(MODEL$, SCHEMA$);

  /**
   * Return the BinaryMessageEncoder instance used by this class.
   * @return the message encoder used by this class
   */
  public static BinaryMessageEncoder<SyncMsg> getEncoder() {
    return ENCODER;
  }

  /**
   * Return the BinaryMessageDecoder instance used by this class.
   * @return the message decoder used by this class
   */
  public static BinaryMessageDecoder<SyncMsg> getDecoder() {
    return DECODER;
  }

  /**
   * Create a new BinaryMessageDecoder instance for this class that uses the specified {@link SchemaStore}.
   * @param resolver a {@link SchemaStore} used to find schemas by fingerprint
   * @return a BinaryMessageDecoder instance for this class backed by the given SchemaStore
   */
  public static BinaryMessageDecoder<SyncMsg> createDecoder(SchemaStore resolver) {
    return new BinaryMessageDecoder<SyncMsg>(MODEL$, SCHEMA$, resolver);
  }

  /**
   * Serializes this SyncMsg to a ByteBuffer.
   * @return a buffer holding the serialized data for this instance
   * @throws java.io.IOException if this instance could not be serialized
   */
  public java.nio.ByteBuffer toByteBuffer() throws java.io.IOException {
    return ENCODER.encode(this);
  }

  /**
   * Deserializes a SyncMsg from a ByteBuffer.
   * @param b a byte buffer holding serialized data for an instance of this class
   * @return a SyncMsg instance decoded from the given buffer
   * @throws java.io.IOException if the given bytes could not be deserialized into an instance of this class
   */
  public static SyncMsg fromByteBuffer(
      java.nio.ByteBuffer b) throws java.io.IOException {
    return DECODER.decode(b);
  }

   private java.lang.CharSequence Sender;
   private java.lang.CharSequence Action;
   private long Time;
   private int Epoch;
   private java.util.Map<java.lang.CharSequence,java.lang.Long> Messages;

  /**
   * Default constructor.  Note that this does not initialize fields
   * to their default values from the schema.  If that is desired then
   * one should use <code>newBuilder()</code>.
   */
  public SyncMsg() {}

  /**
   * All-args constructor.
   * @param Sender The new value for Sender
   * @param Action The new value for Action
   * @param Time The new value for Time
   * @param Epoch The new value for Epoch
   * @param Messages The new value for Messages
   */
  public SyncMsg(java.lang.CharSequence Sender, java.lang.CharSequence Action, java.lang.Long Time, java.lang.Integer Epoch, java.util.Map<java.lang.CharSequence,java.lang.Long> Messages) {
    this.Sender = Sender;
    this.Action = Action;
    this.Time = Time;
    this.Epoch = Epoch;
    this.Messages = Messages;
  }

  public org.apache.avro.specific.SpecificData getSpecificData() { return MODEL$; }
  public org.apache.avro.Schema getSchema() { return SCHEMA$; }
  // Used by DatumWriter.  Applications should not call.
  public java.lang.Object get(int field$) {
    switch (field$) {
    case 0: return Sender;
    case 1: return Action;
    case 2: return Time;
    case 3: return Epoch;
    case 4: return Messages;
    default: throw new IndexOutOfBoundsException("Invalid index: " + field$);
    }
  }

  // Used by DatumReader.  Applications should not call.
  @SuppressWarnings(value="unchecked")
  public void put(int field$, java.lang.Object value$) {
    switch (field$) {
    case 0: Sender = (java.lang.CharSequence)value$; break;
    case 1: Action = (java.lang.CharSequence)value$; break;
    case 2: Time = (java.lang.Long)value$; break;
    case 3: Epoch = (java.lang.Integer)value$; break;
    case 4: Messages = (java.util.Map<java.lang.CharSequence,java.lang.Long>)value$; break;
    default: throw new IndexOutOfBoundsException("Invalid index: " + field$);
    }
  }

  /**
   * Gets the value of the 'Sender' field.
   * @return The value of the 'Sender' field.
   */
  public java.lang.CharSequence getSender() {
    return Sender;
  }


  /**
   * Sets the value of the 'Sender' field.
   * @param value the value to set.
   */
  public void setSender(java.lang.CharSequence value) {
    this.Sender = value;
  }

  /**
   * Gets the value of the 'Action' field.
   * @return The value of the 'Action' field.
   */
  public java.lang.CharSequence getAction() {
    return Action;
  }


  /**
   * Sets the value of the 'Action' field.
   * @param value the value to set.
   */
  public void setAction(java.lang.CharSequence value) {
    this.Action = value;
  }

  /**
   * Gets the value of the 'Time' field.
   * @return The value of the 'Time' field.
   */
  public long getTime() {
    return Time;
  }


  /**
   * Sets the value of the 'Time' field.
   * @param value the value to set.
   */
  public void setTime(long value) {
    this.Time = value;
  }

  /**
   * Gets the value of the 'Epoch' field.
   * @return The value of the 'Epoch' field.
   */
  public int getEpoch() {
    return Epoch;
  }


  /**
   * Sets the value of the 'Epoch' field.
   * @param value the value to set.
   */
  public void setEpoch(int value) {
    this.Epoch = value;
  }

  /**
   * Gets the value of the 'Messages' field.
   * @return The value of the 'Messages' field.
   */
  public java.util.Map<java.lang.CharSequence,java.lang.Long> getMessages() {
    return Messages;
  }


  /**
   * Sets the value of the 'Messages' field.
   * @param value the value to set.
   */
  public void setMessages(java.util.Map<java.lang.CharSequence,java.lang.Long> value) {
    this.Messages = value;
  }

  /**
   * Creates a new SyncMsg RecordBuilder.
   * @return A new SyncMsg RecordBuilder
   */
  public static eu.fau.cs7.daceDS.datamodel.SyncMsg.Builder newBuilder() {
    return new eu.fau.cs7.daceDS.datamodel.SyncMsg.Builder();
  }

  /**
   * Creates a new SyncMsg RecordBuilder by copying an existing Builder.
   * @param other The existing builder to copy.
   * @return A new SyncMsg RecordBuilder
   */
  public static eu.fau.cs7.daceDS.datamodel.SyncMsg.Builder newBuilder(eu.fau.cs7.daceDS.datamodel.SyncMsg.Builder other) {
    if (other == null) {
      return new eu.fau.cs7.daceDS.datamodel.SyncMsg.Builder();
    } else {
      return new eu.fau.cs7.daceDS.datamodel.SyncMsg.Builder(other);
    }
  }

  /**
   * Creates a new SyncMsg RecordBuilder by copying an existing SyncMsg instance.
   * @param other The existing instance to copy.
   * @return A new SyncMsg RecordBuilder
   */
  public static eu.fau.cs7.daceDS.datamodel.SyncMsg.Builder newBuilder(eu.fau.cs7.daceDS.datamodel.SyncMsg other) {
    if (other == null) {
      return new eu.fau.cs7.daceDS.datamodel.SyncMsg.Builder();
    } else {
      return new eu.fau.cs7.daceDS.datamodel.SyncMsg.Builder(other);
    }
  }

  /**
   * RecordBuilder for SyncMsg instances.
   */
  @org.apache.avro.specific.AvroGenerated
  public static class Builder extends org.apache.avro.specific.SpecificRecordBuilderBase<SyncMsg>
    implements org.apache.avro.data.RecordBuilder<SyncMsg> {

    private java.lang.CharSequence Sender;
    private java.lang.CharSequence Action;
    private long Time;
    private int Epoch;
    private java.util.Map<java.lang.CharSequence,java.lang.Long> Messages;

    /** Creates a new Builder */
    private Builder() {
      super(SCHEMA$);
    }

    /**
     * Creates a Builder by copying an existing Builder.
     * @param other The existing Builder to copy.
     */
    private Builder(eu.fau.cs7.daceDS.datamodel.SyncMsg.Builder other) {
      super(other);
      if (isValidValue(fields()[0], other.Sender)) {
        this.Sender = data().deepCopy(fields()[0].schema(), other.Sender);
        fieldSetFlags()[0] = other.fieldSetFlags()[0];
      }
      if (isValidValue(fields()[1], other.Action)) {
        this.Action = data().deepCopy(fields()[1].schema(), other.Action);
        fieldSetFlags()[1] = other.fieldSetFlags()[1];
      }
      if (isValidValue(fields()[2], other.Time)) {
        this.Time = data().deepCopy(fields()[2].schema(), other.Time);
        fieldSetFlags()[2] = other.fieldSetFlags()[2];
      }
      if (isValidValue(fields()[3], other.Epoch)) {
        this.Epoch = data().deepCopy(fields()[3].schema(), other.Epoch);
        fieldSetFlags()[3] = other.fieldSetFlags()[3];
      }
      if (isValidValue(fields()[4], other.Messages)) {
        this.Messages = data().deepCopy(fields()[4].schema(), other.Messages);
        fieldSetFlags()[4] = other.fieldSetFlags()[4];
      }
    }

    /**
     * Creates a Builder by copying an existing SyncMsg instance
     * @param other The existing instance to copy.
     */
    private Builder(eu.fau.cs7.daceDS.datamodel.SyncMsg other) {
      super(SCHEMA$);
      if (isValidValue(fields()[0], other.Sender)) {
        this.Sender = data().deepCopy(fields()[0].schema(), other.Sender);
        fieldSetFlags()[0] = true;
      }
      if (isValidValue(fields()[1], other.Action)) {
        this.Action = data().deepCopy(fields()[1].schema(), other.Action);
        fieldSetFlags()[1] = true;
      }
      if (isValidValue(fields()[2], other.Time)) {
        this.Time = data().deepCopy(fields()[2].schema(), other.Time);
        fieldSetFlags()[2] = true;
      }
      if (isValidValue(fields()[3], other.Epoch)) {
        this.Epoch = data().deepCopy(fields()[3].schema(), other.Epoch);
        fieldSetFlags()[3] = true;
      }
      if (isValidValue(fields()[4], other.Messages)) {
        this.Messages = data().deepCopy(fields()[4].schema(), other.Messages);
        fieldSetFlags()[4] = true;
      }
    }

    /**
      * Gets the value of the 'Sender' field.
      * @return The value.
      */
    public java.lang.CharSequence getSender() {
      return Sender;
    }


    /**
      * Sets the value of the 'Sender' field.
      * @param value The value of 'Sender'.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.SyncMsg.Builder setSender(java.lang.CharSequence value) {
      validate(fields()[0], value);
      this.Sender = value;
      fieldSetFlags()[0] = true;
      return this;
    }

    /**
      * Checks whether the 'Sender' field has been set.
      * @return True if the 'Sender' field has been set, false otherwise.
      */
    public boolean hasSender() {
      return fieldSetFlags()[0];
    }


    /**
      * Clears the value of the 'Sender' field.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.SyncMsg.Builder clearSender() {
      Sender = null;
      fieldSetFlags()[0] = false;
      return this;
    }

    /**
      * Gets the value of the 'Action' field.
      * @return The value.
      */
    public java.lang.CharSequence getAction() {
      return Action;
    }


    /**
      * Sets the value of the 'Action' field.
      * @param value The value of 'Action'.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.SyncMsg.Builder setAction(java.lang.CharSequence value) {
      validate(fields()[1], value);
      this.Action = value;
      fieldSetFlags()[1] = true;
      return this;
    }

    /**
      * Checks whether the 'Action' field has been set.
      * @return True if the 'Action' field has been set, false otherwise.
      */
    public boolean hasAction() {
      return fieldSetFlags()[1];
    }


    /**
      * Clears the value of the 'Action' field.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.SyncMsg.Builder clearAction() {
      Action = null;
      fieldSetFlags()[1] = false;
      return this;
    }

    /**
      * Gets the value of the 'Time' field.
      * @return The value.
      */
    public long getTime() {
      return Time;
    }


    /**
      * Sets the value of the 'Time' field.
      * @param value The value of 'Time'.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.SyncMsg.Builder setTime(long value) {
      validate(fields()[2], value);
      this.Time = value;
      fieldSetFlags()[2] = true;
      return this;
    }

    /**
      * Checks whether the 'Time' field has been set.
      * @return True if the 'Time' field has been set, false otherwise.
      */
    public boolean hasTime() {
      return fieldSetFlags()[2];
    }


    /**
      * Clears the value of the 'Time' field.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.SyncMsg.Builder clearTime() {
      fieldSetFlags()[2] = false;
      return this;
    }

    /**
      * Gets the value of the 'Epoch' field.
      * @return The value.
      */
    public int getEpoch() {
      return Epoch;
    }


    /**
      * Sets the value of the 'Epoch' field.
      * @param value The value of 'Epoch'.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.SyncMsg.Builder setEpoch(int value) {
      validate(fields()[3], value);
      this.Epoch = value;
      fieldSetFlags()[3] = true;
      return this;
    }

    /**
      * Checks whether the 'Epoch' field has been set.
      * @return True if the 'Epoch' field has been set, false otherwise.
      */
    public boolean hasEpoch() {
      return fieldSetFlags()[3];
    }


    /**
      * Clears the value of the 'Epoch' field.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.SyncMsg.Builder clearEpoch() {
      fieldSetFlags()[3] = false;
      return this;
    }

    /**
      * Gets the value of the 'Messages' field.
      * @return The value.
      */
    public java.util.Map<java.lang.CharSequence,java.lang.Long> getMessages() {
      return Messages;
    }


    /**
      * Sets the value of the 'Messages' field.
      * @param value The value of 'Messages'.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.SyncMsg.Builder setMessages(java.util.Map<java.lang.CharSequence,java.lang.Long> value) {
      validate(fields()[4], value);
      this.Messages = value;
      fieldSetFlags()[4] = true;
      return this;
    }

    /**
      * Checks whether the 'Messages' field has been set.
      * @return True if the 'Messages' field has been set, false otherwise.
      */
    public boolean hasMessages() {
      return fieldSetFlags()[4];
    }


    /**
      * Clears the value of the 'Messages' field.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.SyncMsg.Builder clearMessages() {
      Messages = null;
      fieldSetFlags()[4] = false;
      return this;
    }

    @Override
    @SuppressWarnings("unchecked")
    public SyncMsg build() {
      try {
        SyncMsg record = new SyncMsg();
        record.Sender = fieldSetFlags()[0] ? this.Sender : (java.lang.CharSequence) defaultValue(fields()[0]);
        record.Action = fieldSetFlags()[1] ? this.Action : (java.lang.CharSequence) defaultValue(fields()[1]);
        record.Time = fieldSetFlags()[2] ? this.Time : (java.lang.Long) defaultValue(fields()[2]);
        record.Epoch = fieldSetFlags()[3] ? this.Epoch : (java.lang.Integer) defaultValue(fields()[3]);
        record.Messages = fieldSetFlags()[4] ? this.Messages : (java.util.Map<java.lang.CharSequence,java.lang.Long>) defaultValue(fields()[4]);
        return record;
      } catch (org.apache.avro.AvroMissingFieldException e) {
        throw e;
      } catch (java.lang.Exception e) {
        throw new org.apache.avro.AvroRuntimeException(e);
      }
    }
  }

  @SuppressWarnings("unchecked")
  private static final org.apache.avro.io.DatumWriter<SyncMsg>
    WRITER$ = (org.apache.avro.io.DatumWriter<SyncMsg>)MODEL$.createDatumWriter(SCHEMA$);

  @Override public void writeExternal(java.io.ObjectOutput out)
    throws java.io.IOException {
    WRITER$.write(this, SpecificData.getEncoder(out));
  }

  @SuppressWarnings("unchecked")
  private static final org.apache.avro.io.DatumReader<SyncMsg>
    READER$ = (org.apache.avro.io.DatumReader<SyncMsg>)MODEL$.createDatumReader(SCHEMA$);

  @Override public void readExternal(java.io.ObjectInput in)
    throws java.io.IOException {
    READER$.read(this, SpecificData.getDecoder(in));
  }

  @Override protected boolean hasCustomCoders() { return true; }

  @Override public void customEncode(org.apache.avro.io.Encoder out)
    throws java.io.IOException
  {
    out.writeString(this.Sender);

    out.writeString(this.Action);

    out.writeLong(this.Time);

    out.writeInt(this.Epoch);

    long size0 = this.Messages.size();
    out.writeMapStart();
    out.setItemCount(size0);
    long actualSize0 = 0;
    for (java.util.Map.Entry<java.lang.CharSequence, java.lang.Long> e0: this.Messages.entrySet()) {
      actualSize0++;
      out.startItem();
      out.writeString(e0.getKey());
      java.lang.Long v0 = e0.getValue();
      out.writeLong(v0);
    }
    out.writeMapEnd();
    if (actualSize0 != size0)
      throw new java.util.ConcurrentModificationException("Map-size written was " + size0 + ", but element count was " + actualSize0 + ".");

  }

  @Override public void customDecode(org.apache.avro.io.ResolvingDecoder in)
    throws java.io.IOException
  {
    org.apache.avro.Schema.Field[] fieldOrder = in.readFieldOrderIfDiff();
    if (fieldOrder == null) {
      this.Sender = in.readString(this.Sender instanceof Utf8 ? (Utf8)this.Sender : null);

      this.Action = in.readString(this.Action instanceof Utf8 ? (Utf8)this.Action : null);

      this.Time = in.readLong();

      this.Epoch = in.readInt();

      long size0 = in.readMapStart();
      java.util.Map<java.lang.CharSequence,java.lang.Long> m0 = this.Messages; // Need fresh name due to limitation of macro system
      if (m0 == null) {
        m0 = new java.util.HashMap<java.lang.CharSequence,java.lang.Long>((int)size0);
        this.Messages = m0;
      } else m0.clear();
      for ( ; 0 < size0; size0 = in.mapNext()) {
        for ( ; size0 != 0; size0--) {
          java.lang.CharSequence k0 = null;
          k0 = in.readString(k0 instanceof Utf8 ? (Utf8)k0 : null);
          java.lang.Long v0 = null;
          v0 = in.readLong();
          m0.put(k0, v0);
        }
      }

    } else {
      for (int i = 0; i < 5; i++) {
        switch (fieldOrder[i].pos()) {
        case 0:
          this.Sender = in.readString(this.Sender instanceof Utf8 ? (Utf8)this.Sender : null);
          break;

        case 1:
          this.Action = in.readString(this.Action instanceof Utf8 ? (Utf8)this.Action : null);
          break;

        case 2:
          this.Time = in.readLong();
          break;

        case 3:
          this.Epoch = in.readInt();
          break;

        case 4:
          long size0 = in.readMapStart();
          java.util.Map<java.lang.CharSequence,java.lang.Long> m0 = this.Messages; // Need fresh name due to limitation of macro system
          if (m0 == null) {
            m0 = new java.util.HashMap<java.lang.CharSequence,java.lang.Long>((int)size0);
            this.Messages = m0;
          } else m0.clear();
          for ( ; 0 < size0; size0 = in.mapNext()) {
            for ( ; size0 != 0; size0--) {
              java.lang.CharSequence k0 = null;
              k0 = in.readString(k0 instanceof Utf8 ? (Utf8)k0 : null);
              java.lang.Long v0 = null;
              v0 = in.readLong();
              m0.put(k0, v0);
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










