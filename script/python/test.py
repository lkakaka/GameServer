import re

def match_func(matched):
    value = matched.group('value')
    print(value)
    if value.isupper():
        return "_" + value
    return value

proto_name = "LoginReq"
print(re.sub('(?P<value>\w)', match_func, proto_name))


def test_kw(x, a=1):
    print("test kw--", a)

d = {'a': 10}
test_kw(0, **d)
