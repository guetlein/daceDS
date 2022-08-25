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
public class ResourceFile extends org.apache.avro.specific.SpecificRecordBase implements org.apache.avro.specific.SpecificRecord {
  private static final long serialVersionUID = 5605044912525182188L;
  public static final org.apache.avro.Schema SCHEMA$ = new org.apache.avro.Schema.Parser().parse("{\"type\":\"record\",\"name\":\"ResourceFile\",\"namespace\":\"eu.fau.cs7.daceDS.datamodel\",\"fields\":[{\"name\":\"ID\",\"type\":\"string\"},{\"name\":\"Type\",\"type\":\"string\"},{\"name\":\"File\",\"type\":[\"null\",\"bytes\"],\"default\":null},{\"name\":\"FileReference\",\"type\":[\"null\",\"string\"],\"default\":null}]}");
  public static org.apache.avro.Schema getClassSchema() { return SCHEMA$; }

  private static SpecificData MODEL$ = new SpecificData();

  private static final BinaryMessageEncoder<ResourceFile> ENCODER =
      new BinaryMessageEncoder<ResourceFile>(MODEL$, SCHEMA$);

  private static final BinaryMessageDecoder<ResourceFile> DECODER =
      new BinaryMessageDecoder<ResourceFile>(MODEL$, SCHEMA$);

  /**
   * Return the BinaryMessageEncoder instance used by this class.
   * @return the message encoder used by this class
   */
  public static BinaryMessageEncoder<ResourceFile> getEncoder() {
    return ENCODER;
  }

  /**
   * Return the BinaryMessageDecoder instance used by this class.
   * @return the message decoder used by this class
   */
  public static BinaryMessageDecoder<ResourceFile> getDecoder() {
    return DECODER;
  }

  /**
   * Create a new BinaryMessageDecoder instance for this class that uses the specified {@link SchemaStore}.
   * @param resolver a {@link SchemaStore} used to find schemas by fingerprint
   * @return a BinaryMessageDecoder instance for this class backed by the given SchemaStore
   */
  public static BinaryMessageDecoder<ResourceFile> createDecoder(SchemaStore resolver) {
    return new BinaryMessageDecoder<ResourceFile>(MODEL$, SCHEMA$, resolver);
  }

  /**
   * Serializes this ResourceFile to a ByteBuffer.
   * @return a buffer holding the serialized data for this instance
   * @throws java.io.IOException if this instance could not be serialized
   */
  public java.nio.ByteBuffer toByteBuffer() throws java.io.IOException {
    return ENCODER.encode(this);
  }

  /**
   * Deserializes a ResourceFile from a ByteBuffer.
   * @param b a byte buffer holding serialized data for an instance of this class
   * @return a ResourceFile instance decoded from the given buffer
   * @throws java.io.IOException if the given bytes could not be deserialized into an instance of this class
   */
  public static ResourceFile fromByteBuffer(
      java.nio.ByteBuffer b) throws java.io.IOException {
    return DECODER.decode(b);
  }

   private java.lang.CharSequence ID;
   private java.lang.CharSequence Type;
   private java.nio.ByteBuffer File;
   private java.lang.CharSequence FileReference;

  /**
   * Default constructor.  Note that this does not initialize fields
   * to their default values from the schema.  If that is desired then
   * one should use <code>newBuilder()</code>.
   */
  public ResourceFile() {}

  /**
   * All-args constructor.
   * @param ID The new value for ID
   * @param Type The new value for Type
   * @param File The new value for File
   * @param FileReference The new value for FileReference
   */
  public ResourceFile(java.lang.CharSequence ID, java.lang.CharSequence Type, java.nio.ByteBuffer File, java.lang.CharSequence FileReference) {
    this.ID = ID;
    this.Type = Type;
    this.File = File;
    this.FileReference = FileReference;
  }

  public org.apache.avro.specific.SpecificData getSpecificData() { return MODEL$; }
  public org.apache.avro.Schema getSchema() { return SCHEMA$; }
  // Used by DatumWriter.  Applications should not call.
  public java.lang.Object get(int field$) {
    switch (field$) {
    case 0: return ID;
    case 1: return Type;
    case 2: return File;
    case 3: return FileReference;
    default: throw new IndexOutOfBoundsException("Invalid index: " + field$);
    }
  }

