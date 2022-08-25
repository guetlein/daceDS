/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef CPP_RESOURCEFILE_HH_3631740942__H_
#define CPP_RESOURCEFILE_HH_3631740942__H_


#include <sstream>
#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"

namespace daceDS {
namespace datamodel {
struct _ResourceFile_avsc_Union__0__ {
private:
    size_t idx_;
    boost::any value_;
public:
    size_t idx() const { return idx_; }
    bool is_null() const {
        return (idx_ == 0);
    }
    void set_null() {
        idx_ = 0;
        value_ = boost::any();
    }
    std::vector<uint8_t> get_bytes() const;
    void set_bytes(const std::vector<uint8_t>& v);
    _ResourceFile_avsc_Union__0__();
};

struct _ResourceFile_avsc_Union__1__ {
private:
    size_t idx_;
    boost::any value_;
public:
    size_t idx() const { return idx_; }
    bool is_null() const {
        return (idx_ == 0);
    }
    void set_null() {
        idx_ = 0;
        value_ = boost::any();
    }
    std::string get_string() const;
    void set_string(const std::string& v);
    _ResourceFile_avsc_Union__1__();
};

struct ResourceFile {
    typedef _ResourceFile_avsc_Union__0__ File_t;
    typedef _ResourceFile_avsc_Union__1__ FileReference_t;
    std::string ID;
    std::string Type;
    File_t File;
    FileReference_t FileReference;
    ResourceFile() :
        ID(std::string()),
        Type(std::string()),
        File(File_t()),
        FileReference(FileReference_t())
        { }
};

inline
std::vector<uint8_t> _ResourceFile_avsc_Union__0__::get_bytes() const {
    if (idx_ != 1) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<std::vector<uint8_t> >(value_);
}

inline
void _ResourceFile_avsc_Union__0__::set_bytes(const std::vector<uint8_t>& v) {
    idx_ = 1;
    value_ = v;
}

inline
std::string _ResourceFile_avsc_Union__1__::get_string() const {
    if (idx_ != 1) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<std::string >(value_);
}

inline
void _ResourceFile_avsc_Union__1__::set_string(const std::string& v) {
    idx_ = 1;
    value_ = v;
}

inline _ResourceFile_avsc_Union__0__::_ResourceFile_avsc_Union__0__() : idx_(0) { }
inline _ResourceFile_avsc_Union__1__::_ResourceFile_avsc_Union__1__() : idx_(0) { }
}
}
namespace avro {
template<> struct codec_traits<daceDS::datamodel::_ResourceFile_avsc_Union__0__> {
    static void encode(Encoder& e, daceDS::datamodel::_ResourceFile_avsc_Union__0__ v) {
        e.encodeUnionIndex(v.idx());
        switch (v.idx()) {
        case 0:
            e.encodeNull();
            break;
        case 1:
            avro::encode(e, v.get_bytes());
            break;
        }
    }
    static void decode(Decoder& d, daceDS::datamodel::_ResourceFile_avsc_Union__0__& v) {
        size_t n = d.decodeUnionIndex();
        if (n >= 2) { throw avro::Exception("Union index too big"); }
        switch (n) {
        case 0:
            d.decodeNull();
            v.set_null();
            break;
        case 1:
            {
                std::vector<uint8_t> vv;
                avro::decode(d, vv);
                v.set_bytes(vv);
            }
            break;
        }
    }
};

template<> struct codec_traits<daceDS::datamodel::_ResourceFile_avsc_Union__1__> {
    static void encode(Encoder& e, daceDS::datamodel::_ResourceFile_avsc_Union__1__ v) {
        e.encodeUnionIndex(v.idx());
        switch (v.idx()) {
        case 0:
            e.encodeNull();
            break;
        case 1:
            avro::encode(e, v.get_string());
            break;
        }
    }
    static void decode(Decoder& d, daceDS::datamodel::_ResourceFile_avsc_Union__1__& v) {
        size_t n = d.decodeUnionIndex();
        if (n >= 2) { throw avro::Exception("Union index too big"); }
        switch (n) {
        case 0:
            d.decodeNull();
            v.set_null();
            break;
        case 1:
            {
                std::string vv;
                avro::decode(d, vv);
                v.set_string(vv);
            }
            break;
        }
    }
};

template<> struct codec_traits<daceDS::datamodel::ResourceFile> {
    static void encode(Encoder& e, const daceDS::datamodel::ResourceFile& v) {
        avro::encode(e, v.ID);
        avro::encode(e, v.Type);
        avro::encode(e, v.File);
        avro::encode(e, v.FileReference);
    }
    static void decode(Decoder& d, daceDS::datamodel::ResourceFile& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.ID);
                    break;
                case 1:
                    avro::decode(d, v.Type);
                    break;
                case 2:
                    avro::decode(d, v.File);
                    break;
                case 3:
                    avro::decode(d, v.FileReference);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.ID);
            avro::decode(d, v.Type);
            avro::decode(d, v.File);
            avro::decode(d, v.FileReference);
        }
    }
};

}
#endif
