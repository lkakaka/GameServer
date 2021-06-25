# -*- coding:utf-8 -*-


class MulitIndexAttrCannotSetException(Exception):
    pass


class MultiIndexElement(object):
    _in_container = False
    _multi_index_attr_names = []
    # note: 做为索引的字段不能修改
    @staticmethod
    def define_multi_index_attr_names():
        return None

    def on_add_to_container(self):
        if self._in_container:
            return
        self._in_container = True
        self._multi_index_attr_names = []
        attr_names = self.define_multi_index_attr_names()
        for attr_name in attr_names:
            if type(attr_name) is str:
                self._multi_index_attr_names.append(attr_name)
            else:
                for name in attr_name:
                    self._multi_index_attr_names.append(name)

    def on_remove_from_container(self):
        self._in_container = False
        self._multi_index_attr_names = []

    def __setattr__(self, key, value):
        if not self._in_container or key not in self._multi_index_attr_names:
            return object.__setattr__(self, key, value)
        raise MulitIndexAttrCannotSetException("cannot change mulit index attr val!!, attr:{0}".format(key))


class MultiIndexContainer(object):

    def __init__(self, cls):
        self._containers = {}
        self._relation_attr_name = []

        if type(cls) is MultiIndexElement:
            raise Exception("class not support multi index")
        multi_index_attr_names = cls.define_multi_index_attr_names()
        if type(multi_index_attr_names) not in (tuple, list):
            raise Exception("class not define multi_index_attr_names")
        self._attr_names = multi_index_attr_names
        for attr_name in multi_index_attr_names:
            container_name = MultiIndexContainer.gen_container_name(attr_name)
            self._add_container(container_name)
            self._add_attr_name(attr_name)

    def _add_attr_name(self, attr_name):
        if type(attr_name) is str:
            self._add_str_attr_name(attr_name)
        else:
            for name in attr_name:
                self._add_str_attr_name(name)

    def _add_str_attr_name(self, attr_name):
        if attr_name not in self._relation_attr_name:
            self._relation_attr_name.append(attr_name)

    def _add_container(self, container_name):
        if container_name in self._containers:
            raise Exception("container_name {0} existed!!!", container_name)
        self._containers[container_name] = {}

    def _get_container(self, container_name):
        return self._containers.get(container_name)

    @staticmethod
    def gen_container_name(attr_name):
        if type(attr_name) is str:
            return "container_" + attr_name

        if type(attr_name) in (tuple, list) and len(attr_name) > 0:
            return "container_" + "_".join(attr_name)

        raise Exception("multi_index_attr_names define error, attr_name:{0}".format(attr_name))

    @staticmethod
    def get_elem_attr_val_atom(elem, attr_name):
        attr_val = elem.__dict__[attr_name]
        if attr_val is None:
            raise Exception("element attr({0}) val is None!!".format(attr_name))
        return attr_val

    @staticmethod
    def get_elem_attr_val(elem, attr_name):
        if type(attr_name) is str:
            return MultiIndexContainer.get_elem_attr_val_atom(elem, attr_name)

        if type(attr_name) in (tuple, list) and len(attr_name) > 0:
            val = []
            for name in attr_name:
                val.append(MultiIndexContainer.get_elem_attr_val_atom(elem, name))
            return tuple(val)

        raise Exception("multi_index_attr_names define error, attr_name:{0}".format(attr_name))

    def add_elem(self, element):
        for attr_name in self._attr_names:
            container_name = MultiIndexContainer.gen_container_name(attr_name)
            container = self._get_container(container_name)
            attr_val = MultiIndexContainer.get_elem_attr_val(element, attr_name)
            if attr_val not in container:
                container[attr_val] = []
            if element not in container[attr_val]:
                container[attr_val].append(element)
        element.on_add_to_container()

    def remove_elem(self, element):
        for attr_name in self._attr_names:
            container_name = MultiIndexContainer.gen_container_name(attr_name)
            container = self._get_container(container_name)
            attr_val = MultiIndexContainer.get_elem_attr_val(element, attr_name)
            if attr_val not in container:
                return
            if element not in container[attr_val]:
                return
            container[attr_val].remove(element)
        element.on_remove_from_container()

    def get_elems(self, attr_name, attr_val):
        if type(attr_val) is list:
            attr_val = tuple(attr_val)
        container_name = MultiIndexContainer.gen_container_name(attr_name)
        container = self._get_container(container_name)
        return container.get(attr_val)

    def get_one_elem(self, attr_name, attr_val):
        elems = self.get_elems(attr_name, attr_val)
        if not elems:
            return None
        return elems[0]


class MultiIndexElementTest(MultiIndexElement):
    @staticmethod
    def define_multi_index_attr_names():
        return ("a", "b", ("a", "b"))

    def __init__(self, a, b, c):
        self.a = a
        self.b = b
        self.c = c


if __name__ == "__main__":
    m_container = MultiIndexContainer(MultiIndexElementTest)
    lst = [(1, 10, 100), (2, 20, 200), (1, 10, 100)]
    for a, b, c in lst:
        elem = MultiIndexElementTest(a, b, c)
        m_container.add_elem(elem)

    a_val = 1
    expect_a_count = 2
    elems = m_container.get_elems("a", a_val)
    assert (len(elems) == expect_a_count)
    for elem in elems:
        has_ex = False
        try:
            elem.a = 100
        except MulitIndexAttrCannotSetException as e:
            has_ex = True
        assert has_ex, "set index val not raise MulitIndexAttrCannotSetException"
        print(elem.a)
        assert (elem.a == a_val)

    b_val = 20
    expect_b_count = 1
    elems = m_container.get_elems("b", b_val)
    assert (len(elems) == expect_b_count)
    for elem in elems:
        elem.c = 10000
        print(elem.b, elem.c)
        assert (elem.b == b_val)
        assert(elem.c == 10000)

    a_b_val = (1, 10)
    expect_a_b_count = 2
    elems = m_container.get_elems(("a", "b"), a_b_val)
    assert (len(elems) == expect_a_b_count)
    for elem in elems:
        print(elem.a, elem.b)
        assert (elem.a == a_b_val[0] and elem.b == a_b_val[1])

    print("test success")