  // Used by DatumReader.  Applications should not call.
  @SuppressWarnings(value="unchecked")
  public void put(int field$, java.lang.Object value$) {
    switch (field$) {
    case 0: ID = (java.lang.CharSequence)value$; break;
    case 1: Type = (java.lang.CharSequence)value$; break;
    case 2: File = (java.nio.ByteBuffer)value$; break;
    case 3: FileReference = (java.lang.CharSequence)value$; break;
    default: throw new IndexOutOfBoundsException("Invalid index: " + field$);
    }
  }

  /**
   * Gets the value of the 'ID' field.
   * @return The value of the 'ID' field.
   */
  public java.lang.CharSequence getID() {
    return ID;
  }


  /**
   * Sets the value of the 'ID' field.
   * @param value the value to set.
   */
  public void setID(java.lang.CharSequence value) {
    this.ID = value;
  }

  /**
   * Gets the value of the 'Type' field.
   * @return The value of the 'Type' field.
   */
  public java.lang.CharSequence getType() {
    return Type;
  }


  /**
   * Sets the value of the 'Type' field.
   * @param value the value to set.
   */
  public void setType(java.lang.CharSequence value) {
    this.Type = value;
  }

  /**
   * Gets the value of the 'File' field.
   * @return The value of the 'File' field.
   */
  public java.nio.ByteBuffer getFile() {
    return File;
  }


  /**
   * Sets the value of the 'File' field.
   * @param value the value to set.
   */
  public void setFile(java.nio.ByteBuffer value) {
    this.File = value;
  }

  /**
   * Gets the value of the 'FileReference' field.
   * @return The value of the 'FileReference' field.
   */
  public java.lang.CharSequence getFileReference() {
    return FileReference;
  }


  /**
   * Sets the value of the 'FileReference' field.
   * @param value the value to set.
   */
  public void setFileReference(java.lang.CharSequence value) {
    this.FileReference = value;
  }

  /**
   * Creates a new ResourceFile RecordBuilder.
   * @return A new ResourceFile RecordBuilder
   */
  public static eu.fau.cs7.daceDS.datamodel.ResourceFile.Builder newBuilder() {
    return new eu.fau.cs7.daceDS.datamodel.ResourceFile.Builder();
  }

  /**
   * Creates a new ResourceFile RecordBuilder by copying an existing Builder.
   * @param other The existing builder to copy.
   * @return A new ResourceFile RecordBuilder
   */
  public static eu.fau.cs7.daceDS.datamodel.ResourceFile.Builder newBuilder(eu.fau.cs7.daceDS.datamodel.ResourceFile.Builder other) {
    if (other == null) {
      return new eu.fau.cs7.daceDS.datamodel.ResourceFile.Builder();
    } else {
      return new eu.fau.cs7.daceDS.datamodel.ResourceFile.Builder(other);
    }
  }

  /**
   * Creates a new ResourceFile RecordBuilder by copying an existing ResourceFile instance.
   * @param other The existing instance to copy.
   * @return A new ResourceFile RecordBuilder
   */
  public static eu.fau.cs7.daceDS.datamodel.ResourceFile.Builder newBuilder(eu.fau.cs7.daceDS.datamodel.ResourceFile other) {
    if (other == null) {
      return new eu.fau.cs7.daceDS.datamodel.ResourceFile.Builder();
    } else {
      return new eu.fau.cs7.daceDS.datamodel.ResourceFile.Builder(other);
    }
  }

