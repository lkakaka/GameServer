# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: common.proto

import sys
_b=sys.version_info[0]<3 and (lambda x:x) or (lambda x:x.encode('latin1'))
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='common.proto',
  package='',
  syntax='proto3',
  serialized_options=_b('\n\tcom.protoB\006Common'),
  serialized_pb=_b('\n\x0c\x63ommon.proto\"=\n\x06RpcMsg\x12\x0e\n\x06rpc_id\x18\x01 \x01(\x05\x12\x10\n\x08rpc_func\x18\x02 \x01(\t\x12\x11\n\trpc_param\x18\x03 \x01(\t\"-\n\tRpcMsgRsp\x12\x0e\n\x06rpc_id\x18\x01 \x01(\x05\x12\x10\n\x08rpc_data\x18\x02 \x01(\tB\x13\n\tcom.protoB\x06\x43ommonb\x06proto3')
)




_RPCMSG = _descriptor.Descriptor(
  name='RpcMsg',
  full_name='RpcMsg',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='rpc_id', full_name='RpcMsg.rpc_id', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='rpc_func', full_name='RpcMsg.rpc_func', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='rpc_param', full_name='RpcMsg.rpc_param', index=2,
      number=3, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=16,
  serialized_end=77,
)


_RPCMSGRSP = _descriptor.Descriptor(
  name='RpcMsgRsp',
  full_name='RpcMsgRsp',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='rpc_id', full_name='RpcMsgRsp.rpc_id', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='rpc_data', full_name='RpcMsgRsp.rpc_data', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=79,
  serialized_end=124,
)

DESCRIPTOR.message_types_by_name['RpcMsg'] = _RPCMSG
DESCRIPTOR.message_types_by_name['RpcMsgRsp'] = _RPCMSGRSP
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

RpcMsg = _reflection.GeneratedProtocolMessageType('RpcMsg', (_message.Message,), {
  'DESCRIPTOR' : _RPCMSG,
  '__module__' : 'common_pb2'
  # @@protoc_insertion_point(class_scope:RpcMsg)
  })
_sym_db.RegisterMessage(RpcMsg)

RpcMsgRsp = _reflection.GeneratedProtocolMessageType('RpcMsgRsp', (_message.Message,), {
  'DESCRIPTOR' : _RPCMSGRSP,
  '__module__' : 'common_pb2'
  # @@protoc_insertion_point(class_scope:RpcMsgRsp)
  })
_sym_db.RegisterMessage(RpcMsgRsp)


DESCRIPTOR._options = None
# @@protoc_insertion_point(module_scope)
