# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: proto/login.proto

import sys
_b=sys.version_info[0]<3 and (lambda x:x) or (lambda x:x.encode('latin1'))
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='proto/login.proto',
  package='',
  syntax='proto3',
  serialized_options=_b('\n\tcom.protoB\005Login'),
  serialized_pb=_b('\n\x11proto/login.proto\"9\n\x08LoginReq\x12\x0f\n\x07\x61\x63\x63ount\x18\x01 \x01(\t\x12\x0b\n\x03pwd\x18\x02 \x01(\t\x12\x0f\n\x07\x63onn_id\x18\x03 \x01(\x05\"\x1c\n\x08LoginRsp\x12\x10\n\x08\x65rr_code\x18\x01 \x01(\x05\".\n\x08RoleInfo\x12\x0f\n\x07role_id\x18\x01 \x01(\x05\x12\x11\n\trole_name\x18\x02 \x01(\t\"R\n\x0fLoadRoleListRsp\x12\x0f\n\x07\x61\x63\x63ount\x18\x01 \x01(\t\x12\x10\n\x08\x65rr_code\x18\x02 \x01(\x05\x12\x1c\n\trole_list\x18\x03 \x03(\x0b\x32\t.RoleInfo\"3\n\rCreateRoleReq\x12\x0f\n\x07\x61\x63\x63ount\x18\x01 \x01(\t\x12\x11\n\trole_name\x18\x02 \x01(\t\"?\n\rCreateRoleRsp\x12\x10\n\x08\x65rr_code\x18\x01 \x01(\x05\x12\x1c\n\trole_info\x18\x02 \x01(\x0b\x32\t.RoleInfo\"-\n\tEnterGame\x12\x0f\n\x07role_id\x18\x01 \x01(\x05\x12\x0f\n\x07\x61\x63\x63ount\x18\x02 \x01(\t\">\n\x0c\x45nterGameRsp\x12\x10\n\x08\x65rr_code\x18\x01 \x01(\x05\x12\x1c\n\trole_info\x18\x02 \x01(\x0b\x32\t.RoleInfo\"-\n\nDisconnect\x12\x0f\n\x07\x63onn_id\x18\x01 \x01(\x05\x12\x0e\n\x06reason\x18\x02 \x01(\tB\x12\n\tcom.protoB\x05Loginb\x06proto3')
)




_LOGINREQ = _descriptor.Descriptor(
  name='LoginReq',
  full_name='LoginReq',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='account', full_name='LoginReq.account', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='pwd', full_name='LoginReq.pwd', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='conn_id', full_name='LoginReq.conn_id', index=2,
      number=3, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
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
  serialized_start=21,
  serialized_end=78,
)


_LOGINRSP = _descriptor.Descriptor(
  name='LoginRsp',
  full_name='LoginRsp',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='err_code', full_name='LoginRsp.err_code', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
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
  serialized_start=80,
  serialized_end=108,
)


_ROLEINFO = _descriptor.Descriptor(
  name='RoleInfo',
  full_name='RoleInfo',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='role_id', full_name='RoleInfo.role_id', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='role_name', full_name='RoleInfo.role_name', index=1,
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
  serialized_start=110,
  serialized_end=156,
)


_LOADROLELISTRSP = _descriptor.Descriptor(
  name='LoadRoleListRsp',
  full_name='LoadRoleListRsp',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='account', full_name='LoadRoleListRsp.account', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='err_code', full_name='LoadRoleListRsp.err_code', index=1,
      number=2, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='role_list', full_name='LoadRoleListRsp.role_list', index=2,
      number=3, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
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
  serialized_start=158,
  serialized_end=240,
)


_CREATEROLEREQ = _descriptor.Descriptor(
  name='CreateRoleReq',
  full_name='CreateRoleReq',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='account', full_name='CreateRoleReq.account', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='role_name', full_name='CreateRoleReq.role_name', index=1,
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
  serialized_start=242,
  serialized_end=293,
)


_CREATEROLERSP = _descriptor.Descriptor(
  name='CreateRoleRsp',
  full_name='CreateRoleRsp',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='err_code', full_name='CreateRoleRsp.err_code', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='role_info', full_name='CreateRoleRsp.role_info', index=1,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
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
  serialized_start=295,
  serialized_end=358,
)


_ENTERGAME = _descriptor.Descriptor(
  name='EnterGame',
  full_name='EnterGame',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='role_id', full_name='EnterGame.role_id', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='account', full_name='EnterGame.account', index=1,
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
  serialized_start=360,
  serialized_end=405,
)


_ENTERGAMERSP = _descriptor.Descriptor(
  name='EnterGameRsp',
  full_name='EnterGameRsp',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='err_code', full_name='EnterGameRsp.err_code', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='role_info', full_name='EnterGameRsp.role_info', index=1,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
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
  serialized_start=407,
  serialized_end=469,
)