  /**
   * RecordBuilder for ResourceFile instances.
   */
  @org.apache.avro.specific.AvroGenerated
  public static class Builder extends org.apache.avro.specific.SpecificRecordBuilderBase<ResourceFile>
    implements org.apache.avro.data.RecordBuilder<ResourceFile> {

    private java.lang.CharSequence ID;
    private java.lang.CharSequence Type;
    private java.nio.ByteBuffer File;
    private java.lang.CharSequence FileReference;

    /** Creates a new Builder */
    private Builder() {
      super(SCHEMA$);
    }

    /**
     * Creates a Builder by copying an existing Builder.
     * @param other The existing Builder to copy.
     */
    private Builder(eu.fau.cs7.daceDS.datamodel.ResourceFile.Builder other) {
      super(other);
      if (isValidValue(fields()[0], other.ID)) {
        this.ID = data().deepCopy(fields()[0].schema(), other.ID);
        fieldSetFlags()[0] = other.fieldSetFlags()[0];
      }
      if (isValidValue(fields()[1], other.Type)) {
        this.Type = data().deepCopy(fields()[1].schema(), other.Type);
        fieldSetFlags()[1] = other.fieldSetFlags()[1];
      }
      if (isValidValue(fields()[2], other.File)) {
        this.File = data().deepCopy(fields()[2].schema(), other.File);
        fieldSetFlags()[2] = other.fieldSetFlags()[2];
      }
      if (isValidValue(fields()[3], other.FileReference)) {
        this.FileReference = data().deepCopy(fields()[3].schema(), other.FileReference);
        fieldSetFlags()[3] = other.fieldSetFlags()[3];
      }
    }

    /**
     * Creates a Builder by copying an existing ResourceFile instance
     * @param other The existing instance to copy.
     */
    private Builder(eu.fau.cs7.daceDS.datamodel.ResourceFile other) {
      super(SCHEMA$);
      if (isValidValue(fields()[0], other.ID)) {
        this.ID = data().deepCopy(fields()[0].schema(), other.ID);
        fieldSetFlags()[0] = true;
      }
      if (isValidValue(fields()[1], other.Type)) {
        this.Type = data().deepCopy(fields()[1].schema(), other.Type);
        fieldSetFlags()[1] = true;
      }
      if (isValidValue(fields()[2], other.File)) {
        this.File = data().deepCopy(fields()[2].schema(), other.File);
        fieldSetFlags()[2] = true;
      }
      if (isValidValue(fields()[3], other.FileReference)) {
        this.FileReference = data().deepCopy(fields()[3].schema(), other.FileReference);
        fieldSetFlags()[3] = true;
      }
    }

    /**
      * Gets the value of the 'ID' field.
      * @return The value.
      */
    public java.lang.CharSequence getID() {
      return ID;
    }


    /**
      * Sets the value of the 'ID' field.
      * @param value The value of 'ID'.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.ResourceFile.Builder setID(java.lang.CharSequence value) {
      validate(fields()[0], value);
      this.ID = value;
      fieldSetFlags()[0] = true;
      return this;
    }

    /**
      * Checks whether the 'ID' field has been set.
      * @return True if the 'ID' field has been set, false otherwise.
      */
    public boolean hasID() {
      return fieldSetFlags()[0];
    }


    /**
      * Clears the value of the 'ID' field.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.ResourceFile.Builder clearID() {
      ID = null;
      fieldSetFlags()[0] = false;
      return this;
    }

    /**
      * Gets the value of the 'Type' field.
      * @return The value.
      */
    public java.lang.CharSequence getType() {
      return Type;
    }


    /**
      * Sets the value of the 'Type' field.
      * @param value The value of 'Type'.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.ResourceFile.Builder setType(java.lang.CharSequence value) {
      validate(fields()[1], value);
      this.Type = value;
      fieldSetFlags()[1] = true;
      return this;
    }

    /**
      * Checks whether the 'Type' field has been set.
      * @return True if the 'Type' field has been set, false otherwise.
      */
    public boolean hasType() {
      return fieldSetFlags()[1];
    }


    /**
      * Clears the value of the 'Type' field.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.ResourceFile.Builder clearType() {
      Type = null;
      fieldSetFlags()[1] = false;
      return this;
    }

    /**
      * Gets the value of the 'File' field.
      * @return The value.
      */
    public java.nio.ByteBuffer getFile() {
      return File;
    }


    /**
      * Sets the value of the 'File' field.
      * @param value The value of 'File'.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.ResourceFile.Builder setFile(java.nio.ByteBuffer value) {
      validate(fields()[2], value);
      this.File = value;
      fieldSetFlags()[2] = true;
      return this;
    }

    /**
      * Checks whether the 'File' field has been set.
      * @return True if the 'File' field has been set, false otherwise.
      */
    public boolean hasFile() {
      return fieldSetFlags()[2];
    }


    /**
      * Clears the value of the 'File' field.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.ResourceFile.Builder clearFile() {
      File = null;
      fieldSetFlags()[2] = false;
      return this;
    }

    /**
      * Gets the value of the 'FileReference' field.
      * @return The value.
      */
    public java.lang.CharSequence getFileReference() {
      return FileReference;
    }


    /**
      * Sets the value of the 'FileReference' field.
      * @param value The value of 'FileReference'.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.ResourceFile.Builder setFileReference(java.lang.CharSequence value) {
      validate(fields()[3], value);
      this.FileReference = value;
      fieldSetFlags()[3] = true;
      return this;
    }

    /**
      * Checks whether the 'FileReference' field has been set.
      * @return True if the 'FileReference' field has been set, false otherwise.
      */
    public boolean hasFileReference() {
      return fieldSetFlags()[3];
    }


    /**
      * Clears the value of the 'FileReference' field.
      * @return This builder.
      */
    public eu.fau.cs7.daceDS.datamodel.ResourceFile.Builder clearFileReference() {
      FileReference = null;
      fieldSetFlags()[3] = false;
      return this;
    }

    @Override
    @SuppressWarnings("unchecked")
    public ResourceFile build() {
      try {
        ResourceFile record = new ResourceFile();
        record.ID = fieldSetFlags()[0] ? this.ID : (java.lang.CharSequence) defaultValue(fields()[0]);
        record.Type = fieldSetFlags()[1] ? this.Type : (java.lang.CharSequence) defaultValue(fields()[1]);
        record.File = fieldSetFlags()[2] ? this.File : (java.nio.ByteBuffer) defaultValue(fields()[2]);
        record.FileReference = fieldSetFlags()[3] ? this.FileReference : (java.lang.CharSequence) defaultValue(fields()[3]);
        return record;
      } catch (org.apache.avro.AvroMissingFieldException e) {
        throw e;
      } catch (java.lang.Exception e) {
        throw new org.apache.avro.AvroRuntimeException(e);
      }
    }
  }

