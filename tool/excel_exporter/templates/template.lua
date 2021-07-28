
Cfg{{ table_name }} = {}

Cfg{{ table_name }}._DATA = {
{% for key, values in row_data.items() %}
    [{{ key }}] = {{'{'}}{% for value in values %}{% if value is string %}"{{ value }}",{% else %}{{ value }},{% endif %}{% endfor %}
},
{% endfor %}
}

{% for index_name, index_data in index_data.items() %}
Cfg{{ table_name }}.{{ index_name }} = "{{ index_name }}"
Cfg{{ table_name }}._{{ index_name }}_DATA = {
{% for key, values in index_data.items() %}
    [{{ key }}] = {{'{'}}{% for value in values %}{% if value is string %}"{{ value }}",{% else %}{{ value }},{% endif %}{% endfor %}{{'}'}},
{% endfor %}
}
{% endfor %}

Cfg{{ table_name }}._Index_DATA = {
{% for index_name, index_data in index_data.items() %}
    [Cfg{{ table_name }}.{{ index_name }}] = Cfg{{ table_name }}._{{ index_name }}_DATA,
{% endfor %}
}


function Cfg{{ table_name }}.find_by_index(index_name, index_val)
    local index_data = Cfg{{ table_name }}._Index_DATA[index_name]
    if index_data == nil then
        error(string.format("table {{ table_name }} index {0} not exist!!!", index_name))
    end

    local rows = {}
    for _,id in ipairs(index_data[index_val] or {}) do
        table.insert(rows, Cfg{{ table_name }}.find(id))
    end
    return rows
end

function Cfg{{ table_name }}.find(id)
    local data = Cfg{{ table_name }}._DATA[id]
    local _data = {
    {% for col_name, idx in col_name_idx.items() %}
        {{ col_name }} = data[{{ idx + 1 }}],
    {% endfor %}
    }
    return _data
end