_DISCONNECT = _descriptor.Descriptor(
  name='Disconnect',
  full_name='Disconnect',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='conn_id', full_name='Disconnect.conn_id', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='reason', full_name='Disconnect.reason', index=1,
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
  serialized_start=471,
  serialized_end=516,
)

_LOADROLELISTRSP.fields_by_name['role_list'].message_type = _ROLEINFO
_CREATEROLERSP.fields_by_name['role_info'].message_type = _ROLEINFO
_ENTERGAMERSP.fields_by_name['role_info'].message_type = _ROLEINFO
DESCRIPTOR.message_types_by_name['LoginReq'] = _LOGINREQ
DESCRIPTOR.message_types_by_name['LoginRsp'] = _LOGINRSP
DESCRIPTOR.message_types_by_name['RoleInfo'] = _ROLEINFO
DESCRIPTOR.message_types_by_name['LoadRoleListRsp'] = _LOADROLELISTRSP
DESCRIPTOR.message_types_by_name['CreateRoleReq'] = _CREATEROLEREQ
DESCRIPTOR.message_types_by_name['CreateRoleRsp'] = _CREATEROLERSP
DESCRIPTOR.message_types_by_name['EnterGame'] = _ENTERGAME
DESCRIPTOR.message_types_by_name['EnterGameRsp'] = _ENTERGAMERSP
DESCRIPTOR.message_types_by_name['Disconnect'] = _DISCONNECT
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

LoginReq = _reflection.GeneratedProtocolMessageType('LoginReq', (_message.Message,), {
  'DESCRIPTOR' : _LOGINREQ,
  '__module__' : 'proto.login_pb2'
  # @@protoc_insertion_point(class_scope:LoginReq)
  })
_sym_db.RegisterMessage(LoginReq)

LoginRsp = _reflection.GeneratedProtocolMessageType('LoginRsp', (_message.Message,), {
  'DESCRIPTOR' : _LOGINRSP,
  '__module__' : 'proto.login_pb2'
  # @@protoc_insertion_point(class_scope:LoginRsp)
  })
_sym_db.RegisterMessage(LoginRsp)

RoleInfo = _reflection.GeneratedProtocolMessageType('RoleInfo', (_message.Message,), {
  'DESCRIPTOR' : _ROLEINFO,
  '__module__' : 'proto.login_pb2'
  # @@protoc_insertion_point(class_scope:RoleInfo)
  })
_sym_db.RegisterMessage(RoleInfo)

LoadRoleListRsp = _reflection.GeneratedProtocolMessageType('LoadRoleListRsp', (_message.Message,), {
  'DESCRIPTOR' : _LOADROLELISTRSP,
  '__module__' : 'proto.login_pb2'
  # @@protoc_insertion_point(class_scope:LoadRoleListRsp)
  })
_sym_db.RegisterMessage(LoadRoleListRsp)

CreateRoleReq = _reflection.GeneratedProtocolMessageType('CreateRoleReq', (_message.Message,), {
  'DESCRIPTOR' : _CREATEROLEREQ,
  '__module__' : 'proto.login_pb2'
  # @@protoc_insertion_point(class_scope:CreateRoleReq)
  })
_sym_db.RegisterMessage(CreateRoleReq)

CreateRoleRsp = _reflection.GeneratedProtocolMessageType('CreateRoleRsp', (_message.Message,), {
  'DESCRIPTOR' : _CREATEROLERSP,
  '__module__' : 'proto.login_pb2'
  # @@protoc_insertion_point(class_scope:CreateRoleRsp)
  })
_sym_db.RegisterMessage(CreateRoleRsp)

EnterGame = _reflection.GeneratedProtocolMessageType('EnterGame', (_message.Message,), {
  'DESCRIPTOR' : _ENTERGAME,
  '__module__' : 'proto.login_pb2'
  # @@protoc_insertion_point(class_scope:EnterGame)
  })
_sym_db.RegisterMessage(EnterGame)

EnterGameRsp = _reflection.GeneratedProtocolMessageType('EnterGameRsp', (_message.Message,), {
  'DESCRIPTOR' : _ENTERGAMERSP,
  '__module__' : 'proto.login_pb2'
  # @@protoc_insertion_point(class_scope:EnterGameRsp)
  })
_sym_db.RegisterMessage(EnterGameRsp)

Disconnect = _reflection.GeneratedProtocolMessageType('Disconnect', (_message.Message,), {
  'DESCRIPTOR' : _DISCONNECT,
  '__module__' : 'proto.login_pb2'
  # @@protoc_insertion_point(class_scope:Disconnect)
  })
_sym_db.RegisterMessage(Disconnect)


DESCRIPTOR._options = None
# @@protoc_insertion_point(module_scope)
