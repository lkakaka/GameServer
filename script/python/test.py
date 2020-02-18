import re

def match_func(matched):
    value = matched.group('value')
    print(value)
    if value.isupper():
        return "_" + value
    return value

proto_name = "LoginReq"
print(re.sub('(?P<value>\w)', match_func, proto_name))