  @SuppressWarnings("unchecked")
  private static final org.apache.avro.io.DatumWriter<ResourceFile>
    WRITER$ = (org.apache.avro.io.DatumWriter<ResourceFile>)MODEL$.createDatumWriter(SCHEMA$);

  @Override public void writeExternal(java.io.ObjectOutput out)
    throws java.io.IOException {
    WRITER$.write(this, SpecificData.getEncoder(out));
  }

  @SuppressWarnings("unchecked")
  private static final org.apache.avro.io.DatumReader<ResourceFile>
    READER$ = (org.apache.avro.io.DatumReader<ResourceFile>)MODEL$.createDatumReader(SCHEMA$);

  @Override public void readExternal(java.io.ObjectInput in)
    throws java.io.IOException {
    READER$.read(this, SpecificData.getDecoder(in));
  }

  @Override protected boolean hasCustomCoders() { return true; }

  @Override public void customEncode(org.apache.avro.io.Encoder out)
    throws java.io.IOException
  {
    out.writeString(this.ID);

    out.writeString(this.Type);

    if (this.File == null) {
      out.writeIndex(0);
      out.writeNull();
    } else {
      out.writeIndex(1);
      out.writeBytes(this.File);
    }

    if (this.FileReference == null) {
      out.writeIndex(0);
      out.writeNull();
    } else {
      out.writeIndex(1);
      out.writeString(this.FileReference);
    }

  }

  @Override public void customDecode(org.apache.avro.io.ResolvingDecoder in)
    throws java.io.IOException
  {
    org.apache.avro.Schema.Field[] fieldOrder = in.readFieldOrderIfDiff();
    if (fieldOrder == null) {
      this.ID = in.readString(this.ID instanceof Utf8 ? (Utf8)this.ID : null);

      this.Type = in.readString(this.Type instanceof Utf8 ? (Utf8)this.Type : null);

      if (in.readIndex() != 1) {
        in.readNull();
        this.File = null;
      } else {
        this.File = in.readBytes(this.File);
      }

      if (in.readIndex() != 1) {
        in.readNull();
        this.FileReference = null;
      } else {
        this.FileReference = in.readString(this.FileReference instanceof Utf8 ? (Utf8)this.FileReference : null);
      }

    } else {
      for (int i = 0; i < 4; i++) {
        switch (fieldOrder[i].pos()) {
        case 0:
          this.ID = in.readString(this.ID instanceof Utf8 ? (Utf8)this.ID : null);
          break;

        case 1:
          this.Type = in.readString(this.Type instanceof Utf8 ? (Utf8)this.Type : null);
          break;

        case 2:
          if (in.readIndex() != 1) {
            in.readNull();
            this.File = null;
          } else {
            this.File = in.readBytes(this.File);
          }
          break;

        case 3:
          if (in.readIndex() != 1) {
            in.readNull();
            this.FileReference = null;
          } else {
            this.FileReference = in.readString(this.FileReference instanceof Utf8 ? (Utf8)this.FileReference : null);
          }
          break;

        default:
          throw new java.io.IOException("Corrupt ResolvingDecoder.");
        }
      }
    }
  }
}









