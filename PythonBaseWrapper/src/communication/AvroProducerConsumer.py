#!/usr/bin/env python3
#
# Copyright 2016-2020 Confluent Inc.
# Modified 2021 Moritz Gütlein
# 
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy
# of the License at
# 
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.
# 

"""
    Basically, the confluent AvroProducer/AvroConsumer with slight modification
    in order to allow Avro serialized values and plain string keys.
"""

from confluent_kafka import Producer, Consumer
from confluent_kafka.avro.error import ClientError
from confluent_kafka.avro.load import load, loads  # noqa
from confluent_kafka.avro.cached_schema_registry_client import CachedSchemaRegistryClient
from confluent_kafka.avro.serializer import (SerializerError,  # noqa
                                             KeySerializerError,
                                             ValueSerializerError)
from confluent_kafka.avro.serializer.message_serializer import MessageSerializer
from confluent_kafka.serialization import StringSerializer,SerializationContext,MessageField
from confluent_kafka.serialization import StringDeserializer,SerializationContext,MessageField

class AvroProducerStrKey(Producer):
    def __init__(self, config, default_key_schema=None,
                 default_value_schema=None, schema_registry=None):

        sr_conf = {key.replace("schema.registry.", ""): value
                   for key, value in config.items() if key.startswith("schema.registry")}

        if sr_conf.get("basic.auth.credentials.source") == 'SASL_INHERIT':
            # Fallback to plural 'mechanisms' for backward compatibility
            sr_conf['sasl.mechanism'] = config.get('sasl.mechanism', config.get('sasl.mechanisms', ''))
            sr_conf['sasl.username'] = config.get('sasl.username', '')
            sr_conf['sasl.password'] = config.get('sasl.password', '')
            sr_conf['auto.register.schemas'] = config.get('auto.register.schemas', True)

        ap_conf = {key: value
                   for key, value in config.items() if not key.startswith("schema.registry")}

        if schema_registry is None:
            schema_registry = CachedSchemaRegistryClient(sr_conf)
        elif sr_conf.get("url", None) is not None:
            raise ValueError("Cannot pass schema_registry along with schema.registry.url config")

        super(AvroProducerStrKey, self).__init__(ap_conf)
        self._serializer = MessageSerializer(schema_registry)
        self._stringSerializer = StringSerializer()
        self._value_schema = default_value_schema

    def produce(self, **kwargs):
        # get schemas from  kwargs if defined
        value_schema = kwargs.pop('value_schema', self._value_schema)
        topic = kwargs.pop('topic', None)
        if not topic:
            raise ClientError("Topic name not specified.")
        value = kwargs.pop('value', None)
        key = kwargs.pop('key', None)

        if value is not None:
            if value_schema:
                value = self._serializer.encode_record_with_schema(topic, value_schema, value)
            else:
                raise ValueSerializerError("Avro schema required for values")

        ctx = SerializationContext(topic, MessageField.KEY)
        key = self._stringSerializer(key, ctx)

        super(AvroProducerStrKey, self).produce(topic, value, key, **kwargs)


class AvroConsumerStrKey(Consumer):
    def __init__(self, config, schema_registry=None, reader_key_schema=None, reader_value_schema=None):

        sr_conf = {key.replace("schema.registry.", ""): value
                   for key, value in config.items() if key.startswith("schema.registry")}

        if sr_conf.get("basic.auth.credentials.source") == 'SASL_INHERIT':
            # Fallback to plural 'mechanisms' for backward compatibility
            sr_conf['sasl.mechanism'] = config.get('sasl.mechanism', config.get('sasl.mechanisms', ''))
            sr_conf['sasl.username'] = config.get('sasl.username', '')
            sr_conf['sasl.password'] = config.get('sasl.password', '')

        ap_conf = {key: value
                   for key, value in config.items() if not key.startswith("schema.registry")}

        if schema_registry is None:
            schema_registry = CachedSchemaRegistryClient(sr_conf)
        elif sr_conf.get("url", None) is not None:
            raise ValueError("Cannot pass schema_registry along with schema.registry.url config")

        super(AvroConsumerStrKey, self).__init__(ap_conf)
        self._serializer = MessageSerializer(schema_registry, reader_key_schema, reader_value_schema)
        self._stringDeserializer = StringDeserializer()

    def poll(self, timeout=None):
        if timeout is None:
            timeout = -1
        message = super(AvroConsumerStrKey, self).poll(timeout)
        if message is None:
            return None
    
        if not message.error():
            try:
                if message.value() is not None:
                    decoded_value = self._serializer.decode_message(message.value(), is_key=False)
                    message.set_value(decoded_value)
                if message.key() is not None:
                    ctx = SerializationContext(message.topic(), MessageField.KEY)
                    decoded_key = self._stringDeserializer(message.key(), ctx)
                    message.set_key(decoded_key)
            except SerializerError as e:
                raise SerializerError("Message deserialization failed for message at {} [{}] offset {}: {}".format(
                    message.topic(),
                    message.partition(),
                    message.offset(),
                    e))
        return message
    
