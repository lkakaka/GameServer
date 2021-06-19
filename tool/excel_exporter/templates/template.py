
_{{ table_name }}_DATA = {
{% for key, value in row_data.items() %}
	{{ key }}: {{ value }},
{% endfor %}
}

{% for index_name, index_data in index_data.items() %}
{{ index_name }} = "{{ index_name }}"
_{{ index_name }}_DATA = {
{% for key, value in index_data.items() %}
	{{ key }}: {{ value }},
{% endfor %}
}
{% endfor %}

_Index_DATA = {
{% for index_name, index_data in index_data.items() %}
{{ index_name }}: _{{ index_name }}_DATA,
{% endfor %}
}

class _{{ table_name }}_Row:
    def __init__(self, row_data):
{% for col_name, idx in col_name_idx.items() %}
        self.__dict__["{{ col_name }}"] = row_data[{{ idx }}]
{% endfor %}

    def __repr__(self):
        data = {}
{% for col_name, idx in col_name_idx.items() %}
        data["{{ col_name }}"] = self.__dict__["{{ col_name }}"]
{% endfor %}
        return str(data)

def find_by_index(index_name, index_val):
    if index_name not in _Index_DATA:
        raise Exception("table {{ table_name }} index {0} not exist!!!".format(index_name))
    index_data = _Index_DATA.get(index_name)
    rows = []
    for id in index_data.get(index_val, []):
        rows.append(find(id))
    return tuple(rows)

def find(id):
    data = _{{ table_name }}_DATA.get(id)
    return _{{ table_name }}_Row(data)
