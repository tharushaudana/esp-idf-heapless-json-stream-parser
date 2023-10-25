#include <stdio.h>
#include "heapless_json_stream_parser.h"

struct path_t
{
    json_key_t keys[20];
    int n_keys = 0;

    void up(json_key_t k)
    {
        if (k.key.length() == 0) return;
        keys[n_keys++] = k;
    }

    void down()
    {
        n_keys--;
    }

    json_key_t* lkey()
    {
        if (n_keys == 0) return nullptr;
        return &keys[n_keys - 1];
    }

    std::string k_to_str(json_key_t k) 
    {
        if (k.type == JSON_KEY_TYPE_OBJ)
            return k.key;
        else 
            return "[" + k.key + "]";
    }

    std::string to_str(json_key_t suffix_key)
    {
        std::string str = "";

        for (uint8_t i = 0; i < n_keys; i++)
        {
            json_key_t k = keys[i];

            str += k_to_str(k);

            if (i < n_keys - 1)
            {
                str += "/";
            }
        }

        if (suffix_key.key.length() > 0)
        {
            if (str.length() > 0) str += "/";
            str += k_to_str(suffix_key);
        }

        return str;
    }
};

path_t _path;

/*###########################################################################*/

json_stream_parser::json_stream_parser(on_stream_data_cb_t cb)
{
    _cb_data = cb;
}

void json_stream_parser::_notify_data() {
    _cb_data(_path.to_str(_key), _val);
}

void json_stream_parser::_set_current_ptype(int8_t t) {
    _ptype = t;
}

void json_stream_parser::_step_up_path() {
    _depth++;
    _path.up(_key);
}

void json_stream_parser::_step_down_path() {
    json_key_t *lkey = _path.lkey();

    _depth--;
    _path.down();

    //### set data of previous parent

    if (_path.n_keys == 0) {
        _set_current_ptype(JSON_PTYPE_OBJ);
        _sa(ACT_FIND_KEY);
        _key.key = "";
        return;
    }

    if (lkey != nullptr && lkey->type == JSON_KEY_TYPE_ARR) {
        _set_current_ptype(JSON_PTYPE_ARR);
        _sa(ACT_FIND_VAL);
        _key = *lkey;
    } else {
        _set_current_ptype(JSON_PTYPE_OBJ);
        _sa(ACT_FIND_KEY);
    }
}

void json_stream_parser::_set_arr_key() {
    _key.key = "0";
    _key.type = JSON_KEY_TYPE_ARR;
}

void json_stream_parser::_increase_arr_key() {
    if (_key.type != JSON_KEY_TYPE_ARR) return;
    uint8_t new_i = std::stoi(_key.key) + 1;
    _key.key = std::to_string(new_i);
}

void json_stream_parser::_clear_current_key() {
    _key.key = "";
    _key.type = JSON_KEY_TYPE_OBJ;
}

void json_stream_parser::_clear_current_val(int8_t t) {
    _val.val = "";
    _val.type = t;
}

void json_stream_parser::_append_current_key(char c) {
    _key.key += c;
}

void json_stream_parser::_append_current_val(char c) {
    _val.val += c;
}

bool json_stream_parser::_find_value(char c)
{
    if (c == '"') 
    {
        _clear_current_val(JSON_VAL_TYPE_STRING);
    }
    else if (isdigit(c))
    {
        _clear_current_val(JSON_VAL_TYPE_DECIMAL);
        _append_current_val(c);
    }
    else if (c == 't' || c == 'f')
    {
        _clear_current_val(JSON_VAL_TYPE_BOOL);
        _append_current_val(c);
    } 
    else 
    {
        return false;
    }

    _sa(ACT_READ_VAL);

    return true;
}

bool json_stream_parser::_read_value(char c)
{
    if (_val.type == JSON_VAL_TYPE_STRING) 
    {
        if (c == '"')
        {
            return true;
        }
        else 
        {
            _append_current_val(c);
            return false;
        }
    }
    else if (_val.type == JSON_VAL_TYPE_DECIMAL || _val.type == JSON_VAL_TYPE_FLOAT)
    {
        if (c != '.' && !isdigit(c))
        {
            return true;
        }
        else {
            _append_current_val(c);
            if (c == '.') _val.type = JSON_VAL_TYPE_FLOAT;
            return false;
        }
    }
    else if (_val.type == JSON_VAL_TYPE_BOOL)
    {
        if (c == 'e')
        {
            _append_current_val('e');
            return true;
        }
        else 
        {
            _append_current_val(c);
            return false;
        }
    }

    return false;
}

void json_stream_parser::_sa(int8_t a) {
    _act = a;
}

void json_stream_parser::parse(char c)
{
    if (_act == ACT_FIND_VAL)
    {
        if (c == '{')
        {
            _set_current_ptype(JSON_PTYPE_OBJ);
            _sa(ACT_FIND_KEY);
            _step_up_path();
            return;
        }
        else if (c == '[')
        {
            _step_up_path();
            _set_current_ptype(JSON_PTYPE_ARR);
            _sa(ACT_FIND_VAL);
            _set_arr_key();
            return;
        }
        else if (c == ']')
        {
            _step_down_path();

            if (_depth == -1) 
            {
                //... end
                return;
            }

            return;
        }
    }
    else if (_act == ACT_FIND_KEY && c == '}')
    {
        _step_down_path();

        if (_depth == -1) 
        {
            //... end
            return;
        }

        return;
    }
    
    if  (_act == ACT_FIND_KEY && c == '"') 
    {
        _clear_current_key();
        _sa(ACT_READ_KEY);
        return;
    }

    if (_act == ACT_READ_KEY) {
        if (c == '"') 
        {
            _sa(ACT_FIND_COL);
        }
        else 
        {   
            _append_current_key(c);
        }
        return;
    }

    if (_act == ACT_FIND_COL && c == ':') 
    {
        _sa(ACT_FIND_VAL);
        return;
    }

    if (_act == ACT_FIND_VAL) 
    {
        if (_ptype == JSON_PTYPE_ARR && c == ',') 
        {
            _increase_arr_key();
            return;
        }
        _find_value(c);
        return;
    }

    if (_act == ACT_READ_VAL) 
    {
        if (_read_value(c)) 
        {
            if (_ptype == JSON_PTYPE_OBJ)
                _sa(ACT_FIND_KEY);
            else if (_ptype == JSON_PTYPE_ARR)
                _sa(ACT_FIND_VAL);
            _notify_data();
        }
        return;
    }
}
