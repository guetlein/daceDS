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


#ifndef CPP_MICRODETECTOR_HH_1303895446__H_
#define CPP_MICRODETECTOR_HH_1303895446__H_


#include <sstream>
#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"

namespace daceDS {
namespace datamodel {
struct _MicroDetector_avsc_Union__0__ {
private:
    size_t idx_;
    boost::any value_;
public:
    size_t idx() const { return idx_; }
    bool get_bool() const;
    void set_bool(const bool& v);
    double get_double() const;
    void set_double(const double& v);
    int32_t get_int() const;
    void set_int(const int32_t& v);
    std::string get_string() const;
    void set_string(const std::string& v);
    _MicroDetector_avsc_Union__0__();
};

struct MicroDetector {
    std::string detectorID;
    std::map<std::string, _MicroDetector_avsc_Union__0__ > values;
    MicroDetector() :
        detectorID(std::string()),
        values(std::map<std::string, _MicroDetector_avsc_Union__0__ >())
        { }
};

inline
bool _MicroDetector_avsc_Union__0__::get_bool() const {
    if (idx_ != 0) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<bool >(value_);
}

inline
void _MicroDetector_avsc_Union__0__::set_bool(const bool& v) {
    idx_ = 0;
    value_ = v;
}

inline
double _MicroDetector_avsc_Union__0__::get_double() const {
    if (idx_ != 1) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<double >(value_);
}

inline
void _MicroDetector_avsc_Union__0__::set_double(const double& v) {
    idx_ = 1;
    value_ = v;
}

inline
int32_t _MicroDetector_avsc_Union__0__::get_int() const {
    if (idx_ != 2) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<int32_t >(value_);
}

inline
void _MicroDetector_avsc_Union__0__::set_int(const int32_t& v) {
    idx_ = 2;
    value_ = v;
}

inline
std::string _MicroDetector_avsc_Union__0__::get_string() const {
    if (idx_ != 3) {
        throw avro::Exception("Invalid type for union");
    }
    return boost::any_cast<std::string >(value_);
}

inline
void _MicroDetector_avsc_Union__0__::set_string(const std::string& v) {
    idx_ = 3;
    value_ = v;
}

inline _MicroDetector_avsc_Union__0__::_MicroDetector_avsc_Union__0__() : idx_(0), value_(bool()) { }
}
}
namespace avro {
template<> struct codec_traits<daceDS::datamodel::_MicroDetector_avsc_Union__0__> {
    static void encode(Encoder& e, daceDS::datamodel::_MicroDetector_avsc_Union__0__ v) {
        e.encodeUnionIndex(v.idx());
        switch (v.idx()) {
        case 0:
            avro::encode(e, v.get_bool());
            break;
        case 1:
            avro::encode(e, v.get_double());
            break;
        case 2:
            avro::encode(e, v.get_int());
            break;
        case 3:
            avro::encode(e, v.get_string());
            break;
        }
    }
    static void decode(Decoder& d, daceDS::datamodel::_MicroDetector_avsc_Union__0__& v) {
        size_t n = d.decodeUnionIndex();
        if (n >= 4) { throw avro::Exception("Union index too big"); }
        switch (n) {
        case 0:
            {
                bool vv;
                avro::decode(d, vv);
                v.set_bool(vv);
            }
            break;
        case 1:
            {
                double vv;
                avro::decode(d, vv);
                v.set_double(vv);
            }
            break;
        case 2:
            {
                int32_t vv;
                avro::decode(d, vv);
                v.set_int(vv);
            }
            break;
        case 3:
            {
                std::string vv;
                avro::decode(d, vv);
                v.set_string(vv);
            }
            break;
        }
    }
};

template<> struct codec_traits<daceDS::datamodel::MicroDetector> {
    static void encode(Encoder& e, const daceDS::datamodel::MicroDetector& v) {
        avro::encode(e, v.detectorID);
        avro::encode(e, v.values);
    }
    static void decode(Decoder& d, daceDS::datamodel::MicroDetector& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.detectorID);
                    break;
                case 1:
                    avro::decode(d, v.values);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.detectorID);
            avro::decode(d, v.values);
        }
    }
};

}
#endif
